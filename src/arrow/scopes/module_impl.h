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
#include "graph.h"
#include "local_scope.h"
#include "type_table.h"
#include "extern_functions.h"
#include "operator_table.h"
#include "overload_table.h"
#include "instantiation_table.h"
#include "symbol_definition.h"

#include <map>
#include <set>
#include <list>

namespace arrow { namespace details
{

class modules_provider
{
    public:
        //return null module if given module cannot be found
        virtual module  get_module(ast::unique_string name) const = 0;
};

}};

namespace arrow { namespace details
{

struct overload_term_config
{
    using definition_type   = symbol_definition_env;
    using function_type     = ast::type;
};

//-------------------------------------------------------------------
//                   module_impl   
//-------------------------------------------------------------------
class module_impl
{
    private:
        using owner_ptr     = std::weak_ptr<modules_impl>;
        using import_graph  = var_graph<ast::unique_string>;
        using string_loc    = std::pair<ast::unique_string, ast::location>;
                            /// import x alias x location
        using import_info   = std::tuple<ast::unique_string, ast::unique_string, ast::location>;
                            /// import alias => module
        using import_map    = std::map<ast::unique_string, module>;
                            /// module name => alias x location
        using string_map    = std::map<ast::unique_string, string_loc>;
        using defs_vector   = std::vector<ast::definitions>;
        using key_type      = var_info;
        using key_type_string= ast::unique_string;
        using key_type_over = std::pair<ast::unique_string, ast::fixity_type>;
        using fun_sym_map   = std::map<key_type, symbol_definition_env>;
        using term_list     = std::vector<ast::term>;
        using fun_sym_iter  = fun_sym_map::const_iterator;
        using fun_sym_info  = std::pair<bool,fun_sym_iter>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;
        using overload_type_set
                            = ad::overload_set<ad::overload_type_config>;
        using overload_table= overload_table<overload_term_config>;
        using overload_iter = overload_table::const_iterator;
        using over_iter2    = overload_table::iterator;
        using string_sym_map= std::map<key_type_string, overload_iter>;
        using string_sym_set= std::map<key_type_string, ast::location>;
        using def_map       = std::vector<fun_sym_info>;
        using local_scopes  = std::vector<local_scope>;
        using eval_terms    = std::vector<ast::term>;
        using loc_vec       = std::vector<ast::location>;
        using fun_map_list  = std::list<fun_sym_map>;
        using extern_function_ptr   = std::shared_ptr<ast::extern_function_eval>;
        using extern_type_ptr       = std::shared_ptr<ast::extern_type_function_eval>;
        using extern_func_prov      = extern_func_provider;        
        using inst_table    = instantiation_table<inst_term_conf>;

    private:
        ast::unique_string  m_name;
        module_index        m_index;
        owner_ptr           m_owner;
        bool                m_is_shell;
        local_scope         m_this_local_scope;
        local_scopes        m_local_scopes;
        type_table          m_type_table;
        def_map             m_def_map;
        import_map          m_imported_modules;
        string_sym_map      m_symbol_map_string;
        fun_map_list        m_func_map_save;
        mutable long        m_last_def_index;
        inst_table          m_instantiations;
        operator_table      m_operators;
        overload_table      m_overloads;

        graph               m_build_graph;
        string_map          m_build_imports;
        defs_vector         m_build_defs;
        eval_terms          m_build_eval_terms;
        fun_sym_map         m_build_function_map;
        string_sym_set      m_build_symbol_set_string;
        extern_func_prov    m_extern_func;

    public:
        module_impl(ast::unique_string mod_name, module_index mi, 
                    const owner_ptr& owner, bool is_shell_mod);
        ~module_impl();

        //--------------------------------------------------------------------
        //                      memory management
        //--------------------------------------------------------------------
        void                init(const module& owner);
        void                remove_cyclical_data();

