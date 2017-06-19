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

#include "ast_common.inl"
#include "scopes/local_scope.h"
#include "scopes/module.h"
#include "error/error.h"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      variant_case_term
//---------------------------------------------------------------
namespace details
{

class variant_case_term_impl : public term_atom
{
    private:
        using elem_type         = std::pair<annotated_identifier, term>;
        using elem_vec          = std::vector<elem_type>;
        using field_map         = std::map<unique_string, elem_type>;

    public:
        term                    m_case;
        elem_vec                m_data;
        field_map               m_map;
        mutable symbol_data     m_symbol_data;

    public:
        variant_case_term_impl(const location& loc, const symbol_data* sd);
        variant_case_term_impl(const location& loc, const symbol_data& sd);

        void            add_pattern(const annotated_identifier& id, const term& ex);
        bool            has_label(const identifier& label) const;
        term            get_label_bind(const identifier& label) const;
        annotated_identifier 
                        get_label(const ast::identifier& id) const;
};

variant_case_term_impl::variant_case_term_impl(const location& loc, const symbol_data* sd)
    : base_type(loc, term_code::variant_case_term), m_symbol_data(*sd)
{};
variant_case_term_impl::variant_case_term_impl(const location& loc, const symbol_data& sd)
    : base_type(loc, term_code::variant_case_term), m_symbol_data(sd)
{};

void variant_case_term_impl::add_pattern(const annotated_identifier& id, const term& ex)
{
    m_data.push_back(elem_type(id, ex));
    check_null(id);
    check_null(ex);

    ast::fixity_type ft;
    m_map[id.get_identifier().get_name(ft)] = elem_type(id, ex);
};

bool variant_case_term_impl::has_label(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

term variant_case_term_impl::get_label_bind(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return term();
    else
        return pos->second.second;
};

annotated_identifier variant_case_term_impl::get_label(const ast::identifier& id) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(id.get_name(ft));
    if (pos == m_map.end())
        return annotated_identifier();
    else
        return pos->second.first;
};

};

variant_case_term::variant_case_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

variant_case_term::variant_case_term()
{};

variant_case_term::variant_case_term(const location& loc, const symbol_data* sd)
    :base_type(new impl_type(loc, sd))
{}
variant_case_term::variant_case_term(const location& loc, fresh)
    :base_type(new impl_type(loc, symbol_data::make_new()))
{}

void variant_case_term::set_term(const term& case_term)
{
    m_value->m_case = case_term;
    check_null(case_term);
}
void variant_case_term::add_pattern(const annotated_identifier& lab, 
                        const term& ex)
{
    return m_value->add_pattern(lab, ex);
}

bool variant_case_term::has_label(const ast::identifier& id) const
{
    return m_value->has_label(id);
}
term variant_case_term::get_label_bind(const ast::identifier& id) const
{
    return m_value->get_label_bind(id);
}
annotated_identifier variant_case_term::get_label(const ast::identifier& id) const
{
    return m_value->get_label(id);
}

const term& variant_case_term::get_term() const
{
    return m_value->m_case;
}
long variant_case_term::size() const
{
    return (long)m_value->m_data.size();
}

const annotated_identifier& variant_case_term::get_label(long pos) const
{
    check_pos(m_value->m_data, pos);
    return m_value->m_data[pos].first;
}
const term& variant_case_term::get_bind(long pos) const
{
    check_pos(m_value->m_data, pos);
    return m_value->m_data[pos].second;
};

symbol_data* variant_case_term::get_symbol_data() const
{
    return &m_value->m_symbol_data;
}

void variant_case_term::update_scope(const module& mod) const
{
    local_scope ls          = mod.get_scope_for_case(*this);
    long n                  = this->size();
    for (long i = 0; i < n; ++i)
    {
        const identifier& id    = get_label(i).get_identifier();
        const type& ty          = get_label(i).get_type();

        ls.update_pattern_var(id, ty);
    };
};

//---------------------------------------------------------------
//                      variant_term
//---------------------------------------------------------------
namespace details
{

class variant_term_impl : public term_atom
{
    public:
        type                    m_type;
        term                    m_term;
        annotated_identifier    m_label;

    public:
        variant_term_impl(const location& loc, const type& ty, const annotated_identifier& id,
                     const term& ex);
};

variant_term_impl::variant_term_impl(const location& loc, const type& ty, const annotated_identifier& id,
                     const term& ex)
    :base_type(loc, term_code::variant_term), m_type(ty), m_term(ex), m_label(id)
{
    check_null(ty);
    check_null(ex);
    check_null(id);
};

};

variant_term::variant_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

variant_term::variant_term()
{};

variant_term::variant_term(const location& loc, const type& ty, const ast::annotated_identifier& id,
                     const term& ex)
    :base_type(new impl_type(loc, ty, id, ex))
{};

const type& variant_term::get_type() const
{
    return m_value->m_type;
}
const term& variant_term::get_term() const
{
    return m_value->m_term;
}

const annotated_identifier& variant_term::get_label() const
{
    return m_value->m_label;
}

//---------------------------------------------------------------
//                      pack_term
//---------------------------------------------------------------
namespace details
{

class pack_term_impl : public term_atom
{
    public:
        annotated_type_identifier
                    m_id;
        term        m_body;
        type        m_id_type;
        type        m_some_type;

    public:
        pack_term_impl(const location& loc, const annotated_type_identifier& id, const type& t, 
                  const term& e, const type& some_type);
};

pack_term_impl::pack_term_impl(const location& loc, const annotated_type_identifier& id, const type& t, 
                  const term& e, const type& some_type)
    :base_type(loc, term_code::pack_term), m_id(id), m_body(e), m_id_type(t), m_some_type(some_type)
{
    check_null(t);
    check_null(e);
    check_null(id);
};

};

pack_term::pack_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

pack_term::pack_term()
{};

pack_term::pack_term(const location& loc, const annotated_type_identifier& id, const type& t, 
                  const term& e, const type& some_type)
    :base_type(new impl_type(loc, id, t, e, some_type))
{};

const annotated_type_identifier& pack_term::get_var() const
{
    return m_value->m_id;
}
const term& pack_term::get_body() const
{
    return m_value->m_body;
}
const type& pack_term::get_var_type() const
{
    return m_value->m_id_type;
}
const type& pack_term::get_some_type() const
{
    return m_value->m_some_type;
}

//---------------------------------------------------------------
//                      path_term
//---------------------------------------------------------------
namespace details
{

class path_term_impl : public term_atom
{
    public:
        term        m_term;
        identifier  m_id;

    public:
        path_term_impl(const location& loc, const term& x, const identifier& id);
};

path_term_impl::path_term_impl(const location& loc, const term& x, const identifier& id)
    :base_type(loc, term_code::path_term), m_term(x), m_id(id)
{
    check_null(x);
    check_null(id);
};

};

