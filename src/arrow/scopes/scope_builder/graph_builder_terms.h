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
#include "scope_builder.h"
#include "arrow/ast/ast_visitor.h"
#include "graph_builder_types.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_graph_visitor   
//------------------------------------------------------------------
class build_graph_visitor : public ast::traversal_visitor<build_graph_visitor>
{    
    private:
        using base_type     = ast::traversal_visitor<build_graph_visitor>;
        using undef_map     = std::map<ast::unique_string, ast::location>;
        using type_builder  = build_type_graph_visitor;

    private:
        module              m_mod;
        graph               m_graph;
        error_handler_ptr   m_eh;
        bool                m_build;
        type_builder        m_type_builder;

    public:
        build_graph_visitor(const error_handler_ptr& eh, const module& mod, const graph& g,
                            bool build);

        //forward all other functions to the base class
        template<class Ast, class ... Args>
        void eval(const Ast& ast, Args&& ... args)
        {
            return base_type::eval(ast, std::forward<Args>(args)...);
        };

        void eval(const ast::overloaded_symbol& ast);
        void eval(const ast::identifier_term& ast);
        void eval(const ast::let_term& ast);

        //---------------------------------------------------------------
        //               TYPES
        //---------------------------------------------------------------
        void eval_type(const ast::type& ast)
        {
            m_type_builder.visit(ast);
        };

        void eval(const ast::arrow_type& ast)       { eval_type(ast); }
        void eval(const ast::ref_type& ast)         { eval_type(ast); }
        void eval(const ast::if_type& ast)          { eval_type(ast); }
        void eval(const ast::path_int_type& ast)    { eval_type(ast); }
        void eval(const ast::list_type& ast)        { eval_type(ast); }
        void eval(const ast::identifier_type& ast)  { eval_type(ast); }
        void eval(const ast::some_type& ast)        { eval_type(ast); }
        void eval(const ast::all_type& ast)         { eval_type(ast); }
        void eval(const ast::lambda_type& ast)      { eval_type(ast); }
        void eval(const ast::mu_type& ast)          { eval_type(ast); }
        void eval(const ast::variant_type& ast)     { eval_type(ast); }
        void eval(const ast::record_type& ast)      { eval_type(ast); }
        void eval(const ast::type_constant& ast)    { eval_type(ast); }
        void eval(const ast::tuple_type& ast)       { eval_type(ast); }
        void eval(const ast::bracket_type& ast)     { eval_type(ast); }
        void eval(const ast::annotated_type& ast)   { eval_type(ast); }
        void eval(const ast::type_application& ast) { eval_type(ast); }
        void eval(const ast::predefined_type& ast)  { eval_type(ast); }
        void eval(const ast::type_var& ast)         { eval_type(ast); }
        void eval(const ast::product_of_types& ast) { eval_type(ast); }
        void eval(const ast::type_sequence& ast)    { eval_type(ast); }
        void eval(const ast::int_type& ast)         { eval_type(ast); }
        void eval(const ast::error_type& ast)       { eval_type(ast); }
        void eval(const ast::bool_type& ast)        { eval_type(ast); }
        void eval(const ast::char_type& ast)        { eval_type(ast); }
        void eval(const ast::string_type& ast)      { eval_type(ast); }
        void eval(const ast::type_kind_scope& ast)  { eval_type(ast); }
        void eval(const ast::type_kind_application& ast){ eval_type(ast); }
        void eval(const ast::type_type_application& ast){ eval_type(ast); }

    private:        
        bool    build_identifier(const ast::identifier& id, const local_scope& ls,
                    bool& is_bound);
};

};};};
