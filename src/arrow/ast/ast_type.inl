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

#include "ast_common.inl"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      predefined_type
//---------------------------------------------------------------
namespace details
{

class predefined_type_impl : public type_atom
{
    public:
        predefined_type_code  m_code;

    public:
        predefined_type_impl(const location& loc, predefined_type_code tc);
};

predefined_type_impl::predefined_type_impl(const location& loc, predefined_type_code tc)
    :base_type(loc, details::type_code::predefined_type), m_code(tc)
{};

};

predefined_type::predefined_type(const impl_ptr& ip)
    :base_type(ip)
{};

predefined_type::predefined_type()
{};

predefined_type::predefined_type(const location& loc, predefined_type_code tc)
    :base_type(new impl_type(loc, tc))
{};

predefined_type_code predefined_type::get_type_code() const
{
    return m_value->m_code;
}

//---------------------------------------------------------------
//                      type_application
//---------------------------------------------------------------
namespace details
{

class type_application_impl : public type_atom
{
    public:
        type                m_lhs;
        type                m_rhs;

    public:
        type_application_impl(const location& loc, const type& t1, const type& t2);
};

type_application_impl::type_application_impl(const location& loc, const type& t1, const type& t2)
    :base_type(loc, type_code::type_application), m_lhs(t1), m_rhs(t2)
{
    check_null(t1);
    check_null(t2);
};

};

type_application::type_application(const impl_ptr& ip)
    :base_type(ip)
{};

type_application::type_application()
{};

type_application::type_application(const location& loc, const type& t1, const type& t2)
    :base_type(new impl_type(loc, t1, t2))
{};

const type& type_application::get_lhs() const
{
    return m_value->m_lhs;
}
const type& type_application::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      type_kind_application
//---------------------------------------------------------------
namespace details
{

class type_kind_application_impl : public type_atom
{
    public:
        type                m_lhs;
        kind                m_rhs;

    public:
        type_kind_application_impl(const location& loc, const type& t1, const kind& t2);
};

type_kind_application_impl::type_kind_application_impl(const location& loc, const type& t1, 
            const kind& t2)
    :base_type(loc, type_code::type_kind_application), m_lhs(t1), m_rhs(t2)
{
    check_null(t1);
    check_null(t2);
};

};

type_kind_application::type_kind_application(const impl_ptr& ip)
    :base_type(ip)
{};

type_kind_application::type_kind_application()
{};

type_kind_application::type_kind_application(const location& loc, const type& t1, const kind& t2)
    :base_type(new impl_type(loc, t1, t2))
{};

const type& type_kind_application::get_lhs() const
{
    return m_value->m_lhs;
}
const kind& type_kind_application::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      type_type_application
//---------------------------------------------------------------
namespace details
{

class type_type_application_impl : public type_atom
{
    public:
        type                m_lhs;
        type                m_rhs;

    public:
        type_type_application_impl(const location& loc, const type& t1, const type& t2);
};

type_type_application_impl::type_type_application_impl(const location& loc, const type& t1, 
            const type& t2)
    :base_type(loc, type_code::type_type_application), m_lhs(t1), m_rhs(t2)
{
    check_null(t1);
    check_null(t2);
};

};

type_type_application::type_type_application(const impl_ptr& ip)
    :base_type(ip)
{};

type_type_application::type_type_application()
{};

type_type_application::type_type_application(const location& loc, const type& t1, const type& t2)
    :base_type(new impl_type(loc, t1, t2))
{};

const type& type_type_application::get_type() const
{
    return m_value->m_lhs;
}
const type& type_type_application::get_arg() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      list_type
//---------------------------------------------------------------
namespace details
{

class list_type_impl : public type_atom
{
    public:
        type                m_type;

    public:
        list_type_impl(const location& loc, const type& t);
};

list_type_impl::list_type_impl(const location& loc, const type& t)
    :base_type(loc, type_code::list_type), m_type(t)
{
    check_null(t);
};

}

list_type::list_type(const impl_ptr& ip)
    :base_type(ip)
{};

list_type::list_type()
{};

list_type::list_type(const location& loc, const type& t)
    :base_type(new impl_type(loc, t))
{};

const type& list_type::get_type() const
{
    return m_value->m_type;
}

//---------------------------------------------------------------
//                      path_int_type
//---------------------------------------------------------------
namespace details
{

class path_int_type_impl : public type_atom
{
    public:
        type            m_type;
        int_literal     m_index;

    public:
        path_int_type_impl(const location& loc, const type& x, const int_literal& pos);
};

path_int_type_impl::path_int_type_impl(const location& loc, const type& x, const int_literal& pos)
    :base_type(loc, type_code::path_int_type), m_type(x), m_index(pos)
{
    check_null(x);
    check_null(pos);
};

};

