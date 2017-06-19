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
#include "scopes/graph.h"
#include "scopes/symbol_data.h"
#include "scopes/module_impl.h"

#include <map>
#include <list>

namespace arrow { namespace sema { namespace details
{

namespace ad = arrow::details;

//------------------------------------------------------------------
//                   scoped_local_scope   
//------------------------------------------------------------------
//utility class; change current scope and restore old scope at exit
class scoped_local_scope
{
    private:
        local_scope&    old_scope_ref;
        local_scope     old_scope;

    public:
        scoped_local_scope(local_scope& old, const local_scope& new_scope)
            :old_scope_ref(old), old_scope(old)
        {
            old_scope_ref = new_scope;
        };

        ~scoped_local_scope()
        {
            old_scope_ref = old_scope;
        };
};

class scope_builder
{
    private:
        error_handler_ptr   m_eh;
        module              m_module;
        graph               m_graph;

    public:
        /// initialize dependency graph builder
        scope_builder(const module& mod, const error_handler_ptr& eh);

        /// add top-level definitions to module
        void            collect_symbol_definitions(const ast::definitions& ast);
        void            collect_operators(const ast::definitions& ast);

        /// add definitions to current environment
        void            build_scope();

        /// assign scopes to identifiers in a type
        void            build_type_scope(const ast::type& t, const ast::kind& k,
                            const ad::type_var_scope& type_scope, 
                            const ad::type_var_scope& kind_scope);

        /// build dependecy graph
        void            build_dependecy_graph();
        void            build_type_dependecy_graph(const ad::type_table& tt);

    private:
        void            build_from(const ast::identifier& vi, const ast::type& sig, 
                            const ast::term& ex);
        void            build_from(const ast::term& ex);
        void            build_graph_from(const ast::identifier& vi, const ast::term& ex);
        void            build_type_graph_from(const ast::identifier& vi, const ast::type& ex);
};

//------------------------------------------------------------------
//                   process_term_seq   
//------------------------------------------------------------------
class build_scope_visitor;

template<class Seq>
struct process_sequence_config;

template<class Seq> struct seq_config;

template<> 
struct seq_config<ast::term_sequence>
{ 
    using basic_type        = ast::term; 
    using applic_type       = ast::type;
    using applic_kind_type  = ast::kind;
};

template<> 
struct seq_config<ast::type_sequence>
{ 
    using basic_type        = ast::type; 
    using applic_type       = ast::type;
    using applic_kind_type  = ast::kind;
};

template<class Seq, class Owner>
class process_sequence
{
    private:
        using basic_type        = typename seq_config<Seq>::basic_type;
        using applic_type       = typename seq_config<Seq>::applic_type;
        using applic_kind_type  = typename seq_config<Seq>::applic_kind_type;

        using type_vec          = std::vector<applic_type>;
        using kind_vec          = std::vector<applic_kind_type>;
        using config            = process_sequence_config<Seq>;        

    private:
        bool                    m_in_module_path;
        bool                    m_self_allowed;
        module                  m_root_mod;
        module                  m_current_mod;        
        error_handler_ptr       m_eh;
        Owner*                  m_owner;
        //ad::scope_index       m_scope;
        ast::identifier         m_last_mod;

    public:
        process_sequence(const module& mod, const error_handler_ptr& eh, Owner* owner)
            :m_in_module_path(true), m_self_allowed(true), m_root_mod(mod)
            ,m_current_mod(mod), m_eh(eh), m_owner(owner)
            //, m_scope(scope)
        {};

        void    make(const Seq& seq);

    private:
        void    build_identifier(const ast::identifier& id);
        void    build_qualified_identifier(const ast::identifier& id);
        void    process_term(const basic_type& t, long pos);
        void    process_proj();
        void    process_dot(const ast::identifier& id, long pos);
        void    process_type_applic(const type_vec& args);
        void    process_kind_applic(const kind_vec& args);
        void    finalize();
};

};};};
