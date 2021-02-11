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

#include "ast_helpers.h"
#include "ast_atoms.h"

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      TERMS
//---------------------------------------------------------------
class ARROW_EXPORT term : public ast_base<term, term_atom, term_mark>
{
    private:
        term(const impl_ptr&);

    public:
        term();
        term(const closure_term& t);
        term(const unpack_term& t);
        term(const lambda_term& t);
        term(const mu_term& t);
        term(const all_term& t);
        term(const term_kind_scope& t);
        term(const term_type_application& t);
        term(const term_kind_application& t);
        term(const tuple_term& t);
        term(const term_sequence& t);
        term(const list_term& t);
        term(const bracket_term& t);
        term(const record_term& t);
        term(const term_constant& t);
        term(const int_literal& t);
        term(const bool_literal& t);
        term(const float_literal& t);
        term(const char_literal& t);
        term(const string_literal& t);
        term(const error_term& t);
        term(const ref_term& t);
        term(const deref_term& t);
        term(const assign_term& t);
        term(const cell_term& t);
        term(const identifier_term& t);
        term(const overloaded_symbol& t);
        term(const annotated_term& t);
        term(const if_term& t);
        term(const term_application& t);
        term(const let_term& t);
        term(const path_term& t);
        term(const path_int_term& t);
        term(const pack_term& t);
        term(const variant_term& t);
        term(const variant_case_term& t);
        term(const val_term& t);

        template<class Ast>
        Ast                 static_cast_to() const;
};

class ARROW_EXPORT variant_case_term
    : public ast_base<variant_case_term, details::variant_case_term_impl, term_mark>
{
    public:
        /// mark used to create fresh objects
        struct fresh{};

    private:
        variant_case_term(const impl_ptr&);
        friend term;

    public:
        variant_case_term();
        variant_case_term(const location& loc, const symbol_data* sd);
        variant_case_term(const location& loc, fresh);

        void                set_term(const term& case_term);
        void                add_pattern(const annotated_identifier& lab, 
                                const term& ex);

        long                size() const;
        const term&         get_term() const;

        bool                has_label(const ast::identifier& id) const;
        term                get_label_bind(const ast::identifier& id) const;
        annotated_identifier get_label(const ast::identifier& id) const;

        const annotated_identifier&
                            get_label(long pos) const;
        const term&         get_bind(long pos) const;

        symbol_data*        get_symbol_data() const;

        /// update var type stored in the associated scope
        void                update_scope(const module& mod) const;
};

class ARROW_EXPORT variant_term
    : public ast_base<variant_term, details::variant_term_impl, term_mark>
{
    private:
        variant_term(const impl_ptr&);
        friend term;

    public:
        variant_term();
        variant_term(const location& loc, const type& ty, const ast::annotated_identifier& id,
                     const term& ex);

        const type&         get_type() const;
        const term&         get_term() const;
        const annotated_identifier&
                            get_label() const;
};

class ARROW_EXPORT pack_term
    : public ast_base<pack_term, details::pack_term_impl, term_mark>
{
    private:
        pack_term(const impl_ptr&);
        friend term;

    public:
        pack_term();
        pack_term(const location& loc, const annotated_type_identifier& id, const type& t, 
                  const term& e, const type& some_type);

        const annotated_type_identifier&
                            get_var() const;
        const term&         get_body() const;
        const type&         get_var_type() const;
        const type&         get_some_type() const;
};

class ARROW_EXPORT path_term
    : public ast_base<path_term, details::path_term_impl, term_mark>
{
    private:
        path_term(const impl_ptr&);
        friend term;

    public:
        path_term();
        path_term(const location& loc, const term& x, const identifier& id);

        const term&         get_term() const;
        const identifier&   get_identifier() const;
};

class ARROW_EXPORT path_int_term
    : public ast_base<path_int_term, details::path_int_term_impl, term_mark>
{
    private:
        path_int_term(const impl_ptr&);
        friend term;

    public:
        path_int_term();
        path_int_term(const location& loc, const term& x, const int_literal& pos);

        const term&         get_term() const;
        const int_literal&  get_index() const;
};