path_term::path_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

path_term::path_term()
{};

path_term::path_term(const location& loc, const term& x, const identifier& id)
    :base_type(new impl_type(loc, x, id))
{};

const term& path_term::get_term() const
{
    return m_value->m_term;
}
const identifier& path_term::get_identifier() const
{
    return m_value->m_id;
}

//---------------------------------------------------------------
//                      path_int_term
//---------------------------------------------------------------
namespace details
{

class path_int_term_impl : public term_atom
{
    public:
        term            m_term;
        int_literal     m_index;

    public:
        path_int_term_impl(const location& loc, const term& x, const int_literal& pos);
};

path_int_term_impl::path_int_term_impl(const location& loc, const term& x, const int_literal& pos)
    :base_type(loc, term_code::path_int_term), m_term(x), m_index(pos)
{
    check_null(x);
    check_null(pos);
};

};

path_int_term::path_int_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

path_int_term::path_int_term()
{};

path_int_term::path_int_term(const location& loc, const term& x, const int_literal& pos)
    :base_type(new impl_type(loc, x, pos))
{};

const term& path_int_term::get_term() const
{
    return m_value->m_term;
}
const int_literal& path_int_term::get_index() const
{
    return m_value->m_index;
}

//---------------------------------------------------------------
//                      let_term
//---------------------------------------------------------------
namespace details
{

class let_term_impl : public term_atom
{
    public:
        definitions         m_defs;
        term                m_body;
        mutable symbol_data m_symbol_data;

    public:
        let_term_impl(const location& loc, const definitions& defs, const term& body,
                      const symbol_data* sd);
        let_term_impl(const location& loc, const definitions& defs, const term& body,
                      const symbol_data& sd);
};

let_term_impl::let_term_impl(const location& loc, const definitions& defs, 
                const term& body, const symbol_data* sd)
    :base_type(loc, term_code::let_term), m_defs(defs), m_body(body)
    ,m_symbol_data(*sd)
{
    check_null(defs);
    check_null(body);
};

let_term_impl::let_term_impl(const location& loc, const definitions& defs, 
                const term& body, const symbol_data& sd)
    :base_type(loc, term_code::let_term), m_defs(defs), m_body(body)
    ,m_symbol_data(sd)
{
    check_null(defs);
    check_null(body);
};

};

let_term::let_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

let_term::let_term()
{};

let_term::let_term(const location& loc, const definitions& defs, const term& body,
                   const symbol_data* sd)
    :base_type(new impl_type(loc, defs, body, sd))
{};
let_term::let_term(const location& loc, const definitions& defs, const term& body,
                   fresh)
    :base_type(new impl_type(loc, defs, body, symbol_data::make_new()))
{};

const term& let_term::get_body() const
{
    return m_value->m_body;
}
const definitions& let_term::get_definitions() const
{
    return m_value->m_defs;
}
symbol_data* let_term::get_symbol_data() const
{
    return &m_value->m_symbol_data;
}

void let_term::update_scope(const module& mod) const
{
    local_scope ls          = mod.get_scope_for_let(*this);
    ls.update_let_definitions(get_definitions());
};

//---------------------------------------------------------------
//                      term_application
//---------------------------------------------------------------
namespace details
{

class term_application_impl : public term_atom
{
    public:
        term                    m_lhs;
        term                    m_rhs;

    public:
        term_application_impl(const location& loc, const term& t1, const term& t2);
};

term_application_impl::term_application_impl(const location& loc, const term& t1, const term& t2)
    :base_type(loc, term_code::term_application), m_lhs(t1), m_rhs(t2)
{
    check_null(t1);
    check_null(t2);
};

};

term_application::term_application(const impl_ptr& ptr)
    :base_type(ptr)
{};

term_application::term_application()
{};

term_application::term_application(const location& loc, const term& t1, const term& t2)
    :base_type(new impl_type(loc, t1, t2))
{};

const term& term_application::get_lhs() const
{
    return m_value->m_lhs;
}
const term& term_application::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      if_term
//---------------------------------------------------------------
namespace details
{

class if_term_impl : public term_atom
{
    public:
        term                    m_if;
        term                    m_then;
        term                    m_else;

    public:
        if_term_impl(const location& loc, const term& t_if, const term& t_then, 
                    const term& t_else);
};

if_term_impl::if_term_impl(const location& loc, const term& t_if, const term& t_then, 
            const term& t_else)
    :base_type(loc, term_code::if_term), m_if(t_if), m_then(t_then), m_else(t_else)
{
    check_null(t_if);
    check_null(t_then);
    check_null(t_else);
};

};

if_term::if_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

if_term::if_term()
{};

if_term::if_term(const location& loc, const term& t_if, const term& t_then, 
        const term& t_else)
    :base_type(new impl_type(loc, t_if, t_then, t_else))
{};

const term& if_term::get_if() const
{
    return m_value->m_if;
}
const term& if_term::get_then() const
{
    return m_value->m_then;
}
const term& if_term::get_else() const
{
    return m_value->m_else;
}

//---------------------------------------------------------------
//                      annotated_term
//---------------------------------------------------------------
namespace details
{

class annotated_term_impl : public term_atom
{
    public:
        term                    m_term;
        type                    m_type;

    public:
        annotated_term_impl(const location& loc, const term& x, const type& t);
};

annotated_term_impl::annotated_term_impl(const location& loc, const term& x, const type& t)
    :base_type(loc, term_code::annotated_term), m_term(x), m_type(t)
{
    check_null(x);
    check_null(t);
};

};

annotated_term::annotated_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

annotated_term::annotated_term()
{};

annotated_term::annotated_term(const location& loc, const term& x, const type& t)
    :base_type(new impl_type(loc, x, t))
{};

const term& annotated_term::get_term() const
{
    return m_value->m_term;
}
const type& annotated_term::get_type() const
{
    return m_value->m_type;
}

//---------------------------------------------------------------
//                      val_term
//---------------------------------------------------------------
namespace details
{

class val_term_impl : public term_atom
{
    public:
        type                    m_type;

    public:
        val_term_impl(const location& loc, const type& x);
};

val_term_impl::val_term_impl(const location& loc, const type& x)
    :term_atom(loc, term_code::val_term), m_type(x)
{
    check_null(x);
};

};

val_term::val_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

val_term::val_term()
{};

val_term::val_term(const location& loc, const type& t)
    :base_type(new impl_type(loc, t))
{};

const type& val_term::get_type() const
{
    return m_value->m_type;
}

