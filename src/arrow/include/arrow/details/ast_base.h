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

#include "arrow/config.h"

#include <string>

namespace arrow { namespace ast { namespace details
{

template<class T>
class refcount
{
    public:
        using value_type    = T;

    private:
        T*          m_ptr;

    public:
        refcount();
        refcount(T* ptr);

        refcount(const refcount& other);
        refcount(refcount&& other);

        template<class S>
        refcount(const refcount<S>& other);

        template<class S>
        refcount(refcount<S>&& other);

        refcount&   operator=(const refcount& other) &;
        refcount&   operator=(refcount&& other) &;

        template<class S>
        refcount&   operator=(const refcount<S>& other) &;

        template<class S>
        refcount&   operator=(refcount<S>&& other) &;

        template<class S>
        friend class refcount;

        ~refcount();

        T*          operator->();
        const T*    operator->() const;
        T*          get();
        const T*    get() const;

        explicit    operator bool() const;

        template<class S>
        S           static_cast_to() const;

    private:
        void        increase_refcount() const;
        void        decrease_refcount() const;
        void        error_null() const;
};

enum class term_code
{
    int_literal,
    bool_literal,
    float_literal,
    char_literal,
    string_literal,
    error_term,
    variant_case_term,
    variant_term,
    pack_term,
    path_term,
    path_int_term,
    let_term,
    term_application,
    if_term,
    annotated_term,
    ref_term,
    deref_term,
    cell_term,
    identifier_term,
    overloaded_symbol,
    closure_term,
    unpack_term,
    tuple_term,
    term_sequence,
    record_term,
    term_constant,
    lambda_term,
    mu_term,
    all_term,
    term_type_application,
    term_kind_application,
    assign_term,
    list_term,
    bracket_term,
    val_term,
    term_kind_scope,    
};

enum class type_code
{
    predefined_type,
    type_application,
    ref_type,
    if_type,
    path_int_type,
    list_type,
    identifier_type,
    arrow_type,
    some_type,
    all_type,
    lambda_type,
    mu_type,
    variant_type,
    tuple_type,
    bracket_type,
    record_type,
    type_var,
    type_constant,
    product_of_types,
    type_sequence,
    error_type,
    int_type,
    bool_type,
    char_type,
    string_type,
    type_kind_scope,
    type_kind_application,
    type_type_application,
    annotated_type,
};

enum class kind_code
{
    star_kind,
    arrow_kind,
    star_arrow_kind,
    product_kind,
    star_tuple_kind,
    kind_var,
    value_kind,
    kind_identifier,
    kind_scope,
    star_all_kind,
    star_some_kind,
    star_list_kind,
    star_record_kind,
    star_variant_kind,
    star_predefined_kind,
    star_ref_kind,
};

enum class definition_code
{
    term_definition,
    term_use,
    type_definition,
    type_definition_var,
    fixity_definition,
};

}}};

#pragma warning(push)
#pragma warning(disable:4251)   //needs to have dll-interface to be used
#pragma warning(disable:4702)   //unreachable code

namespace arrow { namespace ast
{

struct ast_mark{};
struct term_mark{};
struct type_mark{};
struct kind_mark{};
struct definition_mark{};

template<class M>   struct atom_type{};
template<>          struct atom_type<ast_mark>      { using type = ast_atom; };
template<>          struct atom_type<term_mark>     { using type = term_atom; };
template<>          struct atom_type<type_mark>     { using type = type_atom; };
template<>          struct atom_type<kind_mark>     { using type = kind_atom; };
template<>          struct atom_type<definition_mark>{using type = definition_atom; };

template<class Derived, class Impl, class Mark>
class ARROW_EXPORT ast_base
{
    protected:
        using impl_type     = Impl;
        using impl_ptr      = details::refcount<impl_type>;
        using atom_type     = typename atom_type<Mark>::type;
        using base_type     = ast_base;

    protected:
        impl_ptr            m_value;

    protected:
        ast_base();

        ast_base(Impl* ptr);

        template<class S>
        ast_base(const details::refcount<S>& ptr);

        template<class S>
        ast_base(details::refcount<S>&& ptr);

        ast_base(const ast_base&);
        ast_base(ast_base&&);

        ast_base&           operator=(const ast_base&);
        ast_base&           operator=(ast_base&&);

        ~ast_base();

    public:
        ///pointers comparison
        inline bool         operator==(const ast_base& other) const;
        inline bool         operator!=(const ast_base& other) const;
        inline bool         operator<=(const ast_base& other) const;
        inline bool         operator>=(const ast_base& other) const;
        inline bool         operator<(const ast_base& other) const;
        inline bool         operator>(const ast_base& other) const;

        const atom_type*    operator->() const      { return reinterpret_cast<const atom_type*>(m_value.get()); };
        explicit            operator bool() const   { return (bool)m_value; };
};

}}

#pragma warning(pop)

#include "arrow/details/ast_base.inl"
