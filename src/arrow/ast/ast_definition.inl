/* 
 *  This file is a part of Hasky Lib.
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

#include "ast_common.inl"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      term_definition
//---------------------------------------------------------------
namespace details
{

class term_definition_impl : public definition_atom
{
    public:
        identifier          m_id;
        type                m_type;
        term                m_body;
        bool                m_overload;

    public:
        term_definition_impl(const location& loc, const identifier& id, 
            const type& t, const term& e, bool overload);
};

term_definition_impl::term_definition_impl(const location& loc, const identifier& id, 
    const type& t, const term& e, bool overload)
    :base_type(loc, definition_code::term_definition), m_id(id), m_type(t), m_body(e)
    , m_overload(overload)
{
    check_null(id);

    //can be null
    //check_null(t);
    //check_null(e);
};

};

term_definition::term_definition(const impl_ptr& ip)
    :base_type(ip)
{};

term_definition::term_definition()
{};

term_definition::term_definition(const location& loc, const identifier& id, 
        const type& t, const term& e, bool overload)
    :base_type(new impl_type(loc, id, t, e, overload))
{};

const identifier& term_definition::get_identifier() const
{
    return m_value->m_id;
}
const type& term_definition::get_type() const
{
    return m_value->m_type;
}
const term& term_definition::get_body() const
{
    return m_value->m_body;
}

bool term_definition::is_overload() const
{
    return m_value->m_overload;
};

//---------------------------------------------------------------
//                      term_use
//---------------------------------------------------------------
namespace details
{

class term_use_impl : public definition_atom
{
    public:
        term                m_body;

    public:
        term_use_impl(const location& loc, const term& e);
};

term_use_impl::term_use_impl(const location& loc, const term& e)
    :base_type(loc, definition_code::term_use), m_body(e)
{
    check_null(e);
};

};

term_use::term_use(const impl_ptr& ip)
    :base_type(ip)
{};

term_use::term_use()
{};

term_use::term_use(const location& loc, const term& e)
    :base_type(new impl_type(loc, e))
{};

const term& term_use::get_term() const
{
    return m_value->m_body;
}

//---------------------------------------------------------------
//                      type_definition
//---------------------------------------------------------------
namespace details
{

class type_definition_impl : public definition_atom
{
    public:
        identifier          m_id;
        kind                m_kind;
        type                m_body;
        bool                m_over;

    public:
        type_definition_impl(const location& loc, const identifier& id, 
            const kind& ki, const type& t, bool over);
};

type_definition_impl::type_definition_impl(const location& loc, const identifier& id, 
    const kind& ki, const type& t, bool over)
    :base_type(loc, definition_code::type_definition), m_id(id), m_kind(ki), m_body(t)
    ,m_over(over)
{
    check_null(id);
    if (over == false)
    {
        check_null(ki);
        check_null(t);
    };
}

}

type_definition::type_definition(const impl_ptr& ip)
    :base_type(ip)
{};

type_definition::type_definition()
{};

type_definition::type_definition(const location& loc, const identifier& id, 
        const kind& ki, const type& t, bool over)
    :base_type(new impl_type(loc, id, ki, t, over))
{};

const identifier& type_definition::get_identifier() const
{
    return m_value->m_id;
}
const kind& type_definition::get_kind() const
{
    return m_value->m_kind;
}
const type& type_definition::get_body() const
{
    return m_value->m_body;
}
bool type_definition::is_overload() const
{
    return m_value->m_over;
}

//---------------------------------------------------------------
//                      type_definition_var
//---------------------------------------------------------------
namespace details
{

class type_definition_var_impl : public definition_atom
{
    private:
        using annot_polar   = std::pair<annotated_type_identifier, polarity_type>;
        using var_vec       = std::vector<annot_polar>;
        using kind_id_vec   = std::vector<ast::identifier>;

    public:
        identifier          m_id;
        var_vec             m_vars;
        type                m_body;
        kind                m_return_kind;
        kind_id_vec         m_kind_vars;
        bool                m_over;

    public:
        type_definition_var_impl(const location& loc, const identifier& id, 
                                 const kind_id_vec& kind_vars, bool over);

        void                add_var(const annotated_type_identifier& id,
                                polarity_type pol);
};

type_definition_var_impl::type_definition_var_impl(const location& loc, const identifier& id,
                                                   const kind_id_vec& kind_vars, bool over)
    :base_type(loc, definition_code::type_definition_var), m_id(id), m_kind_vars(kind_vars)
    ,m_over(over)
{
    check_null(id);
}

void type_definition_var_impl::add_var(const annotated_type_identifier& id, polarity_type pol)
{
    m_vars.push_back(annot_polar(id, pol));
    check_null(id);
};

};

type_definition_var::type_definition_var(const impl_ptr& ip)
    :base_type(ip)
{};

type_definition_var::type_definition_var()
{};

type_definition_var::type_definition_var(const location& loc, const identifier& id, 
                                         const kind_id_vec& kind_vars, bool over)
    :base_type(new impl_type(loc, id, kind_vars, over))
{};

void type_definition_var::add_var(const annotated_type_identifier& id, polarity_type pol)
{
    m_value->add_var(id, pol);
};

void type_definition_var::set_body(const type& t)
{
    m_value->m_body = t;
    check_null(t);
};

void type_definition_var::set_return_kind(const kind& k)
{
    m_value->m_return_kind = k;
};

const identifier& type_definition_var::get_identifier() const
{
    return m_value->m_id;
}
const type& type_definition_var::get_body() const
{
    return m_value->m_body;
}
const kind& type_definition_var::get_return_kind() const
{
    return m_value->m_return_kind;
}
long type_definition_var::var_size() const
{
    return (long)m_value->m_vars.size();
}
bool type_definition_var::is_overload() const
{
    return m_value->m_over;
};

const type_definition_var::kind_id_vec& type_definition_var::get_kind_vars() const
{
    return m_value->m_kind_vars;
}

const annotated_type_identifier& type_definition_var::get_arg(long pos) const
{
    check_pos(m_value->m_vars, pos);
    return m_value->m_vars[pos].first;
}
polarity_type type_definition_var::get_polarity(long pos) const
{
    check_pos(m_value->m_vars, pos);
    return m_value->m_vars[pos].second;
}

//--------------------------------------------------------------------------
//                          fixity_definition
//--------------------------------------------------------------------------
namespace details
{
class fixity_definition_impl : public definition_atom
{
    private:
        using base_type     = definition_atom;

    private:
        operator_info       m_decl;
        symbol_sequence     m_symbols;

    public:
        fixity_definition_impl(const location& loc, const operator_info& decl,
                               const symbol_sequence& syms);

        const operator_info&    get_operator_type() const;
        const symbol_sequence&  get_symbols() const;
};

fixity_definition_impl::fixity_definition_impl(const location& loc, const operator_info& decl,
                               const symbol_sequence& syms)
    :base_type(loc, definition_code::fixity_definition), m_decl(decl), m_symbols(syms)
{};

const operator_info& fixity_definition_impl::get_operator_type() const
{
    return m_decl;
}
const symbol_sequence& fixity_definition_impl::get_symbols() const
{
    return m_symbols;
}

};

fixity_definition::fixity_definition()
{};

fixity_definition::fixity_definition(const impl_ptr& ip)
    :base_type(ip)
{};

fixity_definition::fixity_definition(const location& loc, const ast::operator_info& p,
                          const symbol_sequence& syms)
    :base_type(new impl_type(loc, p, syms))
{};

const operator_info& fixity_definition::get_operator_type() const
{
    return m_value->get_operator_type();
}

const symbol_sequence& fixity_definition::get_symbols() const
{
    return m_value->get_symbols();
}

//---------------------------------------------------------------
//                      definition
//---------------------------------------------------------------
definition::definition(const impl_ptr& ip)
    :base_type(ip)
{};

definition::definition()
{};

definition::definition(const term_definition& def)
    :base_type(def.m_value)
{
    check_null(def);
}

definition::definition(const fixity_definition& def)
    :base_type(def.m_value)
{
    check_null(def);
}

definition::definition(const type_definition& def)
    :base_type(def.m_value)
{
    check_null(def);
}

definition::definition(const type_definition_var& def)
    :base_type(def.m_value)
{
    check_null(def);
}

definition::definition(const term_use& def)
    :base_type(def.m_value)
{
    check_null(def);
}

template<class Ast>
Ast definition::static_cast_to() const
{
    using ref_ptr   = typename Ast::impl_ptr;

    ref_ptr ret = m_value.static_cast_to<ref_ptr>();

    return Ast(ret);
};

template term_definition definition::static_cast_to<term_definition>() const;
template fixity_definition definition::static_cast_to<fixity_definition>() const;
template type_definition definition::static_cast_to<type_definition>() const;
template type_definition_var definition::static_cast_to<type_definition_var>() const;
template term_use definition::static_cast_to<term_use>() const;

};};
