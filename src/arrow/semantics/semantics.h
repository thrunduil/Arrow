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

#include "arrow/config.h"
#include "arrow/error_handler.h"
#include "scopes/module.h"

#include <map>
#include <list>

namespace arrow { namespace sema
{

void    check_constraints(const ast::definitions& ast, const error_handler_ptr& eh);

void    collect_symbol_definitions(const ast::definitions& ast, const module& mod, 
            const error_handler_ptr& eh);
void    collect_operator_declarations(const ast::definitions& ast, const module& mod, 
            const error_handler_ptr& eh);

void    build_scopes(const module& mod, const error_handler_ptr& eh);

void    build_type_scopes(const module& mod, const error_handler_ptr& eh, const ast::type& tc, 
            const ast::kind& k, const arrow::details::type_var_scope& type_scope,
            const arrow::details::type_var_scope& kind_scope);

void    type_desugaring(const module& mod, const error_handler_ptr& eh, ast::type& t);
void    desugaring(module& mod, const error_handler_ptr& eh);
void    build_closures(module& mod, const error_handler_ptr& eh);

void    build_type_dependency_graph(const module& mod, const error_handler_ptr& eh, 
            const ad::type_table& tt);
void    build_dependecy_graph(const module& mod, const error_handler_ptr& eh);

void    type_check_types(module& mod, const error_handler_ptr& eh);
void    type_check_terms(module& mod, const error_handler_ptr& eh);

void    eval(module& mod, const error_handler_ptr& eh, std::vector<ast::term>& evaled);

void    print_kind(std::ostream& os, const ast::kind& k);
void    print_type(std::ostream& os, const ast::type& ty);
void    print_term(std::ostream& os, const ast::term& ty, const error_handler_ptr& eh,
            const module& mod);
void    print_type_kind(std::ostream& os, const ast::type& ty, const ast::kind& ki);
void    print_predefined_type(std::ostream& os, ast::predefined_type_code tc);

/// perform decomposition ty := Kind[args ...] body
void        decompose_type(const ast::type& ty, std::vector<ast::identifier>& args, 
                ast::type& body);
void        decompose_term(const ast::term& ex, std::vector<ast::identifier>& args, 
                ast::term& body);
/// inverse of decompose_term
ast::term   compose_term_kind(const ast::term& ex, const std::vector<ast::identifier>& args);

/// perform decomposition ty := All[args ...] body
void        decompose_all_type(const ast::type& ty, std::vector<ast::annotated_type_identifier>& args, 
                ast::type& body);

/// return true if ty contains any unset type variables
bool        has_type_vars(const ast::type& ty);

/// return true if ty is an unset type variable or a kind abstraction
/// of an unset type variable
bool        is_type_var(const ast::type& t);

/// return true if kind k has unset kind variables
bool        has_kind_vars(const ast::kind& k);

/// if tv is a type variables and has substitution then return stores
/// substitution, otherwise return tv;
ast::type   expand_type_var(const ast::type& tv);

/// remove top level kind variables, return empty kind if
/// k is uninitialized kind variable
ast::kind   expand_kind_vars(const ast::kind& k);

/// make independent copy of every identifier
ast::term   copy_identifiers(const ast::term& t);
ast::type   copy_identifiers(const ast::type& t);
ast::kind   copy_identifiers(const ast::kind& t);

/// assign new def_index to all binders
ast::type   make_fresh_binders(const ast::type& t, const error_handler_ptr& eh,
                const module& mod);

/// create fresh binder based on existing one
/// in debugging mode name of the binder can change if can_change_name = true
ast::identifier make_new_binder(const ast::identifier& id, const module& mod,
                    bool can_change_name);

/// for a type Rec[X1] ... Rec[Xn] F where
/// F is not a mu type return n
int         measure_height(const ast::type& mu);

/// return node type at head position
ast::details::type_code
            get_head_type(const ast::type& ty);

/// return true if any variable in vars set is a free variable in type ty
/// it is assumed, that variables in vars set are not abstraction variables
/// used in All, Some, For, Rec, etc. types
bool        is_free_var(const std::set<ad::var_info>& vars, const ast::type& ty);

/// return all free variables in given type ty
void        free_vars(const ast::type& ty, std::set<ad::var_info>& vars);
/// return all free type variables in given term
//bool      has_free_vars(const ast::term& ty);

/// return true if type ty does not contain free type variables
/// if true is returned, then if kind_closed is true, then all
/// kinds are closed; if free_vars is true, then there exist unset
/// kind vars or type vars
bool        is_closed_type(const ast::type& ty, bool& kind_closed, bool& free_vars);
bool        is_closed_kind(const ast::kind& ty, bool& free_vars);

/// collect term, type and kind arguments
void        uncurry_application(const ast::term& ast, 
                std::vector<typing::details::applic_arg_term>& args, ast::term& head);

void        uncurry_application(const ast::type& ast, 
                std::vector<typing::details::applic_arg_type>& args, ast::type& head);

/// return number of arguments, that can be applied to a term with this type
/// it is assumed that t is in normal form
/// if the return argument infinite is true, then infinite number of arguments
/// is possible; if type_dependent is true, then function may take additional
/// arguments depending on context
long        get_number_arguments(const ast::type& t, bool& infinite, bool& type_dependent);

/// make independent copy of an identifier; symbol data are also copied;
/// note: symbol data must be updated if this function is called after scope 
/// building
ast::identifier copy(const ast::identifier& id);

/// finalize infered kinds, fix size of product kind, fix star kinds 
ast::type   close_type(const ast::type& ty, const error_handler_ptr& eh);
ast::kind   close_kind(const ast::kind& ki, const error_handler_ptr& eh,
                bool kindvar_allowed);

/// make all star kinds flexible
ast::kind   make_star_kind_flexible(const ast::kind& k);

};};
