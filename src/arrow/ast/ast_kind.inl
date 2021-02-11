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
//                      star_kind
//---------------------------------------------------------------
namespace details
{

class star_kind_impl : public kind_atom
{
    public:
        star_kind_impl(const location& loc);
};

star_kind_impl::star_kind_impl(const location& loc)
    :base_type(loc, kind_code::star_kind)
{};

};

star_kind::star_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_kind::star_kind()
{};

star_kind::star_kind(const location& loc)
    :base_type(new impl_type(loc))
{};

//---------------------------------------------------------------
//                      value_kind
//---------------------------------------------------------------
namespace details
{

class value_kind_impl : public kind_atom
{
    public:
        value_kind_code     m_code;

    public:
        value_kind_impl(const location& loc, value_kind_code code);
};

value_kind_impl::value_kind_impl(const location& loc, value_kind_code code)
    :base_type(loc, kind_code::value_kind), m_code(code)
{};

};

value_kind::value_kind(const impl_ptr& ip)
    :base_type(ip)
{};

value_kind::value_kind()
{};

value_kind::value_kind(const location& loc, value_kind_code code)
    :base_type(new impl_type(loc, code))
{};

value_kind_code value_kind::get_code() const
{
    return m_value->m_code;
};

//---------------------------------------------------------------
//                      kind_var
//---------------------------------------------------------------
namespace details
{

class kind_var_impl : public kind_atom
{
    private:
        mutable kind        m_substitution;

    public:
        bool                m_is_star;

    public:
        kind_var_impl(const location& loc, bool is_star);

        bool                is_set() const;
        void                set(const kind& other) const;
        const kind&         get() const;

    private:
        const kind&         get(const kind& subs) const;
};

kind_var_impl::kind_var_impl(const location& loc, bool star)
    :base_type(loc, kind_code::kind_var), m_is_star(star)
{};

bool kind_var_impl::is_set() const
{
    if (m_substitution)
        return true;
    else
        return false;
}
void kind_var_impl::set(const kind& other) const
{
    if (other->get_kind_code() == details::kind_code::kind_var)
    {
        const kind_var& k2  = other.static_cast_to<kind_var>();

        if (k2.is_set() == true)
            return set(k2.get());
    };

    m_substitution = other;
};

const kind& kind_var_impl::get() const
{
    return get(m_substitution);
};

const kind& kind_var_impl::get(const kind& subs) const
{
    if (subs->get_kind_code() == details::kind_code::kind_var)
    {
        const kind_var& k2  = subs.static_cast_to<kind_var>();

        if (k2.is_set() == true)
            return get(k2.get());
    };

    return subs;
};

};

kind_var::kind_var(const impl_ptr& ip)
    :base_type(ip)
{};

kind_var::kind_var()
{};

kind_var::kind_var(const location& loc, bool is_star)
    :base_type(new impl_type(loc, is_star))
{};

bool kind_var::is_set() const
{
    return m_value->is_set();
};

void kind_var::set_checked(const kind& other) const
{
    return m_value->set(other);
};
const kind& kind_var::get() const
{
    return m_value->get();
};
bool kind_var::is_star_var() const
{
    return m_value->m_is_star;
};

void kind_var::set_star_var()
{
    m_value->m_is_star = true;
};

//---------------------------------------------------------------
//                      product_kind
//---------------------------------------------------------------
namespace details
{

class product_kind_impl : public kind_atom
{
    private:
        using elem_vec  = std::vector<kind>;

    public:
        elem_vec        m_elems;
        bool            m_fixed;

    public:
        product_kind_impl(const location& loc, bool fixed_size);