path_int_type::path_int_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

path_int_type::path_int_type()
{};

path_int_type::path_int_type(const location& loc, const type& x, const int_literal& pos)
    :base_type(new impl_type(loc, x, pos))
{};

const type& path_int_type::get_type() const
{
    return m_value->m_type;
}
const int_literal& path_int_type::get_index() const
{
    return m_value->m_index;
}

//---------------------------------------------------------------
//                      ref_type
//---------------------------------------------------------------
namespace details
{

class ref_type_impl : public type_atom
{
    public:
        type                m_type;

    public:
        ref_type_impl(const location& loc, const type& t);
};

ref_type_impl::ref_type_impl(const location& loc, const type& t)
    :base_type(loc, type_code::ref_type), m_type(t)
{
    check_null(t);
};

}

ref_type::ref_type(const impl_ptr& ip)
    :base_type(ip)
{};

ref_type::ref_type()
{};

ref_type::ref_type(const location& loc, const type& t)
    :base_type(new impl_type(loc, t))
{};

const type& ref_type::get_type() const
{
    return m_value->m_type;
}

//---------------------------------------------------------------
//                      if_type
//---------------------------------------------------------------
namespace details
{

class if_type_impl : public type_atom
{
    public:
        type                    m_if;
        type                    m_then;
        type                    m_else;

    public:
        if_type_impl(const location& loc, const type& t_if, const type& t_then, 
                    const type& t_else);
};

if_type_impl::if_type_impl(const location& loc, const type& t_if, const type& t_then, 
            const type& t_else)
    :base_type(loc, type_code::if_type), m_if(t_if), m_then(t_then), m_else(t_else)
{
    check_null(t_if);
    check_null(t_then);
    check_null(t_else);
};

};

if_type::if_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

if_type::if_type()
{};

if_type::if_type(const location& loc, const type& t_if, const type& t_then, 
        const type& t_else)
    :base_type(new impl_type(loc, t_if, t_then, t_else))
{};

const type& if_type::get_if() const
{
    return m_value->m_if;
}
const type& if_type::get_then() const
{
    return m_value->m_then;
}
const type& if_type::get_else() const
{
    return m_value->m_else;
}

//---------------------------------------------------------------
//                      identifier_type
//---------------------------------------------------------------
namespace details
{

class identifier_type_impl : public type_atom
{
    public:
        identifier      m_id;

    public:
        identifier_type_impl(const location& loc, const identifier& t);
};

identifier_type_impl::identifier_type_impl(const location& loc, const identifier& id)
    :base_type(loc, type_code::identifier_type), m_id(id)
{
    check_null(id);
};

};

identifier_type::identifier_type(const impl_ptr& ip)
    :base_type(ip)
{}

identifier_type::identifier_type()
{};

identifier_type::identifier_type(const location& loc, const identifier& id)
    :base_type(new impl_type(loc, id))
{};

const identifier& identifier_type::get_identifier() const
{
    return m_value->m_id;
}

//---------------------------------------------------------------
//                      arrow_type
//---------------------------------------------------------------
namespace details
{

class arrow_type_impl : public type_atom
{
    public:
        type            m_lhs;
        type            m_rhs;

    public:
        arrow_type_impl(const location& loc, const type& lhs, const type& rhs);
};

arrow_type_impl::arrow_type_impl(const location& loc, const type& lhs, const type& rhs)
    :base_type(loc, type_code::arrow_type), m_lhs(lhs), m_rhs(rhs)
{
    check_null(lhs);
    check_null(rhs);
};

};

arrow_type::arrow_type(const impl_ptr& ip)
    :base_type(ip)
{};

arrow_type::arrow_type()
{};

arrow_type::arrow_type(const location& loc, const type& lhs, const type& rhs)
    :base_type(new impl_type(loc, lhs, rhs))
{};

const type& arrow_type::get_lhs() const
{
    return m_value->m_lhs;
}
const type& arrow_type::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      some_type
//---------------------------------------------------------------
namespace details
{

class some_type_impl : public type_atom
{
    public:
        annotated_type_identifier
                        m_var;
        type            m_body;

    public:
        some_type_impl(const location& loc, const annotated_type_identifier& ai,
                       const type& t);
};

some_type_impl::some_type_impl(const location& loc, const annotated_type_identifier& ai,
                               const type& t)
    :base_type(loc, type_code::some_type), m_body(t), m_var(ai)
{
    check_null(ai);
    check_null(t);
};

};

some_type::some_type(const impl_ptr& ip)
    :base_type(ip)
{};

some_type::some_type()
{};

some_type::some_type(const location& loc, const annotated_type_identifier& ai, 
                     const type& t)
    :base_type(new impl_type(loc, ai, t))
{};

