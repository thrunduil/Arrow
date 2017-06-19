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

#include "module.h"
#include "module_impl.h"
#include "error/error.h"
#include "local_scope.h"
#include "semantics/semantics.h"
#include "root/modules_impl.h"
#include "type_var_scope.h"
#include "scopes/overload_set.h"

#include <map>
#include <set>
#include <iostream>

namespace arrow { namespace details
{

struct overload_fin : overload_finalizator<overload_term_config>
{
    using overload_set  = ad::overload_set<overload_term_config>;

    module_impl* owner;

    overload_fin(module_impl* mi)   : owner(mi){};

    virtual void make(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft) override
    {
        owner->finalize_new_overload_set(os, name, ft);
    };
};

//-------------------------------------------------------------------
//                   module_impl   
//-------------------------------------------------------------------
module_impl::module_impl(ast::unique_string mod_name, module_index mi, const owner_ptr& owner,
                         bool is_shell)
    :m_name(mod_name), m_this_local_scope(std::shared_ptr<details::local_scope_impl>())
    , m_index(mi), m_owner(owner), m_type_table(mi, this)
    , m_is_shell(is_shell), m_last_def_index(0)
{};

bool module_impl::is_shell_module() const
{
    return m_is_shell;
}

void module_impl::init(const module& owner)
{
    m_this_local_scope = local_scope(owner, scope_index((long)m_local_scopes.size(), scope_index::expr_scope));
    m_local_scopes.push_back(m_this_local_scope);
    m_type_table.set_local_scope(m_this_local_scope.get_scope_index());
}

module_impl::~module_impl()
{};

void module_impl::remove_cyclical_data()
{
    m_imported_modules.clear();    
    m_this_local_scope = local_scope(std::shared_ptr<details::local_scope_impl>());    
    m_local_scopes.clear();
    m_build_defs.clear();
    m_type_table.clear();
    m_build_eval_terms.clear();
    m_func_map_save.clear();
    m_instantiations.clear();
    m_overloads.clear();
};

void module_impl::finish_building()
{
    if (is_shell_module() == true)
    {
        // remove failed defs
        for (auto& item : m_build_function_map)
        {
            if (item.second.has_error() == false)
                continue;

            remove_definition(item.second.get_symbol());
        };
    };

    if (m_build_function_map.size() > 0)
        m_func_map_save.push_back(std::move(m_build_function_map));

    m_build_function_map.clear();
    m_build_imports.clear();
    m_build_defs.clear();
    m_build_eval_terms.clear();
    m_build_graph.clear();    
    m_build_symbol_set_string.clear();
    m_type_table.finish_building(is_shell_module());
};

void module_impl::remove_definition(const ast::identifier& id)
{
    auto pos = m_symbol_map_string.find(id.get_name_simple());

    //we can remove regular functions but overloads cannot be removed
    if (pos != m_symbol_map_string.end())
    {
        if (pos->second->second.is_overload() == false)
            m_symbol_map_string.erase(pos);
    };

    m_overloads.remove_failed_definition(id);
};

ast::unique_string module_impl::get_module_name() const
{
    return m_name;
};

module module_impl::get_unnamed_module(const ast::location& loc, const module& owner, 
                            const error_handler_ptr& eh) const
{
    ast::unique_string null_name("");

    if (m_name == null_name)
        return owner;
    
    auto pos = m_imported_modules.find(null_name);

    if (pos != m_imported_modules.end())
        return pos->second;

    error::scope_error_msg(eh).error_invalid_imported_module_name(loc, null_name, m_imported_modules);
    return owner;
};

bool module_impl::is_module_name(const ast::identifier& id, bool self_allowed) const
{
    ast::fixity_type ft;
    ast::unique_string name = id.get_name(ft);

    if (ft != ast::fixity_type::undefined)
        return false;

    if (self_allowed == true && name == m_name)
        return true;

    auto pos = m_imported_modules.find(name);

    if (pos != m_imported_modules.end())
        return true;
    
    if (is_shell_module() == true && self_allowed == true)
        return name == get_shell_string();

    return false;
};

module module_impl::get_module(const ast::location& loc, const module& owner,
                const ast::identifier& id, bool self_allowed, const error_handler_ptr& eh) const
{
    ast::fixity_type ft;
    ast::unique_string name = id.get_name(ft);

    if (ft != ast::fixity_type::undefined)
    {
        ast::unique_string full_name = ast::unique_string(id.get_name_full());
        error::scope_error_msg(eh).error_invalid_imported_module_name
                            (loc, full_name, m_imported_modules);
        return owner;
    };

    if (self_allowed == true && name == m_name)
        return owner;

    auto pos = m_imported_modules.find(name);

    if (pos != m_imported_modules.end())
        return pos->second;

    if (is_shell_module() == true)
    {
        if (name == get_shell_string() && self_allowed == true)
            return owner;
    }

    error::scope_error_msg(eh).error_invalid_imported_module_name
                        (loc, name, m_imported_modules);
    return owner;
};

void module_impl::make_imports(const modules_provider& mods, std::set<string_loc>& undefined,
                               const error_handler_ptr& eh)
{
    (void)eh;

    //protect from self importing
    {
        auto pos    = m_build_imports.find(m_name);
        if (pos != m_build_imports.end())
            m_build_imports.erase(pos);
    };

    //fill imported modules list
    for (auto pos : m_build_imports)
    {
        module mod  = mods.get_module(pos.first);

        if (mod.is_null() == true)
        {
            undefined.insert(string_loc(pos.first, pos.second.second));
            continue;
        };

        m_imported_modules[pos.second.first] = mod;
    };
};

void module_impl::add_import_edges(import_graph& graph)
{
    graph.add_node(m_name);

    for (auto import : m_imported_modules)
        graph.add_edge(m_name, import.first);
};

void module_impl::add_ast(const ast::definitions& defs)
{
    m_build_defs.push_back(defs);
};

void module_impl::add_imports(const std::vector<import_info>& imports)
{
    for (const auto& pos : imports)
    {
        ast::unique_string imp   = std::get<0>(pos);
        ast::unique_string alias = std::get<1>(pos);
        const ast::location& loc = std::get<2>(pos);
        m_build_imports[imp]     = string_loc(alias, loc);
    }
}

void module_impl::collect_operator_declarations(const error_handler_ptr& eh,
                    const module& owner)
{
    for (const auto& pos : m_build_defs)
        sema::collect_operator_declarations(pos, owner, eh);
};

void module_impl::collect_symbol_definitions(const error_handler_ptr& eh,
                    const module& owner)
{
    for (const auto& pos : m_build_defs)
        sema::collect_symbol_definitions(pos, owner, eh);
};

const graph& module_impl::get_graph() const
{
    return m_build_graph;
}

module_index module_impl::get_module_index() const
{
    return m_index;
};

bool module_impl::check_symbol(const ast::identifier& id, const error_handler_ptr& eh)
{
    ast::unique_string name = id.get_name_simple();

    if (name == m_name)
    {
        error::scope_error_msg(eh).error_module_symbol_used(id->get_location(), id);
        return false;
    };

    auto pos = m_imported_modules.find(name);
    if (pos != m_imported_modules.end())
    {
        error::scope_error_msg(eh).error_module_symbol_used(id->get_location(), id);
        return false;
    };

    if (m_is_shell == true && name == get_shell_string())
    {
        error::scope_error_msg(eh).error_module_symbol_used(id->get_location(), id);
        return false;
    };

    return true;
};

void module_impl::add_definition(const ast::identifier& vi, 
                    const symbol_definition_env& def, const error_handler_ptr& eh)
{
    if (check_symbol(vi, eh) == false)
        return;

    add_function_definition(vi, def, eh);
};

void module_impl::add_function_definition(const ast::identifier& vi, 
                        const symbol_definition_env& def, const error_handler_ptr& eh)
{
    def_index di    = get_new_def_index();
    long group      = get_group_index();
    bool is_brack   = vi.get_symbol_data()->get_kind().is_bracket_full();

    vi.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    vi.get_symbol_data()->set_definition(di);

    ast::unique_string name = vi.get_name_simple();
    ast::fixity_type ft     = vi.get_fixity();
    ast::fixity_type ft_und = ast::fixity_type::undefined;

    key_type        key(vi);
    key_type_string key_string(name);
    key_type_over   key_over(name, ft_und);

    bool is_over    = def.is_overload() || is_overloaded(key_string);
    bool is_oper    = is_any_operator(key_string, true);

    if (ft != ast::fixity_type::undefined && is_oper == false)
    {
        error::operator_error_msg(eh).warning_explicit_fixity_not_for_operator
                        (def.get_location(), name);
        ft          = ast::fixity_type::undefined;
    };

    if (is_oper == true)
    {        
        if (ft == ast::fixity_type::undefined)
        {
            if (is_brack == true)
                ft      = ast::operator_info::default_fixity_bracket;
            else
                ft      = ast::operator_info::default_fixity_operator;
        };

        oper::operator_declaration op_decl;
        loc_vec ambig_loc;
        check_operator_type(name, op_decl, true, ambig_loc);

        bool has_declaration = op_decl.has(ft);

        if (has_declaration == false)
        {
            ast::location loc = def.get_location();
            error::operator_error_msg(eh).error_invalid_operator_definition_no_valid_declaration
                                    (loc, name, ft, op_decl);
            return;
        };
    };

    auto pos2       = m_build_function_map.insert(fun_sym_map::value_type(key, def));    
    pos2.first->second.set_def_index(di, group);
    
    overload_fin fin(this);
    auto pos_over   = m_overloads.get_or_build(vi, ft_und, is_oper || is_over, eh, 
                        m_index, get_this_local_scope().get_scope_index(), &fin);

    pos_over->second.add(&pos2.first->second);
    m_symbol_map_string[key_string] = pos_over;

    if (is_over == false && is_oper == false)
    {
        auto pos = m_build_symbol_set_string.find(key_string);

        if (pos != m_build_symbol_set_string.end() && is_shell_module() == false)
        {
            ast::location this_loc   = def.get_location();
            ast::location other_loc  = pos->second;
            error::scope_error_msg(eh).error_symbol_defined
                        (vi.get_name_simple(), this_loc, other_loc);
            return;
        };

        //used only to detect multiply defined symbols
        m_build_symbol_set_string[key_string]   = vi->get_location();
    };

    int ind     = di.get_value();
    if ((int)m_def_map.size() <= ind)
        m_def_map.resize(2 * ind + 1);

    m_def_map[ind]  = fun_sym_info(true, pos2.first);

    if (is_oper == false)
        return;

    //build overload set with explicit fixity
    auto pos_over2  = m_overloads.get_or_build(vi, ft, true, eh, 
                        m_index, get_this_local_scope().get_scope_index(), &fin);

    pos_over2->second.add(&pos2.first->second);
};

void module_impl::add_overload(const ast::identifier& vi, const ast::type& ty, 
                               const error_handler_ptr& eh)
{
    add_overload(vi, ast::fixity_type::undefined, ty, eh);
    ast::fixity_type ft     = vi.get_fixity();

    ast::unique_string name = vi.get_name_simple();
    bool is_oper            = is_any_operator(name, true);

    if (is_oper == false)
    {
        if (ft == ast::fixity_type::undefined)
            return;

        error::operator_error_msg(eh).warning_explicit_fixity_not_for_operator
                        (vi->get_location(), name);
        return;
    };

    // add overload for operator with explicit fixity
    bool is_brack   = vi.get_symbol_data()->get_kind().is_bracket_full();

    if (ft == ast::fixity_type::undefined)
    {
        if (is_brack == true)
            ft      = ast::operator_info::default_fixity_bracket;
        else
            ft      = ast::operator_info::default_fixity_operator;
    };

    add_overload(vi, ft, ty, eh);
};

void module_impl::add_overload(const ast::identifier& vi, ast::fixity_type ft, const ast::type& ty, 
                               const error_handler_ptr& eh)
{
    if (check_symbol(vi, eh) == false)
        return;

    bool has_def        = ty != ast::type();
    def_index di        = get_new_def_index();
    long group          = get_group_index();

    vi.get_symbol_data()->get_kind().set_sector_term().set_function_overload();
    vi.get_symbol_data()->set_definition(di);

    ast::unique_string name = vi.get_name_simple();
    key_type_over key_string(name, ft);

    overload_fin fin(this);
    auto pos_over       = m_overloads.add_overload(vi, ft, ty, eh,
                            m_index, get_this_local_scope().get_scope_index(), &fin);

    m_symbol_map_string[name] = pos_over;

    if (has_def == true)
    {
        symbol_definition_env def(vi->get_location(), ty, vi, ast::term(), true);

        key_type key(vi);

        auto pos2       = m_build_function_map.insert(fun_sym_map::value_type(key, def));    
        pos2.first->second.set_def_index(di, group);

        pos_over->second.add(&pos2.first->second);
    };

    int ind     = di.get_value();
    if ((int)m_def_map.size() <= ind)
        m_def_map.resize(2 * ind + 1);

    m_def_map[ind]  = fun_sym_info(false, fun_sym_iter());
};

void module_impl::register_local_function_subst(const ast::term_definition& td)
{
    const ast::identifier& id   = td.get_identifier();
    const ast::type& ty         = td.get_type();
    const ast::term& body       = td.get_body();

    def_index di    = get_new_def_index();
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    id.get_symbol_data()->set_definition(di);
    id.get_symbol_data()->set_module(this->get_module_index());
    id.get_symbol_data()->set_local_scope(this->get_this_local_scope().get_scope_index());

    int ind     = di.get_value();
    if ((int)m_def_map.size() <= ind)
        m_def_map.resize(2 * ind + 1);

    symbol_definition_env sd(td->get_location(), ty, id, body, td.is_overload());
    sd.set_def_index(di, group);

    auto pos        = m_instantiations.register_local_function_subst(id, std::move(sd));
    m_def_map[ind]  = fun_sym_info(true, pos);
};

void module_impl::add_instantiation_term_type(const ast::identifier_term& id_base, 
                const ast::term_definition& td, const std::vector<ast::type>& args, 
                bool true_inst)
{
    const ast::identifier& id   = td.get_identifier();
    const ast::type& ty         = td.get_type();
    const ast::term& body       = td.get_body();

    def_index di    = get_new_def_index();
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    id.get_symbol_data()->set_definition(di);
    id.get_symbol_data()->set_module(this->get_module_index());
    id.get_symbol_data()->set_local_scope(this->get_this_local_scope().get_scope_index());

    int ind     = di.get_value();
    if ((int)m_def_map.size() <= ind)
        m_def_map.resize(2 * ind + 1);

    symbol_definition_env sd(td->get_location(), ty, id, body, td.is_overload());
    sd.set_def_index(di, group);

    auto pos        = m_instantiations.add_type(id_base, id, std::move(sd), args, true_inst);
    m_def_map[ind]  = fun_sym_info(true, pos);
};

void module_impl::add_instantiation_type_type(const ast::identifier_type& id_base, 
                const ast::type_definition& td, const std::vector<ast::type>& args, 
                bool true_inst)
{
    return m_type_table.add_instantiation_type(id_base, td, args, true_inst);
};

void module_impl::add_instantiation_term_kind(const ast::identifier_term& id_base, 
                const ast::term_definition& td, const std::vector<ast::kind>& args, 
                bool true_inst)
{
    const ast::identifier& id   = td.get_identifier();
    const ast::type& ty         = td.get_type();
    const ast::term& body       = td.get_body();

    def_index di    = get_new_def_index();
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    id.get_symbol_data()->set_definition(di);
    id.get_symbol_data()->set_module(this->get_module_index());
    id.get_symbol_data()->set_local_scope(this->get_this_local_scope().get_scope_index());

    int ind     = di.get_value();
    if ((int)m_def_map.size() <= ind)
        m_def_map.resize(2 * ind + 1);

    symbol_definition_env sd(td->get_location(), ty, id, body, td.is_overload());
    sd.set_def_index(di, group);

    auto pos        = m_instantiations.add_kind(id_base, id, std::move(sd), args, true_inst);
    m_def_map[ind]  = fun_sym_info(true, pos);
};

void module_impl::add_instantiation_type_kind(const ast::identifier_type& id_base, 
                const ast::type_definition& td, const std::vector<ast::kind>& args, 
                bool true_inst)
{
    return m_type_table.add_instantiation_kind(id_base, td, args, true_inst);
};

ast::identifier module_impl::get_instantiation_term_type(const ast::identifier_term& base_id, 
                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                long& num_matched)
{
    return m_instantiations.get_existing_type(base_id, args, has, comp, num_matched);
};

ast::identifier module_impl::get_instantiation_type_type(const ast::identifier_type& base_id, 
                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                long& num_matched)
{
    return m_type_table.get_instantiation_type(base_id, args, has, comp, num_matched);
};

ast::identifier module_impl::get_instantiation_term_kind(const ast::identifier_term& base_id, 
                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                long& num_matched)
{
    return m_instantiations.get_existing_kind(base_id, args, has, comp, num_matched);
};

ast::identifier module_impl::get_instantiation_type_kind(const ast::identifier_type& base_id, 
                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                long& num_matched)
{
    return m_type_table.get_instantiation_kind(base_id, args, has, comp, num_matched);
};

bool module_impl::is_overloaded(const ast::unique_string id) const
{
    return m_overloads.is_overloaded(id);
};

def_index module_impl::get_new_def_index() const
{
    long n_defs     = m_last_def_index++;
    def_index di    = def_index(m_index, get_this_local_scope().get_scope_index(), n_defs);            
    return di;
};
long module_impl::get_group_index() const
{
    return (long)m_func_map_save.size();
};

const local_scope& module_impl::get_this_local_scope() const
{
    return this->m_this_local_scope;
};

void module_impl::add_type_definition(const ast::type_definition& decl, const error_handler_ptr& eh)
{
    if (check_symbol(decl.get_identifier(), eh) == false)
        return;

    m_type_table.add_type_definition(decl, eh);
}

void module_impl::add_type_definition(const ast::type_definition_var& decl, const error_handler_ptr& eh)
{
    if (check_symbol(decl.get_identifier(), eh) == false)
        return;

    m_type_table.add_type_definition(decl, eh);
}

void module_impl::add_eval_term(const ast::term_use& def, const error_handler_ptr& eh)
{
    (void)eh;
    const ast::term& t = def.get_term();
    m_build_eval_terms.push_back(t);
};

void module_impl::build_type_scopes(const module& mod, const error_handler_ptr& eh)
{
    return m_type_table.build_type_scopes(mod, eh);
};

void module_impl::type_desugaring(const module& mod, const error_handler_ptr& eh)
{
    return m_type_table.type_desugaring(mod, eh);
}
void module_impl::build_type_dependency_graph(const module& mod, const error_handler_ptr& eh)
{
    return m_type_table.build_type_dependency_graph(mod, eh);
}

type_var_scope module_impl::new_typevar_scope(const ast::location& loc)
{
    return m_type_table.new_typevar_scope(loc);
};
type_var_scope module_impl::new_kindvar_scope(const ast::location& loc)
{
    return m_type_table.new_kindvar_scope(loc);
};

type_var_scope module_impl::make_new_var_scope(const ast::location& loc, 
                                const type_var_scope& parent)
{
    return m_type_table.make_new_var_scope(loc, parent);
};

const module_impl::overload_type_set&
module_impl::get_type_definition(ast::unique_string ti, bool search_imports, 
                                bool& found, loc_vec& ambiguous_locations) const
{
    {
        const overload_type_set& res  = m_type_table.get_type_definition(ti, found);
        
        if (found == true)
            return res;
    };

    static overload_type_set empty_def;

    if (search_imports == false)
    {
        found = false;
        return empty_def;
    }

    bool ambiguous = false;
    loc_vec ambig2;
    const overload_type_set* def   = nullptr;
    const overload_type_set* ret   = nullptr;

    // in imports the same operator can be declared many times
    // collect locations of different operator declarations 
    for (const auto& pos2 : m_imported_modules)
    {
        bool found_loc;
        ast::location loc_loc;

        def         = &pos2.second.get_type_definition(ti, false, found_loc, ambig2);

        if (found_loc == true)
        {
            if (found == false)
            {
                found   = true;
                ret     = def;
            }
            else
            {
                ambiguous_locations.push_back(def->get_location());
                ambiguous = true;
            };
        }
    };

    if (ambiguous == true)
        ambiguous_locations.push_back(ret->get_location());

    if (found == true)
        return *ret;
    else
        return empty_def;
};

void module_impl::get_all_type_definition(ast::unique_string ti, bool search_imports, 
                                            std::vector<const overload_type_set*>& defs) const
{
    {
        bool found;
        const overload_type_set& res  = m_type_table.get_type_definition(ti, found);
        
        if (found == true)
        {
            defs.push_back(&res);
            return;
        }
    };

    if (search_imports == false)
        return;

    for (const auto& pos2 : m_imported_modules)
        pos2.second.get_all_type_definition(ti, false, defs);
};

def_index module_impl::get_type_definition_index(ast::unique_string ti, bool search_imports, 
                         ast::location& loc, bool& found, loc_vec& ambiguous_locations) const
{
    {
        def_index res   = m_type_table.get_type_definition_index(ti, loc, found);
        
        if (found == true)
            return res;
    };

    if (search_imports == false)
    {
        found = false;
        return def_index(def_index::fresh());
    }

    bool ambiguous = false;
    loc_vec ambig2;
    def_index def   = def_index(def_index::fresh());
    def_index ret   = def_index(def_index::fresh());
    loc             = ast::location();

    // in imports the same operator can be declared many times
    // collect locations of different operator declarations 
    for (const auto& pos2 : m_imported_modules)
    {
        bool found_loc;
        ast::location loc_loc;

        def         = pos2.second.get_type_definition_index(ti, false, loc_loc, 
                            found_loc, ambig2);

        if (found_loc == true)
        {
            if (found == false)
            {
                found   = true;
                ret     = def;
                loc     = loc_loc;
            }
            else
            {
                ambiguous_locations.push_back(loc_loc);
                ambiguous = true;
            };
        }
    };

    if (ambiguous == true)
        ambiguous_locations.push_back(loc);

    return ret;
};

bool module_impl::is_symbol_defined(const ast::identifier& id, bool search_imports,
                                    std::vector<ast::location>& ambig) const
{
    bool found;
    const overload_set& def = get_symbol_definition(id.get_name_simple(), 
                                    search_imports, found, ambig);

    if (found == false)
        return false;

    def_index di = def.get_definition_index();
    id.get_symbol_data()->set_definition(di);

    return true;
};

const module_impl::fun_sym_map& 
module_impl::get_function_map() const
{
    return m_build_function_map;
};

const module_impl::term_list& module_impl::get_eval_terms() const
{
    return m_build_eval_terms;
}

module_impl::term_list& module_impl::get_eval_terms_modify()
{
    return m_build_eval_terms;
}

module_impl::fun_sym_map& 
module_impl::get_function_map_modify()
{
    return m_build_function_map;
};

void module_impl::build_fresh_type_variable(const ast::identifier& id)
{
    m_type_table.build_fresh_type_variable(id);
};

ast::kind module_impl::get_type_constructor_kind(const error_handler_ptr& eh, 
            const ast::identifier_type& idt, bool& overloaded, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_type_constructor_kind(eh, idt, overloaded, has_error);
    };