        void            add(const kind& t);
};

product_kind_impl::product_kind_impl(const location& loc, bool fixed)
    :base_type(loc, kind_code::product_kind), m_fixed(fixed)
{};

void product_kind_impl::add(const kind& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

product_kind::product_kind(const impl_ptr& ip)
    :base_type(ip)
{};

product_kind::product_kind()
{};

product_kind::product_kind(const location& loc, bool fixed_size)
    :base_type(new impl_type(loc, fixed_size))
{};

void product_kind::add(const kind& t)
{
    m_value->add(t);
}
void product_kind::fix_size()
{
    m_value->m_fixed = true;
}

long product_kind::size() const
{
    return (long)m_value->m_elems.size();
}
const kind& product_kind::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}
bool product_kind::is_size_fixed() const
{
    return m_value->m_fixed;
}

//---------------------------------------------------------------
//                      star_tuple_kind
//---------------------------------------------------------------
namespace details
{

class star_tuple_kind_impl : public kind_atom
{
    private:
        using elem_vec  = std::vector<kind>;

    public:
        elem_vec        m_elems;
        bool            m_fixed;
        mutable star_kind_state 
                        m_state;

    public:
        star_tuple_kind_impl(const location& loc, bool fixed_size, star_kind_state state);

        void            add(const kind& t);
};

star_tuple_kind_impl::star_tuple_kind_impl(const location& loc, bool fixed, 
                                           star_kind_state state)
    :base_type(loc, kind_code::star_tuple_kind), m_fixed(fixed), m_state(state)
{};

void star_tuple_kind_impl::add(const kind& t)
{
    m_elems.push_back(t);
    check_null(t);
}

};

star_tuple_kind::star_tuple_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_tuple_kind::star_tuple_kind()
{};

star_tuple_kind::star_tuple_kind(const location& loc, bool fixed_size, 
                                 star_kind_state state)
    :base_type(new impl_type(loc, fixed_size, state))
{};

void star_tuple_kind::add(const kind& t)
{
    m_value->add(t);
}
void star_tuple_kind::fix_size()
{
    m_value->m_fixed = true;
}

long star_tuple_kind::size() const
{
    return (long)m_value->m_elems.size();
}
const kind& star_tuple_kind::get(long pos) const
{
    check_pos(m_value->m_elems, pos);
    return m_value->m_elems[pos];
}
bool star_tuple_kind::is_size_fixed() const
{
    return m_value->m_fixed;
}

bool star_tuple_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_tuple_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_tuple_kind::get_state() const
{
    return m_value->m_state;
}
bool star_tuple_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}

void star_tuple_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      arrow_kind
//---------------------------------------------------------------
namespace details
{

class arrow_kind_impl : public kind_atom
{
    public:
        kind                m_lhs;
        kind                m_rhs;
        polarity_type       m_arrow_type;

    public:
        arrow_kind_impl(const location& loc, polarity_type akt, const kind& lhs, const kind& rhs);
};

arrow_kind_impl::arrow_kind_impl(const location& loc, polarity_type akt, const kind& lhs, 
            const kind& rhs)
    :base_type(loc, kind_code::arrow_kind), m_rhs(rhs), m_lhs(lhs), m_arrow_type(akt)
{
    check_null(lhs);
    check_null(rhs);
};

};

arrow_kind::arrow_kind(const impl_ptr& ip)
    :base_type(ip)
{};

arrow_kind::arrow_kind()
{};

arrow_kind::arrow_kind(const location& loc, polarity_type akt, const kind& lhs, const kind& rhs)
    :base_type(new impl_type(loc, akt, lhs, rhs))
{};

polarity_type arrow_kind::get_type() const
{
    return m_value->m_arrow_type;
};
const kind& arrow_kind::get_lhs() const
{
    return m_value->m_lhs;
}
const kind& arrow_kind::get_rhs() const
{
    return m_value->m_rhs;
}

//---------------------------------------------------------------
//                      star_arrow_kind
//---------------------------------------------------------------
namespace details
{

class star_arrow_kind_impl : public kind_atom
{
    public:
        kind                m_lhs;
        kind                m_rhs;
        mutable star_kind_state
                            m_state;

