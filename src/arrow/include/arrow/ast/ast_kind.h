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

#include "ast_helpers.h"
#include "ast_atoms.h"

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      KINDS
//---------------------------------------------------------------
class ARROW_EXPORT kind : public ast_base<kind, kind_atom, kind_mark>
{
    private:
        kind(const impl_ptr& ip);

    public:
        kind();
        kind(const star_kind& k);
        kind(const value_kind& k);
        kind(const kind_identifier& k);
        kind(const arrow_kind& k);
        kind(const star_arrow_kind& k);
        kind(const product_kind& k);
        kind(const star_tuple_kind& k);
        kind(const kind_var& k);
        kind(const kind_scope& k);
        kind(const star_all_kind& k);
        kind(const star_some_kind& k);
        kind(const star_list_kind& k);
        kind(const star_record_kind& k);
        kind(const star_variant_kind& k);
        kind(const star_predefined_kind& k);
        kind(const star_ref_kind& k);

        template<class Ast>
        Ast                 static_cast_to() const;
};

//rule: *
class ARROW_EXPORT star_kind
    : public ast_base<star_kind, details::star_kind_impl, kind_mark>
{
    private:
        star_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_kind();
        star_kind(const location& loc);
};

//rule: Int | Bool
class ARROW_EXPORT value_kind
    : public ast_base<value_kind, details::value_kind_impl, kind_mark>
{
    private:
        value_kind(const impl_ptr& ip);
        friend kind;

    public:
        value_kind();
        value_kind(const location& loc, value_kind_code code);

        value_kind_code     get_code() const;
};

//rule: identifier
class ARROW_EXPORT kind_identifier
    : public ast_base<kind_identifier, details::kind_identifier_impl, kind_mark>
{
    private:
        kind_identifier(const impl_ptr& ip);
        friend kind;

    public:
        kind_identifier();
        kind_identifier(const location& loc, const identifier& id);

        const identifier&   get_identifier() const;
};

//rule: kind (at)-> kind, where at = + | - | [empty]
class ARROW_EXPORT arrow_kind
    : public ast_base<arrow_kind, details::arrow_kind_impl, kind_mark>
{
    private:
        arrow_kind(const impl_ptr& ip);
        friend kind;

    public:
        arrow_kind();
        arrow_kind(const location& loc, polarity_type akt, const kind& lhs, const kind& rhs);
            
        const kind&         get_lhs() const;
        const kind&         get_rhs() const;
        polarity_type       get_type() const;
};

enum class star_kind_state
{
    flexible,   // can be converted to star
    star,       // is converted to star
    fixed,      // state cannot change
};

//rule: '*{' star_kind -> star_kind '}'
class ARROW_EXPORT star_arrow_kind
    : public ast_base<star_arrow_kind, details::star_arrow_kind_impl, kind_mark>
{
    private:
        star_arrow_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_arrow_kind();
        star_arrow_kind(const location& loc, const kind& lhs, const kind& rhs,
                        star_kind_state state);
            
        const kind&         get_lhs() const;
        const kind&         get_rhs() const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;
};

// rule: '{' (kind ( ',' kind)*)? '}'
class ARROW_EXPORT product_kind
    : public ast_base<product_kind, details::product_kind_impl, kind_mark>
{
    private:
        product_kind(const impl_ptr& ip);
        friend kind;

    public:
        product_kind();
        product_kind(const location& loc, bool fixed_size);

        void                add(const kind& k);
        void                fix_size();

        long                size() const;
        const kind&         get(long pos) const;
        bool                is_size_fixed() const;
};

// rule: '*{' (star_kind ( ',' star_kind)*)? '}'
class ARROW_EXPORT star_tuple_kind
    : public ast_base<star_tuple_kind, details::star_tuple_kind_impl, kind_mark>
{
    private:
        star_tuple_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_tuple_kind();
        star_tuple_kind(const location& loc, bool fixed_size, star_kind_state state);

        void                add(const kind& k);
        void                fix_size();
        
        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;

        long                size() const;
        const kind&         get(long pos) const;
        bool                is_size_fixed() const;
};

