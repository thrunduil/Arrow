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

#include "graph.h"
#include "scopes/symbol_data.h"

#include <map>
#include <set>
#include <iostream>

#pragma warning(push)

//bug in boost::graph?
#pragma warning(disable:4172)   //returning address of local variable or temporary

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/strong_components.hpp"

namespace arrow { namespace details
{

//-------------------------------------------------------------------
//                   graph_impl   
//-------------------------------------------------------------------
struct vertex_descr
{
    long            m_code;
    bool            is_internal;
};

class graph_impl
{
    private:
        using key_type      = var_info;
        using vertex_type   = key_type;
        using edge_container= boost::listS;
        using node_container= boost::vecS;
        using directed      = boost::bidirectionalS;
        using graph_type    = boost::adjacency_list<edge_container, node_container, directed,
                                    vertex_type>;
        using map_vectex    = std::map<vertex_type, vertex_descr>;
        using symbol_group  = std::vector<key_type>;
        using symbol_set    = std::set<key_type>;
        using component_vec = std::vector<symbol_group>;        

    private:
        graph_type          m_graph;
        map_vectex          m_vertex_map;
        vertex_descr        m_to_descr;

    public:
        graph_impl();
        ~graph_impl();

        long                get_number_nodes() const;
        long                get_number_edges() const;

        void                reserve(long number_nodes);
        void                current_node_to(const ast::identifier& to);
        void                add_node(const ast::identifier& vi);

        void                add_edge(const ast::identifier& from);
        void                add_extern_edge(const ast::identifier& from);

        void                strongly_connected_components(component_vec& comp) const;
        void                get_referred_symbols(const symbol_group& group,
                                 symbol_set& referred) const;

        bool                is_self_recursive(const key_type& key) const;

    private:
        bool                is_external(const vertex_type& node) const;
        vertex_descr        get_node_descriptor(const vertex_type& node) const;
};

graph_impl::graph_impl()
    :m_graph()
{};

graph_impl::~graph_impl()
{};

void graph_impl::reserve(long number_nodes)
{
    m_graph.m_vertices.reserve(number_nodes);
};

void graph_impl::add_node(const ast::identifier& vi)
{
    key_type key        = key_type(vi);

    long descriptor     = (long)add_vertex(key, m_graph);
    m_vertex_map[key]   = vertex_descr{descriptor, true};
};

void graph_impl::current_node_to(const ast::identifier& to)
{
    key_type key        = key_type(to);

    auto pos            = m_vertex_map.find(key);

    if (pos != m_vertex_map.end())
    {
        m_to_descr      = pos->second;
        return;
    };

    long descriptor     = (long)add_vertex(key, m_graph);
    m_to_descr          = vertex_descr{descriptor, true};
    m_vertex_map[key]   = m_to_descr;
};

void graph_impl::add_edge(const ast::identifier& from)
{
    key_type key        = key_type(from);

    auto pos            = m_vertex_map.find(key);
    long descr_from;

    if (pos == m_vertex_map.end())
    {
        descr_from  = (long)add_vertex(key, m_graph);
        m_vertex_map.insert(pos,map_vectex::value_type(key, {descr_from, false}));
    }
    else
    {
        descr_from  = pos->second.m_code;
    }

    boost::add_edge(m_to_descr.m_code, descr_from, m_graph);
};

void graph_impl::add_extern_edge(const ast::identifier& from)
{
    key_type key        = key_type(from);

    auto pos        = m_vertex_map.find(key);
    long descr_from;

    if (pos == m_vertex_map.end())
    {
        descr_from  = (long)add_vertex(key, m_graph);
        m_vertex_map.insert(pos,map_vectex::value_type(key, {descr_from, false}));
    }
    else
    {
        descr_from  = pos->second.m_code;
    }

    boost::add_edge(m_to_descr.m_code, descr_from, m_graph);
};

long graph_impl::get_number_nodes() const
{
    return (long)num_vertices(m_graph);
}
long graph_impl::get_number_edges() const
{
    return (long)num_edges(m_graph);
}

void graph_impl::strongly_connected_components(component_vec& comp) const
{
    long n_nodes    = get_number_nodes();
    std::vector<long> component(n_nodes);

    auto component_map  = boost::make_iterator_property_map(component.begin(), 
                                    get(boost::vertex_index, m_graph));
    int num = strong_components(m_graph, component_map);

    comp.resize(num);

    for (int i = 0; i < n_nodes; ++i)
    {
        int position            = component[i];
        const key_type& node    = m_graph[i];

        if (is_external(node) == true)
            continue;

        comp[position].push_back(node);
    };
};

bool graph_impl::is_self_recursive(const key_type& key) const
{
    auto pos = m_vertex_map.find(key);
    if (pos == m_vertex_map.end())
        return false;

    auto descr = pos->second.m_code;
    return boost::edge(descr, descr, m_graph).second;
};

void graph_impl::get_referred_symbols(const symbol_group& group, symbol_set& referred) const
{
    for (const key_type& node : group)
    {
        auto descr          = get_node_descriptor(node);
        auto edge_iterator  = out_edges(descr.m_code, m_graph);

        for (auto it = edge_iterator.first; it != edge_iterator.second; ++it)
        {
            long node_to    = (long)target(*it, m_graph);
            auto to_descr   = m_graph[node_to];

            referred.insert(to_descr);
        };
    };
};

bool graph_impl::is_external(const vertex_type& node) const
{
    auto pos = m_vertex_map.find(node);

    //should never happened
    if (pos == m_vertex_map.end())
        return true;

    return pos->second.is_internal == false;
}

vertex_descr graph_impl::get_node_descriptor(const vertex_type& node) const
{
    auto pos = m_vertex_map.find(node);

    if (pos != m_vertex_map.end())
        return pos->second;

    return vertex_descr();
};

//-------------------------------------------------------------------
//                   var_graph_impl   
//-------------------------------------------------------------------
template<class Key>
class var_graph_impl
{
    private:
        using vertex_type   = Key;
        using vertex_descr  = long;
        using edge_container= boost::listS;
        using node_container= boost::vecS;
        using directed      = boost::bidirectionalS;
        using graph_type    = boost::adjacency_list<edge_container, node_container, directed,
                                    vertex_type>;
        using map_vectex    = std::map<vertex_type, vertex_descr>;
        using component_vec = std::vector<std::vector<Key>>;
                            
