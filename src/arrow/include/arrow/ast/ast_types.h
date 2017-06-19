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
//                      TYPES
//---------------------------------------------------------------
class ARROW_EXPORT type : public ast_base<type, type_atom, type_mark>
{
    private:
        type(const impl_ptr& ip);

    public:
        type();
        type(const arrow_type& t);
        type(const ref_type& t);
        type(const if_type& t);
        type(const path_int_type& t);
        type(const identifier_type& t);
        type(const some_type& t);
        type(const all_type& t);
        type(const type_kind_scope& t);
        type(const lambda_type& t);
        type(const mu_type& t);
        type(const variant_type& t);
        type(const record_type& t);
        type(const annotated_type& t);
        type(const tuple_type& t);
        type(const bracket_type& t);
        type(const product_of_types& t);
        type(const type_sequence& t);
        type(const type_application& t);
        type(const type_kind_application& t);
        type(const type_type_application& t);
        type(const predefined_type& t);
        type(const type_var& t);
        type(const list_type& t);
        type(const type_constant& t);
        type(const error_type& t);
        type(const int_type& t);
        type(const bool_type& t);
        type(const char_type& t);
        type(const string_type& t);

        template<class Ast>
        Ast                 static_cast_to() const;
};

class ARROW_EXPORT predefined_type
    : public ast_base<predefined_type, details::predefined_type_impl, type_mark>
{
    private:
        predefined_type(const impl_ptr& ip);
        friend type;

    public:
        predefined_type();
        predefined_type(const location& loc, predefined_type_code tc);

        predefined_type_code    get_type_code() const;
};

class ARROW_EXPORT error_type
    : public ast_base<error_type, details::error_type_impl, type_mark>
{
    private:
        error_type(const impl_ptr&);
        friend type;

    public:
        error_type();
        error_type(const location& loc);
};

class ARROW_EXPORT int_type
    : public ast_base<int_type, details::int_type_impl, type_mark>
{
    private:
        int_type(const impl_ptr&);
        friend type;

    public:
        int_type();
        int_type(const location& loc, int64_t val);

        int64_t             get_value() const;
};

class ARROW_EXPORT bool_type
    : public ast_base<bool_type, details::bool_type_impl, type_mark>
{
    private:
        bool_type(const impl_ptr&);
        friend type;

    public:
        bool_type();
        bool_type(const location& loc, bool val);

        bool                get_value() const;
};

class ARROW_EXPORT char_type
    : public ast_base<char_type, details::char_type_impl, type_mark>
{
    private:
        char_type(const impl_ptr&);
        friend type;

    public:
        char_type();
        char_type(const location& loc, int32_t val);

        int32_t             get_value() const;
};

class ARROW_EXPORT string_type
    : public ast_base<string_type, details::string_type_impl, type_mark>
{
    private:
        string_type(const impl_ptr&);
        friend type;

    public:
        string_type();
        string_type(const location& loc, const std::string& val);

        const std::string&  get_value() const;
};

class ARROW_EXPORT type_application
    : public ast_base<type_application, details::type_application_impl, type_mark>
{
    private:
        type_application(const impl_ptr& ip);
        friend type;

    public:
        type_application();
        type_application(const location& loc, const type& t1, const type& t2);

        const type&         get_lhs() const;
        const type&         get_rhs() const;
};

class ARROW_EXPORT type_kind_application
    : public ast_base<type_kind_application, details::type_kind_application_impl, type_mark>
{
    private:
        type_kind_application(const impl_ptr& ip);
        friend type;

    public:
        type_kind_application();
        type_kind_application(const location& loc, const type& t1, const kind& t2);

        const type&         get_lhs() const;
        const kind&         get_rhs() const;
};

class ARROW_EXPORT type_type_application
    : public ast_base<type_type_application, details::type_type_application_impl, term_mark>
{
    private:
        type_type_application(const impl_ptr&);
        friend type;

    public:
        type_type_application();
        type_type_application(const location& loc, const type& ty, const type& arg);
        
        const type&         get_type() const;
        const type&         get_arg() const;
};

class ARROW_EXPORT list_type
    : public ast_base<list_type, details::list_type_impl, type_mark>
{
    private:
        list_type(const impl_ptr& ip);
        friend type;

    public:
        list_type();
        list_type(const location& loc, const type& t);

        const type&         get_type() const;
};

class ARROW_EXPORT ref_type
    : public ast_base<ref_type, details::ref_type_impl, type_mark>
{
    private:
        ref_type(const impl_ptr& ip);
        friend type;

    public:
        ref_type();
        ref_type(const location& loc, const type& t);

        const type&         get_type() const;
};

class ARROW_EXPORT if_type
    : public ast_base<if_type, details::if_type_impl, type_mark>
{
    private:
        if_type(const impl_ptr&);
        friend type;

    public:
        if_type();
        if_type(const location& loc, const type& t_if, const type& t_then, 
                const type& t_else);

        const type&         get_if() const;
        const type&         get_then() const;
        const type&         get_else() const;
};