        //--------------------------------------------------------------------
        //                      modules
        //--------------------------------------------------------------------
        void                add_imports(const std::vector<import_info>& imports);
        void                add_ast(const ast::definitions& defs);
        void                collect_operator_declarations(const error_handler_ptr& eh,
                                const module& owner);
        void                collect_symbol_definitions(const error_handler_ptr& eh,
                                const module& owner);
        void                add_import_edges(import_graph& graph);
        void                make_imports(const modules_provider& mods, std::set<string_loc>& undefined,
                                const error_handler_ptr& eh);        
        ast::unique_string  get_module_name() const;
        const graph&        get_graph() const; 
        bool                is_shell_module() const;
        void                finish_building();

        module              get_unnamed_module(const ast::location& loc, const module& owner, 
                                const error_handler_ptr& eh) const;
        module              get_module(const ast::location& loc, const module& owner,
                                const ast::identifier& id, bool self_allowed, 
                                const error_handler_ptr& eh) const;
        bool                is_module_name(const ast::identifier& id, bool self_allowed) const;

        //--------------------------------------------------------------------
        //                      scopes
        //--------------------------------------------------------------------
        module_index        get_module_index() const;
        const local_scope&  get_this_local_scope() const;

        local_scope         get_scope_for_let(const ast::let_term& let) const;
        local_scope         get_scope_for_lambda(const ast::lambda_term& lam) const;
        local_scope         get_scope_for_mu(const ast::mu_term& lam) const;
        local_scope         get_scope_for_unpack(const ast::unpack_term& lam) const;
        local_scope         get_scope_for_case(const ast::variant_case_term& lam) const;

        local_scope         get_scope_by_index(scope_index index) const;

        local_scope         get_scope_for_var(const ast::identifier& vi) const;                

        void                type_check_types(typing::type_checker& tc, const error_handler_ptr& eh,
                                const module& mod);
        void                build_type_scopes(const module& mod, const error_handler_ptr& eh);
        void                type_desugaring(const module& mod, const error_handler_ptr& eh);
        void                build_type_dependency_graph(const module& mod, const error_handler_ptr& eh);

        type_var_scope      new_typevar_scope(const ast::location& loc);
        type_var_scope      new_kindvar_scope(const ast::location& loc);
        type_var_scope      make_new_var_scope(const ast::location& loc, 
                                const type_var_scope& parent);   

        void                build_fresh_type_variable(const ast::identifier& id);

        local_scope         create_child_scope(const details::local_scope_impl& parent);        

        //--------------------------------------------------------------------
        //                      symbol table
        //--------------------------------------------------------------------       
        void                add_definition(const ast::identifier& vi, 
                                const symbol_definition_env& def, const error_handler_ptr& eh);
        void                add_operator_declaration(const ast::identifier& vi, 
                                const ast::operator_info& fix, const error_handler_ptr& eh);
        void                add_overload(const ast::identifier& id, const ast::type& ty, 
                                const error_handler_ptr& eh);
        void                add_overload(const ast::identifier& id, ast::fixity_type ft,
                                const ast::type& ty, const error_handler_ptr& eh);
        void                add_instantiation_term_type(const ast::identifier_term& id, 
                                const ast::term_definition& td, const std::vector<ast::type>& args, 
                                bool true_inst);
        void                add_instantiation_type_type(const ast::identifier_type& id, 
                                const ast::type_definition& td, const std::vector<ast::type>& args, 
                                bool true_inst);
        void                add_instantiation_term_kind(const ast::identifier_term& id, 
                                const ast::term_definition& td, const std::vector<ast::kind>& args, 
                                bool true_inst);
        void                add_instantiation_type_kind(const ast::identifier_type& id, 
                                const ast::type_definition& td, const std::vector<ast::kind>& args, 
                                bool true_inst);
        void                register_local_function_subst(const ast::term_definition& td);
        ast::identifier     get_instantiation_term_type(const ast::identifier_term& base_id, 
                                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                                long& num_matched);
        ast::identifier     get_instantiation_type_type(const ast::identifier_type& base_id, 
                                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                                long& num_matched);
        ast::identifier     get_instantiation_term_kind(const ast::identifier_term& base_id, 
                                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                                long& num_matched);
        ast::identifier     get_instantiation_type_kind(const ast::identifier_type& base_id, 
                                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                                long& num_matched);
        void                add_eval_term(const ast::term_use& vi, const error_handler_ptr& eh);