class ARROW_EXPORT term_application
    : public ast_base<term_application, details::term_application_impl, term_mark>
{
    private:
        term_application(const impl_ptr&);
        friend term;

    public:
        term_application();
        term_application(const location& loc, const term& t1, const term& t2);

        const term&         get_lhs() const;
        const term&         get_rhs() const;
};

class ARROW_EXPORT if_term
    : public ast_base<if_term, details::if_term_impl, term_mark>
{
    private:
        if_term(const impl_ptr&);
        friend term;

    public:
        if_term();
        if_term(const location& loc, const term& t_if, const term& t_then, 
                const term& t_else);

        const term&         get_if() const;
        const term&         get_then() const;
        const term&         get_else() const;
};

class ARROW_EXPORT annotated_term
    : public ast_base<annotated_term, details::annotated_term_impl, term_mark>
{
    private:
        annotated_term(const impl_ptr&);
        friend term;

    public:
        annotated_term();
        annotated_term(const location& loc, const term& x, const type& t);

        const term&         get_term() const;
        const type&         get_type() const;
};

class ARROW_EXPORT val_term
    : public ast_base<val_term, details::val_term_impl, term_mark>
{
    private:
        val_term(const impl_ptr&);
        friend term;

    public:
        val_term();
        val_term(const location& loc, const type& t);

        const type&         get_type() const;
};

class ARROW_EXPORT ref_term
    : public ast_base<ref_term, details::ref_term_impl, term_mark>
{
    private:
        ref_term(const impl_ptr&);
        friend term;

    public:
        ref_term();
        ref_term(const location& loc, const term& t);

        const term&         get_term() const;
};

class ARROW_EXPORT assign_term
    : public ast_base<assign_term, details::assign_term_impl, term_mark>
{
    private:
        assign_term(const impl_ptr&);
        friend term;

    public:
        assign_term();
        assign_term(const location& loc, const term& lhs, const term& rhs);

        const term&         get_lhs() const;
        const term&         get_rhs() const;
};

class ARROW_EXPORT deref_term
    : public ast_base<deref_term, details::deref_term_impl, term_mark>
{
    private:
        deref_term(const impl_ptr&);
        friend term;

    public:
        deref_term();
        deref_term(const location& loc, const term& t);

        const term&         get_term() const;
};

class ARROW_EXPORT cell_term
    : public ast_base<cell_term, details::cell_term_impl, term_mark>
{
    private:
        cell_term(const impl_ptr&);
        friend term;

    public:
        cell_term();
        cell_term(const location& loc, long index);

        long                get_index() const;
};

class ARROW_EXPORT identifier_term
    : public ast_base<identifier_term, details::identifier_term_impl, term_mark>
{
    private:
        identifier_term(const impl_ptr&);
        friend term;

    public:
        identifier_term();
        identifier_term(const location& loc, const identifier& t);

        const identifier&   get_identifier() const;
};

class ARROW_EXPORT overloaded_symbol
    : public ast_base<overloaded_symbol, details::overloaded_symbol_impl, term_mark>
{
    private:
        overloaded_symbol(const impl_ptr&);
        friend term;

    public:
        enum class arg_class
        {
            class_term, class_type, class_kind
        };

    public:
        overloaded_symbol();
        overloaded_symbol(const location& loc, const identifier_term& id, const type& func_ty,
                          const ast::type& ret_type, const overload_index* ind);

        void                    add_term_arg(const type& ty);
        void                    add_type_arg(const type& ty);
        void                    add_kind_arg(const kind& ty);

        const identifier_term&  get_identifier() const;
        const type&             get_type() const;

        long                    arg_size() const;
        arg_class               get_arg_class(long pos) const;
        const ast::type&        get_arg_type(long pos) const;
        const ast::kind&        get_arg_kind(long pos) const;
        
        const ast::type&        get_return_type() const;
        const overload_index*   get_index() const;
};

