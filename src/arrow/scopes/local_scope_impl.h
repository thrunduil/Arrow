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

#include "local_scope.h"
#include "error/error.h"
#include "module.h"
#include "module_impl.h"

#include <map>
#include <set>

namespace arrow { namespace details
{

class symbol_iter
{
    public:
        using pattern_iter  = local_variables::def_map::iterator;
        using fun_sym_map   = std::map<ad::var_info, symbol_definition_env>;
        using fun_sym_iter  = fun_sym_map::iterator;

        enum elem_kind { pattern, function, overload};

    private:
        pattern_iter        m_pattern_iter;
        fun_sym_iter        m_func_iter;
        elem_kind           m_kind;

    public:
        symbol_iter();
        symbol_iter(pattern_iter it);
        symbol_iter(bool has_definition, fun_sym_iter it);

        term_symbol_kind    get_symbol_kind() const;
        const ast::type&    get_signature(bool& has_error) const;
        bool                is_overloaded() const;

        const ast::term&    get_definition(const error_handler_ptr& eh, bool& has_error) const;
        void                update_type(const ast::type& new_ty);
        void                update_let_definition(const ast::term_definition& td);
};

class local_scope_impl : public std::enable_shared_from_this<local_scope_impl>
{
    private:
        using ptr_type      = std::shared_ptr<local_scope_impl>;
        using loc_vec       = std::vector<ast::location>;        
        using key_type      = ad::var_info;
        using key_type_string= ast::unique_string;
        using key_type_over = std::pair<ast::unique_string, ast::fixity_type>;
        using components    = std::vector<std::vector<key_type>>;
        using fun_sym_map   = std::map<key_type, symbol_definition_env>;
        using fun_sym_iter  = fun_sym_map::const_iterator;
        using fun_sym_info  = std::pair<bool,fun_sym_iter>;
        using def_map       = std::vector<symbol_iter>;
        using overload_table= ad::overload_table<ad::overload_term_config>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;
        using over_iter     = overload_table::iterator;

    private:
        module              m_module;
        const local_scope   m_parent;
        scope_index         m_index;
        local_variables     m_map;
        def_map             m_definition_map;
        operator_table      m_operators;
        overload_table      m_overloads;
        long                m_last_def_index;
        fun_sym_map         m_function_map;

        graph               m_graph;
        bool                m_building_graph;
        components          m_components;
        bool                m_has_components;

    public:
        local_scope_impl(const module& m, scope_index index);
        local_scope_impl(const local_scope_impl* parent, scope_index index);

        scope_index         get_scope_index() const;
        local_scope         new_scope(const module& owner) const;

        bool                is_bound(const ast::identifier& vi) const;
        bool                is_referred_function(const ast::identifier& vi) const;
        void                add_referred(const ast::identifier& vi);
        const graph&        get_graph() const;
        void                building_graph_start();
        void                building_graph_end();
        const components&   get_connected_components();

        const fun_sym_map&  get_local_vars() const;
        fun_sym_map&        get_local_vars_modify();

        bool                check_symbol(const ast::identifier& id, 
                                const error_handler_ptr& eh) const;
        bool                is_symbol_defined(const ast::identifier& id, bool search_parent,
                                std::vector<ast::location>& ambiguous_locations) const;
        bool                is_local_symbol_defined(const module& mod, const ad::var_info& vi) const;
        bool                is_operator_declared(ast::unique_string vi, ast::fixity_type ft, 
                                bool search_parent) const;

        void                check_operator_type(ast::unique_string vi, oper::operator_declaration& oi,
                                bool search_parent, loc_vec& ambig) const;

        ast::location       get_symbol_definition(ast::unique_string str, bool search_parent, 
                                bool& found, loc_vec& ambig) const;
        oper::operator_declaration 
                            get_operator_declaration(ast::unique_string str, ast::fixity_type ft,
                                bool search_parent, bool& found, loc_vec& ambig) const;

        bool                is_overloaded(const ast::unique_string id) const;
        bool                is_any_operator(ast::unique_string vi) const;

        ast::type           get_symbol_type(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded, bool& has_error) const;
        ast::term           get_symbol_term(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded, bool& has_error) const;
        const overload_set* get_overload_set(const error_handler_ptr& eh, 
                                const ast::identifier_term& id) const;
        const overload_set* get_overload_set(const ast::identifier& id) const;
        term_symbol_kind    get_term_symbol_kind(const ast::identifier_term& id) const;
        bool                is_overloaded_term_def(const ast::identifier_term& id, bool& has_error) const;

        void                add_pattern_var(module_index mi, const ast::annotated_identifier& vi,
                                const error_handler_ptr& eh);
        void                update_pattern_var(const ast::identifier& id, const ast::type& new_ty);
        void                update_let_definitions(const ast::definitions& defs);

        void                add_let_var(module_index mi, const ast::identifier& vi, 
                                const ast::term_definition& def, const error_handler_ptr& eh);
        void                add_operator_declaration(module_index mi, const ast::identifier& vi, 
                                const ast::operator_info& decl, const error_handler_ptr& ef);
        void                add_overload(module_index mi, const ast::identifier& id, ast::fixity_type ft,
                                const ast::type& ty, const error_handler_ptr& eh);
        void                add_overload(module_index mi, const ast::identifier& id, const ast::type& ty, 
                                const error_handler_ptr& eh);

        void                import_overloads(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft) const;

    private:
        void                add_function_definition(module_index mi, const ast::identifier& vi, 
                                ast::unique_string v, const ast::term_definition& def, 
                                const error_handler_ptr& eh);
        def_index           get_new_def_index(module_index mi);
        void                add_to_defmap(int index, symbol_iter it);

    public:
        void                finalize_new_overload_set(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft, module_index mi);
};

};};