    return m_type_table.get_type_constructor_kind(eh, idt, overloaded, has_error);
};

bool module_impl::is_overloaded_term_def(const ast::identifier_term& idt, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();
    has_error                   = false;

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.is_overloaded_term_def(idt, has_error);
    };

    if (si == m_this_local_scope.get_scope_index())
    {
        int index   = id.get_symbol_data()->get_def_index().get_value();

        if (index >= (int)m_def_map.size())
        {
            has_error = true;
            return false;
        }
        
        auto info   = m_def_map[index];

        if (info.first == false)
            return true;

        if (info.second->second.has_error())
            has_error = true;

        return false;
    };   

    local_scope ls = this->get_scope_by_index(si);
    return ls.is_overloaded_term_def(idt, has_error);
};

bool module_impl::is_overloaded_type_def(const ast::identifier_type& idt, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();
    has_error                   = false;

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.is_overloaded_type_def(idt, has_error);
    };

    return m_type_table.is_overloaded_def(idt, has_error);
};

ast::type module_impl::get_symbol_type(const error_handler_ptr& eh, 
                    const ast::identifier_term& idt, bool& overloaded, 
                    bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();
    has_error                   = false;

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_symbol_type(eh, idt, overloaded, has_error);
    };

    overloaded      = false;

    if (si == m_this_local_scope.get_scope_index())
    {
        int index   = id.get_symbol_data()->get_def_index().get_value();

        if (index >= (int)m_def_map.size())
        {
            std::string msg = std::string() + "symbol " + id.get_name_simple().get_string()
                            + " has not a type assignment";
            error::error_base_msg(eh).internal_error(id->get_location(), msg);

            has_error = true;
            return ast::error_type(id->get_location());
        }
        
        auto info   = m_def_map[index];

        if (info.first == false)
        {
            overloaded = true;
            return ast::type();
        };

        if (info.second->second.has_error())
            has_error = true;

        return info.second->second.get_signature();
    };   

    local_scope ls = this->get_scope_by_index(si);
    return ls.get_symbol_type(eh, idt, overloaded, has_error);
};