class ARROW_EXPORT unpack_term
    : public ast_base<unpack_term, details::unpack_term_impl, term_mark>
{
    public:
        /// mark used to create fresh objects
        struct fresh{};

    private:
        unpack_term(const impl_ptr&);
        friend term;

    public:
        unpack_term();

        unpack_term(const location& loc, const term& package, const annotated_type_identifier& type_id, 
                    const annotated_identifier& term_id, const term& expr, const symbol_data* sd);
        unpack_term(const location& loc, const term& package, const annotated_type_identifier& type_id, 
                    const annotated_identifier& term_id, const term& expr, fresh);

        const term&         get_package() const;
        const term&         get_term() const;
        const annotated_type_identifier&
                            get_type_identifier() const;
        const annotated_identifier&   
                            get_term_identifier() const;

        symbol_data*        get_symbol_data() const;

        /// update var type stored in the associated scope
        void                update_scope(const module& mod) const;
};

class ARROW_EXPORT let_term
    : public ast_base<let_term, details::let_term_impl, term_mark>
{
    public:
        /// mark used to create fresh objects
        struct fresh{};

    private:
        let_term(const impl_ptr&);
        friend term;

    public:
        let_term();
        let_term(const location& loc, const definitions& defs, const term& body,
                 const symbol_data* sd);
        let_term(const location& loc, const definitions& defs, const term& body,
                 fresh);

        const term&         get_body() const;
        const definitions&  get_definitions() const;
        symbol_data*        get_symbol_data() const;

        void                update_scope(const module& mod) const;
};

class ARROW_EXPORT lambda_term
    : public ast_base<lambda_term, details::lambda_term_impl, term_mark>
{
    public:
        /// mark used to create fresh objects
        struct fresh{};

    private:
        lambda_term(const impl_ptr&);
        friend term;

    public:
        lambda_term();
        lambda_term(const location& loc, const annotated_identifier& id, const term& t,
                    const symbol_data* sd);
        lambda_term(const location& loc, const annotated_identifier& id, const term& t,
                    fresh);

        const term&         get_body() const;
        const annotated_identifier&
                            get_var() const;
        symbol_data*        get_symbol_data() const;

        /// update var type stored in the associated scope
        void                update_scope(const module& mod) const;
};

class ARROW_EXPORT mu_term
    : public ast_base<mu_term, details::mu_term_impl, term_mark>
{
    public:
        /// mark used to create fresh objects
        struct fresh{};

    private:
        mu_term(const impl_ptr&);
        friend term;

    public:
        mu_term();
        mu_term(const location& loc, const annotated_identifier& id, const term& t,
                    const symbol_data* sd);
        mu_term(const location& loc, const annotated_identifier& id, const term& t,
                    fresh);

        const term&         get_body() const;
        const annotated_identifier&
                            get_var() const;
        symbol_data*        get_symbol_data() const;

        /// update var type stored in the associated scope
        void                update_scope(const module& mod) const;
};

class ARROW_EXPORT all_term
    : public ast_base<all_term, details::all_term_impl, term_mark>
{
    private:
        all_term(const impl_ptr&);
        friend term;

    public:
        all_term();
        all_term(const location& loc, const annotated_type_identifier& id, 
                         const term& t);

        const term&         get_body() const;
        const annotated_type_identifier&
                            get_var() const;

};

//rule: Kind[k] term
class ARROW_EXPORT term_kind_scope
    : public ast_base<term_kind_scope, details::term_kind_scope_impl, term_mark>
{
    private:
        term_kind_scope(const impl_ptr& ip);
        friend term;

    public:
        term_kind_scope();
        term_kind_scope(const location& loc, const identifier& ai, const term& t);

        const term&         get_body() const;
        const identifier&   get_var() const;
};

class ARROW_EXPORT term_type_application
    : public ast_base<term_type_application, details::term_type_application_impl, term_mark>
{
    private:
        term_type_application(const impl_ptr&);
        friend term;

    public:
        term_type_application();
        term_type_application(const location& loc, const term& ex, const type& ty);

        const term&         get_term() const;
        const type&         get_type() const;
};

class ARROW_EXPORT term_kind_application
    : public ast_base<term_kind_application, details::term_kind_application_impl, term_mark>
{
    private:
        term_kind_application(const impl_ptr&);
        friend term;

    public:
        term_kind_application();
        term_kind_application(const location& loc, const term& ex, const kind& ty);

        const term&         get_term() const;
        const kind&         get_kind() const;
};