//---------------------------------------------------------------
//                      ref_term
//---------------------------------------------------------------
namespace details
{

class ref_term_impl : public term_atom
{
    public:
        term                    m_term;

    public:
        ref_term_impl(const location& loc, const term& x);
};

ref_term_impl::ref_term_impl(const location& loc, const term& x)
    :base_type(loc, term_code::ref_term), m_term(x)
{
    check_null(x);
};

};

ref_term::ref_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

ref_term::ref_term()
{};

ref_term::ref_term(const location& loc, const term& t)
    :base_type(new impl_type(loc, t))
{};

const term& ref_term::get_term() const
{
    return m_value->m_term;
}

//---------------------------------------------------------------
//                      assign_term
//---------------------------------------------------------------
namespace details
{

class assign_term_impl : public term_atom
{
    public:
        term                    m_lhs;
        term                    m_rhs;

    public:
        assign_term_impl(const location& loc, const term& lhs, const term& rhs);
};

assign_term_impl::assign_term_impl(const location& loc, const term& lhs, const term& rhs)
    :base_type(loc, term_code::assign_term), m_lhs(lhs), m_rhs(rhs)
{
    check_null(lhs);
    check_null(rhs);
};

};

assign_term::assign_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

assign_term::assign_term()
{};

assign_term::assign_term(const location& loc, const term& lhs, const term& rhs)
    :base_type(new impl_type(loc, lhs, rhs))
{};

const term& assign_term::get_lhs() const
{
    return m_value->m_lhs;
}

const term& assign_term::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      deref_term
//---------------------------------------------------------------
namespace details
{

class deref_term_impl : public term_atom
{
    public:
        term                    m_term;

    public:
        deref_term_impl(const location& loc, const term& x);
};

deref_term_impl::deref_term_impl(const location& loc, const term& x)
    :base_type(loc, term_code::deref_term), m_term(x)
{
    check_null(x);
};

};

deref_term::deref_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

deref_term::deref_term()
{};

deref_term::deref_term(const location& loc, const term& t)
    :base_type(new impl_type(loc, t))
{};

const term& deref_term::get_term() const
{
    return m_value->m_term;
}

//---------------------------------------------------------------
//                      cell_term
//---------------------------------------------------------------
namespace details
{

class cell_term_impl : public term_atom
{
    public:
        long            m_index;

    public:
        cell_term_impl(const location& loc, long index);
};

cell_term_impl::cell_term_impl(const location& loc, long index)
    :base_type(loc, term_code::cell_term), m_index(index)
{};

};

cell_term::cell_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

cell_term::cell_term()
{};

cell_term::cell_term(const location& loc, long index)
    :base_type(new impl_type(loc, index))
{};

long cell_term::get_index() const
{
    return m_value->m_index;
}

//---------------------------------------------------------------
//                      identifier_term
//---------------------------------------------------------------
namespace details
{

class identifier_term_impl : public term_atom
{
    public:
        identifier      m_id;

    public:
        identifier_term_impl(const location& loc, const identifier& t);
};

identifier_term_impl::identifier_term_impl(const location& loc, const identifier& id)
    :base_type(loc, term_code::identifier_term), m_id(id)
{
    check_null(id);
};

};

identifier_term::identifier_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

identifier_term::identifier_term()
{};

identifier_term::identifier_term(const location& loc, const identifier& t)
    :base_type(new impl_type(loc, t))
{};

const identifier& identifier_term::get_identifier() const
{
    return m_value->m_id;
}

//---------------------------------------------------------------
//                      overloaded_symbol
//---------------------------------------------------------------
namespace details
{

struct applic_arg
{
    using arg_class = overloaded_symbol::arg_class;
    type            m_type;
    kind            m_kind;
    arg_class       m_arg_class;

    applic_arg(const ast::type& ty, arg_class arg_cl);
    applic_arg(const ast::kind& ty);
};

class overloaded_symbol_impl : public term_atom
{
    public:
        using arg_vec   = std::vector<applic_arg>;
        using arg_class = overloaded_symbol::arg_class;

    public:
        identifier_term m_id;
        arg_vec         m_args;
        type            m_type;
        type            m_return;
        overload_index  m_index;

    public:
        overloaded_symbol_impl(const location& loc, const identifier_term& id, 
                const type& ty, const ast::type& ret_ty, const overload_index* ind);

        const applic_arg&   get(long pos) const;
};

applic_arg::applic_arg(const ast::type& ty, arg_class arg_cl)
    :m_type(ty), m_arg_class(arg_cl)
{};
applic_arg::applic_arg(const ast::kind& ty)
    :m_kind(ty), m_arg_class(arg_class::class_kind)
{};

overloaded_symbol_impl::overloaded_symbol_impl(const location& loc, const identifier_term& id,
                                const type& ty, const type& ret_ty, const overload_index* ind)
    :base_type(loc, term_code::overloaded_symbol), m_id(id), m_type(ty), m_return(ret_ty)
    , m_index(ind ? *ind : overload_index())
{
    check_null(id);
    check_null(ty);
    check_null(ret_ty);
};

const applic_arg& overloaded_symbol_impl::get(long pos) const
{
    check_pos(m_args, pos);
    return m_args[pos];
};

};

overloaded_symbol::overloaded_symbol(const impl_ptr& ptr)
    :base_type(ptr)
{};

overloaded_symbol::overloaded_symbol()
{};

overloaded_symbol::overloaded_symbol(const location& loc, const identifier_term& id,
                        const type& ty, const ast::type& ret_ty, const overload_index* ind)
    :base_type(new impl_type(loc, id, ty, ret_ty, ind))
{};

const ast::type& overloaded_symbol::get_return_type() const
{
    return m_value->m_return;
};

const identifier_term& overloaded_symbol::get_identifier() const
{
    return m_value->m_id;
}

const overload_index* overloaded_symbol::get_index() const
{
    return &m_value->m_index;
};

const type& overloaded_symbol::get_type() const
{
    return m_value->m_type;
}

void overloaded_symbol::add_term_arg(const type& ty)
{
    m_value->m_args.push_back(details::applic_arg(ty, arg_class::class_term));
};

void overloaded_symbol::add_type_arg(const type& ty)
{
    m_value->m_args.push_back(details::applic_arg(ty, arg_class::class_type));
};
void overloaded_symbol::add_kind_arg(const kind& ty)
{
    m_value->m_args.push_back(details::applic_arg(ty));
};

long overloaded_symbol::arg_size() const
{
    return (long)m_value->m_args.size();
};

overloaded_symbol::arg_class overloaded_symbol::get_arg_class(long pos) const
{
    return m_value->get(pos).m_arg_class;
}

const ast::type& overloaded_symbol::get_arg_type(long pos) const
{
    return m_value->get(pos).m_type;
};
const ast::kind& overloaded_symbol::get_arg_kind(long pos) const
{
    return m_value->get(pos).m_kind;
};

