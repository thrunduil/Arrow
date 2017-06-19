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
#include "error/error.h"
#include "symbol_data.h"
#include "type_var_scope.h"
#include "scopes/graph.h"
#include "overload_table.h"
#include "instantiation_table.h"

#include <list>

namespace arrow { namespace details
{

class type_alias_info
{
    private:
        ast::location       m_loc;
        ast::identifier     m_name;
        ast::kind           m_kind;
        ast::type           m_def;
        def_index           m_def_index;
        mutable bool        m_has_error;
        long                m_group_index;

    public:
        type_alias_info();
        type_alias_info(const ast::location& loc, const ast::identifier& name, 
                        const ast::kind& k, const ast::type& def, bool is_overload);

        const ast::identifier&  get_name() const;
        const ast::kind&        get_kind() const;
        const ast::type&        get_def() const;
        bool                    has_error() const;
        void                    set_error() const;
        void                    set_def_index(const def_index& def, long group);
        const def_index&        get_definition_index() const;
        const ast::location&    get_location() const;

        void                    set_def(const ast::type& ty);
        void                    set_kind(const ast::kind& ki);
};

struct overload_type_config
{
    using definition_type       = type_alias_info;
    using function_type         = ast::kind;
};

class type_table
{
    private:
        using annot_id          = ast::annotated_type_identifier;
        using kind_polar        = std::pair<ast::kind, ast::polarity_type>;

    public:
        using key_type          = ad::var_info;
        using key_type_string   = ast::unique_string;
        using key_type_over     = std::pair<ast::unique_string, ast::fixity_type>;
        using type_alias_map    = std::map<key_type, type_alias_info>;
        using overload_set      = overload_set<overload_type_config>;
        using overload_table    = overload_table<overload_type_config>;
        using iterator          = type_alias_map::iterator;
        using overload_iter     = overload_table::const_iterator;
        using type_iter_map     = std::map<key_type_string, overload_iter>;
        using type_map_list     = std::list<type_alias_map>;
        using loc_vec           = std::vector<ast::location>;
        using string_sym_set    = std::map<key_type_string, ast::location>;
        using fun_sym_iter      = type_alias_map::const_iterator;
        using fun_sym_info      = std::pair<bool,fun_sym_iter>;
        using def_map           = std::vector<fun_sym_info>;
        using inst_table        = instantiation_table<inst_type_conf>;

    private:
        long                m_typevar_scopes;
        module_index        m_index;
        scope_index         m_scope_index;
        module_impl*        m_owner;
        type_var_scope      m_global_typevar_scope;
        type_iter_map       m_type_alias;
        type_map_list       m_saved_maps;
        int                 m_last_def_index;
        inst_table          m_instantiations;
        overload_table      m_overloads;
        def_map             m_def_map;

        string_sym_set      m_build_symbol_set_string;
        type_alias_map      m_build_type_iter;
        graph               m_build_graph;

    public:
        type_table(module_index mi, module_impl* owner);

        void                set_local_scope(scope_index si);
        void                clear();
        void                finish_building(bool is_shell);

        void                add_type_definition(const ast::type_definition& def, 
                                const error_handler_ptr& eh);
        void                add_type_definition(const ast::type_definition_var& def, 
                                const error_handler_ptr& eh);

        void                add_instantiation_type(const ast::identifier_type& id, 
                                const ast::type_definition& td, const std::vector<ast::type>& args, 
                                bool true_inst);
        void                add_instantiation_kind(const ast::identifier_type& id, 
                                const ast::type_definition& td, const std::vector<ast::kind>& args, 
                                bool true_inst);

        ast::identifier     get_instantiation_type(const ast::identifier_type& base_id, 
                                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                                long& num_matched);
        ast::identifier     get_instantiation_kind(const ast::identifier_type& base_id, 
                                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                                long& num_matched);

        void                build_type_scopes(const module& mod, const error_handler_ptr& eh);
        void                type_desugaring(const module& mod, const error_handler_ptr& eh);
        void                build_type_dependency_graph(const module& mod, const error_handler_ptr& eh);
        void                type_check_types(typing::type_checker& tc, const error_handler_ptr& eh,
                                const module& mod);

        type_var_scope      new_typevar_scope(const ast::location& loc);
        type_var_scope      new_kindvar_scope(const ast::location& loc);
        type_var_scope      make_new_var_scope(const ast::location& loc, 
                                const type_var_scope& parent);   

        ast::type           get_type_constructor_definition(const error_handler_ptr& eh, 
                                const ast::identifier& id, bool& overloaded, bool& has_error) const;
        ast::kind           get_type_constructor_kind(const error_handler_ptr& eh, 
                                const ast::identifier_type& id, bool& overloaded, bool& has_error) const;

        def_index           get_type_definition_index(ast::unique_string ti, ast::location& loc, 
                                bool& found) const;
        const overload_set& get_type_definition(ast::unique_string ti, bool& found) const;
        const overload_set* get_overload_set(const ast::identifier& id) const;

        bool                is_overloaded_def(const ast::identifier_type& id, bool& has_error) const;
        type_symbol_kind    get_symbol_kind(const ast::identifier_type& id) const;

        void                build_fresh_type_variable(const ast::identifier& id);

        void                import_type_overloads(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft) const;

        const type_alias_map&   get_alias_map() const;
        const graph&            get_graph() const;        

    private:
        void                add_overload(const ast::identifier& id, const ast::kind& ki,
                                const error_handler_ptr& eh);
        void                add_overload(const ast::identifier& id, ast::fixity_type ft,
                                const ast::kind& ki, const error_handler_ptr& eh);
        bool                is_overloaded(ast::unique_string id) const;
        bool                is_any_operator(ast::unique_string id);

        void                check_operator_type(ast::unique_string vi, oper::operator_declaration& oi,
                                loc_vec& ambig) const;

        bool                is_shell_module() const;
        bool                check_global_symbol(ast::unique_string name) const;
        scope_index         get_typealias_scope_index() const;
        void                build_scopes(const module& mod, const error_handler_ptr& eh, const ast::type& tc, 
                                const ast::kind& k, const type_var_scope& type_scope,
                                const type_var_scope& kind_scope);
        scope_index         get_new_typevar_scope();        
        ast::type           make_type_def_body(const ast::type& t, const ast::type_definition& def,
                                const error_handler_ptr& eh);
        ast::type           make_type_def_body(const ast::type& t, const ast::type_definition_var& def,
                                ast::kind& def_kind, const error_handler_ptr& eh);
        void                get_polarities(const ast::kind& k, ast::kind& body, std::vector<kind_polar>& ki_vec);
        int                 get_new_def_index();
        long                get_group_index();
        void                remove_definition(const ast::identifier& id);
        void                add_to_defmap(int index, fun_sym_info it);

    public:
        void                finalize_new_overload_set(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft);
};

};};