class ARROW_EXPORT int_literal
    : public ast_base<int_literal, details::int_literal_impl, term_mark>
{
    private:
        int_literal(const impl_ptr&);
        friend term;

    public:
        int_literal();
        int_literal(const location& loc, int64_t val);

        int64_t             get_value() const;
};

class ARROW_EXPORT bool_literal
    : public ast_base<bool_literal, details::bool_literal_impl, term_mark>
{
    private:
        bool_literal(const impl_ptr&);
        friend term;

    public:
        bool_literal();
        bool_literal(const location& loc, bool val);

        bool            get_value() const;
};

class ARROW_EXPORT float_literal
    : public ast_base<float_literal, details::float_literal_impl, term_mark>
{
    private:
        float_literal(const impl_ptr&);
        friend term;

    public:
        float_literal();
        float_literal(const location& loc, double val);

        double          get_value() const;
};

class ARROW_EXPORT char_literal
    : public ast_base<char_literal, details::char_literal_impl, term_mark>
{
    private:
        char_literal(const impl_ptr&);
        friend term;

    public:
        char_literal();
        char_literal(const location& loc, int32_t val);

        int32_t         get_value() const;
};

class ARROW_EXPORT string_literal
    : public ast_base<string_literal, details::string_literal_impl, term_mark>
{
    private:
        string_literal(const impl_ptr&);
        friend term;

    public:
        string_literal();
        string_literal(const location& loc, const std::string& val);

        const std::string&  get_value() const;
};

class ARROW_EXPORT error_term
    : public ast_base<error_term, details::error_term_impl, term_mark>
{
    private:
        error_term(const impl_ptr&);
        friend term;

    public:
        error_term();
        error_term(const location& loc);
};

class ARROW_EXPORT tuple_term
    : public ast_base<tuple_term, details::tuple_term_impl, term_mark>
{
    private:
        tuple_term(const impl_ptr&);
        friend term;

    public:
        enum tuple_state
        {
            can_expand,     // expand 1-element tuple later
            fixed,          // 1-element tuple cannot be expanded
            dot,            // this is dot operator (i.e. f .(args) )
        };

    public:
        tuple_term();
        tuple_term(const location& loc, tuple_state state);

        void                add(const term& t);
        void                set_state(tuple_state state);

        long                size() const;
        const term&         get(long pos) const;
        tuple_state         get_state() const;
};

class ARROW_EXPORT term_sequence_item
{
    public:
        struct basic_tag{};
        struct proj_tag{};
        struct dot_tag{};
        struct type_applic_tag{};
        struct kind_applic_tag{};

    public:
        using type_vec          = std::vector<ast::type>;
        using kind_vec          = std::vector<ast::kind>;
        enum class item_type    { basic_item, proj_item, dot_item, type_applic_item, 
                                    kind_applic_item};

    private:
        location            m_loc;
        item_type           m_type;
        term                m_term;
        identifier          m_ident;
        int_literal         m_int_lit;
        type_vec            m_type_args;
        kind_vec            m_kind_args;

    public:
        term_sequence_item(const location& loc, const term& item, basic_tag);
        term_sequence_item(const location& loc, const int_literal& item, proj_tag);
        term_sequence_item(const location& loc, const identifier& item, dot_tag);
        term_sequence_item(const location& loc, const type_vec& item, type_applic_tag);
        term_sequence_item(const location& loc, const kind_vec& item, kind_applic_tag);

        item_type           get_item_type() const;
        const location&     get_location() const;

        const term&         get_basic() const;
        const int_literal&  get_projection() const;
        const identifier&   get_dot() const;
        const type_vec&     get_type_applic_args() const;
        const kind_vec&     get_kind_applic_args() const;
};

class ARROW_EXPORT term_sequence
    : public ast_base<tuple_term, details::term_sequence_impl, term_mark>
{
    private:
        term_sequence(const impl_ptr&);
        friend term;

    public:
        term_sequence();
        term_sequence(const location& loc);

        void                add(const location& loc, const term& t);
        void                add_projection(const location& loc, const ast::int_literal& pos);
        void                add_dot(const location& loc, const ast::identifier& id);
        void                add_type_application(const location& loc, 
                                const std::vector<ast::type>& args);
        void                add_kind_application(const location& loc, 
                                const std::vector<ast::kind>& args);

        long                size() const;
        const term_sequence_item&
                            get(long pos) const;
};