term_symbol_kind module_impl::get_term_symbol_kind(const ast::identifier_term& idt) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_term_symbol_kind(idt);
    };

    if (si == m_this_local_scope.get_scope_index())
    {
        int index   = id.get_symbol_data()->get_def_index().get_value();

        if (index >= (int)m_def_map.size())
            return term_symbol_kind::undefined;
        
        auto info   = m_def_map[index];

        if (info.first == false)
            return term_symbol_kind::overloaded_symbol;
        else
            return term_symbol_kind::global_symbol;
    };   

    local_scope ls = this->get_scope_by_index(si);
    return ls.get_term_symbol_kind(idt);
};

type_symbol_kind module_impl::get_type_symbol_kind(const ast::identifier_type& idt) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_type_symbol_kind(idt);
    };

    return m_type_table.get_symbol_kind(idt);
};

ast::term module_impl::get_symbol_term(const error_handler_ptr& eh, 
                        const ast::identifier_term& idt, bool& overloaded, 
                        bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();
    has_error                   = false;

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_symbol_term(eh, idt, overloaded, has_error);
    };

    overloaded      = false;

    if (si == m_this_local_scope.get_scope_index())
    {
        int index   = id.get_symbol_data()->get_def_index().get_value();

        if (index >= (int)m_def_map.size())
        {
            std::string msg = std::string() + "symbol " + id.get_name_simple().get_string()
                            + " has not a type assignment";
            error::error_base_msg(eh).internal_error(id->get_location(), msg);

            has_error = true;
            return ast::error_term(id->get_location());
        }
        
        auto info   = m_def_map[index];
        if (info.first == false)
        {
            overloaded = true;
            return ast::term();
        };

        has_error = info.second->second.has_error();

        // only abstract definitions of overloaded symbols have not body
        bool has_body;
        return info.second->second.get_body(has_body);
    };   

    local_scope ls = this->get_scope_by_index(si);
    return ls.get_symbol_term(eh, idt, overloaded, has_error);
};

