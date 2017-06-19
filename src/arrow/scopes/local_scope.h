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
#include "arrow/ast/ast.h"
#include "symbol_data.h"
#include "semantics/operators/operator_declaration.h"

#include <memory>
#include <list>
#include <map>

namespace arrow
{

namespace ad = arrow::details;

class local_var_info
{    
    private:
        using key_type_over = std::pair<ast::unique_string, ast::fixity_type>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;
        using overload_map  = std::map<key_type_over, overload_set>;
        using iter_over     = overload_map::const_iterator;

    private:
        ast::type               m_pattern_type;
        term_symbol_kind        m_symbol_kind;

        ///def is available only in scopes for let expression
        iter_over               m_def_iter;
        bool                    m_is_def;

    public:
        ast::location           m_location;
        ad::def_index           m_def_index;
        bool                    m_is_referred;
        bool                    m_has_error;

    public:
        local_var_info();
        local_var_info(const ast::location& loc, ad::def_index di, const ast::type& t);
        local_var_info(const ast::location& loc, iter_over term_def_iter);

        /// only for let scopes
        const ast::identifier&  get_symbol(const error_handler_ptr& eh) const;        

        //should be called only for pattern vars scopes
        const ast::type&        get_pattern_signature(bool& has_error) const;
        void                    update_pattern_type(const ast::type& ty);

        term_symbol_kind        get_symbol_kind() const;
        ad::def_index           get_definition_index() const;
        void                    set_referred();
        bool                    is_referred() const;
};

class local_variables
{
    public:
        using key_type      = ast::unique_string;
        using def_map       = std::map<key_type, local_var_info>;

    public:
        def_map             m_vars;
};

class local_scope
{
    private:
        using impl_type     = std::shared_ptr<details::local_scope_impl>;

    public:
        using key_type      = ad::var_info;
        using components    = std::vector<std::vector<key_type>>;
        using fun_sym_map   = std::map<key_type, ad::symbol_definition_env>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;

    private:
        impl_type           m_impl;

    private:
        /// initialize to empty scope
        local_scope(const module& m, details::scope_index index);

        local_scope(const details::local_scope_impl& parent, details::scope_index index);

        friend details::module_impl;

    public:
        /// internal use only
        local_scope(const impl_type& impl);

        static local_scope      null_scope();

        details::scope_index    get_scope_index() const;

        /// create a child scope
        local_scope         new_scope(const module& owner);

        /// return true if given variable is defined in this scope
        /// or parent scopes; if is defined, then assign def index
        bool                is_bound(const ast::identifier& vi) const;

        /// return true if a local function with name vi defined in this
        /// scope is referred (or could be referred in case of overloaded
        /// symbols)
        bool                is_referred_function(const ast::identifier& vi) const;

        /// return true is this is an uninitialized object; notice that
        /// default constructor returns an initialized object
        bool                is_null() const;

        const graph&        get_graph() const;
        void                building_graph_start();
        void                building_graph_end();

        const components&   get_connected_components() const;

        //--------------------------------------------------------------------
        //                      symbol table
        //--------------------------------------------------------------------
        /// add operator declaration to this scope
        void                add_operator_declaration(ad::module_index mi, const ast::identifier& vi, 
                                const ast::operator_info& fix, const error_handler_ptr& eh);

        void                add_overload(ad::module_index mi, const ast::identifier& id, 
                                const ast::type& ty, const error_handler_ptr& eh);

        /// add local symbol definitions
        void                add_pattern_var(ad::module_index mi, const ast::annotated_identifier& id,
                                const error_handler_ptr& eh);

        void                add_let_var(ad::module_index mi, const ast::identifier& vi, 
                                const ast::term_definition& def, const error_handler_ptr& eh);

        /// get structure representing local variables
        const fun_sym_map&  get_local_vars() const;
        fun_sym_map&        get_local_vars_modify() const;

        /// return true if given variable is defined in this scope
        /// or parent scopes
        bool                is_symbol_defined(const ast::identifier& id, bool search_parent,
                                std::vector<ast::location>& ambiguous_locations) const;

        /// return true if given symbol (pattern var or local function
        /// is defined in this local scope or parrent local scopes
        bool                is_local_symbol_defined(const module& mod, 
                                const ad::var_info& vi) const;

        /// return true if operator with given fixity is registered
        /// all infix fixity types are considered as equal
        bool                is_operator_declared(ast::unique_string vi, ast::fixity_type ft, 
                                bool search_parent) const;

        bool                check_symbol(const ast::identifier& id, 
                                const error_handler_ptr& eh) const;

        /// return location of given symbol declaration; return empty 
        /// location if this symbol is not declared
        ast::location       get_symbol_definition(ast::unique_string str, bool search_imports, 
                                bool& found, std::vector<ast::location>& ambiguous_locations) const;

        /// return declarations of given operator; set found tp false if operator
        /// is not declared; if search_imports = true then also imported 
        /// modules are checked
        oper::operator_declaration get_operator_declaration(ast::unique_string str, 
                                ast::fixity_type ft, bool search_imports, 
                                bool& found, std::vector<ast::location>& ambig) const;

        /// add referred bound variable
        void                add_referred(const ast::identifier& vi) const;

        ast::type           get_symbol_type(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded, 
                                bool& has_error) const;
        ast::term           get_symbol_term(const error_handler_ptr& eh, 
                                const ast::identifier_term& id, bool& overloaded, 
                                bool& has_error) const;
        const overload_set* get_overload_set(const error_handler_ptr& eh, 
                                const ast::identifier_term& id) const;
        const overload_set* get_overload_set(const ast::identifier& id) const;
        bool                is_overloaded_term_def(const ast::identifier_term& id,
                                bool& has_error) const;

        term_symbol_kind    get_term_symbol_kind(const ast::identifier_term& id) const;

        void                update_pattern_var(const ast::identifier& id, const ast::type& new_ty);
        void                update_let_definitions(const ast::definitions& defs);

        bool                is_overloaded(const ast::unique_string id) const;
        bool                is_any_operator(ast::unique_string vi) const;

        void                import_overloads(overload_set& os, ast::unique_string id, 
                                ast::fixity_type ft) const;

        /// check if given symbol is registered as an operator; return 
        /// operator fixities
        void                check_operator_type(ast::unique_string vi, 
                                oper::operator_declaration& oi, bool search_parent,
                                std::vector<ast::location>& ambig) const;
};

};
