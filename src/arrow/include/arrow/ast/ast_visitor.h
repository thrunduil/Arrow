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

#include "arrow/ast/ast.h"

namespace arrow { namespace ast
{

template<class Derived>
class ast_visitor
{
    public:
        template<class Ast, class ... Args>
        auto visit(const Ast& ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)); 

        template<class ... Args>
        auto visit(const term& ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...));

        template<class ... Args>
        auto visit(const type& ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...));

        template<class ... Args>
        auto visit(const definition& ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...));

        template<class ... Args>
        auto visit(const kind& ast, Args&& ... args)
            -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...));
};

template<class Derived>
struct rewrite_visitor : public ast_visitor<Derived>
{
    //---------------------------------------------------------------
    //                  VALUES
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(int32_t val, Args&& ... args)             -> int32_t;

    template<class ... Args>
    auto eval(const std::string& val, Args&& ... args)  -> std::string;

    template<class ... Args>
    auto eval(int64_t val, Args&& ... args)             -> int64_t;

    template<class ... Args>
    auto eval(bool val, Args&& ... args)                -> bool;

    template<class ... Args>
    auto eval(double val, Args&& ... args)              -> double;

    template<class ... Args>
    auto eval(unique_string val, Args&& ... args)       -> unique_string;

    template<class ... Args>
    auto eval(predefined_type_code val, Args&& ... args)-> predefined_type_code;

    template<class ... Args>
    auto eval(const operator_info& val, Args&& ... args)-> operator_info;

    template<class ... Args>
    auto eval(star_kind_state st, Args&& ... args)      -> star_kind_state;