const module_impl::overload_set*  
module_impl::get_overload_set(const error_handler_ptr& eh, 
                        const ast::identifier_term& idt) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_overload_set(eh, idt);
    };

    if (si == m_this_local_scope.get_scope_index())
    {
        ast::unique_string name = id.get_name_simple();
        ast::fixity_type ft     = id.get_fixity();
        bool has;
        auto pos                = m_overloads.get(name, ft, has);

        if (has == false)
        {
            std::string msg = std::string() + "symbol " + name.get_string()
                            + " is not defined";
            error::error_base_msg(eh).internal_error(id->get_location(), msg);
            return nullptr;
        }

        return &pos->second;
    };

    local_scope ls = this->get_scope_by_index(si);
    return ls.get_overload_set(eh, idt);
};

const module_impl::overload_set*
module_impl::get_overload_set(const ast::identifier& id) const
{
    const ast::symbol_data* sd  = id.get_symbol_data();
    scope_index si              = sd->get_def_index().get_scope();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_overload_set(id);
    };

    if (si == m_this_local_scope.get_scope_index())
    {
        ast::unique_string name = id.get_name_simple();
        ast::fixity_type ft     = id.get_fixity();

        bool has;
        auto pos                = m_overloads.get(name, ft, has);

        if (has == false)
            return nullptr;

        return &pos->second;
    };

    local_scope ls = this->get_scope_by_index(si);
    return ls.get_overload_set(id);
};

