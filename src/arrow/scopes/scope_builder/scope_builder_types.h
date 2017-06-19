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

#include "scope_builder.h"
#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_type_scope_visitor   
//------------------------------------------------------------------
class build_type_scope_visitor: public ast::traversal_visitor<build_type_scope_visitor>
{
    private:
        using base_type = ast::traversal_visitor<build_type_scope_visitor>;
        using undef_map = std::map<ast::unique_string, ast::location>;

    private:
        ad::type_var_scope  m_type_var_scope;
        ad::type_var_scope  m_kind_var_scope;
        module              m_mod;
        error_handler_ptr   m_eh;

    public:
        build_type_scope_visitor(const error_handler_ptr& eh, const module& mod, 
                        const ad::type_var_scope& type_scope, 
                        const ad::type_var_scope& kind_scope);

        void build_qualified_identifier(const ast::identifier& id,
                const module& qualified_mod);

        void build_identifier(const ast::identifier& id);

    public:
        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            return base_type::eval(ast);
        };

        //---------------------------------------------------------------
        //                      TYPES
        //---------------------------------------------------------------
        void eval(const ast::type_kind_scope& ast);
        void eval(const ast::all_type& ast);
        void eval(const ast::some_type& ast);
        void eval(const ast::mu_type& ast);
        void eval(const ast::lambda_type& ast);
        void eval(const ast::identifier_type& ast);
        void eval(const ast::type_constant& ast);
        void eval_type_identifier(const ast::identifier& ti);
        void eval(const ast::type_sequence& ast);

        //use default traversal
        //void eval(const ast::annotated_type& ast)
        //void eval(const ast::tuple_type& ast);        
        //void eval(const ast::bracket_type& ast);        
        //void eval(const ast::type_application& ast);
        //void eval(const ast::predefined_type& ast);
        //void eval(const ast::type_var& ast);
        //void eval(const ast::arrow_type& ast);
        //void eval(const ast::ref_type& ast);
        //void eval(const ast::if_type& ast);
        //void eval(const ast::path_int_type& ast);
        //void eval(const ast::list_type& ast);
        //void eval(const ast::variant_type& ast);
        //void eval(const ast::record_type& ast);
        //void eval(const ast::product_of_types& ast);        
        //void eval(const ast::int_type& ast);
        //void eval(const ast::error_type& ast);
        //void eval(const ast::bool_type& ast);
        //void eval(const ast::char_type& ast);
        //void eval(const ast::int_type& ast);
        //void eval(const ast::type_kind_application& ast);
        //void eval(const ast::type_type_application& ast);

        //---------------------------------------------------------------
        //                      KINDS
        //---------------------------------------------------------------        
        void eval(const ast::kind_scope& ast);
        void eval(const ast::kind_identifier& ast);

        //use default traversal
        //void eval(const ast::value_kind& ast);
        //void eval(const ast::product_kind& ast);
        //void eval(const ast::star_tuple_kind& ast);
        //void eval(const ast::star_all_kind& ast);
        //void eval(const ast::star_some_kind& ast);
        //void eval(const ast::star_list_kind& ast);
        //void eval(const ast::star_record_kind& ast);
        //void eval(const ast::star_variant_kind& ast);
        //void eval(const ast::star_predefined_kind& ast);
        //void eval(const ast::star_ref_kind& ast);
};

};};};