//---------------------------------------------------------------
//                      unpack_term
//---------------------------------------------------------------
namespace details
{

class unpack_term_impl : public term_atom
{
    public:
        term                        m_package;
        annotated_type_identifier   m_type_id;
        annotated_identifier        m_term_id;
        term                        m_term;
        mutable symbol_data         m_symbol_data;

    public:
        unpack_term_impl(const location& loc, const term& package, 
                const annotated_type_identifier& type_id, const annotated_identifier& term_id, 
                const term& expr, const symbol_data* sd);
        unpack_term_impl(const location& loc, const term& package, 
                const annotated_type_identifier& type_id, const annotated_identifier& term_id, 
                const term& expr, const symbol_data& sd);
};

unpack_term_impl::unpack_term_impl(const location& loc, const term& package, 
                    const annotated_type_identifier& type_id, 
                    const annotated_identifier& term_id, const term& expr, const symbol_data* sd)
    :base_type(loc, term_code::unpack_term), m_package(package), m_type_id(type_id)
    , m_term_id(term_id), m_term(expr), m_symbol_data(*sd)
{
    check_null(package);
    check_null(type_id);
    check_null(term_id);
    check_null(expr);
};
unpack_term_impl::unpack_term_impl(const location& loc, const term& package, 
                    const annotated_type_identifier& type_id, 
                    const annotated_identifier& term_id, const term& expr, const symbol_data& sd)
    :base_type(loc, term_code::unpack_term), m_package(package), m_type_id(type_id)
    , m_term_id(term_id), m_term(expr), m_symbol_data(sd)
{
    check_null(package);
    check_null(type_id);
    check_null(term_id);
    check_null(expr);
};

};

unpack_term::unpack_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

unpack_term::unpack_term()
{};

unpack_term::unpack_term(const location& loc, const term& package, 
            const annotated_type_identifier& type_id, const annotated_identifier& term_id, 
            const term& expr, const symbol_data* sd)
    :base_type(new impl_type(loc, package, type_id, term_id, expr, sd))
{};

unpack_term::unpack_term(const location& loc, const term& package, const annotated_type_identifier& type_id, 
            const annotated_identifier& term_id, const term& expr, fresh)
    :base_type(new impl_type(loc, package, type_id, term_id, expr, symbol_data::make_new()))
{};

const term& unpack_term::get_package() const
{
    return m_value->m_package;
}
const term& unpack_term::get_term() const
{
    return m_value->m_term;
}
const annotated_type_identifier& unpack_term::get_type_identifier() const
{
    return m_value->m_type_id;
}
const annotated_identifier& unpack_term::get_term_identifier() const
{
    return m_value->m_term_id;
}
symbol_data* unpack_term::get_symbol_data() const
{
    return &m_value->m_symbol_data;
}

void unpack_term::update_scope(const module& mod) const
{
    local_scope ls          = mod.get_scope_for_unpack(*this);
    const identifier& id    = get_term_identifier().get_identifier();
    const type& ty          = get_term_identifier().get_type();

    ls.update_pattern_var(id, ty);
};

//---------------------------------------------------------------
//                      int_literal
//---------------------------------------------------------------
namespace details
{

class int_literal_impl : public term_atom
{
    public:
        int64_t     m_value;

    public:
        int_literal_impl(const location& loc, int64_t val);
};

int_literal_impl::int_literal_impl(const location& loc, int64_t val)
    :base_type(loc, term_code::int_literal), m_value(val)
{};

};

int_literal::int_literal(const impl_ptr& ptr)
    :base_type(ptr)
{};

int_literal::int_literal()
{};

int_literal::int_literal(const location& loc, int64_t val)
    :base_type(new impl_type(loc, val))
{};

int64_t int_literal::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      bool_literal
//---------------------------------------------------------------
namespace details
{

class bool_literal_impl : public term_atom
{
    public:
        bool        m_value;

    public:
        bool_literal_impl(const location& loc, bool val);
};

bool_literal_impl::bool_literal_impl(const location& loc, bool val)
    :base_type(loc, term_code::bool_literal), m_value(val)
{};

};

bool_literal::bool_literal(const impl_ptr& ptr)
    :base_type(ptr)
{};

bool_literal::bool_literal()
{};

bool_literal::bool_literal(const location& loc, bool val)
    :base_type(new impl_type(loc, val))
{};

bool bool_literal::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      float_literal
//---------------------------------------------------------------
namespace details
{

class float_literal_impl : public term_atom
{
    public:
        double      m_value;

    public:
        float_literal_impl(const location& loc, double val);
};

float_literal_impl::float_literal_impl(const location& loc, double val)
    :base_type(loc, term_code::float_literal), m_value(val)
{};

};

float_literal::float_literal(const impl_ptr& ptr)
    :base_type(ptr)
{};

float_literal::float_literal()
{};

float_literal::float_literal(const location& loc, double val)
    :base_type(new impl_type(loc, val))
{};

double float_literal::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      char_literal
//---------------------------------------------------------------
namespace details
{

class char_literal_impl : public term_atom
{
    public:
        int32_t         m_value;

    public:
        char_literal_impl(const location& loc, int32_t val);
};

char_literal_impl::char_literal_impl(const location& loc, int32_t val)
    :base_type(loc, term_code::char_literal), m_value(val)
{};

};

char_literal::char_literal(const impl_ptr& ptr)
    :base_type(ptr)
{};

char_literal::char_literal()
{};

char_literal::char_literal(const location& loc, int32_t val)
    :base_type(new impl_type(loc, val))
{};

int32_t char_literal::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      string_literal
//---------------------------------------------------------------
namespace details
{

class string_literal_impl : public term_atom
{
    public:
        std::string     m_value;

    public:
        string_literal_impl(const location& loc, const std::string& val);
};

string_literal_impl::string_literal_impl(const location& loc, const std::string& val)
    :base_type(loc, term_code::string_literal), m_value(val)
{};

};

string_literal::string_literal(const impl_ptr& ptr)
    :base_type(ptr)
{};

string_literal::string_literal()
{};

string_literal::string_literal(const location& loc, const std::string& val)
    :base_type(new impl_type(loc, val))
{};

const std::string& string_literal::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      error_term
//---------------------------------------------------------------
namespace details
{

class error_term_impl : public term_atom
{
    public:
        error_term_impl(const location& loc);
};

error_term_impl::error_term_impl(const location& loc)
    :base_type(loc, term_code::error_term)
{};

};

error_term::error_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

error_term::error_term()
{};

error_term::error_term(const location& loc)
    :base_type(new impl_type(loc))
{};

