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

#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace ast
{

#define REWRITE_EVAL_TERM_CASE(ast_type)                                                \
case ast::details::term_code::ast_type:                                                 \
    return static_cast<Derived*>(this)                                                  \
        ->eval(ast.static_cast_to<ast_type>(), std::forward<Args>(args)...);            \

#define REWRITE_EVAL_TYPE_CASE(ast_type)                                                \
case ast::details::type_code::ast_type:                                                 \
    return static_cast<Derived*>(this)                                                  \
        ->eval(ast.static_cast_to<ast_type>(), std::forward<Args>(args)...);            \

#define REWRITE_EVAL_DEF_CASE(ast_type)                                                 \
case ast::details::definition_code::ast_type:                                           \
    return static_cast<Derived*>(this)                                                  \
        ->eval(ast.static_cast_to<ast_type>(), std::forward<Args>(args)...);            \

#define REWRITE_EVAL_KIND_CASE(ast_type)                                                \
case ast::details::kind_code::ast_type:                                                 \
    return static_cast<Derived*>(this)                                                  \
        ->eval(ast.static_cast_to<ast_type>(), std::forward<Args>(args)...);            \

template<class Derived>
template<class Ast, class ... Args>
auto ast_visitor<Derived>::visit(const Ast& ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    return static_cast<Derived*>(this)
        ->eval(ast, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
auto ast_visitor<Derived>::visit(const term& ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    if (!ast)
    {
        return static_cast<Derived*>(this)
            ->eval(ast, std::forward<Args>(args)...);
    };

    switch (ast->get_term_code())
    {
        REWRITE_EVAL_TERM_CASE(unpack_term)
        REWRITE_EVAL_TERM_CASE(lambda_term)
        REWRITE_EVAL_TERM_CASE(mu_term)
        REWRITE_EVAL_TERM_CASE(all_term)
        REWRITE_EVAL_TERM_CASE(term_type_application)
        REWRITE_EVAL_TERM_CASE(term_kind_application)
        REWRITE_EVAL_TERM_CASE(tuple_term)
        REWRITE_EVAL_TERM_CASE(term_sequence)
        REWRITE_EVAL_TERM_CASE(list_term)
        REWRITE_EVAL_TERM_CASE(bracket_term)
        REWRITE_EVAL_TERM_CASE(record_term)
        REWRITE_EVAL_TERM_CASE(term_constant)
        REWRITE_EVAL_TERM_CASE(int_literal)
        REWRITE_EVAL_TERM_CASE(bool_literal)
        REWRITE_EVAL_TERM_CASE(float_literal)
        REWRITE_EVAL_TERM_CASE(char_literal)
        REWRITE_EVAL_TERM_CASE(string_literal)
        REWRITE_EVAL_TERM_CASE(error_term)
        REWRITE_EVAL_TERM_CASE(ref_term)
        REWRITE_EVAL_TERM_CASE(assign_term)
        REWRITE_EVAL_TERM_CASE(deref_term)
        REWRITE_EVAL_TERM_CASE(cell_term)
        REWRITE_EVAL_TERM_CASE(identifier_term)
        REWRITE_EVAL_TERM_CASE(overloaded_symbol)
        REWRITE_EVAL_TERM_CASE(closure_term)
        REWRITE_EVAL_TERM_CASE(annotated_term)
        REWRITE_EVAL_TERM_CASE(if_term)
        REWRITE_EVAL_TERM_CASE(term_application)
        REWRITE_EVAL_TERM_CASE(let_term)
        REWRITE_EVAL_TERM_CASE(path_term)
        REWRITE_EVAL_TERM_CASE(path_int_term)
        REWRITE_EVAL_TERM_CASE(pack_term)
        REWRITE_EVAL_TERM_CASE(variant_term)
        REWRITE_EVAL_TERM_CASE(variant_case_term)
        REWRITE_EVAL_TERM_CASE(val_term)
        REWRITE_EVAL_TERM_CASE(term_kind_scope)
    };

    throw std::runtime_error("invalid expr code");
};

template<class Derived>
template<class ... Args>
auto ast_visitor<Derived>::visit(const type& ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    if (!ast)
    {
        return static_cast<Derived*>(this)
            ->eval(ast, std::forward<Args>(args)...);
    };

    switch (ast->get_type_code())
    {
        REWRITE_EVAL_TYPE_CASE(arrow_type)
        REWRITE_EVAL_TYPE_CASE(ref_type)
        REWRITE_EVAL_TYPE_CASE(if_type)
        REWRITE_EVAL_TYPE_CASE(path_int_type)
        REWRITE_EVAL_TYPE_CASE(list_type)
        REWRITE_EVAL_TYPE_CASE(identifier_type)
        REWRITE_EVAL_TYPE_CASE(some_type)
        REWRITE_EVAL_TYPE_CASE(all_type)
        REWRITE_EVAL_TYPE_CASE(lambda_type)
        REWRITE_EVAL_TYPE_CASE(mu_type)
        REWRITE_EVAL_TYPE_CASE(variant_type)
        REWRITE_EVAL_TYPE_CASE(record_type)
        REWRITE_EVAL_TYPE_CASE(type_constant)
        REWRITE_EVAL_TYPE_CASE(tuple_type)
        REWRITE_EVAL_TYPE_CASE(bracket_type)
        REWRITE_EVAL_TYPE_CASE(annotated_type)
        REWRITE_EVAL_TYPE_CASE(type_application)
        REWRITE_EVAL_TYPE_CASE(predefined_type)
        REWRITE_EVAL_TYPE_CASE(type_var)
        REWRITE_EVAL_TYPE_CASE(product_of_types)
        REWRITE_EVAL_TYPE_CASE(type_sequence)
        REWRITE_EVAL_TYPE_CASE(error_type)
        REWRITE_EVAL_TYPE_CASE(int_type)
        REWRITE_EVAL_TYPE_CASE(bool_type)
        REWRITE_EVAL_TYPE_CASE(char_type)
        REWRITE_EVAL_TYPE_CASE(string_type)
        REWRITE_EVAL_TYPE_CASE(type_kind_scope)
        REWRITE_EVAL_TYPE_CASE(type_kind_application)
        REWRITE_EVAL_TYPE_CASE(type_type_application)
    };

    throw std::runtime_error("invalid type code");
};

template<class Derived>
template<class ... Args>
auto ast_visitor<Derived>::visit(const definition& ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    if (!ast)
    {
        return static_cast<Derived*>(this)
            ->eval(ast, std::forward<Args>(args)...);
    };

    switch (ast->get_def_code())
    {
        REWRITE_EVAL_DEF_CASE(term_definition)
        REWRITE_EVAL_DEF_CASE(fixity_definition)
        REWRITE_EVAL_DEF_CASE(type_definition)
        REWRITE_EVAL_DEF_CASE(type_definition_var)
        REWRITE_EVAL_DEF_CASE(term_use)
    };

    throw std::runtime_error("invalid type code");
};

template<class Derived>
template<class ... Args>
auto ast_visitor<Derived>::visit(const kind& ast, Args&& ... args)
    -> decltype(std::declval<Derived>().eval(ast, std::forward<Args>(args)...)) 
{
    if (!ast)
    {
        return static_cast<Derived*>(this)
            ->eval(ast, std::forward<Args>(args)...);
    };

    switch (ast->get_kind_code())
    {
        REWRITE_EVAL_KIND_CASE(star_kind)
        REWRITE_EVAL_KIND_CASE(arrow_kind)
        REWRITE_EVAL_KIND_CASE(star_arrow_kind)
        REWRITE_EVAL_KIND_CASE(product_kind)
        REWRITE_EVAL_KIND_CASE(star_tuple_kind)
        REWRITE_EVAL_KIND_CASE(kind_var)
        REWRITE_EVAL_KIND_CASE(value_kind)
        REWRITE_EVAL_KIND_CASE(kind_identifier)
        REWRITE_EVAL_KIND_CASE(kind_scope)
        REWRITE_EVAL_KIND_CASE(star_all_kind)
        REWRITE_EVAL_KIND_CASE(star_some_kind)
        REWRITE_EVAL_KIND_CASE(star_list_kind)
        REWRITE_EVAL_KIND_CASE(star_record_kind)
        REWRITE_EVAL_KIND_CASE(star_variant_kind)
        REWRITE_EVAL_KIND_CASE(star_predefined_kind)
        REWRITE_EVAL_KIND_CASE(star_ref_kind)
    };

    throw std::runtime_error("invalid type code");
};

};};