const type& some_type::get_body() const
{
    return m_value->m_body;
}
const annotated_type_identifier& some_type::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      type_kind_scope
//---------------------------------------------------------------
namespace details
{

class type_kind_scope_impl : public type_atom
{
    public:
        identifier      m_var;
        type            m_body;

    public:
        type_kind_scope_impl(const location& loc, const identifier& ai, const type& t);
};

type_kind_scope_impl::type_kind_scope_impl(const location& loc, const identifier& ai, const type& t)
    :base_type(loc, type_code::type_kind_scope), m_body(t), m_var(ai)
{
    check_null(ai);
    check_null(t);
};

};

type_kind_scope::type_kind_scope(const impl_ptr& ip)
    :base_type(ip)
{};

type_kind_scope::type_kind_scope()
{};

type_kind_scope::type_kind_scope(const location& loc, const identifier& ai, const type& t)
    :base_type(new impl_type(loc, ai, t))
{};

const type& type_kind_scope::get_body() const
{
    return m_value->m_body;
}
const identifier& type_kind_scope::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      all_type
//---------------------------------------------------------------
namespace details
{

class all_type_impl : public type_atom
{
    public:
        annotated_type_identifier
                        m_var;
        type            m_body;

    public:
        all_type_impl(const location& loc, const annotated_type_identifier& ai,
                      const type& t);
};

all_type_impl::all_type_impl(const location& loc, const annotated_type_identifier& ai,
                             const type& t)
    :base_type(loc, type_code::all_type), m_body(t), m_var(ai)
{
    check_null(ai);
    check_null(t);
};

};

all_type::all_type(const impl_ptr& ip)
    :base_type(ip)
{};

all_type::all_type()
{};

all_type::all_type(const location& loc, const annotated_type_identifier& ai, 
                   const type& t)
    :base_type(new impl_type(loc, ai, t))
{};

const type& all_type::get_body() const
{
    return m_value->m_body;
}
const annotated_type_identifier& all_type::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      error_type
//---------------------------------------------------------------
namespace details
{

class error_type_impl : public type_atom
{
    public:
        error_type_impl(const location& loc);
};

error_type_impl::error_type_impl(const location& loc)
    :base_type(loc, type_code::error_type)
{};

};

error_type::error_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

error_type::error_type()
{};

error_type::error_type(const location& loc)
    :base_type(new impl_type(loc))
{};

//---------------------------------------------------------------
//                      int_type
//---------------------------------------------------------------
namespace details
{

class int_type_impl : public type_atom
{
    public:
        int64_t     m_value;

    public:
        int_type_impl(const location& loc, int64_t val);
};

int_type_impl::int_type_impl(const location& loc, int64_t val)
    :base_type(loc, type_code::int_type), m_value(val)
{};

};

int_type::int_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

int_type::int_type()
{};

int_type::int_type(const location& loc, int64_t val)
    :base_type(new impl_type(loc, val))
{};

int64_t int_type::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      bool_type
//---------------------------------------------------------------
namespace details
{

class bool_type_impl : public type_atom
{
    public:
        bool        m_value;

    public:
        bool_type_impl(const location& loc, bool val);
};

bool_type_impl::bool_type_impl(const location& loc, bool val)
    :base_type(loc, type_code::bool_type), m_value(val)
{};

};

bool_type::bool_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

bool_type::bool_type()
{};

bool_type::bool_type(const location& loc, bool val)
    :base_type(new impl_type(loc, val))
{};

bool bool_type::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      char_type
//---------------------------------------------------------------
namespace details
{

class char_type_impl : public type_atom
{
    public:
        int32_t     m_value;

    public:
        char_type_impl(const location& loc, int32_t val);
};

char_type_impl::char_type_impl(const location& loc, int32_t val)
    :base_type(loc, type_code::char_type), m_value(val)
{};

};

char_type::char_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

char_type::char_type()
{};

char_type::char_type(const location& loc, int32_t val)
    :base_type(new impl_type(loc, val))
{};

int32_t char_type::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      string_type
//---------------------------------------------------------------
namespace details
{

class string_type_impl : public type_atom
{
    public:
        std::string     m_value;

    public:
        string_type_impl(const location& loc, const std::string& val);
};

string_type_impl::string_type_impl(const location& loc, const std::string& val)
    :base_type(loc, type_code::string_type), m_value(val)
{};

};

string_type::string_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

string_type::string_type()
{};

string_type::string_type(const location& loc, const std::string& val)
    :base_type(new impl_type(loc, val))
{};

const std::string& string_type::get_value() const
{
    return m_value->m_value;
}

