/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#pragma once

#include "kind_inference_context.h"
#include "computation_on_types.h"

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              kind_inference_scope
//------------------------------------------------------------------
kind_inference_scope::kind_inference_scope(kind_inference_scope&& other)
    :m_owner(other.m_owner), m_var(other.m_var), m_oper(other.m_oper)
    ,m_old_polarity(other.m_old_polarity)
{
    other.m_owner = nullptr;
};

kind_inference_scope::kind_inference_scope(kind_inference_context* owner, ast::polarity_type old,
                                           oper_type oper)
    :m_owner(owner), m_old_polarity(old), m_oper(oper)
{};

kind_inference_scope::kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi)
    :m_owner(owner), m_var(vi), m_oper(remove_var)
{};

kind_inference_scope::kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi,
                               const kind_pol& item)
    :m_owner(owner), m_var(vi), m_oper(restore_var), m_old_elem(item)
{};

kind_inference_scope::kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi,
                               const ast::type& ty)
    :m_owner(owner), m_var(vi), m_oper(restore_type_subs), m_old_elem_type(ty)
{};

kind_inference_scope::kind_inference_scope()
    :m_owner(nullptr)
{};

kind_inference_scope::~kind_inference_scope()
{
    if (!m_owner)
        return;

    switch(m_oper)
    {
        case restore_var:
            m_owner->restore_var(m_var, m_old_elem);
            break;
        case restore_type_subs:
            m_owner->restore_type_subs(m_var, m_old_elem_type);
            break;
        case remove_var:
            m_owner->remove_var(m_var);
            break;
        case negate_polarities:        
            m_owner->restore_negate_polarities();
            break;
        case neutral_polarities:        
            m_owner->restore_neutral_polarities(m_old_polarity);
            break;
    };        
}

//------------------------------------------------------------------
//              kind_pol
//------------------------------------------------------------------
kind_pol::kind_pol()
    :m_discard_count(0)
{};

kind_pol::kind_pol(const ast::kind& k, polarity pol)
    :m_kind(k), m_pol_orig(pol), m_pol_current(pol), m_discard_count(0)
{};

void kind_pol::negate()
{
    m_pol_current = kind_inference_context::negate(m_pol_current);
};

void kind_pol::discard_polarized()
{
    ++m_discard_count;
};
void kind_pol::restore_polarized()
{
    --m_discard_count;
}

kind_pol::polarity kind_pol::get_original_polarity() const
{
    return m_pol_orig;
};
kind_pol::polarity kind_pol::get_current_polarity(bool& is_visible) const
{
    if (m_discard_count > 0)
    {
        if (m_pol_orig == polarity::neutral)
            is_visible = true;
        else
            is_visible = false;
    }
    else
    {
        is_visible = true;
    };

    return m_pol_current;
};

const ast::kind& kind_pol::get_kind() const
{
    return m_kind;
}

//------------------------------------------------------------------
//              kind_inference_context
//------------------------------------------------------------------
kind_inference_context::kind_inference_context()
{
    m_polarity_state    = polarity::positive;
}

void kind_inference_context::clear()
{
    m_polarity_state    = polarity::positive;
    m_vars.clear();
    m_recursive.clear();
    m_gen_items_k.clear();
};

void kind_inference_context::add_generalize_kind(const vec_ident& args, const ast::kind& k_body, 
                    const ast::kind& k_check, const ad::var_info& sym)
{
    m_gen_items_k.push_back(gen_item_k(args, k_body, k_check, sym));
};

kind_inference_context::polarity kind_inference_context::mult_by_state(polarity pol) const
{
    return mult_polarities(m_polarity_state, pol);
}

kind_inference_context::polarity kind_inference_context::negate(polarity pol)
{
    if (pol == polarity::positive)
        return polarity::negative;
    else if (pol == polarity::negative)
        return polarity::positive;
    else
        return pol;
};

kind_inference_scope kind_inference_context::add_var(const ast::identifier& id, const ast::kind& k,
                                         ast::polarity_type pol)
{
    ad::var_info vi(id);
    auto pos = m_vars.find(vi);

    if (pos == m_vars.end())
    {
        m_vars.insert(pos, var_map::value_type(vi, kind_pol(k, pol)));
        return kind_inference_scope(this, vi);
    }
    else
    {
        const kind_pol& old_kp = pos->second;
        pos->second = kind_pol(k, pol);
        return kind_inference_scope(this, vi, old_kp);
    }
};