        void                add_type_definition(const ast::type_definition& vi, 
                                const error_handler_ptr& eh);
        void                add_type_definition(const ast::type_definition_var& vi, 
                                const error_handler_ptr& eh);

        bool                check_symbol(const ast::identifier& id, const error_handler_ptr& eh);

        bool                is_symbol_defined(const ast::identifier& id, bool search_imports,
                                std::vector<ast::location>& ambiguous_locations) const;
        bool                is_operator_declared(ast::unique_string vi, ast::fixity_type ft,
                                bool search_imports) const;
        bool                is_any_operator(ast::unique_string vi, bool search_imports) const;
        bool                is_overloaded(const ast::unique_string id) const;

        def_index           get_type_definition_index(ast::unique_string ti, bool search_imports, 
                                ast::location& loc, bool& found, loc_vec& ambiguous_locations) const;

        const overload_type_set&
                            get_type_definition(ast::unique_string ti, bool search_imports, 
                                bool& found, loc_vec& ambiguous_locations) const;

        const overload_set& get_symbol_definition(ast::unique_string str, bool search_imports, 
                                bool& found, loc_vec& ambiguous_locations) const;
        oper::operator_declaration 
                            get_operator_declaration(ast::unique_string str, ast::fixity_type ft,
                                bool search_imports, bool& found, 
                                std::vector<ast::location>& ambig_locations) const;

        void                get_all_type_definition(ast::unique_string ti, bool search_imports, 
                                std::vector<const overload_type_set*>& defs) const;        

        void                get_all_symbol_definition(ast::unique_string str, bool search_imports, 
                                std::vector<const overload_set*>& defs) const;

        void                get_all_operator_declaration(ast::unique_string str, 
                                bool search_imports, oper::operator_declaration& ret) const;

        void                check_operator_type(ast::unique_string vi, 
                                oper::operator_declaration& oi, bool search_imports, 
                                std::vector<ast::location>& ambig_locations) const;

        extern_function_ptr get_extern_function_def(const error_handler_ptr& eh, 
                                const ast::identifier& id) const;
        extern_type_ptr     get_extern_type_function_def(const error_handler_ptr& eh, 
                                const ast::identifier& id) const;

        const fun_sym_map&  get_function_map() const;
        fun_sym_map&        get_function_map_modify();
        const term_list&    get_eval_terms() const;
        term_list&          get_eval_terms_modify();

        ast::kind           get_type_constructor_kind(const error_handler_ptr& eh, 
                                const ast::identifier_type& id, bool& overloaded, 
                                bool& has_error) const;
        ast::type           get_type_constructor_definition(const error_handler_ptr& eh, 
                                const ast::identifier_type& id, bool& overloaded, 
                                bool& has_error) const;
        ast::type           get_symbol_type(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded,
                                bool& has_error) const;
        ast::term           get_symbol_term(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded, 
                                bool& has_error) const;
        const overload_set* get_overload_set(const error_handler_ptr& eh, 
                                const ast::identifier_term& id) const;
        const overload_set* get_overload_set(const ast::identifier& id) const;
        const overload_type_set* 
                            get_overload_type_set(const ast::identifier& id) const;
        ast::overload_index get_overload_index(const overload_set* os) const;
        const overload_set* get_overload_set(const ast::overload_index* oi) const;
        bool                is_overloaded_term_def(const ast::identifier_term& id,
                                bool& has_error) const;
        bool                is_overloaded_type_def(const ast::identifier_type& id,
                                bool& has_error) const;

        term_symbol_kind    get_term_symbol_kind(const ast::identifier_term& id) const;
        type_symbol_kind    get_type_symbol_kind(const ast::identifier_type& id) const;

        void                import_overloads(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft) const;
        void                import_type_overloads(overload_type_set& os, ast::unique_string id, 
                                ast::fixity_type ft, bool from_imports_only) const;

    private:
        void                add_function_definition(const ast::identifier& vi, 
                                const symbol_definition_env& def, const error_handler_ptr& eh);        
        def_index           get_new_def_index() const;
        long                get_group_index() const;        
        ast::unique_string  get_shell_string() const;

    public:
        void                finalize_new_overload_set(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft);
        void                remove_definition(const ast::identifier& id);
};

};}