//---------------------------------------------------------------
//                      lambda_type
//---------------------------------------------------------------
namespace details
{

class lambda_type_impl : public type_atom
{
    public:
        annotated_type_identifier
                        m_var;
        type            m_body;
        polarity_type   m_polarity;

    public:
        lambda_type_impl(const location& loc, polarity_type pol,
                         const annotated_type_identifier& ai, const type& t);
};

lambda_type_impl::lambda_type_impl(const location& loc, polarity_type pol,
                        const annotated_type_identifier& ai, const type& t)
    :base_type(loc, type_code::lambda_type), m_body(t), m_var(ai), m_polarity(pol)
{
    check_null(t);
    check_null(ai);
};

};

lambda_type::lambda_type(const impl_ptr& ip)
    :base_type(ip)
{};

lambda_type::lambda_type()
{};

lambda_type::lambda_type(const location& loc, polarity_type pol,
                         const annotated_type_identifier& ai, const type& t)
    :base_type(new impl_type(loc, pol, ai, t))
{};

const type& lambda_type::get_body() const
{
    return m_value->m_body;
}
const annotated_type_identifier& lambda_type::get_var() const
{
    return m_value->m_var;
}
ast::polarity_type lambda_type::get_polarity() const
{
    return m_value->m_polarity;
}

//---------------------------------------------------------------
//                      mu_type
//---------------------------------------------------------------
namespace details
{

class mu_type_impl : public type_atom
{
    public:
        annotated_type_identifier
                        m_var;
        type            m_body;

    public:
        mu_type_impl(const location& loc, const annotated_type_identifier& ai, 
                     const type& t);
};

mu_type_impl::mu_type_impl(const location& loc, const annotated_type_identifier& ai, 
                           const type& t)
    :base_type(loc, type_code::mu_type), m_body(t), m_var(ai)
{
    check_null(t);
    check_null(ai);
};

};

mu_type::mu_type(const impl_ptr& ip)
    :base_type(ip)
{};

mu_type::mu_type()
{};

mu_type::mu_type(const location& loc, const annotated_type_identifier& ai, const type& t)
    :base_type(new impl_type(loc, ai, t))
{};

const type& mu_type::get_body() const
{
    return m_value->m_body;
}
const annotated_type_identifier& mu_type::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      annotated_type
//---------------------------------------------------------------
namespace details
{

class annotated_type_impl : public type_atom
{
    public:
        kind                    m_kind;
        type                    m_type;

    public:
        annotated_type_impl(const location& loc, const type& t, const kind& k);
};

annotated_type_impl::annotated_type_impl(const location& loc, const type& t, const kind& k)
    :base_type(loc, type_code::annotated_type), m_type(t), m_kind(k)
{
    check_null(t);
    check_null(k);
};

};

annotated_type::annotated_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

annotated_type::annotated_type()
{};

annotated_type::annotated_type(const location& loc, const type& t, const kind& k)
    :base_type(new impl_type(loc, t, k))
{};

const type& annotated_type::get_type() const
{
    return m_value->m_type;
}
const kind& annotated_type::get_kind() const
{
    return m_value->m_kind;
}

//---------------------------------------------------------------
//                      tuple_type
//---------------------------------------------------------------
namespace details
{

class tuple_type_impl : public type_atom
{
    private:
        using elem_vec      = std::vector<type>;
        using tuple_state   = tuple_type::tuple_state;

    public:
        elem_vec        m_elems;
        tuple_state     m_state;

    public:
        tuple_type_impl(const location& loc, tuple_state state);

        void            add(const type& t);
};

tuple_type_impl::tuple_type_impl(const location& loc, tuple_state state)
    :base_type(loc, type_code::tuple_type), m_state(state)
{};

void tuple_type_impl::add(const type& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

tuple_type::tuple_type(const impl_ptr& ip)
    :base_type(ip)
{};

tuple_type::tuple_type()
{};

tuple_type::tuple_type(const location& loc, tuple_state state)
    :base_type(new impl_type(loc, state))
{};

void tuple_type::add(const type& t)
{
    m_value->add(t);
}

long tuple_type::size() const
{
    return (long)m_value->m_elems.size();
}
const type& tuple_type::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}
tuple_type::tuple_state tuple_type::get_state() const
{
    return m_value->m_state;
};

void tuple_type::set_state(tuple_state val)
{
    m_value->m_state = val;
};

//---------------------------------------------------------------
//                      bracket_type
//---------------------------------------------------------------
namespace details
{

class bracket_type_impl : public type_atom
{
    private:
        using elem_vec  = std::vector<type>;

    public:
        elem_vec        m_elems;
        identifier      m_opening;
        identifier      m_closing;
        bool            m_is_dot;

    public:
        bracket_type_impl(const location& loc);