    public:
        star_arrow_kind_impl(const location& loc, const kind& lhs, const kind& rhs,
                             star_kind_state state);
};

star_arrow_kind_impl::star_arrow_kind_impl(const location& loc, const kind& lhs, 
            const kind& rhs, star_kind_state state)
    :base_type(loc, kind_code::star_arrow_kind), m_rhs(rhs), m_lhs(lhs)
    ,m_state(state)
{
    check_null(lhs);
    check_null(rhs);
};

};

star_arrow_kind::star_arrow_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_arrow_kind::star_arrow_kind()
{};

star_arrow_kind::star_arrow_kind(const location& loc, const kind& lhs, const kind& rhs,
                                 star_kind_state state)
    :base_type(new impl_type(loc, lhs, rhs, state))
{};

const kind& star_arrow_kind::get_lhs() const
{
    return m_value->m_lhs;
}
const kind& star_arrow_kind::get_rhs() const
{
    return m_value->m_rhs;
}

bool star_arrow_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_arrow_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_arrow_kind::get_state() const
{
    return m_value->m_state;
}
bool star_arrow_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}

void star_arrow_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      kind_identifier
//---------------------------------------------------------------
namespace details
{

class kind_identifier_impl : public kind_atom
{
    public:
        identifier      m_id;

    public:
        kind_identifier_impl(const location& loc, const identifier& t);
};

kind_identifier_impl::kind_identifier_impl(const location& loc, const identifier& id)
    :base_type(loc, kind_code::kind_identifier), m_id(id)
{
    check_null(id);
};

};

kind_identifier::kind_identifier(const impl_ptr& ip)
    :base_type(ip)
{}

kind_identifier::kind_identifier()
{};

kind_identifier::kind_identifier(const location& loc, const identifier& id)
    :base_type(new impl_type(loc, id))
{};

const identifier& kind_identifier::get_identifier() const
{
    return m_value->m_id;
}

//---------------------------------------------------------------
//                      kind_scope
//---------------------------------------------------------------
namespace details
{

class kind_scope_impl : public kind_atom
{
    public:
        identifier      m_var;
        kind            m_body;

    public:
        kind_scope_impl(const location& loc, const identifier& ai, const kind& t);
};

kind_scope_impl::kind_scope_impl(const location& loc, const identifier& ai, const kind& t)
    :base_type(loc, kind_code::kind_scope), m_body(t), m_var(ai)
{
    check_null(ai);
    check_null(t);
};

};

kind_scope::kind_scope(const impl_ptr& ip)
    :base_type(ip)
{};

kind_scope::kind_scope()
{};

kind_scope::kind_scope(const location& loc, const identifier& ai, const kind& t)
    :base_type(new impl_type(loc, ai, t))
{};

const kind& kind_scope::get_body() const
{
    return m_value->m_body;
}
const identifier& kind_scope::get_var() const
{
    return m_value->m_var;
}

//---------------------------------------------------------------
//                      star_all_kind
//---------------------------------------------------------------
namespace details
{

class star_all_kind_impl : public kind_atom
{
    public:
        kind            m_var;
        kind            m_body;
        mutable star_kind_state
                        m_state;

    public:
        star_all_kind_impl(const location& loc, const kind& var, const kind& t,
                           star_kind_state state);
};

star_all_kind_impl::star_all_kind_impl(const location& loc, const kind& var, const kind& t,
                                       star_kind_state state)
    :base_type(loc, kind_code::star_all_kind), m_body(t), m_var(var)
    , m_state(state)
{
    check_null(var);
    check_null(t);
};

};

star_all_kind::star_all_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_all_kind::star_all_kind()
{};

star_all_kind::star_all_kind(const location& loc, const kind& var, const kind& t, 
                             star_kind_state state)
    :base_type(new impl_type(loc, var, t, state))
{};

const kind& star_all_kind::get_body() const
{
    return m_value->m_body;
}
const kind& star_all_kind::get_var() const
{
    return m_value->m_var;
}