const module_impl::overload_type_set* 
module_impl::get_overload_type_set(const ast::identifier& id) const
{
    const ast::symbol_data* sd  = id.get_symbol_data();
    module_index mi             = sd->get_def_index().get_module();

    if (mi != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_overload_type_set(id);
    };

    return m_type_table.get_overload_set(id);
};

ast::overload_index 
module_impl::get_overload_index(const overload_set* os) const
{
    ast::overload_index ind;
    ind.m_set = os;

    return ind;
};

const module_impl::overload_set* 
module_impl::get_overload_set(const ast::overload_index* oi) const
{
    if (!oi)
        return nullptr;
    else
        return oi->m_set;
};

ast::type module_impl::get_type_constructor_definition(const error_handler_ptr& eh, 
                        const ast::identifier_type& idt, bool& overloaded, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    module_index mi             = sd->get_def_index().get_module();

    if (sd->get_def_index().get_module() != get_module_index())
    {
        const module& mod   = m_owner.lock()->get_module_by_index(mi);
        return mod.get_type_constructor_definition(eh, idt, overloaded, has_error);
    };

    return m_type_table.get_type_constructor_definition(eh, id, overloaded, has_error);
};

local_scope module_impl::create_child_scope(const details::local_scope_impl& parent)
{
    scope_index index   = scope_index((long)m_local_scopes.size(), scope_index::expr_scope);
    local_scope ret     = local_scope(parent, index);

    m_local_scopes.push_back(ret);
    return ret;
}