        void            add(const type& t);
};

bracket_type_impl::bracket_type_impl(const location& loc)
    :base_type(loc, type_code::bracket_type), m_is_dot(false)
{};

void bracket_type_impl::add(const type& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

bracket_type::bracket_type(const impl_ptr& ptr)
    :base_type(ptr)
{};

bracket_type::bracket_type()
{};

bracket_type::bracket_type(const location& loc)
    :base_type(new impl_type(loc))
{};

void bracket_type::set_dot(bool is_dot)
{
    m_value->m_is_dot   = is_dot;
};
void bracket_type::set_brackets(bool is_dot, const identifier& open, const identifier& close)
{
    m_value->m_opening  = open;
    m_value->m_closing  = close;
    m_value->m_is_dot   = is_dot;
    check_null(open);
    check_null(close);
};
void bracket_type::add(const type& t)
{
    m_value->add(t);
}
long bracket_type::size() const
{
    return (long)m_value->m_elems.size();
}
const type& bracket_type::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

const identifier& bracket_type::get_opening_bracket() const
{
    return m_value->m_opening;
}
const identifier& bracket_type::get_closing_bracket() const
{
    return m_value->m_closing;
}
bool bracket_type::is_dot() const
{
    return m_value->m_is_dot;
};

//---------------------------------------------------------------
//                      product_of_types
//---------------------------------------------------------------
namespace details
{

class product_of_types_impl : public type_atom
{
    private:
        using elem_vec  = std::vector<type>;

    public:
        elem_vec        m_elems;

    public:
        product_of_types_impl(const location& loc);

        void            add(const type& t);
};

product_of_types_impl::product_of_types_impl(const location& loc)
    :base_type(loc, type_code::product_of_types)
{};

void product_of_types_impl::add(const type& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

product_of_types::product_of_types(const impl_ptr& ip)
    :base_type(ip)
{};

product_of_types::product_of_types()
{};

product_of_types::product_of_types(const location& loc)
    :base_type(new impl_type(loc))
{};

void product_of_types::add(const type& t)
{
    m_value->add(t);
}

long product_of_types::size() const
{
    return (long)m_value->m_elems.size();
}
const type& product_of_types::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

//---------------------------------------------------------------
//                      term_sequence_item
//---------------------------------------------------------------
type_sequence_item::type_sequence_item(const location& loc, const type& item, basic_tag)
    :m_item_type(item_type::basic_item), m_type(item), m_loc(loc)
{
    check_null(item);
};

type_sequence_item::type_sequence_item(const location& loc, const int_literal& item, proj_tag)
    :m_item_type(item_type::proj_item), m_int_lit(item), m_loc(loc)
{
    check_null(item);
};

type_sequence_item::type_sequence_item(const location& loc, const identifier& item, dot_tag)
    :m_item_type(item_type::dot_item), m_ident(item), m_loc(loc)
{
    check_null(item);
};

type_sequence_item::type_sequence_item(const location& loc, const kind_vec& item, kind_applic_tag)
    :m_item_type(item_type::kind_applic_item), m_kind_args(item), m_loc(loc)
{
    for(const auto& elem: item)
        check_null(elem);
};

type_sequence_item::type_sequence_item(const location& loc, const type_vec& item, type_applic_tag)
    :m_item_type(item_type::type_applic_item), m_type_args(item), m_loc(loc)
{
    for(const auto& elem: item)
        check_null(elem);
};

type_sequence_item::item_type type_sequence_item::get_item_type() const
{
    return m_item_type;
};

const location& type_sequence_item::get_location() const
{
    return m_loc;
};

const type& type_sequence_item::get_basic() const
{
    return m_type;
}
const int_literal& type_sequence_item::get_projection() const
{
    return m_int_lit;
};
const identifier& type_sequence_item::get_dot() const
{
    return m_ident;
};
const type_sequence_item::type_vec& type_sequence_item::get_type_applic_args() const
{
    return m_type_args;
};
const type_sequence_item::kind_vec& type_sequence_item::get_kind_applic_args() const
{
    return m_kind_args;
};

//---------------------------------------------------------------
//                      type_sequence
//---------------------------------------------------------------
namespace details
{

class type_sequence_impl : public type_atom
{
    private:
        using elem_vec  = std::vector<type_sequence_item>;

    public:
        elem_vec        m_elems;

    public:
        type_sequence_impl(const location& loc);

        void            add(const type_sequence_item& t);
};

type_sequence_impl::type_sequence_impl(const location& loc)
    :base_type(loc, type_code::type_sequence)
{};

void type_sequence_impl::add(const type_sequence_item& t)
{
    m_elems.push_back(t);
}

};

type_sequence::type_sequence(const impl_ptr& ip)
    :base_type(ip)
{};

type_sequence::type_sequence()
{};

type_sequence::type_sequence(const location& loc)
    :base_type(new impl_type(loc))
{};

void type_sequence::add(const location& loc, const type& t)
{
    m_value->add(type_sequence_item(loc, t, type_sequence_item::basic_tag()));
}
void type_sequence::add_projection(const location& loc, const ast::int_literal& pos)
{
    m_value->add(type_sequence_item(loc, pos, type_sequence_item::proj_tag()));
}
void type_sequence::add_dot(const location& loc, const ast::identifier& id)
{
    m_value->add(type_sequence_item(loc, id, type_sequence_item::dot_tag()));
}
void type_sequence::add_kind_application(const location& loc, const std::vector<ast::kind>& args)
{
    m_value->add(type_sequence_item(loc, args, type_sequence_item::kind_applic_tag()));
}
void type_sequence::add_type_application(const location& loc, const std::vector<ast::type>& args)
{
    m_value->add(type_sequence_item(loc, args, type_sequence_item::type_applic_tag()));
}

long type_sequence::size() const
{
    return (long)m_value->m_elems.size();
}
const type_sequence_item& type_sequence::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}

//---------------------------------------------------------------
//                      variant_type
//---------------------------------------------------------------
namespace details
{

class variant_type_impl : public type_atom
{
    private:
        using elem_type = std::pair<annotated_type_identifier, type>;
        using elem_vec  = std::vector<elem_type>;
        using field_map = std::map<unique_string, elem_type>;

    public:
        elem_vec        m_elems;
        field_map       m_map;

    public:
        variant_type_impl(const location& loc);

        void            add(const annotated_type_identifier& id, const type& t);

        const annotated_type_identifier&
                        get_identifier(long pos) const;
        const type&     get_type(long pos) const;

        bool            has_field(const identifier& label) const;
        type            get_field(const identifier& label) const;
        annotated_type_identifier 
                        get_identifier(const identifier& label) const;
};

variant_type_impl::variant_type_impl(const location& loc)
    :base_type(loc, type_code::variant_type)
{};

void variant_type_impl::add(const annotated_type_identifier& id, const type& t)
{
    m_elems.push_back(elem_type(id, t));
    check_null(t);
    check_null(id);

    ast::fixity_type ft;
    m_map[id.get_identifier().get_name(ft)] = elem_type(id, t);
}

const annotated_type_identifier& variant_type_impl::get_identifier(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].first;
}
const type& variant_type_impl::get_type(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].second;
}

bool variant_type_impl::has_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

type variant_type_impl::get_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return type();
    else
        return pos->second.second;
};

annotated_type_identifier variant_type_impl::get_identifier(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return annotated_type_identifier();
    else
        return pos->second.first;
};

};

variant_type::variant_type(const impl_ptr& ip)
    :base_type(ip)
{};

variant_type::variant_type()
{};

variant_type::variant_type(const location& loc)
    :base_type(new impl_type(loc))
{};

void variant_type::add(const annotated_type_identifier& id, const type& t)
{
    m_value->add(id, t);
}

long variant_type::size() const
{
    return (long)m_value->m_elems.size();
}
const type& variant_type::get_type(long pos) const
{
    return m_value->get_type(pos);
}
const annotated_type_identifier& variant_type::get_identifier(long pos) const
{
    return m_value->get_identifier(pos);
}

bool variant_type::has_field(const identifier& label) const
{
    return m_value->has_field(label);
}
type variant_type::get_field(const identifier& label) const
{
    return m_value->get_field(label);
}
annotated_type_identifier variant_type::get_identifier(const identifier& label) const
{
    return m_value->get_identifier(label);
};

//---------------------------------------------------------------
//                      record_type
//---------------------------------------------------------------
namespace details
{

class record_type_impl : public type_atom
{
    private:
        using elem_type = std::pair<annotated_type_identifier, type>;
        using elem_vec  = std::vector<elem_type>;
        using field_map = std::map<unique_string, elem_type>;