bool star_all_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_all_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_all_kind::get_state() const
{
    return m_value->m_state;
}
bool star_all_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}

void star_all_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_some_kind
//---------------------------------------------------------------
namespace details
{

class star_some_kind_impl : public kind_atom
{
    public:
        kind            m_var;
        kind            m_body;
        mutable star_kind_state
                        m_state;

    public:
        star_some_kind_impl(const location& loc, const kind& var, const kind& t,
                            star_kind_state state);
};

star_some_kind_impl::star_some_kind_impl(const location& loc, const kind& var, const kind& t,
                                         star_kind_state state)
    :base_type(loc, kind_code::star_some_kind), m_body(t), m_var(var)
    ,m_state(state)
{
    check_null(var);
    check_null(t);
};

};

star_some_kind::star_some_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_some_kind::star_some_kind()
{};

star_some_kind::star_some_kind(const location& loc, const kind& var, const kind& t,
                               star_kind_state state)
    :base_type(new impl_type(loc, var, t, state))
{};

const kind& star_some_kind::get_body() const
{
    return m_value->m_body;
}
const kind& star_some_kind::get_var() const
{
    return m_value->m_var;
}

bool star_some_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_some_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_some_kind::get_state() const
{
    return m_value->m_state;
}
bool star_some_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_some_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_list_kind
//---------------------------------------------------------------
namespace details
{

class star_list_kind_impl : public kind_atom
{
    public:
        kind            m_body;
        mutable star_kind_state 
                        m_state;

    public:
        star_list_kind_impl(const location& loc, const kind& t, 
                            star_kind_state state);
};

star_list_kind_impl::star_list_kind_impl(const location& loc, const kind& t,
                                         star_kind_state state)
    :base_type(loc, kind_code::star_list_kind), m_body(t), m_state(state)
{
    check_null(t);
};

};

star_list_kind::star_list_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_list_kind::star_list_kind()
{};

star_list_kind::star_list_kind(const location& loc, const kind& t, star_kind_state state)
    :base_type(new impl_type(loc, t, state))
{};

const kind& star_list_kind::get_body() const
{
    return m_value->m_body;
}

bool star_list_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_list_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_list_kind::get_state() const
{
    return m_value->m_state;
}
bool star_list_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_list_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_record_kind
//---------------------------------------------------------------
namespace details
{

class star_record_kind_impl : public kind_atom
{
    private:
        using elem_type = std::pair<identifier, kind>;
        using elem_vec  = std::vector<elem_type>;
        using field_map = std::map<unique_string, elem_type>;

    public:
        elem_vec        m_elems;
        field_map       m_map;
        mutable star_kind_state 
                        m_state;

    public:
        star_record_kind_impl(const location& loc, star_kind_state state);

        void            add(const identifier& id, const kind& k);

        const identifier&
                        get_identifier(long pos) const;
        const kind&     get_kind(long pos) const;

        bool            has_field(const identifier& label) const;
        kind            get_field(const identifier& label) const;
};

star_record_kind_impl::star_record_kind_impl(const location& loc, star_kind_state state)
    :base_type(loc, kind_code::star_record_kind), m_state(state)
{};

void star_record_kind_impl::add(const identifier& id, const kind& t)
{
    m_elems.push_back(elem_type(id, t));
    check_null(t);
    check_null(id);

    ast::fixity_type ft;
    m_map[id.get_name(ft)] = elem_type(id, t);
}

const identifier& star_record_kind_impl::get_identifier(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].first;
}
const kind& star_record_kind_impl::get_kind(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].second;
}

bool star_record_kind_impl::has_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

kind star_record_kind_impl::get_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return kind();
    else
        return pos->second.second;
};

};

star_record_kind::star_record_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_record_kind::star_record_kind()
{};

star_record_kind::star_record_kind(const location& loc, star_kind_state state)
    :base_type(new impl_type(loc, state))
{};

void star_record_kind::add(const identifier& id, const kind& t)
{
    m_value->add(id, t);
}