//---------------------------------------------------------------
//                      tuple_term
//---------------------------------------------------------------
namespace details
{

class tuple_term_impl : public term_atom
{
    private:
        using elem_vec      = std::vector<term>;
        using tuple_state   = tuple_term::tuple_state;

    public:
        elem_vec        m_elems;
        tuple_state     m_type;

    public:
        tuple_term_impl(const location& loc, tuple_state type);

        void            add(const term& t);
};

tuple_term_impl::tuple_term_impl(const location& loc, tuple_state ty)
    :base_type(loc, term_code::tuple_term), m_type(ty)
{};

void tuple_term_impl::add(const term& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

tuple_term::tuple_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

tuple_term::tuple_term()
{};

tuple_term::tuple_term(const location& loc, tuple_state ty)
    :base_type(new impl_type(loc, ty))
{};

void tuple_term::add(const term& t)
{
    m_value->add(t);
}
long tuple_term::size() const
{
    return (long)m_value->m_elems.size();
}
const term& tuple_term::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}
tuple_term::tuple_state tuple_term::get_state() const
{
    return m_value->m_type;
};

void tuple_term::set_state(tuple_state ty)
{
    m_value->m_type = ty;
};

//---------------------------------------------------------------
//                      term_sequence_item
//---------------------------------------------------------------
term_sequence_item::term_sequence_item(const location& loc, const term& item, basic_tag)
    :m_type(item_type::basic_item), m_term(item), m_loc(loc)
{
    check_null(item);
};

term_sequence_item::term_sequence_item(const location& loc, const int_literal& item, proj_tag)
    :m_type(item_type::proj_item), m_int_lit(item), m_loc(loc)
{
    check_null(item);
};

term_sequence_item::term_sequence_item(const location& loc, const identifier& item, dot_tag)
    :m_type(item_type::dot_item), m_ident(item), m_loc(loc)
{
    check_null(item);
};

term_sequence_item::term_sequence_item(const location& loc, const type_vec& item, type_applic_tag)
    :m_type(item_type::type_applic_item), m_type_args(item), m_loc(loc)
{
    for(const auto& elem: item)
        check_null(elem);
};

term_sequence_item::term_sequence_item(const location& loc, const kind_vec& item, kind_applic_tag)
    :m_type(item_type::kind_applic_item), m_kind_args(item), m_loc(loc)
{
    for(const auto& elem: item)
        check_null(elem);
};

term_sequence_item::item_type term_sequence_item::get_item_type() const
{
    return m_type;
};

const location& term_sequence_item::get_location() const
{
    return m_loc;
};

const term& term_sequence_item::get_basic() const
{
    return m_term;
}
const int_literal& term_sequence_item::get_projection() const
{
    return m_int_lit;
};
const identifier& term_sequence_item::get_dot() const
{
    return m_ident;
};
const term_sequence_item::type_vec& term_sequence_item::get_type_applic_args() const
{
    return m_type_args;
};
const term_sequence_item::kind_vec& term_sequence_item::get_kind_applic_args() const
{
    return m_kind_args;
};

//---------------------------------------------------------------
//                      term_sequence
//---------------------------------------------------------------
namespace details
{

class term_sequence_impl : public term_atom
{
    private:
        using elem_vec  = std::vector<term_sequence_item>;

    public:
        elem_vec        m_elems;

    public:
        term_sequence_impl(const location& loc);

        void            add(const term_sequence_item& t);
};

term_sequence_impl::term_sequence_impl(const location& loc)
    :base_type(loc, term_code::term_sequence)
{};

void term_sequence_impl::add(const term_sequence_item& t)
{
    m_elems.push_back(t);
}

};

term_sequence::term_sequence(const impl_ptr& ptr)
    :base_type(ptr)
{};

term_sequence::term_sequence()
{};

term_sequence::term_sequence(const location& loc)
    :base_type(new impl_type(loc))
{};

void term_sequence::add(const location& loc, const term& t)
{
    m_value->add(term_sequence_item(loc, t, term_sequence_item::basic_tag()));
}
void term_sequence::add_projection(const location& loc, const ast::int_literal& pos)
{
    m_value->add(term_sequence_item(loc, pos, term_sequence_item::proj_tag()));
}
void term_sequence::add_dot(const location& loc, const ast::identifier& id)
{
    m_value->add(term_sequence_item(loc, id, term_sequence_item::dot_tag()));
}
void term_sequence::add_type_application(const location& loc, const std::vector<ast::type>& args)
{
    m_value->add(term_sequence_item(loc, args, term_sequence_item::type_applic_tag()));
}
void term_sequence::add_kind_application(const location& loc, const std::vector<ast::kind>& args)
{
    m_value->add(term_sequence_item(loc, args, term_sequence_item::kind_applic_tag()));
};

long term_sequence::size() const
{
    return (long)m_value->m_elems.size();
}
const term_sequence_item& term_sequence::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

//---------------------------------------------------------------
//                      list_term
//---------------------------------------------------------------
namespace details
{

class list_term_impl : public term_atom
{
    private:
        using elem_vec  = std::vector<term>;

    public:
        elem_vec        m_elems;

    public:
        list_term_impl(const location& loc);