    public:
        elem_vec        m_elems;
        field_map       m_map;

    public:
        record_type_impl(const location& loc);

        void            add(const annotated_type_identifier& id, const type& t);

        const annotated_type_identifier&
                        get_identifier(long pos) const;
        const type&     get_type(long pos) const;

        bool            has_field(const identifier& label) const;
        type            get_field(const identifier& label) const;
        annotated_type_identifier 
                        get_identifier(const identifier& label) const;
};

record_type_impl::record_type_impl(const location& loc)
    :base_type(loc, type_code::record_type)
{};

void record_type_impl::add(const annotated_type_identifier& id, const type& t)
{
    m_elems.push_back(elem_type(id, t));
    check_null(t);
    check_null(id);

    ast::fixity_type ft;
    m_map[id.get_identifier().get_name(ft)] = elem_type(id, t);
}

const annotated_type_identifier& record_type_impl::get_identifier(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].first;
}
const type& record_type_impl::get_type(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].second;
}

bool record_type_impl::has_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

type record_type_impl::get_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return type();
    else
        return pos->second.second;
};

annotated_type_identifier record_type_impl::get_identifier(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return annotated_type_identifier();
    else
        return pos->second.first;
};

};

record_type::record_type(const impl_ptr& ip)
    :base_type(ip)
{};