long star_record_kind::size() const
{
    return (long)m_value->m_elems.size();
}
const kind& star_record_kind::get_kind(long pos) const
{
    return m_value->get_kind(pos);
}
const identifier& star_record_kind::get_identifier(long pos) const
{
    return m_value->get_identifier(pos);
}

bool star_record_kind::has_field(const identifier& label) const
{
    return m_value->has_field(label);
}
kind star_record_kind::get_field(const identifier& label) const
{
    return m_value->get_field(label);
}

bool star_record_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_record_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_record_kind::get_state() const
{
    return m_value->m_state;
}
bool star_record_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_record_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_variant_kind
//---------------------------------------------------------------
namespace details
{

class star_variant_kind_impl : public kind_atom
{
    private:
        using elem_type = std::pair<identifier, kind>;
        using elem_vec  = std::vector<elem_type>;
        using field_map = std::map<unique_string, elem_type>;

    public:
        elem_vec        m_elems;
        field_map       m_map;
        mutable star_kind_state 
                        m_state;

    public:
        star_variant_kind_impl(const location& loc, star_kind_state state);

        void            add(const identifier& id, const kind& k);

        const identifier&
                        get_identifier(long pos) const;
        const kind&     get_kind(long pos) const;

        bool            has_field(const identifier& label) const;
        kind            get_field(const identifier& label) const;
};

star_variant_kind_impl::star_variant_kind_impl(const location& loc, star_kind_state state)
    :base_type(loc, kind_code::star_variant_kind), m_state(state)
{};

void star_variant_kind_impl::add(const identifier& id, const kind& t)
{
    m_elems.push_back(elem_type(id, t));
    check_null(t);
    check_null(id);

    ast::fixity_type ft;
    m_map[id.get_name(ft)] = elem_type(id, t);
}

const identifier& star_variant_kind_impl::get_identifier(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].first;
}
const kind& star_variant_kind_impl::get_kind(long pos) const
{
    check_pos(m_elems, pos);
    return m_elems[pos].second;
}

bool star_variant_kind_impl::has_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return false;
    else
        return true;
};

kind star_variant_kind_impl::get_field(const identifier& label) const
{
    ast::fixity_type ft;
    auto pos = m_map.find(label.get_name(ft));
    if (pos == m_map.end())
        return kind();
    else
        return pos->second.second;
};

};

star_variant_kind::star_variant_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_variant_kind::star_variant_kind()
{};

star_variant_kind::star_variant_kind(const location& loc, star_kind_state state)
    :base_type(new impl_type(loc, state))
{};

void star_variant_kind::add(const identifier& id, const kind& t)
{
    m_value->add(id, t);
}

long star_variant_kind::size() const
{
    return (long)m_value->m_elems.size();
}
const kind& star_variant_kind::get_kind(long pos) const
{
    return m_value->get_kind(pos);
}
const identifier& star_variant_kind::get_identifier(long pos) const
{
    return m_value->get_identifier(pos);
}

bool star_variant_kind::has_field(const identifier& label) const
{
    return m_value->has_field(label);
}
kind star_variant_kind::get_field(const identifier& label) const
{
    return m_value->get_field(label);
}

bool star_variant_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_variant_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_variant_kind::get_state() const
{
    return m_value->m_state;
}
bool star_variant_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_variant_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_predefined_kind
//---------------------------------------------------------------
namespace details
{

class star_predefined_kind_impl : public kind_atom
{
    public:
        predefined_type_code    m_code;
        mutable star_kind_state m_state;

    public:
        star_predefined_kind_impl(const location& loc, predefined_type_code code,
                                  star_kind_state state);
};

star_predefined_kind_impl::star_predefined_kind_impl(const location& loc, 
                            predefined_type_code code, star_kind_state state)
    :base_type(loc, kind_code::star_predefined_kind), m_code(code)
    ,m_state(state)
{};

};

