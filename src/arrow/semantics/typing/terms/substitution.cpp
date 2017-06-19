/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017
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

#include "substitution.h"
#include "arrow/ast/ast_visitor.h"
#include "scopes/module.h"
#include "root/options.h"
#include "semantics/semantics.h"
#include "semantics/typing/terms/type_equaler.h"
#include "error/error.h"

#include "boost/lexical_cast.hpp"

#pragma warning(push)
#pragma warning(disable: 4127)  //conditional expression is constant

namespace arrow { namespace typing
{

var_subs_info::var_subs_info(const ast::type_var& tv, const annot_id& id)
    :m_type_var(tv), m_id(id), m_is_bound(false), m_binders(fresh_binders())
    ,m_state(deduction_state::initialized)
{};

var_subs_info::var_subs_info(const annot_id& id, const fresh_binders& fb)
    :m_type_var(ast::type_var()), m_id(id), m_is_bound(true), m_binders(fb)
    ,m_state(deduction_state::initialized)
{};

const ast::type_var& var_subs_info::get_type_var() const
{
    return m_type_var;
}
const var_subs_info::annot_id& var_subs_info::get_identifier() const
{
    return m_id;
}
bool var_subs_info::is_bound() const
{
    return m_is_bound;
}
void var_subs_info::set_binder(const ast::identifier_type& bind)
{
    m_is_bound = true;
    m_binders.set_binder(bind);
}
void var_subs_info::mark_visited()
{
    m_state = deduction_state::visited;
};
void var_subs_info::mark_infered()
{
    m_state = deduction_state::infered;
};

bool var_subs_info::is_visited() const
{
    return m_state == deduction_state::visited;
};
bool var_subs_info::is_infered() const
{
    return m_state == deduction_state::infered;
};

const ast::identifier_type& var_subs_info::get_binder() const
{
    return m_binders.get_binder();
}

void var_substitution::add(const ast::annotated_type_identifier& vi, const ast::type_var& t)
{
    ad::var_info info(vi.get_identifier());

    auto pos = m_map.insert(subs_map::value_type(info, var_subs_info(t, vi)));
    m_vec.push_back(pos.first);
};
void var_substitution::add_free(const ast::annotated_type_identifier& vi)
{
    ad::var_info info(vi.get_identifier());

    const ast::identifier& id   = vi.get_identifier();

    bool can_change             = true;
    ast::identifier id1         = sema::make_new_binder(id, m_mod, can_change);
    ast::identifier_type id_ty  = ast::identifier_type(id1->get_location(), id1);

    var_subs_info::fresh_binders fb;
    fb.set_binder(id_ty);

    auto pos = m_map.insert(subs_map::value_type(info, var_subs_info(vi, fb)));    
    m_vec.push_back(pos.first);
};

long var_substitution::size() const
{
    return (long)m_map.size();
}

void var_substitution::mark_free_variables()
{
    for (auto& pos : m_vec)
    {
        const ast::type_var& tv = pos->second.get_type_var();
        if (tv.is_set() == true)
            continue;

        const ast::identifier& id   = pos->second.get_identifier().get_identifier();

        bool can_change             = true;
        ast::identifier id1         = sema::make_new_binder(id, m_mod, can_change);
        ast::identifier_type id_ty  = ast::identifier_type(id1->get_location(), id1);

        pos->second.set_binder(id_ty);
    };
};

class subs_visitor : public ast::rewrite_visitor<subs_visitor>
{
    private:
        using base_type     = ast::rewrite_visitor<subs_visitor>;
        using subs_map      = var_substitution::subs_map;

    private:
        const subs_map&     m_map;