record_type::record_type()
{};

record_type::record_type(const location& loc)
    :base_type(new impl_type(loc))
{};

void record_type::add(const annotated_type_identifier& id, const type& t)
{
    m_value->add(id, t);
}

long record_type::size() const
{
    return (long)m_value->m_elems.size();
}
const type& record_type::get_type(long pos) const
{
    return m_value->get_type(pos);
}
const annotated_type_identifier& record_type::get_identifier(long pos) const
{
    return m_value->get_identifier(pos);
}

bool record_type::has_field(const identifier& label) const
{
    return m_value->has_field(label);
}
type record_type::get_field(const identifier& label) const
{
    return m_value->get_field(label);
}
annotated_type_identifier record_type::get_identifier(const identifier& label) const
{
    return m_value->get_identifier(label);
};

//---------------------------------------------------------------
//                      type_constant
//---------------------------------------------------------------
namespace details
{
class type_constant_impl : public type_atom
{
    private:
        using elem_type = std::tuple<annotated_type_identifier, type>;
        using elem_vec  = std::vector<elem_type>;
        using iterator  = elem_vec::iterator;
        using elem_map  = std::map<unique_string, iterator>;
        using extern_function_ptr   = std::shared_ptr<extern_type_function_eval>;

    public:
        identifier          m_name;
        elem_vec            m_elems;
        elem_map            m_elem_map;
        kind                m_return_kind;
        bool                m_is_extern;
        extern_function_ptr m_func;

    public:
        type_constant_impl(const location& loc, const identifier& name, 
                           const kind& ret, bool is_extern);

        void                add_label(const annotated_type_identifier& id, const type& t);
        void                set_label(const identifier& id, const type& t);

        long                size() const;
        const annotated_type_identifier&
                            get_label(long i) const;
        const type&         get_type(long i) const;
};

type_constant_impl::type_constant_impl(const location& loc, const identifier& name,
                                       const kind& ret, bool is_extern)
    :base_type(loc, type_code::type_constant), m_name(name), m_is_extern(is_extern)
    , m_return_kind(ret)
{
    check_null(name);
    //ret can be null
};

void type_constant_impl::add_label(const annotated_type_identifier& label, const type& ty)
{
    m_elems.push_back(elem_type(label,ty));
    check_null(label);

    //check_null(ty);

    iterator it = m_elems.end();
    --it;

    ast::fixity_type ft;
    m_elem_map.insert(elem_map::value_type(label.get_identifier().get_name(ft), it)); 
};

void type_constant_impl::set_label(const identifier& id, const type& t)
{
    ast::fixity_type ft;
    auto pos = m_elem_map.find(id.get_name(ft));

    check_null(t);

    if (pos != m_elem_map.end())
    {
        std::get<1>(*pos->second) = t;
    }
};

long type_constant_impl::size() const
{
    return (long)m_elems.size();
};
const annotated_type_identifier& type_constant_impl::get_label(long i) const
{
    check_pos(m_elems, i);
    return std::get<0>(m_elems[i]);
}

const type& type_constant_impl::get_type(long i) const
{
    check_pos(m_elems, i);
    return std::get<1>(m_elems[i]);
}

};

type_constant::type_constant(const impl_ptr& ip)
    :base_type(ip)
{};

type_constant::type_constant()
{};

type_constant::type_constant(const location& loc, const identifier& id, 
                             const ast::kind& ret, bool is_extern)
    :base_type(new impl_type(loc, id, ret, is_extern))
{};

void type_constant::add_label(const annotated_type_identifier& id, const type& t)
{
    m_value->add_label(id, t);
}
void type_constant::set_return_kind(const ast::kind& ki)
{
    check_null(ki);
    m_value->m_return_kind = ki;
}

const ast::kind& type_constant::get_return_kind() const
{
    return m_value->m_return_kind;
};

void type_constant::set_label(const identifier& id, const type& t)
{
    m_value->set_label(id, t);
}

void type_constant::set_function(const extern_function_ptr& func)
{
    m_value->m_func = func;
};

long type_constant::var_size() const
{
    return m_value->size();
}

const identifier& type_constant::get_name() const
{
    return m_value->m_name;
};