void module_impl::get_all_symbol_definition(ast::unique_string str, bool search_imports, 
                        std::vector<const overload_set*>& defs) const
{
    key_type_string key(str);
    auto pos = m_symbol_map_string.find(key);

    if (pos != m_symbol_map_string.end())
    {
        defs.push_back(&pos->second->second);
        return;
    }

    if (search_imports == false)        
        return;

    for (const auto& pos2 : m_imported_modules)
        pos2.second.get_all_symbol_definition(str, false, defs);

    return;
};

void module_impl::get_all_operator_declaration(ast::unique_string str, 
                        bool search_imports, oper::operator_declaration& ret) const
{
    bool found = m_operators.get_all_operator_declaration(str, ret);

    if (found == true)
        return;

    if (search_imports == false)        
        return;

    for (const auto& pos2 : m_imported_modules)
        pos2.second.get_all_operator_declaration(str, false, ret);

    return;
};

void module_impl::check_operator_type(ast::unique_string str, oper::operator_declaration& od,
                            bool search_imports, loc_vec& ambig_locations) const
{
    bool found_all  = m_operators.check_operator_type(str, od);

    if (found_all == true)
        return;
    
    if (search_imports == false)
        return;
    
    oper::operator_declaration collected  = od;
    loc_vec ambig2;

    // in imports the same operator can be declared many times
    // collect locations of different operator declarations;
    // ignore declarations of operators declared in this module
    for (const auto& pos2 : m_imported_modules)
    {
        oper::operator_declaration loc  = od;

        pos2.second.check_operator_type(str, loc, false, ambig2);
        bool ambig_loc  = collected.equal_defined(loc, ambig_locations) == false;

        if (ambig_loc == false)
        {
            collected.add_definitions(loc);
        };
    };

    od = collected;
};