        void            add(const term& t);
};

list_term_impl::list_term_impl(const location& loc)
    :base_type(loc, term_code::list_term)
{};

void list_term_impl::add(const term& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

list_term::list_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

list_term::list_term()
{};

list_term::list_term(const location& loc)
    :base_type(new impl_type(loc))
{};

void list_term::add(const term& t)
{
    m_value->add(t);
}
long list_term::size() const
{
    return (long)m_value->m_elems.size();
}
const term& list_term::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

//---------------------------------------------------------------
//                      bracket_term
//---------------------------------------------------------------
namespace details
{

class bracket_term_impl : public term_atom
{
    private:
        using elem_vec  = std::vector<term>;

    public:
        elem_vec        m_elems;
        identifier      m_opening;
        identifier      m_closing;
        bool            m_is_dot;

    public:
        bracket_term_impl(const location& loc);

        void            add(const term& t);
};

bracket_term_impl::bracket_term_impl(const location& loc)
    :base_type(loc, term_code::bracket_term), m_is_dot(false)
{};

void bracket_term_impl::add(const term& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

bracket_term::bracket_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

bracket_term::bracket_term()
{};

bracket_term::bracket_term(const location& loc)
    :base_type(new impl_type(loc))
{};

void bracket_term::set_dot(bool is_dot)
{
    m_value->m_is_dot   = is_dot;
};
void bracket_term::set_brackets(bool is_dot, const identifier& open, const identifier& close)
{
    m_value->m_opening  = open;
    m_value->m_closing  = close;
    m_value->m_is_dot   = is_dot;
    check_null(open);
    check_null(close);
};
void bracket_term::add(const term& t)
{
    m_value->add(t);
}
long bracket_term::size() const
{
    return (long)m_value->m_elems.size();
}
const term& bracket_term::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

const identifier& bracket_term::get_opening_bracket() const
{
    return m_value->m_opening;
}
const identifier& bracket_term::get_closing_bracket() const
{
    return m_value->m_closing;
}
bool bracket_term::is_dot() const
{
    return m_value->m_is_dot;
};

//---------------------------------------------------------------
//                      record_term
//---------------------------------------------------------------
namespace details
{

class record_term_impl : public term_atom
{
    private:
        using lab_term  = std::pair<annotated_identifier, term>;
        using elem_vec  = std::vector<lab_term>;
        using field_map = std::map<unique_string, lab_term>;

    public:
        elem_vec        m_elems;
        field_map       m_map;

    public:
        record_term_impl(const location& loc);

        void            add(const annotated_identifier& label, const term& t);

        bool            has_field(const identifier& label) const;
        term            get_field(const identifier& label) const;
        annotated_identifier 
                        get_identifier(const identifier& label) const;
};

record_term_impl::record_term_impl(const location& loc)
    :base_type(loc, term_code::record_term)
{};

void record_term_impl::add(const annotated_identifier& label, const term& t)
{
    m_elems.push_back(lab_term(label,t));
    check_null(t);
    check_null(label);

    ast::fixity_type ft;
    m_map[label.get_identifier().get_name(ft)] = lab_term(label, t);
}

bool record_term_impl::has_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

term record_term_impl::get_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return term();
    else
        return pos->second.second;
};

annotated_identifier record_term_impl::get_identifier(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return annotated_identifier();
    else
        return pos->second.first;
};

};

record_term::record_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

record_term::record_term()
{};

record_term::record_term(const location& loc)
    :base_type(new impl_type(loc))
{};

void record_term::add(const annotated_identifier& label, const term& t)
{
    m_value->add(label, t);
}

long record_term::size() const
{
    return (long)m_value->m_elems.size();
}
const term& record_term::get_term(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos].second;
}
const annotated_identifier& record_term::get_label(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos].first;
}

bool record_term::has_field(const identifier& label) const
{
    return m_value->has_field(label);
}
term record_term::get_field(const identifier& label) const
{
    return m_value->get_field(label);
}
annotated_identifier record_term::get_identifier(const identifier& label) const
{
    return m_value->get_identifier(label);
}

//---------------------------------------------------------------
//                      lambda_term
//---------------------------------------------------------------
namespace details
{

class lambda_term_impl : public term_atom
{        
    public:
        annotated_identifier m_var;
        term                m_body;
        mutable symbol_data m_symbol_data;

    public:
        lambda_term_impl(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data* sd);
        lambda_term_impl(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data& sd);
};

lambda_term_impl::lambda_term_impl(const location& loc, const annotated_identifier& id, 
                        const term& t, const symbol_data* sd)
    :base_type(loc, term_code::lambda_term), m_var(id), m_body(t), m_symbol_data(*sd)
{
    check_null(t);
    check_null(id);
};

lambda_term_impl::lambda_term_impl(const location& loc, const annotated_identifier& id, 
                        const term& t, const symbol_data& sd)
    :base_type(loc, term_code::lambda_term), m_var(id), m_body(t), m_symbol_data(sd)
{
    check_null(t);
    check_null(id);
};

};

lambda_term::lambda_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

lambda_term::lambda_term()
{};

lambda_term::lambda_term(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data* sd)
    :base_type(new impl_type(loc, id, t, sd))
{};

lambda_term::lambda_term(const location& loc, const annotated_identifier& id, const term& t,
                         fresh)
    :base_type(new impl_type(loc, id, t, symbol_data::make_new()))
{};

const term& lambda_term::get_body() const
{
    return m_value->m_body;
}
const annotated_identifier& lambda_term::get_var() const
{
    return m_value->m_var;
}
symbol_data* lambda_term::get_symbol_data() const
{
    return &m_value->m_symbol_data;
};

void lambda_term::update_scope(const module& mod) const
{
    local_scope ls          = mod.get_scope_for_lambda(*this);
    const identifier& id    = get_var().get_identifier();
    const type& ty          = get_var().get_type();

    ls.update_pattern_var(id, ty);
};

//---------------------------------------------------------------
//                      mu_term
//---------------------------------------------------------------
namespace details
{

class mu_term_impl : public term_atom
{        
    public:
        annotated_identifier m_var;
        term                m_body;
        mutable symbol_data m_symbol_data;

    public:
        mu_term_impl(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data* sd);
        mu_term_impl(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data& sd);
};

mu_term_impl::mu_term_impl(const location& loc, const annotated_identifier& id, 
                        const term& t, const symbol_data* sd)
    :base_type(loc, term_code::mu_term), m_var(id), m_body(t), m_symbol_data(*sd)
{
    check_null(t);
    check_null(id);
};

mu_term_impl::mu_term_impl(const location& loc, const annotated_identifier& id, 
                        const term& t, const symbol_data& sd)
    :base_type(loc, term_code::mu_term), m_var(id), m_body(t), m_symbol_data(sd)
{
    check_null(t);
    check_null(id);
};

};

mu_term::mu_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

mu_term::mu_term()
{};

mu_term::mu_term(const location& loc, const annotated_identifier& id, const term& t,
                         const symbol_data* sd)
    :base_type(new impl_type(loc, id, t, sd))
{};

mu_term::mu_term(const location& loc, const annotated_identifier& id, const term& t,
                         fresh)
    :base_type(new impl_type(loc, id, t, symbol_data::make_new()))
{};

const term& mu_term::get_body() const
{
    return m_value->m_body;
}
const annotated_identifier& mu_term::get_var() const
{
    return m_value->m_var;
}
symbol_data* mu_term::get_symbol_data() const
{
    return &m_value->m_symbol_data;
};

void mu_term::update_scope(const module& mod) const
{
    local_scope ls          = mod.get_scope_for_mu(*this);
    const identifier& id    = get_var().get_identifier();
    const type& ty          = get_var().get_type();

    ls.update_pattern_var(id, ty);
};

//---------------------------------------------------------------
//                      term_kind_scope
//---------------------------------------------------------------
namespace details
{

class term_kind_scope_impl : public term_atom
{
    public:
        identifier      m_var;
        term            m_body;