class ARROW_EXPORT path_int_type
    : public ast_base<path_int_type, details::path_int_type_impl, type_mark>
{
    private:
        path_int_type(const impl_ptr&);
        friend type;

    public:
        path_int_type();
        path_int_type(const location& loc, const type& x, const int_literal& pos);

        const type&         get_type() const;
        const int_literal&  get_index() const;
};

class ARROW_EXPORT identifier_type
    : public ast_base<identifier_type, details::identifier_type_impl, type_mark>
{
    private:
        identifier_type(const impl_ptr& ip);
        friend type;

    public:
        identifier_type();
        identifier_type(const location& loc, const identifier& id);

        const identifier&   get_identifier() const;
};

class ARROW_EXPORT arrow_type
    : public ast_base<arrow_type, details::arrow_type_impl, type_mark>
{
    private:
        arrow_type(const impl_ptr& ip);
        friend type;

    public:
        arrow_type();
        arrow_type(const location& loc, const type& lhs, const type& rhs);

        const type&         get_lhs() const;
        const type&         get_rhs() const;
};

class ARROW_EXPORT some_type
    : public ast_base<some_type, details::some_type_impl, type_mark>
{
    private:
        some_type(const impl_ptr& ip);
        friend type;

    public:
        some_type();
        some_type(const location& loc, const annotated_type_identifier& ai, 
                  const type& t);

        const type&         get_body() const;
        const annotated_type_identifier&
                            get_var() const;
};

class ARROW_EXPORT all_type
    : public ast_base<all_type, details::all_type_impl, type_mark>
{
    private:
        all_type(const impl_ptr& ip);
        friend type;

    public:
        all_type();
        all_type(const location& loc, const annotated_type_identifier& ai, 
                 const type& t);

        const type&         get_body() const;
        const annotated_type_identifier&
                            get_var() const;
};

//rule: Kind[k] type
class ARROW_EXPORT type_kind_scope
    : public ast_base<type_kind_scope, details::type_kind_scope_impl, type_mark>
{
    private:
        type_kind_scope(const impl_ptr& ip);
        friend type;

    public:
        type_kind_scope();
        type_kind_scope(const location& loc, const identifier& ai, const type& t);

        const type&         get_body() const;
        const identifier&   get_var() const;
};

class ARROW_EXPORT lambda_type
    : public ast_base<lambda_type, details::lambda_type_impl, type_mark>
{
    private:
        lambda_type(const impl_ptr& ip);
        friend type;

    public:
        lambda_type();
        lambda_type(const location& loc, polarity_type pol, 
                    const annotated_type_identifier& ai, const type& t);

        const type&         get_body() const;
        const annotated_type_identifier&
                            get_var() const;
        polarity_type       get_polarity() const;
};

class ARROW_EXPORT mu_type
    : public ast_base<mu_type, details::mu_type_impl, type_mark>
{
    private:
        mu_type(const impl_ptr& ip);
        friend type;

    public:
        mu_type();
        mu_type(const location& loc, const annotated_type_identifier& ai, 
                const type& t);

        const type&         get_body() const;
        const annotated_type_identifier&
                            get_var() const;
};

class ARROW_EXPORT variant_type
    : public ast_base<variant_type, details::variant_type_impl, type_mark>
{
    private:
        variant_type(const impl_ptr& ip);
        friend type;

    public:
        variant_type();
        variant_type(const location& loc);

        void                add(const annotated_type_identifier& id, const type& t);

        long                size() const;
        const annotated_type_identifier&
                            get_identifier(long pos) const;
        const type&         get_type(long pos) const;

        bool                has_field(const identifier& label) const;
        type                get_field(const identifier& label) const;
        annotated_type_identifier
                            get_identifier(const identifier& label) const;
};

class ARROW_EXPORT record_type
    : public ast_base<record_type, details::record_type_impl, type_mark>
{
    private:
        record_type(const impl_ptr& ip);
        friend type;

    public:
        record_type();
        record_type(const location& loc);

        void                add(const annotated_type_identifier& id, const type& t);

        long                size() const;
        const annotated_type_identifier&
                            get_identifier(long pos) const;
        const type&         get_type(long pos) const;

        bool                has_field(const identifier& label) const;
        type                get_field(const identifier& label) const;
        annotated_type_identifier
                            get_identifier(const identifier& label) const;
};

class ARROW_EXPORT annotated_type
    : public ast_base<annotated_type, details::annotated_type_impl, type_mark>
{
    private:
        annotated_type(const impl_ptr&);
        friend type;

    public:
        annotated_type();
        annotated_type(const location& loc, const type& x, const kind& t);

        const kind&         get_kind() const;
        const type&         get_type() const;
};

class ARROW_EXPORT tuple_type
    : public ast_base<tuple_type, details::tuple_type_impl, type_mark>
{
    private:
        tuple_type(const impl_ptr& ip);
        friend type;

    public:
        enum tuple_state
        {
            can_expand,     // expand 1-element tuple later
            fixed,          // 1-element tuple cannot be expanded
            dot,            // this is dot operator (i.e. f .(args) )
        };

    public:
        tuple_type();
        tuple_type(const location& loc, tuple_state state);

        void                add(const type& ft);
        void                set_state(tuple_state state);

        long                size() const;
        const type&         get(long pos) const;
        tuple_state         get_state() const;
};