const module_impl::overload_set& 
module_impl::get_symbol_definition(ast::unique_string str, bool search_imports,
                                   bool& found, loc_vec& ambiguous_locations) const
{
    static overload_set empty_def;

    found   = false;

    key_type_string key(str);
    auto pos = m_symbol_map_string.find(key);

    if (pos != m_symbol_map_string.end())
    {
        found = true;
        return pos->second->second;
    }

    if (search_imports == false)        
        return empty_def;

    bool ambiguous = false;
    loc_vec ambig2;
    const overload_set* def     = nullptr;
    const overload_set* ret_def = nullptr;

    // in imports the same operator can be declared many times
    // collect locations of different operator declarations 
    for (const auto& pos2 : m_imported_modules)
    {
        bool found_loc;
        def         = &pos2.second.get_symbol_definition(str, false, found_loc, ambig2);

        if (found_loc == true)
        {
            if (found == false)
            {
                found   = true;
                ret_def = def;
            }
            else
            {
                ambiguous_locations.push_back(def->get_location());
                ambiguous = true;
            };
        }
    };

    if (ambiguous == true)
        ambiguous_locations.push_back(ret_def->get_location());

    if (found)
        return *ret_def;
    else
        return empty_def;
};

module_impl::extern_function_ptr 
module_impl::get_extern_function_def(const error_handler_ptr& eh, const ast::identifier& id) const
{
    extern_function_ptr ret;
    ret = m_extern_func.get(id.get_name_simple());

    if (!ret)
    { 
        error::scope_error_msg(eh).error_extern_function_not_defined(id->get_location(), id);
        return extern_func_provider::error_function();
    };

    return ret;
};

module_impl::extern_type_ptr
module_impl::get_extern_type_function_def(const error_handler_ptr& eh, const ast::identifier& id) const
{
    extern_type_ptr ret;
    ret = m_extern_func.get_type(id.get_name_simple());

    if (!ret)
    { 
        error::scope_error_msg(eh).error_extern_type_function_not_defined(id->get_location(), id);
        return extern_func_provider::error_type_function();
    };

    return ret;
};

void module_impl::type_check_types(typing::type_checker& tc, const error_handler_ptr& eh,
                                   const module& mod)
{
    return m_type_table.type_check_types(tc, eh, mod);
};

local_scope module_impl::get_scope_for_let(const ast::let_term& let) const
{
    module_index mi     = let.get_symbol_data()->get_module_index();
    scope_index si      = let.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
}

local_scope module_impl::get_scope_for_case(const ast::variant_case_term& lam) const
{
    module_index mi     = lam.get_symbol_data()->get_module_index();
    scope_index si      = lam.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
};

local_scope module_impl::get_scope_for_unpack(const ast::unpack_term& lam) const
{
    module_index mi     = lam.get_symbol_data()->get_module_index();
    scope_index si      = lam.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
};

local_scope module_impl::get_scope_for_lambda(const ast::lambda_term& lam) const
{
    module_index mi     = lam.get_symbol_data()->get_module_index();
    scope_index si      = lam.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
};

local_scope module_impl::get_scope_for_mu(const ast::mu_term& lam) const
{
    module_index mi     = lam.get_symbol_data()->get_module_index();
    scope_index si      = lam.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
};

local_scope module_impl::get_scope_by_index(scope_index index) const
{
    int value = index.get_value();

    if (value < 0 || value >= (int)m_local_scopes.size())
        throw std::runtime_error("invalid scope index");

    return m_local_scopes[value];
};