kind_inference_context::scope_type 
kind_inference_context::add_type_subs(const ast::identifier& id, const ast::type& ty)
{
    ad::var_info vi(id);

    auto pos    = m_type_subs.find(vi);

    if (pos == m_type_subs.end())
    {
        m_type_subs.insert(pos, subs_map::value_type(vi, ty));
        return kind_inference_scope(this, vi, ast::type());
    }
    else
    {
        ast::type old_ty    = pos->second;
        pos->second         = ty;
        return kind_inference_scope(this, vi, old_ty);
    };
};

kind_inference_scope kind_inference_context::negate_polarities()
{
    auto oper = kind_inference_scope::negate_polarities;
    kind_inference_scope sc(this, m_polarity_state, oper);

    m_polarity_state    = negate(m_polarity_state);

    for (auto& elem : m_vars)
        elem.second.negate();  
    
    return sc;
};
void kind_inference_context::restore_negate_polarities()
{
    m_polarity_state    = negate(m_polarity_state);

    for (auto& elem : m_vars)
        elem.second.negate();
};

kind_inference_scope kind_inference_context::mult_polarities(ast::polarity_type pol)
{
    if (pol == ast::polarity_type::positive)
        return kind_inference_scope();

    if (pol == ast::polarity_type::negative)
        return negate_polarities();

    auto oper = kind_inference_scope::neutral_polarities;
    kind_inference_scope sc(this, m_polarity_state, oper);
    
    m_polarity_state = ast::polarity_type::neutral;

    for (auto& elem : m_vars)
        elem.second.discard_polarized();    

    return sc;
};

void kind_inference_context::restore_neutral_polarities(polarity old)
{    
    m_polarity_state = old;

    for (auto& elem : m_vars)
        elem.second.restore_polarized();    
};

ast::polarity_type kind_inference_context::mult_polarities(ast::polarity_type pol1, 
                                    ast::polarity_type pol2)
{
    if (pol1 == polarity::positive)
        return pol2;
    else if (pol1 == polarity::negative)
        return negate(pol2);
    else
        return pol1;
};

void kind_inference_context::add_recursive_symbol(const ad::var_info& sym)
{
    m_recursive.insert(sym);
}

void kind_inference_context::remove_recursive_symbol(const ad::var_info& sym)
{
    m_recursive.erase(sym);
}

const kind_inference_context::gen_item_k_vec& 
kind_inference_context::get_generalize_items_kind() const
{
    return m_gen_items_k;
};

bool kind_inference_context::make_substitution(ast::type& t) const
{
    /// it is assumed, that types in subs map have fresh binders
    return type_substitution_head().make(m_type_subs, t);
};

ast::polarity_type kind_inference_context::get_symbol_polarity(const ast::identifier& id, 
            ast::kind& k, ast::polarity_type& orig_pol, bool& is_binder, bool& is_rec,
            bool& is_visible) const
{
    if (m_recursive.find(ad::var_info(id)) != m_recursive.end())
    {
        orig_pol    = ast::polarity_type::positive;
        is_binder   = false;
        is_rec      = true;

        if (m_polarity_state == ast::polarity_type::neutral)
            is_visible = false;
        else
            is_visible = true;

        return mult_polarities(orig_pol, m_polarity_state);
    };

    auto pos = m_vars.find(ad::var_info(id));
    if (pos == m_vars.end())
    {
        orig_pol    = ast::polarity_type::neutral;
        is_binder   = false;
        is_rec      = false;
        is_visible  = false;
        return orig_pol;
    };

    orig_pol    = pos->second.get_original_polarity();
    is_binder   = true;
    is_rec      = false;
    k           = pos->second.get_kind();
    
    return pos->second.get_current_polarity(is_visible);
}

void kind_inference_context::remove_var(const ad::var_info& vi)
{
    m_vars.erase(vi);
}
void kind_inference_context::restore_var(const ad::var_info& vi, const kind_pol& old)
{
    m_vars[vi] = old;
}
void kind_inference_context::restore_type_subs(const ad::var_info& vi, const ast::type& old)
{
    if (old)
        m_type_subs[vi] = old;
    else
        m_type_subs.erase(vi);
}

}}}