const annotated_type_identifier& type_constant::get_label(long pos) const
{
    return m_value->get_label(pos);
}
const type& type_constant::get_type(long pos) const
{
    return m_value->get_type(pos);
}

const type_constant::extern_function_ptr& type_constant::get_function() const
{
    return m_value->m_func;
};
bool type_constant::is_extern_func() const
{
    return m_value->m_is_extern;
};

ast::type type_constant::eval() const
{
    return m_value->m_func->eval(*this);
};

//---------------------------------------------------------------
//                      type_var
//---------------------------------------------------------------
namespace details
{

class type_var_impl : public type_atom
{
    private:
        mutable type        m_substitution;

    public:
        type_var_impl(const location& loc);

        bool                is_set() const;
        void                set(const type& other) const;
        const type&         get() const;

    private:
        const type&         get(const type& subs) const;
};

type_var_impl::type_var_impl(const location& loc)
    :base_type(loc, type_code::type_var)
{};

bool type_var_impl::is_set() const
{
    if (m_substitution)
        return true;
    else
        return false;
}
void type_var_impl::set(const type& other) const
{
    if (other->get_type_code() == details::type_code::type_var)
    {
        const type_var& t2  = other.static_cast_to<type_var>();

        if (t2.is_set() == true)
            return set(t2.get());
    };

    m_substitution = other;
};

const type& type_var_impl::get() const
{
    return get(m_substitution);
};

const type& type_var_impl::get(const type& subs) const
{
    if (subs->get_type_code() == details::type_code::type_var)
    {
        const type_var& t2  = subs.static_cast_to<type_var>();

        if (t2.is_set() == true)
            return get(t2.get());
    };

    return subs;
};

};

type_var::type_var(const impl_ptr& ip)
    :base_type(ip)
{};

type_var::type_var()
{};

type_var::type_var(const location& loc)
    :base_type(new impl_type(loc))
{};

bool type_var::is_set() const
{
    return m_value->is_set();
};

void type_var::set_checked(const type& other) const
{
    return m_value->set(other);
};

const type& type_var::get() const
{
    return m_value->get();
};

//---------------------------------------------------------------
//                      type
//---------------------------------------------------------------
type::type(const impl_ptr& ip)
    :base_type(ip)
{};

type::type()
{};

type::type(const arrow_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const list_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const annotated_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const ref_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const if_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const path_int_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const identifier_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const some_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const all_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const lambda_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const mu_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const variant_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const record_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_constant& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_kind_scope& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const tuple_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const bracket_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const product_of_types& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_sequence& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_kind_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_type_application& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const predefined_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const type_var& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const error_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const int_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const bool_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const char_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

type::type(const string_type& t)
    :base_type(t.m_value)
{
    check_null(t);
};

template<class Ast>
Ast type::static_cast_to() const
{
    using ref_ptr   = typename Ast::impl_ptr;

    ref_ptr ret = m_value.static_cast_to<ref_ptr>();

    return Ast(ret);
};

template error_type type::static_cast_to<error_type>() const;
template int_type type::static_cast_to<int_type>() const;
template bool_type type::static_cast_to<bool_type>() const;
template char_type type::static_cast_to<char_type>() const;
template string_type type::static_cast_to<string_type>() const;
template arrow_type type::static_cast_to<arrow_type>() const;
template ref_type type::static_cast_to<ref_type>() const;
template if_type type::static_cast_to<if_type>() const;
template path_int_type type::static_cast_to<path_int_type>() const;
template list_type type::static_cast_to<list_type>() const;
template identifier_type type::static_cast_to<identifier_type>() const;
template some_type type::static_cast_to<some_type>() const;
template all_type type::static_cast_to<all_type>() const;
template lambda_type type::static_cast_to<lambda_type>() const;
template mu_type type::static_cast_to<mu_type>() const;
template variant_type type::static_cast_to<variant_type>() const;
template record_type type::static_cast_to<record_type>() const;
template type_constant type::static_cast_to<type_constant>() const;
template tuple_type type::static_cast_to<tuple_type>() const;
template bracket_type type::static_cast_to<bracket_type>() const;
template annotated_type type::static_cast_to<annotated_type>() const;
template product_of_types type::static_cast_to<product_of_types>() const;
template type_sequence type::static_cast_to<type_sequence>() const;
template type_application type::static_cast_to<type_application>() const;
template type_kind_application type::static_cast_to<type_kind_application>() const;
template type_type_application type::static_cast_to<type_type_application>() const;
template predefined_type type::static_cast_to<predefined_type>() const;
template type_var type::static_cast_to<type_var>() const;
template type_kind_scope type::static_cast_to<type_kind_scope>() const;

};};