    private:
        graph_type          m_graph;
        map_vectex          m_vertex_map;

    public:
        var_graph_impl();
        ~var_graph_impl();

        void                reserve(long number_nodes);
        long                get_number_nodes() const;
        void                add_edge(const Key& mod, const Key& import);
        void                add_node(const Key& mod);
        void                strongly_connected_components(component_vec& comp) const;

    private:
        long                get_descriptor(const Key& name);
};

template<class Key>
var_graph_impl<Key>::var_graph_impl()
{};

template<class Key>
var_graph_impl<Key>::~var_graph_impl()
{};

template<class Key>
void var_graph_impl<Key>::reserve(long number_nodes)
{
    m_graph.m_vertices.reserve(number_nodes);
}

template<class Key>
long var_graph_impl<Key>::get_number_nodes() const
{
    return (long)num_vertices(m_graph);
}

template<class Key>
void var_graph_impl<Key>::add_edge(const Key& mod, const Key& import)
{
    auto descr_mod      = get_descriptor(mod);
    auto descr_imp      = get_descriptor(import);
    boost::add_edge(descr_mod, descr_imp, m_graph);
}

template<class Key>
void var_graph_impl<Key>::add_node(const Key& mod)
{
    auto descr_mod      = get_descriptor(mod);
    (void)descr_mod;
}

template<class Key>
long var_graph_impl<Key>::get_descriptor(const Key& name)
{
    auto pos = m_vertex_map.find(name);

    if (pos != m_vertex_map.end())
        return pos->second;

    long descr      = (long)add_vertex(name, m_graph);
    m_vertex_map.insert(pos, map_vectex::value_type(name, descr));
    return descr;

};

template<class Key>
void var_graph_impl<Key>::strongly_connected_components(component_vec& comp) const
{
    long n_nodes    = get_number_nodes();
    std::vector<long> component(n_nodes);

    auto component_map  = boost::make_iterator_property_map(component.begin(), 
                                    get(boost::vertex_index, m_graph));
    int num = strong_components(m_graph, component_map);

    comp.resize(num);

    for (int i = 0; i < n_nodes; ++i)
    {
        int position    = component[i];

        comp[position].push_back(m_graph[i]);
    };
};

}};

namespace arrow
{

//-------------------------------------------------------------------
//                   graph   
//-------------------------------------------------------------------
graph::graph()
    :m_impl(new details::graph_impl())
{};

graph::graph(const impl_type& ptr)
    :m_impl(ptr)
{};

graph::~graph()
{};

void graph::reserve(long number_nodes)
{
    m_impl->reserve(number_nodes);
};
void graph::clear()
{
    *this = graph();
};


void graph::add_node(const ast::identifier& vi)
{
    return m_impl->add_node(vi);
}
void graph::add_edge(const ast::identifier& from)
{
    return m_impl->add_edge(from);
}
void graph::add_extern_edge(const ast::identifier& from)
{
    return m_impl->add_extern_edge(from);
};
void graph::current_node_to(const ast::identifier& to)
{
    m_impl->current_node_to(to);
};

void graph::strongly_connected_components(component_vec& comp) const
{
    m_impl->strongly_connected_components(comp);
};

bool graph::is_self_recursive(const key_type& key) const
{
    return m_impl->is_self_recursive(key);
};

void graph::get_referred_symbols(const symbol_group& group,
                                 symbol_set& referred) const
{
    m_impl->get_referred_symbols(group, referred);
};

//-------------------------------------------------------------------
//                   var_graph   
//-------------------------------------------------------------------
template<class Key>
var_graph<Key>::var_graph(const impl_type& impl)
    :m_impl(impl)
{};

template<class Key>
var_graph<Key>::var_graph()
    :m_impl(new details::var_graph_impl<Key>())
{};

template<class Key>
var_graph<Key>::~var_graph()
{};

template<class Key>
void var_graph<Key>::reserve(long number_nodes)
{
    m_impl->reserve(number_nodes);
}

template<class Key>
void var_graph<Key>::add_edge(const Key& module, const Key& import)
{
    m_impl->add_edge(module, import);
};

template<class Key>
void var_graph<Key>::add_node(const Key& module)
{
    m_impl->add_node(module);
};

template<class Key>
void var_graph<Key>::strongly_connected_components(component_vec& comp) const
{
    m_impl->strongly_connected_components(comp);
};

template class var_graph<ast::unique_string>;
template class var_graph<ad::var_info>;
};

#pragma warning(pop)