//rule: Kind[k] kind
class ARROW_EXPORT kind_scope
    : public ast_base<kind_scope, details::kind_scope_impl, kind_mark>
{
    private:
        kind_scope(const impl_ptr& ip);
        friend kind;

    public:
        kind_scope();
        kind_scope(const location& loc, const identifier& ai, const kind& t);

        const kind&         get_body() const;
        const identifier&   get_var() const;
};

//rule: *{ All[kind] kind }
class ARROW_EXPORT star_all_kind
    : public ast_base<star_all_kind, details::star_all_kind_impl, kind_mark>
{
    private:
        star_all_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_all_kind();
        star_all_kind(const location& loc, const kind& var, const kind& t,
                      star_kind_state state);

        const kind&         get_body() const;
        const kind&         get_var() const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;
};

//rule: *{ Some[kind] kind }
class ARROW_EXPORT star_some_kind
    : public ast_base<star_some_kind, details::star_some_kind_impl, kind_mark>
{
    private:
        star_some_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_some_kind();
        star_some_kind(const location& loc, const kind& var, const kind& t,
                       star_kind_state state);

        const kind&         get_body() const;
        const kind&         get_var() const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;
};

//rule: *{ list[kind] }
class ARROW_EXPORT star_list_kind
    : public ast_base<star_list_kind, details::star_list_kind_impl, kind_mark>
{
    private:
        star_list_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_list_kind();
        star_list_kind(const location& loc, const kind& t, star_kind_state state);

        const kind&         get_body() const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;
};

//rule: *{ Record{...} }
class ARROW_EXPORT star_record_kind
    : public ast_base<star_record_kind, details::star_record_kind_impl, kind_mark>
{
    private:
        star_record_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_record_kind();
        star_record_kind(const location& loc, star_kind_state state);

        void                add(const identifier& id, const kind& t);

        long                size() const;
        const identifier&   get_identifier(long pos) const;
        const kind&         get_kind(long pos) const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;

        bool                has_field(const identifier& label) const;
        kind                get_field(const identifier& label) const;
};

//rule: *{ Variant{...} }
class ARROW_EXPORT star_variant_kind
    : public ast_base<star_variant_kind, details::star_variant_kind_impl, kind_mark>
{
    private:
        star_variant_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_variant_kind();
        star_variant_kind(const location& loc, star_kind_state state);

        void                add(const identifier& id, const kind& t);

        long                size() const;
        const identifier&   get_identifier(long pos) const;
        const kind&         get_kind(long pos) const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;

        bool                has_field(const identifier& label) const;
        kind                get_field(const identifier& label) const;
};

//rule: *{ Ref kind }
class ARROW_EXPORT star_ref_kind
    : public ast_base<star_ref_kind, details::star_ref_kind_impl, kind_mark>
{
    private:
        star_ref_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_ref_kind();
        star_ref_kind(const location& loc, const kind& t, star_kind_state state);

        const kind&         get_body() const;

        bool                is_star() const;
        bool                is_fixed() const;
        star_kind_state     get_state() const;
        bool                make_star() const;
        void                make_fixed() const;
};

//rule: *{ int|bool|float|string|char }
class ARROW_EXPORT star_predefined_kind
    : public ast_base<star_predefined_kind, details::star_predefined_kind_impl, kind_mark>
{
    private:
        star_predefined_kind(const impl_ptr& ip);
        friend kind;

    public:
        star_predefined_kind();
        star_predefined_kind(const location& loc, predefined_type_code code,
                             star_kind_state state);

        predefined_type_code    get_code() const;

        bool                    is_star() const;
        bool                    is_fixed() const;
        star_kind_state         get_state() const;
        bool                    make_star() const;
        void                    make_fixed() const;
};

//rule: ?
class ARROW_EXPORT kind_var
    : public ast_base<kind_var, details::kind_var_impl, kind_mark>
{
    private:
        kind_var(const impl_ptr& ip);
        friend kind;

    public:
        kind_var();
        kind_var(const location& loc, bool star_var);

        bool                is_set() const;        
        const kind&         get() const;
        bool                is_star_var() const;

        /// occurs check must be performed before; or other cannot
        /// be recursive
        void                set_checked(const kind& other) const;

        void                set_star_var();
};

};};

#pragma warning(pop)