class ARROW_EXPORT list_term
    : public ast_base<list_term, details::list_term_impl, term_mark>
{
    private:
        list_term(const impl_ptr&);
        friend term;

    public:
        list_term();
        list_term(const location& loc);

        void                add(const term& t);

        long                size() const;
        const term&         get(long pos) const;
};

class ARROW_EXPORT bracket_term
    : public ast_base<bracket_term, details::bracket_term_impl, term_mark>
{
    private:
        bracket_term(const impl_ptr&);
        friend term;

    public:
        bracket_term();
        bracket_term(const location& loc);

        void                add(const term& t);
        void                set_brackets(bool is_dot, const identifier& open, 
                                const identifier& close);
        void                set_dot(bool is_dot);

        long                size() const;
        const term&         get(long pos) const;
        const identifier&   get_opening_bracket() const;
        const identifier&   get_closing_bracket() const;
        bool                is_dot() const;
};

class ARROW_EXPORT record_term
    : public ast_base<record_term, details::record_term_impl, term_mark>
{
    private:
        record_term(const impl_ptr&);
        friend term;

    public:
        record_term();
        record_term(const location& loc);

        void                add(const annotated_identifier& label, const term& t);

        long                size() const;
        const term&         get_term(long pos) const;
        const annotated_identifier&
                            get_label(long pos) const;

        bool                has_field(const identifier& label) const;
        term                get_field(const identifier& label) const;
        annotated_identifier
                            get_identifier(const identifier& label) const;
};

class ARROW_EXPORT extern_function_eval
{
    public:
        virtual ~extern_function_eval(){};      

        virtual term    eval(const term_constant& data) const = 0;
        virtual type    get_return_type() const = 0;
        virtual long    number_arguments() const = 0;
        virtual type    get_arg_type(long i) const = 0;
};

class ARROW_EXPORT term_constant
    : public ast_base<term_constant, details::term_constant_impl, term_mark>
{
    public:
        using extern_function_ptr   = std::shared_ptr<extern_function_eval>;

    private:
        term_constant(const impl_ptr&);
        friend term;

    public:
        term_constant();
        term_constant(const location& loc, const identifier& id, bool is_extern_func);
     
        void                add_label(const annotated_identifier& id, const term&);

        /// assign term to existing label
        void                set_label(const identifier& id, const term& t);
        void                set_function(const extern_function_ptr& func);

        /// set return type of this function
        void                set_return_type(const type& ki);

        long                var_size() const;

        const type&         get_return_type() const;
        const identifier&   get_name() const;
        const annotated_identifier& 
                            get_label(long pos) const;

        const extern_function_ptr&
                            get_function() const;
        bool                is_extern_func() const;

        /// can be null
        const term&         get_term(long pos) const;

        ast::term           eval() const;
};

enum class closure_kind
{
    identifier, overload, function
};

class ARROW_EXPORT closure_term 
    : public ast_base<closure_term, details::closure_term_impl, ast_mark>
{
    private:
        closure_term(const impl_ptr& ip);
        friend term;

    public:
        closure_term();
        closure_term(const location& loc, const term& body, closure_kind kind);

        void                add_item(const ast::term& subs, 
                                const ast::identifier& internal_id, const ast::type& ty);

        const term&         get_body() const;
        long                size() const;
        closure_kind        get_kind() const;

        const term&         get_substitution(long pos) const;
        const identifier&   get_internal_ident(long pos) const;
        const type&         get_type(long pos) const;        

        closure_term        change_body_func(const ast::term& new_body) const;
        closure_term        change_body(const ast::identifier_term& new_body) const;
        closure_term        change_body(const ast::overloaded_symbol& new_body) const;

        /// new body must be identifier or overloaded symbol
        closure_term        change_body(const ast::term& new_body, 
                                const error_handler_ptr& eh) const;
};

};};

#pragma warning(pop)