class ARROW_EXPORT bracket_type
    : public ast_base<bracket_type, details::bracket_type_impl, type_mark>
{
    private:
        bracket_type(const impl_ptr&);
        friend type;

    public:
        bracket_type();
        bracket_type(const location& loc);

        void                add(const type& t);
        void                set_brackets(bool is_dot, const identifier& open, 
                                const identifier& close);
        void                set_dot(bool is_dot);

        long                size() const;
        const type&         get(long pos) const;
        const identifier&   get_opening_bracket() const;
        const identifier&   get_closing_bracket() const;
        bool                is_dot() const;
};

class ARROW_EXPORT product_of_types
    : public ast_base<product_of_types, details::product_of_types_impl, type_mark>
{
    private:
        product_of_types(const impl_ptr& ip);
        friend type;

    public:
        product_of_types();
        product_of_types(const location& loc);

        void                add(const type& ft);

        long                size() const;
        const type&         get(long pos) const;
};

class ARROW_EXPORT type_sequence_item
{
    public:
        struct basic_tag{};
        struct proj_tag{};
        struct dot_tag{};
        struct type_applic_tag{};
        struct kind_applic_tag{};

    public:
        using kind_vec          = std::vector<ast::kind>;
        using type_vec          = std::vector<ast::type>;
        enum class item_type    { basic_item, proj_item, dot_item, type_applic_item,
                                    kind_applic_item};

    private:
        location            m_loc;
        item_type           m_item_type;
        type                m_type;
        identifier          m_ident;
        int_literal         m_int_lit;
        type_vec            m_type_args;
        kind_vec            m_kind_args;

    public:
        type_sequence_item(const location& loc, const type& item, basic_tag);
        type_sequence_item(const location& loc, const int_literal& item, proj_tag);
        type_sequence_item(const location& loc, const identifier& item, dot_tag);
        type_sequence_item(const location& loc, const kind_vec& item, kind_applic_tag);
        type_sequence_item(const location& loc, const type_vec& item, type_applic_tag);

        item_type           get_item_type() const;
        const location&     get_location() const;

        const type&         get_basic() const;
        const int_literal&  get_projection() const;
        const identifier&   get_dot() const;
        const type_vec&     get_type_applic_args() const;
        const kind_vec&     get_kind_applic_args() const;
};

class ARROW_EXPORT type_sequence
    : public ast_base<type_sequence, details::type_sequence_impl, type_mark>
{
    private:
        type_sequence(const impl_ptr& ip);
        friend type;

    public:
        type_sequence();
        type_sequence(const location& loc);

        void                add(const location& loc, const type& t);
        void                add_projection(const location& loc, const ast::int_literal& pos);
        void                add_dot(const location& loc, const ast::identifier& id);
        void                add_type_application(const location& loc, 
                                const std::vector<ast::type>& args);
        void                add_kind_application(const location& loc, 
                                const std::vector<ast::kind>& args);

        long                size() const;
        const type_sequence_item&
                            get(long pos) const;
};

class ARROW_EXPORT type_var
    : public ast_base<type_var, details::type_var_impl, type_mark>
{
    private:
        type_var(const impl_ptr& ip);
        friend type;

    public:
        type_var();
        type_var(const location& loc);

        bool                is_set() const;

        /// get stored substitution; this function is called recursively
        /// if stored substitution is a type_var again
        const type&         get() const;

        /// occurs check must be performed before; or other cannot
        /// be recursive
        void                set_checked(const type& other) const;
};

class ARROW_EXPORT extern_type_function_eval
{
    public:
        virtual ~extern_type_function_eval(){};      

        virtual type    eval(const type_constant& data) const = 0;
        virtual kind    get_return_kind() const = 0;
        virtual long    number_arguments() const = 0;
        virtual kind    get_arg_kind(long i) const = 0;
};

/// this node represents types without definitions, i.e.
/// Kind[k] type Name : kind; or
/// Kind[k] type Name (args)*
/// parser produces this node without a content; content is added later
class ARROW_EXPORT type_constant
    : public ast_base<type_constant, details::type_constant_impl, type_mark>
{
    public:
        using extern_function_ptr   = std::shared_ptr<extern_type_function_eval>;

    private:
        type_constant(const impl_ptr& ip);
        friend type;

    public:
        type_constant();
        type_constant(const location& loc, const identifier& id, const kind& ret_kind,
                      bool is_extern);

        void                add_label(const annotated_type_identifier& id, const type& );

        /// set type of existing label
        void                set_label(const identifier& id, const type& t);
        void                set_function(const extern_function_ptr& func);

        /// set kind of this type constant
        void                set_return_kind(const kind& ki);

        long                var_size() const;

        const kind&         get_return_kind() const;
        const identifier&   get_name() const;
        const annotated_type_identifier&
                            get_label(long pos) const;

        /// can be null
        const type&         get_type(long pos) const;

        const extern_function_ptr&
                            get_function() const;
        bool                is_extern_func() const;
        ast::type           eval() const;
};

};};

#pragma warning(pop)
