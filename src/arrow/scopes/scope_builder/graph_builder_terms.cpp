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

#include "graph_builder_terms.h"
#include "scopes/overload_set.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_graph_visitor   
//------------------------------------------------------------------
build_graph_visitor::build_graph_visitor(const error_handler_ptr& eh, 
            const module& mod, const graph& g, bool build)
    :m_mod(mod), m_eh(eh), m_graph(g), m_build(build)
    ,m_type_builder(eh, mod, graph(), false)
{};

void build_graph_visitor::eval(const ast::overloaded_symbol& ast)
{
    error::scope_error_msg(m_eh).error_overloaded_symbol_not_allowed
                                    (ast->get_location());
};

bool build_graph_visitor::build_identifier(const ast::identifier& id, 
                            const local_scope& ls, bool& is_bound)
{
    is_bound    = false;

    // symbol can have definition at this stage; in this case
    // this is a global symbol
    if (id.get_symbol_data()->has_definition() == true)
        return true;

    if (ls.is_bound(id) == true)
    {
        is_bound = true;
        return true;
    };

    std::vector<ast::location> ambig;
    if (m_mod.is_symbol_defined(id, true, ambig) == true)
    {
        if (ambig.size() > 0)
        {
            error::scope_error_msg(m_eh).error_ambiguous_symbol
                    (id->get_location(), id.get_name_simple(), ambig);
            return false;
        };

        return true;
    }

    ast::unique_string vs       = id.get_name_simple();
    error::scope_error_msg(m_eh).error_undefined_variable(vs, id->get_location());
    return false;
};

void build_graph_visitor::eval(const ast::identifier_term& ast)
{
    const ast::identifier& v = ast.get_identifier();
    const local_scope& ls    = m_mod.get_scope_for_var(v);
    bool is_bound;
    bool ok                  = build_identifier(v, ls, is_bound);

    if (ok == false)
        return;    

    if (is_bound == true)
    {
        ls.add_referred(v);
        return;
    }

    ast::symbol_data* sd    = v.get_symbol_data();
    ad::module_index mi     = sd->get_def_index().get_module();

    if (m_build == true)
    {
        if (mi != m_mod.get_module_index())
            m_graph.add_extern_edge(v);
        else
            m_graph.add_edge(v);
    };
};

void build_graph_visitor::eval(const ast::let_term& ast)
{
    if (m_build == false)
    {
        // just assign def indices
        return base_type::eval(ast);
    };

    using overload_set      = ad::overload_set<ad::overload_term_config>;

    const ast::term& ex     = ast.get_body();
    local_scope ls          = m_mod.get_scope_for_let(ast);

    visit(ex);

    using fun_sym_map       = local_scope::fun_sym_map;
    const fun_sym_map& loc  = ls.get_local_vars();

    long s          = (long)loc.size();
    graph loc_gr    = ls.get_graph();

    ls.building_graph_start();
    loc_gr.reserve(s);

    //collect internal nodes
    for (const auto& pos : loc)
    {
        const ad::symbol_definition_env& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi       = sd.get_symbol();
        const ast::type& sig            = sd.get_signature();        

        auto scope = m_eh->push_error_scope();

        m_graph.add_node(vi);

        visit(sig);

        const overload_set* os      = ls.get_overload_set(vi);

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

    using key_type      = fun_sym_map::key_type;
    using delayed_map   = std::map<key_type, const ad::symbol_definition_env*>;

    delayed_map delayed;

    for (const auto& pos : loc)
    {
        const ad::symbol_definition_env& sd = pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi       = sd.get_symbol();

        if (ls.is_referred_function(vi) == false)
        {
            delayed.insert(delayed_map::value_type(pos.first, &pos.second));
            continue;
        }

        bool has_body;
        const ast::term& body   = sd.get_body(has_body);

        auto scope = m_eh->push_error_scope();

        loc_gr.current_node_to(vi);
        visit(body);

        if (m_eh->has_errors() == true)
            sd.set_error_flag();
    };

    //now process delayed definitions
    for(;;)
    {
        for (const auto& pos : delayed)
        {
            const ad::symbol_definition_env& sd = *pos.second;

            if (sd.has_error() == true)
            {
                m_eh->add_error();
                continue;
            };

            const ast::identifier& vi   = sd.get_symbol();

            if (ls.is_referred_function(vi) == true)
            {
                bool has_body;
                const ast::term& body   = sd.get_body(has_body);

                auto scope = m_eh->push_error_scope();

                loc_gr.current_node_to(vi);

                visit(body);

                if (m_eh->has_errors() == true)
                    sd.set_error_flag();

                delayed.erase(pos.first);
                goto lab_continue_loop;
            }
        };

        //delayed map is empty or no definition is referred
        break;

        lab_continue_loop:
        continue;
    };

    //warn about unused definitions;
    for (const auto& pos : delayed)
    {
        ast::unique_string str   = pos.first.get_name_simple();
        ast::location loc2       = pos.second->get_location();
        error::scope_error_msg(m_eh).warning_unused_let_definition(str, loc2);

        const ad::symbol_definition_env& sd = *pos.second;

        if (sd.has_error() == true)
        {
            m_eh->add_error();
            continue;
        };

        const ast::identifier& vi   = sd.get_symbol();

        bool has_body;
        const ast::term& body   = sd.get_body(has_body);

        auto scope = m_eh->push_error_scope();

        loc_gr.current_node_to(vi);
        visit(body);

        if (m_eh->has_errors() == true)
            sd.set_error_flag();
    };

    ls.building_graph_end();
};

};};};
