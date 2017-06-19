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
#include "scope_builder.h"
#include "arrow/ast/ast_visitor.h"
#include "insert_symbols.h"
#include "scope_builder_types.h"
#include "scope_builder_terms.h"
#include "graph_builder_terms.h"
#include "graph_builder_types.h"
#include "scopes/overload_set.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   scope_builder   
//------------------------------------------------------------------
scope_builder::scope_builder(const module& mod, const error_handler_ptr& eh)
    :m_eh(eh), m_module(mod), m_graph(mod.get_graph())
{};

void scope_builder::collect_operators(const ast::definitions& ast)
{
    long s = ast.size();    

    //add definitions to the environment
    for (long i = 0; i < s; ++i)
    {
        const ast::definition& def      = ast.get_definition(i);
        insert_operators().visit(def, m_module, m_eh);
    };
};

void scope_builder::collect_symbol_definitions(const ast::definitions& ast)
{
    long s = ast.size();    

    //add definitions to the environment
    for (long i = 0; i < s; ++i)
    {
        const ast::definition& def      = ast.get_definition(i);
        insert_symbols().visit(def, m_module, m_eh);
    };
};

void scope_builder::build_type_scope(const ast::type& t, const ast::kind& k,
                                     const ad::type_var_scope& type_scope,
                                     const ad::type_var_scope& kind_scope)
{
    build_type_scope_visitor ae(m_eh, m_module, type_scope, kind_scope);
    ae.visit(k);
    ae.visit(t);
};

void scope_builder::build_scope()
{
    const module::fun_sym_map& sym_map = m_module.get_function_map();

    for (auto& pos : sym_map)
    {
        const ad::symbol_definition_env& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::type& sig            = sd.get_signature();
        const ast::identifier& vi       = sd.get_symbol();
        bool has_body;
        const ast::term& ex             = sd.get_body(has_body);

        auto scope = m_eh->push_error_scope();

        build_from(vi, sig, ex);

        if (m_eh->has_errors() == true)
            sd.set_error_flag();
    };

    using term_list         = module::term_list;
    const term_list& terms  = m_module.get_eval_terms();

    for (const auto& ex : terms)
        build_from(ex);
};

void scope_builder::build_from(const ast::identifier& vi, const ast::type& sig, 
                               const ast::term& ex)
{
    build_scope_visitor ae(m_eh, vi->get_location(), m_module);

    if (sig)
        ae.process_toplevel_signature(sig);

    ae.start_expression();

    if (ex)
        ae.visit(ex);
};

void scope_builder::build_from(const ast::term& ex)
{
    if (!ex)
        return;

    build_scope_visitor ae(m_eh, ex->get_location(), m_module);

    ae.start_expression();
    ae.visit(ex);
};

void scope_builder::build_type_dependecy_graph(const ad::type_table& tt)
{
    using overload_set          = ad::overload_set<ad::overload_type_config>;

    using alias_map             = ad::type_table::type_alias_map;
    const alias_map& sym_map    = tt.get_alias_map();

    long s      = (long)sym_map.size();

    m_graph     = tt.get_graph();
    m_graph.reserve(s);

    //collect internal nodes
    for (const auto& pos : sym_map)
    {
        const ad::type_alias_info& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi   = sd.get_name();

        auto scope = m_eh->push_error_scope();

        m_graph.add_node(vi);

        const overload_set* os      = m_module.get_overload_type_set(vi);

        if (!os)
            continue;
        if (os->is_overload() == false)
            continue;

        const ast::identifier& root_id  = os->get_identifier();

        m_graph.current_node_to(root_id);
        m_graph.add_edge(vi);

        if (m_eh->has_errors() == true)
            sd.set_error();
    };

    //build dependency graph
    for (const auto& pos : sym_map)
    {
        const ad::type_alias_info& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi       = sd.get_name();
        const ast::type& body           = sd.get_def();

        auto scope  = m_eh->push_error_scope();

        build_type_graph_from(vi, body);

        if (m_eh->has_errors() == true)
            sd.set_error();
    };
};

void scope_builder::build_dependecy_graph()
{
    using overload_set      = ad::overload_set<ad::overload_term_config>;

    const module::fun_sym_map& sym_map = m_module.get_function_map();

    long s      = (long)sym_map.size();

    m_graph.reserve(s);

    //collect internal nodes
    for (const auto& pos : sym_map)
    {
        const ad::symbol_definition_env& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi       = sd.get_symbol();

        auto scope = m_eh->push_error_scope();

        m_graph.add_node(vi);

        const overload_set* os          = m_module.get_overload_set(vi);

        if (!os)
            continue;
        if (os->is_overload() == false)
            continue;

        const ast::identifier& root_id  = os->get_identifier();

        m_graph.current_node_to(root_id);
        m_graph.add_edge(vi);

        if (m_eh->has_errors() == true)
            sd.set_error_flag();
    };

    //build dependency graph
    for (const auto& pos : sym_map)
    {
        const ad::symbol_definition_env& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi       = sd.get_symbol();
        bool has_body;
        const ast::term& ex             = sd.get_body(has_body);
        const ast::type& ty             = sd.get_signature();

        auto scope = m_eh->push_error_scope();

        if (has_body == true)
            build_graph_from(vi, ex);

        if (m_eh->has_errors() == true)
            sd.set_error_flag();

        build_type_graph_visitor ae(m_eh, m_module, graph(), false);
        ae.visit(ty);
    };

    // val terms
    using term_list         = module::term_list;
    term_list& terms        = m_module.get_eval_terms_modify();

    for (auto& ex : terms)
    {
        auto scope = m_eh->push_error_scope();

        build_graph_visitor ae(m_eh, m_module, m_graph, false);
        ae.visit(ex);
    }
};

void scope_builder::build_graph_from(const ast::identifier& vi, const ast::term& ex)
{
    m_graph.current_node_to(vi);
    build_graph_visitor ae(m_eh, m_module, m_graph, true);
    ae.visit(ex);
};

void scope_builder::build_type_graph_from(const ast::identifier& vi, const ast::type& ex)
{
    m_graph.current_node_to(vi);
    build_type_graph_visitor ae(m_eh, m_module, m_graph, true);
    ae.visit(ex);
};

};};};