    //---------------------------------------------------------------
    //                  TERMS
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const all_term& ast, Args&& ... args)
        -> all_term;

    template<class ... Args>
    auto eval(const term_type_application& ast, Args&& ... args)
        -> term_type_application;

    template<class ... Args>
    auto eval(const term_kind_application& ast, Args&& ... args)
        -> term_kind_application;

    template<class ... Args>
    auto eval(const tuple_term& ast, Args&& ... args)
        -> tuple_term;

    template<class ... Args>
    auto eval(const term_sequence& ast, Args&& ... args)
        -> term_sequence;

    template<class ... Args>
    auto eval(const list_term& ast, Args&& ... args)
        -> list_term;

    template<class ... Args>
    auto eval(const bracket_term& ast, Args&& ... args)
        -> bracket_term;

    template<class ... Args>
    auto eval(const record_term& ast, Args&& ... args)
        -> record_term;

    template<class ... Args>
    auto eval(const term_constant& ast, Args&& ... args)
        -> term_constant;

    template<class ... Args>
    auto eval(const int_literal& ast, Args&& ... args)
        -> int_literal;

    template<class ... Args>
    auto eval(const bool_literal& ast, Args&& ... args)
        -> bool_literal;

    template<class ... Args>
    auto eval(const float_literal& ast, Args&& ... args)
        -> float_literal;

    template<class ... Args>
    auto eval(const char_literal& ast, Args&& ... args)
        -> char_literal;

    template<class ... Args>
    auto eval(const string_literal& ast, Args&& ... args)
        -> string_literal;

    template<class ... Args>
    auto eval(const error_term& ast, Args&& ... args)
        -> error_term;

    template<class ... Args>
    auto eval(const assign_term& ast, Args&& ... args)
        -> assign_term;

    template<class ... Args>
    auto eval(const val_term& ast, Args&& ... args)
        -> val_term;

    template<class ... Args>
    auto eval(const ref_term& ast, Args&& ... args)
        -> ref_term;

    template<class ... Args>
    auto eval(const deref_term& ast, Args&& ... args)
        -> deref_term;

    template<class ... Args>
    auto eval(const cell_term& ast, Args&& ... args)
        -> cell_term;

    template<class ... Args>
    auto eval(const identifier_term& ast, Args&& ... args)
        -> identifier_term;

    template<class ... Args>
    auto eval(const overloaded_symbol& ast, Args&& ... args)
        -> overloaded_symbol;

    template<class ... Args>
    auto eval(const closure_term& ast, Args&& ... args)
        -> closure_term;

    template<class ... Args>
    auto eval(const annotated_term& ast, Args&& ... args)
        -> annotated_term;

    template<class ... Args>
    auto eval(const if_term& ast, Args&& ... args)
        -> if_term;

    template<class ... Args>
    auto eval(const term_application& ast, Args&& ... args)
        -> term_application;

    template<class ... Args>
    auto eval(const path_term& ast, Args&& ... args)
        -> path_term;

    template<class ... Args>
    auto eval(const path_int_term& ast, Args&& ... args)
        -> path_int_term;

    template<class ... Args>
    auto eval(const pack_term& ast, Args&& ... args)
        -> pack_term;

    template<class ... Args>
    auto eval(const variant_term& ast, Args&& ... args)
        -> variant_term;

    template<class ... Args>
    auto eval(const term_kind_scope& ast, Args&& ... args)
        -> term_kind_scope;

    //---------------------------------------------------------------
    //                  TYPES
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const arrow_type& ast, Args&& ... args)
        -> arrow_type;

    template<class ... Args>
    auto eval(const path_int_type& ast, Args&& ... args)
        -> path_int_type;

    template<class ... Args>
    auto eval(const ref_type& ast, Args&& ... args)
        -> ref_type;

    template<class ... Args>
    auto eval(const if_type& ast, Args&& ... args)
        -> if_type;

    template<class ... Args>
    auto eval(const list_type& ast, Args&& ... args)
        -> list_type;

    template<class ... Args>
    auto eval(const identifier_type& ast, Args&& ... args)
        -> identifier_type;

    template<class ... Args>
    auto eval(const some_type& ast, Args&& ... args)
        -> some_type;

    template<class ... Args>
    auto eval(const all_type& ast, Args&& ... args)
        -> all_type;

    template<class ... Args>
    auto eval(const lambda_type& ast, Args&& ... args)
        -> lambda_type;

    template<class ... Args>
    auto eval(const mu_type& ast, Args&& ... args)
        -> mu_type;

    template<class ... Args>
    auto eval(const variant_type& ast, Args&& ... args)
        -> variant_type;

    template<class ... Args>
    auto eval(const record_type& ast, Args&& ... args)
        -> record_type;

    template<class ... Args>
    auto eval(const type_constant& ast, Args&& ... args)
        -> type_constant;

    template<class ... Args>
    auto eval(const annotated_type& ast, Args&& ... args)
        -> annotated_type;

    template<class ... Args>
    auto eval(const tuple_type& ast, Args&& ... args)
        -> tuple_type;

    template<class ... Args>
    auto eval(const bracket_type& ast, Args&& ... args)
        -> bracket_type;

    template<class ... Args>
    auto eval(const type_application& ast, Args&& ... args)
        -> type_application;

    template<class ... Args>
    auto eval(const predefined_type& ast, Args&& ... args)
        -> predefined_type;

    template<class ... Args>
    auto eval(const type_var& ast, Args&& ... args)
        -> type;

    template<class ... Args>
    auto eval(const product_of_types& ast, Args&& ... args)
        -> product_of_types;

    template<class ... Args>
    auto eval(const type_sequence& ast, Args&& ... args)
        -> type_sequence;

    template<class ... Args>
    auto eval(const error_type& ast, Args&& ... args)
        -> error_type;

    template<class ... Args>
    auto eval(const int_type& ast, Args&& ... args)
        -> int_type;

    template<class ... Args>
    auto eval(const bool_type& ast, Args&& ... args)
        -> bool_type;

    template<class ... Args>
    auto eval(const char_type& ast, Args&& ... args)
        -> char_type;

    template<class ... Args>
    auto eval(const string_type& ast, Args&& ... args)
        -> string_type;

    template<class ... Args>
    auto eval(const type_kind_scope& ast, Args&& ... args)
        -> type_kind_scope;

    template<class ... Args>
    auto eval(const type_kind_application& ast, Args&& ... args)
        -> type_kind_application;

    template<class ... Args>
    auto eval(const type_type_application& ast, Args&& ... args)
        -> type_type_application;

    //---------------------------------------------------------------
    //                  DEFINITIONS
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const term_definition& ast, Args&& ... args)
        -> term_definition;

    template<class ... Args>
    auto eval(const fixity_definition& ast, Args&& ... args)
        -> fixity_definition;

    template<class ... Args>
    auto eval(const type_definition& ast, Args&& ... args)
        -> type_definition;

    template<class ... Args>
    auto eval(const type_definition_var& ast, Args&& ... args)
        -> type_definition_var;

    template<class ... Args>
    auto eval(const term_use& ast, Args&& ... args)
        -> term_use;

    //---------------------------------------------------------------
    //                  KINDS
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const star_kind& ast, Args&& ... args)
        -> star_kind;

    template<class ... Args>
    auto eval(const product_kind& ast, Args&& ... args)
        -> product_kind;

    template<class ... Args>
    auto eval(const star_tuple_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const arrow_kind& ast, Args&& ... args)
        -> arrow_kind;

    template<class ... Args>
    auto eval(const star_arrow_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const value_kind& ast, Args&& ... args)
        -> value_kind;

    template<class ... Args>
    auto eval(const kind_identifier& ast, Args&& ... args)
        -> kind_identifier;

    template<class ... Args>
    auto eval(const kind_scope& ast, Args&& ... args)
        -> kind_scope;

    template<class ... Args>
    auto eval(const star_all_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_some_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_list_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_record_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_variant_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_predefined_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const star_ref_kind& ast, Args&& ... args)
        -> kind;

    template<class ... Args>
    auto eval(const kind_var& ast, Args&& ... args)
        -> kind;

    //---------------------------------------------------------------
    //                  OTHER
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const module_definition& seq, Args&& ... args)
        -> module_definition;

    template<class ... Args>
    auto eval(const definitions& seq, Args&& ... args)
        -> definitions;

    template<class ... Args>
    auto eval(const identifier& seq, Args&& ... args)
        -> identifier;

    template<class ... Args>
    auto eval(const annotated_type_identifier& seq, Args&& ... args)
        -> annotated_type_identifier;

    template<class ... Args>
    auto eval(const annotated_identifier& seq, Args&& ... args)
        -> annotated_identifier;

    template<class ... Args>
    auto eval(const symbol_sequence& ast, Args&& ... args)
                -> symbol_sequence;

    //---------------------------------------------------------------
    //                  NULL VALUES
    //---------------------------------------------------------------
    template<class ... Args>
    auto eval(const type& ast, Args&& ... args)
                -> type;

    template<class ... Args>
    auto eval(const term& ast, Args&& ... args)
                -> term;

    template<class ... Args>
    auto eval(const definition& ast, Args&& ... args)
                -> definition;

    template<class ... Args>
    auto eval(const kind& ast, Args&& ... args)
                -> kind;

    //---------------------------------------------------------------
    //                  DELETED FUNCTIONS
    //---------------------------------------------------------------
    // for these functions scopes must be updated (types are stored in
    // local scopes, similarly definitions in let term)

    template<class ... Args>
    auto eval(const lambda_term& ast, Args&& ... args)
        -> lambda_term = delete;

    template<class ... Args>
    auto eval(const mu_term& ast, Args&& ... args)
        -> mu_term = delete;

    template<class ... Args>
    auto eval(const let_term& ast, Args&& ... args)
        -> let_term = delete;

    template<class ... Args>
    auto eval(const unpack_term& ast, Args&& ... args)
        -> unpack_term = delete;

    template<class ... Args>
    auto eval(const variant_case_term& ast, Args&& ... args)
        -> variant_case_term = delete;

    // default implementations, that do not update scopes
    template<class ... Args>
    auto eval_lambda(const lambda_term& ast, Args&& ... args)
        -> lambda_term;

    template<class ... Args>
    auto eval_mu(const mu_term& ast, Args&& ... args)
        -> mu_term;

    template<class ... Args>
    auto eval_unpack(const unpack_term& ast, Args&& ... args)
        -> unpack_term;

    template<class ... Args>
    auto eval_case(const variant_case_term& ast, Args&& ... args)
        -> variant_case_term;

    template<class ... Args>
    auto eval_let(const let_term& ast, Args&& ... args)
        -> let_term;
};