star_predefined_kind::star_predefined_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_predefined_kind::star_predefined_kind()
{};

star_predefined_kind::star_predefined_kind(const location& loc, predefined_type_code c,
                                           star_kind_state state)
    :base_type(new impl_type(loc, c, state))
{};

predefined_type_code star_predefined_kind::get_code() const
{
    return m_value->m_code;
}

bool star_predefined_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_predefined_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_predefined_kind::get_state() const
{
    return m_value->m_state;
}
bool star_predefined_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_predefined_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      star_ref_kind
//---------------------------------------------------------------
namespace details
{

class star_ref_kind_impl : public kind_atom
{
    public:
        kind            m_body;
        mutable star_kind_state 
                        m_state;

    public:
        star_ref_kind_impl(const location& loc, const kind& t, star_kind_state state);
};

star_ref_kind_impl::star_ref_kind_impl(const location& loc, const kind& t, 
                                       star_kind_state state)
    :base_type(loc, kind_code::star_ref_kind), m_body(t), m_state(state)
{
    check_null(t);
};

};

star_ref_kind::star_ref_kind(const impl_ptr& ip)
    :base_type(ip)
{};

star_ref_kind::star_ref_kind()
{};

star_ref_kind::star_ref_kind(const location& loc, const kind& t, star_kind_state state)
    :base_type(new impl_type(loc, t, state))
{};

const kind& star_ref_kind::get_body() const
{
    return m_value->m_body;
}

bool star_ref_kind::is_star() const
{ 
    return get_state() == star_kind_state::star; 
};
bool star_ref_kind::is_fixed() const
{ 
    return get_state() != star_kind_state::flexible; 
};
star_kind_state star_ref_kind::get_state() const
{
    return m_value->m_state;
}
bool star_ref_kind::make_star() const
{
    if (get_state() == star_kind_state::fixed)
        return false;

    m_value->m_state = star_kind_state::star;
    return true;
}
void star_ref_kind::make_fixed() const
{
    if (get_state() == star_kind_state::flexible)
        m_value->m_state = star_kind_state::fixed;
};

//---------------------------------------------------------------
//                      kind
//---------------------------------------------------------------
kind::kind(const impl_ptr& ip)
    :base_type(ip)
{};

kind::kind()
{};

kind::kind(const star_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const product_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_tuple_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const arrow_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_arrow_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const kind_var& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const value_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const kind_identifier& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const kind_scope& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_all_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_some_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_list_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_record_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_variant_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_predefined_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

kind::kind(const star_ref_kind& k)
    :base_type(k.m_value)
{
    check_null(k);
};

template<class Ast>
Ast kind::static_cast_to() const
{
    using ref_ptr   = typename Ast::impl_ptr;

    ref_ptr ret = m_value.static_cast_to<ref_ptr>();

    return Ast(ret);
};

template star_kind kind::static_cast_to<star_kind>() const;
template arrow_kind kind::static_cast_to<arrow_kind>() const;
template star_arrow_kind kind::static_cast_to<star_arrow_kind>() const;
template product_kind kind::static_cast_to<product_kind>() const;
template star_tuple_kind kind::static_cast_to<star_tuple_kind>() const;
template kind_var kind::static_cast_to<kind_var>() const;
template value_kind kind::static_cast_to<value_kind>() const;
template kind_scope kind::static_cast_to<kind_scope>() const;
template star_all_kind kind::static_cast_to<star_all_kind>() const;
template star_some_kind kind::static_cast_to<star_some_kind>() const;
template star_list_kind kind::static_cast_to<star_list_kind>() const;
template star_record_kind kind::static_cast_to<star_record_kind>() const;
template star_variant_kind kind::static_cast_to<star_variant_kind>() const;
template star_predefined_kind kind::static_cast_to<star_predefined_kind>() const;
template star_ref_kind kind::static_cast_to<star_ref_kind>() const;
template kind_identifier kind::static_cast_to<kind_identifier>() const;

};};