    public:
        /// 
        subs_visitor(const subs_map& map)
            :m_map(map)
        {};

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        ast::type eval(const ast::identifier_type& ast)
        { 
            const ast::identifier& id   = ast.get_identifier();
            auto pos = m_map.find(ad::var_info(id));

            if (pos == m_map.end())
                return ast;

            //check if this var is free
            if (pos->second.is_bound() == false)
                return pos->second.get_type_var();
            else
                return pos->second.get_binder();
        }
};

ast::type var_substitution::apply(const ast::type& t) const
{
    if (m_map.size() == 0)
        return t;

    return subs_visitor(m_map).visit(t);
};

bool var_substitution::deduce(const ast::location& loc, const ast::type& subs_type, 
                    const ast::type& target_type, type_equaler& type_eq, bound_map& bm,
                    const context_type& ctx, bool show_errors)
{
    bool res = type_eq.deduce_subs(loc, subs_type, target_type, bm, ctx, 
                                   *this, show_errors);
    return res;
};

bool var_substitution::check_deduction(const ast::location& loc, const ast::type& ty, 
        type_equaler& eq, const context_type& ctx, const error_handler_ptr& eh, 
        bool show_errors)
{
    if (m_delays.size() == 0)
        return true;

    long num_deduced    = 0;
    bool has_visited    = false;

    for (auto& item : m_vec)
    {
        var_subs_info& info   = item->second;

        if (info.is_bound() == true)
            continue;

        bool is_set = info.get_type_var().is_set();

        if (is_set == true)
        {
            if (info.is_infered() == true)
                continue;

            num_deduced += 1;
            info.mark_infered();
            continue;
        };

        has_visited |= info.is_visited();               
    };

    if (has_visited == false)
        return process_delayed(loc, ty, eq, ctx, eh, show_errors);

    if (num_deduced == 0)
    {
        if (show_errors == false)
            return false;

        report_deduction_error(loc, ty, eh);
        return false;
    }

    return process_delayed(loc, ty, eq, ctx, eh, show_errors);
};

bool var_substitution::process_delayed(const ast::location& loc, const ast::type& root_ty,
        type_equaler& eq, const context_type& ctx, const error_handler_ptr& eh, 
        bool show_errors)
{
    delay_vec delays = m_delays;
    m_delays.clear();

    for (const auto& item : delays)
    {
        const ast::type_var& var    = std::get<0>(item);
        const ast::type& ty         = std::get<1>(item);
        bound_map* bm_loc           = std::get<2>(item);

        if (this->deduce(loc, var, ty, eq, *bm_loc, ctx, show_errors) == false)
        {
            return false;
        }
    };

    return check_deduction(loc, root_ty, eq, ctx, eh, show_errors);
};

void var_substitution::report_deduction_error(const ast::location& loc, const ast::type& ty, 
                                    const error_handler_ptr& eh) const
{
    std::vector<ast::identifier> idents;

    for (auto& item : m_vec)
    {
        var_subs_info& info   = item->second;

        if (info.is_bound() == true)
            continue;

        bool is_set = info.get_type_var().is_set();

        if (is_set == true)
            continue;

        if(info.is_visited() == false)
            continue;

        const ast::identifier& id   = info.get_identifier().get_identifier();
        idents.push_back(id);
    };

    error::type_error_msg(eh).error_deduction_failure(loc, idents, ty);
};

void var_substitution::register_type_var(const ast::type_var& tv)
{
    for (auto& item : m_vec)
    {
        if (item->second.get_type_var() == tv)
            item->second.mark_visited();
    }
};

void var_substitution::add_type_var(const ast::type_var& var, const ast::type& ty)
{
    m_delays.push_back(delay_info(var, ty, nullptr));
};

void var_substitution::finalize_normalization(bound_map& bm_in)
{
    if ((long)m_delays.size() == n_finalized_delays)
        return;

    m_contexts.push_back(bm_in);
    bound_map* bm   = &m_contexts.back();
    long n          = (long)m_delays.size();

    for (long i = n_finalized_delays; i < n; ++i)
        std::get<2>(m_delays[i]) = bm;

    n_finalized_delays = n;
};

ast::term var_substitution::make_type_application(const ast::term& t)
{
    if (m_map.size() == 0)
        return t;

    ast::term ret   = t;
    for (auto& pos : m_vec)
    {
        bool is_bound               = pos->second.is_bound();
        const ast::type_var& subs   = pos->second.get_type_var();
        
        if (is_bound == false)
        {
            ast::type type_subs     = subs.get();
            ret = ast::term_type_application(t->get_location(), ret, type_subs);
            continue;
        };

        ast::identifier_type idt    = pos->second.get_binder();

        ret = ast::term_type_application(t->get_location(), ret, idt);
    };

    return ret;
};

ast::term var_substitution::generalize(const ast::term& t) const
{
    if (m_map.size() == 0)
        return t;

    using annot_id  = ast::annotated_type_identifier;
    using type_id   = ast::identifier_type;

    ast::term ret   = t;
    long n          = (long)m_vec.size();

    for (long i = n - 1; i >= 0; --i)
    {
        bool is_bound               = m_vec[i]->second.is_bound();
        
        if (is_bound == false)
            continue;

        const ast::kind& ki         = m_vec[i]->second.get_identifier().get_kind();
        const type_id& bind         = m_vec[i]->second.get_binder();

        annot_id ait                = annot_id(bind->get_location(), bind.get_identifier(), ki);

        ret = ast::all_term(t->get_location(), ait, ret);
    };

    return ret;
};

ast::type var_substitution::generalize_type(const ast::type& t) const
{
    if (m_map.size() == 0)
        return t;

    using annot_id  = ast::annotated_type_identifier;
    using type_id   = ast::identifier_type;

    ast::type ret   = t;
    long n          = (long)m_vec.size();

    for (long i = n - 1; i >= 0; --i)
    {
        bool is_bound               = m_vec[i]->second.is_bound();
        
        if (is_bound == false)
            continue;

        const ast::kind& ki         = m_vec[i]->second.get_identifier().get_kind();
        const type_id& bind         = m_vec[i]->second.get_binder();

        annot_id ait                = annot_id(bind->get_location(), bind.get_identifier(), ki);

        ret = ast::all_type(t->get_location(), ait, ret);
    };

    return ret;
};

};};

#pragma warning(pop)