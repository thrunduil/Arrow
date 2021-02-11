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
#include "scope_builder.h"
#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_type_graph_visitor   
//------------------------------------------------------------------
class build_type_graph_visitor : public ast::traversal_visitor<build_type_graph_visitor>
{    
    private:
        using base_type = ast::traversal_visitor<build_type_graph_visitor>;

    private:
        module              m_mod;
        graph               m_graph;
        error_handler_ptr   m_eh;
        bool                m_build;

    public:
        build_type_graph_visitor(const error_handler_ptr& eh, const module& mod, const graph& g,
                                 bool build_graph);

        //forward all other functions to the base class
        template<class Ast, class ... Args>
        void eval(const Ast& ast, Args&& ... args)
        {
            return base_type::eval(ast, std::forward<Args>(args)...);
        };

        void build_identifier(const ast::identifier& ast, bool& errors);

        //---------------------------------------------------------------
        //               TYPES
        //---------------------------------------------------------------
        void eval(const ast::identifier_type& ast);
};

};};};
