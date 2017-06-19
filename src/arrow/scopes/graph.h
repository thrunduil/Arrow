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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "arrow/error_handler.h"

#include <memory>
#include <set>

namespace arrow
{

namespace ad = arrow::details;

template<class Key>
class var_graph
{
    private:
        using impl_type     = std::shared_ptr<details::var_graph_impl<Key>>;

        friend details::var_graph_impl<Key>;

    public:
        using component_vec = std::vector<std::vector<Key>>;

    private:
        impl_type           m_impl;
        
    private:
        var_graph(const impl_type&);

    public:
        var_graph();
        ~var_graph();

        void                reserve(long number_nodes);
        void                add_edge(const Key& parent, const Key& child);
        void                add_node(const Key& node);
        void                strongly_connected_components(component_vec& comp) const;
};

class graph
{
    private:
        using impl_type     = std::shared_ptr<details::graph_impl>;
        using key_type      = ad::var_info;
        using symbol_group  = std::vector<key_type>;
        using component_vec = std::vector<symbol_group>;
        using symbol_set    = std::set<key_type>;

        friend details::graph_impl;

    private:
        impl_type           m_impl;

    private:
        graph(const impl_type&);

    public:
        graph();
        ~graph();

        void                reserve(long number_nodes);
        void                clear();

        void                add_node(const ast::identifier& vi);
        void                current_node_to(const ast::identifier& to);

        /// variable from is referred in expression associated with to
        void                add_edge(const ast::identifier& from);
        void                add_extern_edge(const ast::identifier& from);

        void                strongly_connected_components(component_vec& comp) const;
        bool                is_self_recursive(const key_type& key) const;
        void                get_referred_symbols(const symbol_group& group,
                                symbol_set& referred) const;
};

};