template<class Derived>
struct traversal_visitor : public ast_visitor<Derived>
{
    //---------------------------------------------------------------
    //                  VALUES
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(int32_t val, Args&& ... args);

    template<class ... Args>
    void eval(const std::string& val, Args&& ... args);

    template<class ... Args>
    void eval(int64_t val, Args&& ... args);

    template<class ... Args>
    void eval(bool val, Args&& ... args);

    template<class ... Args>
    void eval(double val, Args&& ... args);

    template<class ... Args>
    void eval(unique_string val, Args&& ... args);

    template<class ... Args>
    void eval(predefined_type_code val, Args&& ... args);    

    template<class ... Args>
    void eval(const operator_info& val, Args&& ... args);

    //---------------------------------------------------------------
    //                  TERMS
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const unpack_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const lambda_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const mu_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const all_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_type_application& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_kind_application& ast, Args&& ... args);

    template<class ... Args>
    void eval(const tuple_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_sequence& ast, Args&& ... args);

    template<class ... Args>
    void eval(const list_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const bracket_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const record_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_constant& ast, Args&& ... args);

    template<class ... Args>
    void eval(const int_literal& ast, Args&& ... args);

    template<class ... Args>
    void eval(const bool_literal& ast, Args&& ... args);

    template<class ... Args>
    void eval(const float_literal& ast, Args&& ... args);

    template<class ... Args>
    void eval(const char_literal& ast, Args&& ... args);

    template<class ... Args>
    void eval(const string_literal& ast, Args&& ... args);

    template<class ... Args>
    void eval(const error_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const val_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const ref_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const assign_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const deref_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const cell_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const identifier_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const overloaded_symbol& ast, Args&& ... args);

    template<class ... Args>
    void eval(const closure_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const annotated_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const if_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_application& ast, Args&& ... args);

    template<class ... Args>
    void eval(const let_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const path_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const path_int_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const pack_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const variant_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const variant_case_term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_kind_scope& ast, Args&& ... args);

    //---------------------------------------------------------------
    //                  TYPES
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const arrow_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const ref_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const if_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const path_int_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const list_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const identifier_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const some_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const all_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const lambda_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const mu_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const variant_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const record_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_constant& ast, Args&& ... args);

    template<class ... Args>
    void eval(const annotated_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const tuple_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const bracket_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_application& ast, Args&& ... args);

    template<class ... Args>
    void eval(const predefined_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_var& ast, Args&& ... args);

    template<class ... Args>
    void eval(const product_of_types& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_sequence& ast, Args&& ... args);

    template<class ... Args>
    void eval(const error_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const int_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const bool_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const char_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const string_type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_kind_scope& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_kind_application& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_type_application& ast, Args&& ... args);

    //---------------------------------------------------------------
    //                  DEFINITIONS
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const term_definition& ast, Args&& ... args);

    template<class ... Args>
    void eval(const fixity_definition& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_definition& ast, Args&& ... args);

    template<class ... Args>
    void eval(const type_definition_var& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term_use& ast, Args&& ... args);

    //---------------------------------------------------------------
    //                  KIND
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const star_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const product_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_tuple_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const arrow_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_arrow_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const value_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const kind_identifier& ast, Args&& ... args);

    template<class ... Args>
    void eval(const kind_scope& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_all_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_some_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_list_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_record_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_variant_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_predefined_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const star_ref_kind& ast, Args&& ... args);

    template<class ... Args>
    void eval(const kind_var& ast, Args&& ... args);

    //---------------------------------------------------------------
    //                  OTHER
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const module_definition& seq, Args&& ... args);

    template<class ... Args>
    void eval(const definitions& seq, Args&& ... args);

    template<class ... Args>
    void eval(const identifier& seq, Args&& ... args);

    template<class ... Args>
    void eval(const annotated_type_identifier& seq, Args&& ... args);

    template<class ... Args>
    void eval(const annotated_identifier& seq, Args&& ... args);

    template<class ... Args>
    void eval(const symbol_sequence& ast, Args&& ... args);

    //---------------------------------------------------------------
    //                  NULL VALUES
    //---------------------------------------------------------------
    template<class ... Args>
    void eval(const type& ast, Args&& ... args);

    template<class ... Args>
    void eval(const term& ast, Args&& ... args);

    template<class ... Args>
    void eval(const definition& ast, Args&& ... args);

    template<class ... Args>
    void eval(const kind& ast, Args&& ... args);
};

};};

#include "arrow/details/ast_visitor.inl"
#include "arrow/details/rewrite_visitor.inl"
#include "arrow/details/traversal_visitor.inl"
