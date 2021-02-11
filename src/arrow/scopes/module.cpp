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

#include "module.h"
#include "module_impl.h"
#include "error/error.h"
#include "type_var_scope.h"

#include <map>
#include <set>
#include <iostream>

namespace arrow
{

namespace ae = arrow::error;

//-------------------------------------------------------------------
//                   module   
//-------------------------------------------------------------------

module::module()
{};

module::module(ast::unique_string mod_name, details::module_index mi, 
               const owner_ptr& owner, bool is_shell_mod)
    :m_impl(new details::module_impl(mod_name, mi, owner, is_shell_mod))
{
    m_impl->init(*this);
};

bool module::is_shell_module() const
{
    return m_impl->is_shell_module();
}

void module::finish_building() const
{
    return m_impl->finish_building();
}

void module::add_imports(const std::vector<import_info>& imports)
{
    m_impl->add_imports(imports);
}
void module::add_ast(const ast::definitions& defs)
{
    m_impl->add_ast(defs);
}

void module::collect_operator_declarations(const error_handler_ptr& eh)
{
    return m_impl->collect_operator_declarations(eh, *this);
};

void module::collect_symbol_definitions(const error_handler_ptr& eh)
{
    return m_impl->collect_symbol_definitions(eh, *this);
};

void module::add_import_edges(import_graph& graph)
{
    return m_impl->add_import_edges(graph);
};

void module::make_imports(const details::modules_provider& mods, std::set<string_loc>& undefined, 
                          const error_handler_ptr& eh)
{
    m_impl->make_imports(mods, undefined, eh);
};

void module::remove_cyclical_data() const
{
    m_impl->remove_cyclical_data();
};

bool module::is_null() const
{
    return m_impl == nullptr;
};

const graph& module::get_graph() const
{
    return m_impl->get_graph();
}

details::module_index module::get_module_index() const
{
    return m_impl->get_module_index();
}

void module::add_operator_declaration(const ast::identifier& vi, 
                        const ast::operator_info& fix, const error_handler_ptr& eh)
{
    return m_impl->add_operator_declaration(vi, fix, eh);
};

void module::add_definition(const ast::identifier& vi, 
            const details::symbol_definition_env& def, const error_handler_ptr& eh)
{
    return m_impl->add_definition(vi, def, eh);
}

void module::add_instantiation_term_type(const ast::identifier_term& id, const ast::term_definition& td,
                               const std::vector<ast::type>& args, bool true_inst)
{    
    return m_impl->add_instantiation_term_type(id, td, args, true_inst);
}

void module::add_instantiation_type_type(const ast::identifier_type& id, const ast::type_definition& td,
                               const std::vector<ast::type>& args, bool true_inst)
{    
    return m_impl->add_instantiation_type_type(id, td, args, true_inst);
}

void module::register_local_function_subst(const ast::term_definition& td)
{    
    return m_impl->register_local_function_subst(td);
}

void module::add_instantiation_term_kind(const ast::identifier_term& id, const ast::term_definition& td,
                               const std::vector<ast::kind>& args, bool true_inst)
{    
    return m_impl->add_instantiation_term_kind(id, td, args, true_inst);
}

void module::add_instantiation_type_kind(const ast::identifier_type& id, const ast::type_definition& td,
                               const std::vector<ast::kind>& args, bool true_inst)
{    
    return m_impl->add_instantiation_type_kind(id, td, args, true_inst);
}

ast::identifier module::get_instantiation_term_type(const ast::identifier_term& base_id, 
                        const std::vector<ast::type>& args, bool& has, ad::type_comparer& comp, 
                        long& num_matched)
{
    return m_impl->get_instantiation_term_type(base_id, args, has, comp, num_matched);
};

ast::identifier module::get_instantiation_type_type(const ast::identifier_type& base_id, 
                        const std::vector<ast::type>& args, bool& has, ad::type_comparer& comp, 
                        long& num_matched)
{
    return m_impl->get_instantiation_type_type(base_id, args, has, comp, num_matched);
};

ast::identifier module::get_instantiation_type_kind(const ast::identifier_type& base_id, 
                        const std::vector<ast::kind>& args, bool& has, ad::kind_comparer& comp, 
                        long& num_matched)
{
    return m_impl->get_instantiation_type_kind(base_id, args, has, comp, num_matched);
};

ast::identifier module::get_instantiation_term_kind(const ast::identifier_term& base_id, 
                        const std::vector<ast::kind>& args, bool& has, ad::kind_comparer& comp, 
                        long& num_matched)
{
    return m_impl->get_instantiation_term_kind(base_id, args, has, comp, num_matched);
};

void module::add_overload(const ast::identifier& id, const ast::type& ty, const error_handler_ptr& eh)
{
    return m_impl->add_overload(id, ty, eh);
};

void module::add_type_definition(const ast::type_definition& vi, const error_handler_ptr& eh)
{
    return m_impl->add_type_definition(vi, eh);
}

void module::add_type_definition(const ast::type_definition_var& vi, const error_handler_ptr& eh)
{
    return m_impl->add_type_definition(vi, eh);
}

void module::add_eval_term(const ast::term_use& vi, const error_handler_ptr& eh)
{
    return m_impl->add_eval_term(vi, eh);
}

ast::unique_string module::get_module_name() const
{
    return m_impl->get_module_name();
};

void module::build_type_scopes(const error_handler_ptr& eh)
{
    return m_impl->build_type_scopes(*this, eh);
}

void module::type_desugaring(const error_handler_ptr& eh)
{
    return m_impl->type_desugaring(*this, eh);
}

void module::build_type_dependency_graph(const error_handler_ptr& eh)
{
    return m_impl->build_type_dependency_graph(*this, eh);
}


ad::type_var_scope module::new_typevar_scope(const ast::location& loc) const
{
    return m_impl->new_typevar_scope(loc);
}
ad::type_var_scope module::new_kindvar_scope(const ast::location& loc) const
{
    return m_impl->new_kindvar_scope(loc);
}

ad::type_var_scope module::make_new_var_scope(const ast::location& loc, 
                                const ad::type_var_scope& parent) const
{
    return m_impl->make_new_var_scope(loc, parent);
}

ad::def_index module::get_type_definition_index(ast::unique_string ti, 
                    bool search_imports, ast::location& loc, bool& found, 
                    loc_vec& ambiguous_locations) const
{
    return m_impl->get_type_definition_index(ti, search_imports, loc, 
                            found, ambiguous_locations);
}

const module::overload_type_set&
module::get_type_definition(ast::unique_string ti, bool search_imports, 
                        bool& found, loc_vec& ambiguous_locations) const
{
    return m_impl->get_type_definition(ti, search_imports, found, ambiguous_locations);
}

const local_scope& module::get_this_local_scope() const
{
    return m_impl->get_this_local_scope();
};

bool module::is_symbol_defined(const ast::identifier& vi, bool search_imports,
                               std::vector<ast::location>& ambig) const
{
    return m_impl->is_symbol_defined(vi, search_imports, ambig);
}

bool module::is_any_operator(ast::unique_string vi, bool search_imports) const
{
    return m_impl->is_any_operator(vi, search_imports);
}
bool module::is_overloaded(const ast::unique_string id) const
{
    return m_impl->is_overloaded(id);
};

bool module::check_symbol(const ast::identifier& id, const error_handler_ptr& eh) const
{
    return m_impl->check_symbol(id, eh);
};

bool module::is_operator_declared(ast::unique_string vi, ast::fixity_type ft,
                        bool search_imports) const
{
    return m_impl->is_operator_declared(vi, ft, search_imports);
}

const module::fun_sym_map& module::get_function_map() const
{
    return m_impl->get_function_map();
};

const module::term_list& module::get_eval_terms() const
{
    return m_impl->get_eval_terms();
}
module::term_list& module::get_eval_terms_modify() const
{
    return m_impl->get_eval_terms_modify();
}

module::fun_sym_map& module::get_function_map_modify()
{
    return m_impl->get_function_map_modify();
};

ast::kind module::get_type_constructor_kind(const error_handler_ptr& eh, 
                const ast::identifier_type& id, bool& overloaded, bool& has_errors) const
{
    return m_impl->get_type_constructor_kind(eh, id, overloaded, has_errors);
}

term_symbol_kind module::get_term_symbol_kind(const ast::identifier_term& id) const
{
    return m_impl->get_term_symbol_kind(id);
};
type_symbol_kind module::get_type_symbol_kind(const ast::identifier_type& id) const
{
    return m_impl->get_type_symbol_kind(id);
};

bool module::is_overloaded_term_def(const ast::identifier_term& id, bool& has_error) const
{
    return m_impl->is_overloaded_term_def(id, has_error);
}
bool module::is_overloaded_type_def(const ast::identifier_type& id, bool& has_error) const
{
    return m_impl->is_overloaded_type_def(id, has_error);
}

ast::type module::get_symbol_type(const error_handler_ptr& eh, const ast::identifier_term& id, 
                                  bool& overloaded, bool& has_errors) const
{
    return m_impl->get_symbol_type(eh, id, overloaded, has_errors);
}
ast::term module::get_symbol_term(const error_handler_ptr& eh, const ast::identifier_term& id, 
                                  bool& overloaded, bool& has_errors) const
{
    return m_impl->get_symbol_term(eh, id, overloaded, has_errors);
}
const module::overload_set* 
module::get_overload_set(const error_handler_ptr& eh, 
                                const ast::identifier_term& id) const
{
    return m_impl->get_overload_set(eh, id);
};
const module::overload_set* 
module::get_overload_set(const ast::identifier& id) const
{
    return m_impl->get_overload_set(id);
};

const module::overload_type_set* 
module::get_overload_type_set(const ast::identifier& id) const
{
    return m_impl->get_overload_type_set(id);
};

ast::overload_index module::get_overload_index(const overload_set* os) const
{
    return m_impl->get_overload_index(os);
};
const module::overload_set* module::get_overload_set(const ast::overload_index* oi) const
{
    return m_impl->get_overload_set(oi);
};

ast::type module::get_type_constructor_definition(const error_handler_ptr& eh, 
                        const ast::identifier_type& id, bool& overloaded, bool& has_errors) const
{
    return m_impl->get_type_constructor_definition(eh, id, overloaded, has_errors);
};

module::extern_function_ptr module::get_extern_function_def(const error_handler_ptr& eh, 
                        const ast::identifier& id) const
{
    return m_impl->get_extern_function_def(eh, id);
};

local_scope module::create_child_scope(const details::local_scope_impl& parent) const
{
    return m_impl->create_child_scope(parent);
};

const module::overload_set& 
module::get_symbol_definition(ast::unique_string str, bool search_imports, 
                            bool& found, loc_vec& ambiguous_locations) const
{
    return m_impl->get_symbol_definition(str, search_imports, found, ambiguous_locations);
}

oper::operator_declaration module::get_operator_declaration(ast::unique_string str, ast::fixity_type ft,
            bool search_imports, bool& found, std::vector<ast::location>& ambig_locations) const
{
    return m_impl->get_operator_declaration(str, ft, search_imports, found, ambig_locations);
};

void module::get_all_type_definition(ast::unique_string ti, bool search_imports, 
                        std::vector<const overload_type_set*>& defs) const
{
    return m_impl->get_all_type_definition(ti, search_imports, defs);
}

void module::get_all_symbol_definition(ast::unique_string str, bool search_imports, 
                        std::vector<const overload_set*>& defs) const
{
    return m_impl->get_all_symbol_definition(str, search_imports, defs);
};

void module::get_all_operator_declaration(ast::unique_string str, 
                        bool search_imports, oper::operator_declaration& ret) const
{
    return m_impl->get_all_operator_declaration(str, search_imports, ret);
};

void module::type_check_types(typing::type_checker& tc, const error_handler_ptr& eh)
{
    return m_impl->type_check_types(tc, eh, *this);
};

void module::build_fresh_type_variable(const ast::identifier& id)
{
    return m_impl->build_fresh_type_variable(id);
};

local_scope module::get_scope_for_let(const ast::let_term& let) const
{
    return m_impl->get_scope_for_let(let);
}

local_scope module::get_scope_for_lambda(const ast::lambda_term& lam) const
{
    return m_impl->get_scope_for_lambda(lam);
}
local_scope module::get_scope_for_mu(const ast::mu_term& lam) const
{
    return m_impl->get_scope_for_mu(lam);
}

local_scope module::get_scope_for_unpack(const ast::unpack_term& lam) const
{
    return m_impl->get_scope_for_unpack(lam);
}
local_scope module::get_scope_for_case(const ast::variant_case_term& lam) const
{
    return m_impl->get_scope_for_case(lam);
}

local_scope module::get_scope_by_index(ad::scope_index index) const
{
    return m_impl->get_scope_by_index(index);
};

local_scope module::get_scope_for_var(const ast::identifier& vi)
{
    return m_impl->get_scope_for_var(vi);
};

void module::import_overloads(overload_set& os, ast::unique_string id, 
                        ast::fixity_type ft) const
{
    return m_impl->import_overloads(os, id, ft);
};

void module::import_type_overloads(overload_type_set& os, ast::unique_string id, 
                        ast::fixity_type ft, bool from_imports_only) const
{
    return m_impl->import_type_overloads(os, id, ft, from_imports_only);
};

module module::get_unnamed_module(const ast::location& loc, const error_handler_ptr& eh) const
{
    return m_impl->get_unnamed_module(loc, *this, eh);
}

module module::get_module(const ast::location& loc, const ast::identifier& id, 
                          bool self_allowed, const error_handler_ptr& eh) const
{
    return m_impl->get_module(loc, *this, id, self_allowed, eh);
}

bool module::is_module_name(const ast::identifier& id, bool self_allowed) const
{
    return m_impl->is_module_name(id, self_allowed);
};

void module::check_operator_type(ast::unique_string vi, oper::operator_declaration& oi, 
                    bool search_imports, std::vector<ast::location>& ambig) const
{
    return m_impl->check_operator_type(vi, oi, search_imports, ambig);
};

};