    public:
        term_kind_scope_impl(const location& loc, const identifier& ai, const term& t);
};

term_kind_scope_impl::term_kind_scope_impl(const location& loc, const identifier& ai, const term& t)
    :base_type(loc, term_code::term_kind_scope), m_body(t), m_var(ai)
{
    check_null(ai);
    check_null(t);
};

};

term_kind_scope::term_kind_scope(const impl_ptr& ip)
    :base_type(ip)
{};

term_kind_scope::term_kind_scope()
{};

term_kind_scope::term_kind_scope(const location& loc, const identifier& ai, const term& t)
    :base_type(new impl_type(loc, ai, t))
{};

const term& term_kind_scope::get_body() const
{
    return m_value->m_body;
}
const identifier& term_kind_scope::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      all_term
//---------------------------------------------------------------
namespace details
{

class all_term_impl : public term_atom
{
    public:
        annotated_type_identifier
                        m_var;
        term            m_body;

    public:
        all_term_impl(const location& loc, const annotated_type_identifier& id, const term& t);
};

all_term_impl::all_term_impl(const location& loc, 
                const annotated_type_identifier& id, const term& t)
    :base_type(loc, term_code::all_term), m_var(id), m_body(t)
{
    check_null(id);
    check_null(t);
};

};

all_term::all_term(const impl_ptr& ptr)
    :base_type(ptr)
{};

all_term::all_term()
{};

all_term::all_term(const location& loc, const annotated_type_identifier& id, const term& t)
    :base_type(new impl_type(loc, id, t))
{};

const term& all_term::get_body() const
{
    return m_value->m_body;
}
const annotated_type_identifier& all_term::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      term_type_application
//---------------------------------------------------------------
namespace details
{

class term_type_application_impl : public term_atom
{
    public:
        type            m_var;
        term            m_body;

    public:
        term_type_application_impl(const location& loc, const term& ex, const type& t);
};

term_type_application_impl::term_type_application_impl(const location& loc, const term& ex, 
                                const type& t)
    :base_type(loc, term_code::term_type_application), m_body(ex), m_var(t)
{
    check_null(ex);
    check_null(t);
};

};

term_type_application::term_type_application(const impl_ptr& ptr)
    :base_type(ptr)
{};

term_type_application::term_type_application()
{};

term_type_application::term_type_application(const location& loc, const term& ex, const type& t)
    :base_type(new impl_type(loc, ex, t))
{};

const term& term_type_application::get_term() const
{
    return m_value->m_body;
}
const type& term_type_application::get_type() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      term_kind_application
//---------------------------------------------------------------
namespace details
{

class term_kind_application_impl : public term_atom
{
    public:
        kind            m_var;
        term            m_body;

    public:
        term_kind_application_impl(const location& loc, const term& ex, const kind& k);
};

term_kind_application_impl::term_kind_application_impl(const location& loc, const term& ex, 
                                const kind& k)
    :base_type(loc, term_code::term_kind_application), m_body(ex), m_var(k)
{
    check_null(ex);
    check_null(k);
};

};

term_kind_application::term_kind_application(const impl_ptr& ptr)
    :base_type(ptr)
{};

term_kind_application::term_kind_application()
{};

term_kind_application::term_kind_application(const location& loc, const term& ex, const kind& t)
    :base_type(new impl_type(loc, ex, t))
{};

const term& term_kind_application::get_term() const
{
    return m_value->m_body;
}
const kind& term_kind_application::get_kind() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      term_constant
//---------------------------------------------------------------
namespace details
{
class term_constant_impl : public term_atom
{
    private:
        using elem_type     = std::tuple<annotated_identifier, term>;
        using elem_vec      = std::vector<elem_type>;
        using iterator      = elem_vec::iterator;
        using elem_map      = std::map<unique_string, iterator>;
        using function_ptr  = std::shared_ptr<extern_function_eval>;

    public:
        identifier          m_name;
        elem_vec            m_elems;
        elem_map            m_elem_map;
        type                m_return_type;
        function_ptr        m_func;
        bool                m_is_extern_func;

    public:
        term_constant_impl(const location& loc, const identifier& name, bool is_extern);

        void                add_label(const annotated_identifier& id, const term& t);
        void                set_label(const identifier& id, const term& t);

        long                size() const;
        const annotated_identifier&
                            get_label(long i) const;
        const term&         get_term(long i) const;
};

term_constant_impl::term_constant_impl(const location& loc, const identifier& name, bool is_extern)
    :base_type(loc, term_code::term_constant), m_name(name), m_is_extern_func(is_extern)
{
    check_null(name);
};

void term_constant_impl::add_label(const annotated_identifier& label, const term& ex)
{
    m_elems.push_back(elem_type(label,ex));
    check_null(label);

    //check_null(ty);

    iterator it = m_elems.end();
    --it;

    ast::fixity_type ft;
    m_elem_map.insert(elem_map::value_type(label.get_identifier().get_name(ft), it)); 
};

void term_constant_impl::set_label(const identifier& id, const term& t)
{
    ast::fixity_type ft;
    auto pos = m_elem_map.find(id.get_name(ft));

    check_null(t);

    if (pos != m_elem_map.end())
    {
        std::get<1>(*pos->second) = t;
    }
};

long term_constant_impl::size() const
{
    return (long)m_elems.size();
};
const annotated_identifier& term_constant_impl::get_label(long i) const
{
    check_pos(m_elems, i);
    return std::get<0>(m_elems[i]);
}

const term& term_constant_impl::get_term(long i) const
{
    check_pos(m_elems, i);
    return std::get<1>(m_elems[i]);
}

};

term_constant::term_constant(const impl_ptr& ip)
    :base_type(ip)
{};

term_constant::term_constant()
{};

term_constant::term_constant(const location& loc, const identifier& id, bool is_extern_func)
    :base_type(new impl_type(loc, id, is_extern_func))
{};

void term_constant::add_label(const annotated_identifier& id, const term& t)
{
    m_value->add_label(id, t);
}
void term_constant::set_return_type(const ast::type& ty)
{
    check_null(ty);
    m_value->m_return_type = ty;
}

const ast::type& term_constant::get_return_type() const
{
    return m_value->m_return_type;
};

bool term_constant::is_extern_func() const
{
    return m_value->m_is_extern_func;
};

void term_constant::set_label(const identifier& id, const term& t)
{
    m_value->set_label(id, t);
}

long term_constant::var_size() const
{
    return m_value->size();
}

const identifier& term_constant::get_name() const
{
    return m_value->m_name;
};

const annotated_identifier& term_constant::get_label(long pos) const
{
    return m_value->get_label(pos);
}
const term& term_constant::get_term(long pos) const
{
    return m_value->get_term(pos);
}

void term_constant::set_function(const extern_function_ptr& func)
{
    m_value->m_func = func;
};

const term_constant::extern_function_ptr& term_constant::get_function() const
{
    return m_value->m_func;
};

ast::term term_constant::eval() const
{
    return m_value->m_func->eval(*this);
};

//---------------------------------------------------------------
//                      closure_term
//---------------------------------------------------------------
namespace details
{

class closure_term_impl : public term_atom
{
    private:
        using item      = std::tuple<term, identifier, type>;
        using item_vec  = std::vector<item>;