local_scope module_impl::get_scope_for_var(const ast::identifier& vi) const
{
    module_index mi     = vi.get_symbol_data()->get_module_index();
    scope_index si      = vi.get_symbol_data()->get_scope_index();

    const module& mod   = m_owner.lock()->get_module_by_index(mi);
    local_scope sc      = mod.get_scope_by_index(si);

    return sc;
}

void module_impl::add_operator_declaration(const ast::identifier& vi, const ast::operator_info& fixity,
                                           const error_handler_ptr& eh)
{
    if (check_symbol(vi, eh) == false)
        return;

    ast::fixity_type fix_type   = fixity.get_fixity_type();
    ast::unique_string str      = vi.get_name_simple();

    if (is_shell_module() == false && is_operator_declared(str, fix_type, false) == true)
    {
        bool found;
        std::vector<ast::location> ambig;
        ast::location old       = get_operator_declaration(str, fix_type, 
                                    false, found, ambig).get_location(fix_type); 

        error::operator_error_msg(eh).error_fixity_defined(str, vi->get_location(), old);
        return;
    };
            
    m_operators.add_operator_declaration(vi, fixity);
    add_overload(vi, fix_type, ast::type(), eh);
    add_overload(vi, ast::fixity_type::undefined, ast::type(), eh);
};

bool module_impl::is_any_operator(ast::unique_string vi, bool search_imports) const
{
    if (m_operators.is_any_operator(vi) == true)
        return true;

    if (search_imports == false)
        return false;

    for (const auto& pos2 : m_imported_modules)
    {
        if (pos2.second.is_any_operator(vi, false) == true)
            return true;
    }

    return false;
};

bool module_impl::is_operator_declared(ast::unique_string vi, ast::fixity_type ft, 
                        bool search_imports) const
{
    if (m_operators.is_operator_declared(vi, ft) == true)
        return true;

    if (search_imports == false)
        return false;

    for (const auto& pos2 : m_imported_modules)
    {
        if (pos2.second.is_operator_declared(vi, ft, false) == true)
            return true;
    }

    return false;
};

oper::operator_declaration 
module_impl::get_operator_declaration(ast::unique_string str, ast::fixity_type ft,
                    bool search_imports, bool& found, loc_vec& ambiguous_locations) const
{
    found       = false;

    // operator defined in given module hides definitions of an operator
    // of the same kind in other modules
    {
        auto res = m_operators.get_operator_declaration(str, ft, found);

        if (found == true)
            return res;
    }

    if (search_imports == false)
    {
        found = false;
        return oper::operator_declaration();
    }

    oper::operator_declaration ret;
    bool ambiguous = false;
    loc_vec ambig2;

    // in imports the same operator can be declared many times
    // collect locations of different operator declarations 
    for (const auto& pos2 : m_imported_modules)
    {
        bool found_loc;
        oper::operator_declaration decl = pos2.second.get_operator_declaration
                                        (str, ft, false, found_loc, ambig2);

        if (found_loc == true)
        {
            if (found == false)
            {
                found = true;
                ret = decl;
            }
            else
            {
                if (ret.get_info(ft) != decl.get_info(ft))
                {
                    ambiguous_locations.push_back(decl.get_location(ft));
                    ambiguous = true;
                };
            };
        }
    };

    if (ambiguous == true)
        ambiguous_locations.push_back(ret.get_location(ft));

    return ret;
};

void module_impl::import_type_overloads(overload_type_set& os, ast::unique_string id, 
                        ast::fixity_type ft, bool from_imports_only) const
{
    if (from_imports_only == false)
    {
        m_type_table.import_type_overloads(os, id, ft);
    }
    else
    {
        for (const auto& pos : m_imported_modules)
            pos.second.import_type_overloads(os, id, ft, false);
    };
};

void module_impl::import_overloads(overload_set& os, ast::unique_string id, 
                        ast::fixity_type ft) const
{
    bool has;
    auto pos = m_overloads.get(id, ft, has);
    
    if (has == false)
        return;

    os.import_from(pos->second);
};

ast::unique_string module_impl::get_shell_string() const
{
    static ast::unique_string name("shell");
    return name;
};

void module_impl::finalize_new_overload_set(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft)
{
    if (os.is_overload() == false)
        return;

    for (const auto& pos : m_imported_modules)
        pos.second.import_overloads(os, name, ft);

    // build def index
    if (ft == ast::fixity_type::undefined)
    {
        // overload set must have unique def index the same for
        // overload sets with different fixities; for overload sets
        // with fixities different than undenfined identifiers are the same
        // as for undefined fixity; therefor we must creare new def index
        // for undefined fixity only
        const ast::identifier& id   = os.get_identifier();
        def_index di        = get_new_def_index();

        id.get_symbol_data()->get_kind().set_sector_term().set_function_overload();
        id.get_symbol_data()->set_definition(di);

        int ind     = di.get_value();
        if ((int)m_def_map.size() <= ind)
            m_def_map.resize(2 * ind + 1);

        m_def_map[ind]  = fun_sym_info(false, fun_sym_iter());
    };
};

}};