    public:
        term            m_body;
        item_vec        m_items;
        closure_kind    m_kind;

    public:
        closure_term_impl(const location& loc, const term& body, closure_kind kind);

        void            add_item(const ast::term& subs, 
                            const ast::identifier& internal_id, const ast::type& ty);
};

closure_term_impl::closure_term_impl(const location& loc, const term& body, closure_kind kind)
    :base_type(loc, term_code::closure_term), m_body(body), m_kind(kind)
{
    check_null(body);
};

void closure_term_impl::add_item(const ast::term& subs, const ast::identifier& internal_id,
                                 const ast::type& ty)
{
    m_items.push_back(item(subs, internal_id, ty));
    check_null(subs);
    check_null(internal_id);
    check_null(ty);
};

};

closure_term::closure_term(const impl_ptr& ip)
    :base_type(ip)
{};

closure_term::closure_term()
{};

closure_term::closure_term(const location& loc, const term& body, closure_kind kind)
    :base_type(new impl_type(loc, body, kind))
{};

const term& closure_term::closure_term::get_body() const
{
    return m_value->m_body;
}

closure_kind closure_term::get_kind() const
{
    return m_value->m_kind;
}

void closure_term::add_item(const ast::term& subs, 
                        const ast::identifier& internal_id, const ast::type& ty)
{
    m_value->add_item(subs, internal_id, ty);
};

long closure_term::size() const
{
    return (long)m_value->m_items.size();
}

const term& closure_term::get_substitution(long i) const
{
    check_pos(m_value->m_items, i);
    return std::get<0>(m_value->m_items[i]);
}

const identifier& closure_term::get_internal_ident(long i) const
{
    check_pos(m_value->m_items, i);
    return std::get<1>(m_value->m_items[i]);
}

const type& closure_term::get_type(long i) const
{
    check_pos(m_value->m_items, i);
    return std::get<2>(m_value->m_items[i]);
}

closure_term closure_term::change_body_func(const ast::term& new_body) const
{
    closure_term ret(m_value->get_location(), new_body, closure_kind::function);
    ret.m_value->m_items = this->m_value->m_items;
    return ret;
};

closure_term closure_term::change_body(const ast::identifier_term& new_body) const
{
    closure_term ret(m_value->get_location(), new_body, closure_kind::identifier);
    ret.m_value->m_items = this->m_value->m_items;
    return ret;
};

closure_term closure_term::change_body(const ast::overloaded_symbol& new_body) const
{
    closure_term ret(m_value->get_location(), new_body, closure_kind::overload);
    ret.m_value->m_items = this->m_value->m_items;
    return ret;
};

closure_term closure_term::change_body(const ast::term& new_body, 
                        const error_handler_ptr& eh) const
{
    if (new_body->get_term_code() == details::term_code::identifier_term)
    {
        identifier_term id = new_body.static_cast_to<ast::identifier_term>();
        return change_body(id);
    }
    if (new_body->get_term_code() == details::term_code::overloaded_symbol)
    {
        overloaded_symbol os = new_body.static_cast_to<ast::overloaded_symbol>();
        return change_body(os);
    }

    error::error_base_msg(eh).internal_error(new_body->get_location(), "invalid body of a closure");
    return change_body_func(error_term(new_body->get_location()));
};

//---------------------------------------------------------------
//                      term
//---------------------------------------------------------------
term::term(const impl_ptr& ptr)
    :base_type(ptr)
{};

term::term()
{};

term::term(const unpack_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const lambda_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const mu_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const list_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const bracket_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const assign_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const all_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_type_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_kind_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const tuple_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_sequence& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const record_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_constant& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const int_literal& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const bool_literal& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const float_literal& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const char_literal& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const string_literal& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const error_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const val_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const ref_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const deref_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const cell_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const identifier_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const overloaded_symbol& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const closure_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const annotated_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const if_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const let_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const path_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const path_int_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const pack_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const variant_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const variant_case_term& t)
    :base_type(t.m_value)
{
    check_null(t);
};

term::term(const term_kind_scope& t)
    :base_type(t.m_value)
{
    check_null(t);
};

template<class Ast>
Ast term::static_cast_to() const
{
    using ref_ptr   = typename Ast::impl_ptr;

    ref_ptr ret = m_value.static_cast_to<ref_ptr>();

    return Ast(ret);
};

template unpack_term term::static_cast_to<unpack_term>() const;
template lambda_term term::static_cast_to<lambda_term>() const;
template mu_term term::static_cast_to<mu_term>() const;
template all_term term::static_cast_to<all_term>() const;
template term_type_application term::static_cast_to<term_type_application>() const;
template term_kind_application term::static_cast_to<term_kind_application>() const;
template tuple_term term::static_cast_to<tuple_term>() const;
template term_sequence term::static_cast_to<term_sequence>() const;
template list_term term::static_cast_to<list_term>() const;
template bracket_term term::static_cast_to<bracket_term>() const;
template record_term term::static_cast_to<record_term>() const;
template term_constant term::static_cast_to<term_constant>() const;
template int_literal term::static_cast_to<int_literal>() const;
template bool_literal term::static_cast_to<bool_literal>() const;
template float_literal term::static_cast_to<float_literal>() const;
template char_literal term::static_cast_to<char_literal>() const;
template string_literal term::static_cast_to<string_literal>() const;
template error_term term::static_cast_to<error_term>() const;
template val_term term::static_cast_to<val_term>() const;
template ref_term term::static_cast_to<ref_term>() const;
template assign_term term::static_cast_to<assign_term>() const;
template deref_term term::static_cast_to<deref_term>() const;
template cell_term term::static_cast_to<cell_term>() const;
template identifier_term term::static_cast_to<identifier_term>() const;
template overloaded_symbol term::static_cast_to<overloaded_symbol>() const;
template closure_term term::static_cast_to<closure_term>() const;
template annotated_term term::static_cast_to<annotated_term>() const;
template if_term term::static_cast_to<if_term>() const;
template term_application term::static_cast_to<term_application>() const;
template let_term term::static_cast_to<let_term>() const;
template path_term term::static_cast_to<path_term>() const;
template path_int_term term::static_cast_to<path_int_term>() const;
template pack_term term::static_cast_to<pack_term>() const;
template variant_term term::static_cast_to<variant_term>() const;
template variant_case_term term::static_cast_to<variant_case_term>() const;
template term_kind_scope term::static_cast_to<term_kind_scope>() const;

};};
