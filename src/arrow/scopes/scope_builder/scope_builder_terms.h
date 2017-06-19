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
//                   build_scope_visitor   
//------------------------------------------------------------------
class build_scope_visitor : public ast::traversal_visitor<build_scope_visitor>
{    
    private:
        using base_type = ast::traversal_visitor<build_scope_visitor>;
        using undef_map = std::map<ast::unique_string, ast::location>;

    private:
        ast::location       m_loc;
        module              m_mod;
        error_handler_ptr   m_eh;
        local_scope         m_bound;
        ad::type_var_scope  m_type_var_scope;
        ad::type_var_scope  m_kind_var_scope;

    public:
        build_scope_visitor(const error_handler_ptr& eh, const ast::location& loc, 
                          const module& mod);

        void start_expression();
        void process_toplevel_signature(const ast::type& sig);

        void build_qualified_identifier(const ast::identifier& id, 
                const module& qualified_mod);

        void build_identifier(const ast::identifier& id);

        //---------------------------------------------------------------
        //                      TERMS
        //---------------------------------------------------------------
        void eval(const ast::term_sequence& ast);

        //use default traversal
        //void eval(const ast::tuple_term& ast);        
        //void eval(const ast::list_term& ast);
        //void eval(const ast::bracket_term& ast);
        //void eval(const ast::int_literal& ast);
        //void eval(const ast::bool_literal& ast);
        //void eval(const ast::float_literal& ast);
        //void eval(const ast::char_literal& ast);
        //void eval(const ast::string_literal& ast);
        //void eval(const ast::error_term& ast);
        //void eval(const ast::ref_term& ast);
        //void eval(const ast::assign_term& ast);
        //void eval(const ast::deref_term& ast);
        //void eval(const ast::cell_term& ast);
        //void eval(const ast::if_term& ast);
        //void eval(const ast::term_application& ast);
        //void eval(const ast::path_term& ast)
        //void eval(const ast::path_int_term& ast);
        //void eval(const ast::annotated_term& ast);
        //void eval(const ast::term_type_application& ast);
        //void eval(const ast::variant_term& ast);
        //void eval(const ast::pack_term& ast);
        //void eval(const ast::val_term& ast);

        void eval(const ast::lambda_term& ast);
        void eval(const ast::mu_term& ast);
        void eval(const ast::term_kind_scope& ast);
        void eval(const ast::all_term& ast);
        void eval(const ast::pack_term& ast);
        void eval(const ast::overloaded_symbol& ast);
        void eval(const ast::identifier& id);
        void eval(const ast::identifier_term& ast);
        void eval(const ast::variant_case_term& ast);
        void eval(const ast::let_term& ast);
        void eval(const ast::term_constant& ast);
        void eval(const ast::record_term& ast);
        void eval(const ast::unpack_term& ast);

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            return base_type::eval(ast);
        };

        //---------------------------------------------------------------
        //                      DEFINITIONS
        //---------------------------------------------------------------
        void eval(const ast::term_definition& ast);

        //---------------------------------------------------------------
        //                      TYPES
        //---------------------------------------------------------------
        void eval(const ast::arrow_type& ast)       { visit_type(ast); }
        void eval(const ast::ref_type& ast)         { visit_type(ast); }
        void eval(const ast::if_type& ast)          { visit_type(ast); }
        void eval(const ast::path_int_type& ast)    { visit_type(ast); }
        void eval(const ast::list_type& ast)        { visit_type(ast); }
        void eval(const ast::identifier_type& ast)  { visit_type(ast); }
        void eval(const ast::some_type& ast)        { visit_type(ast); }
        void eval(const ast::all_type& ast)         { visit_type(ast); }
        void eval(const ast::lambda_type& ast)      { visit_type(ast); }
        void eval(const ast::mu_type& ast)          { visit_type(ast); }
        void eval(const ast::variant_type& ast)     { visit_type(ast); }
        void eval(const ast::record_type& ast)      { visit_type(ast); }
        void eval(const ast::type_constant& ast)    { visit_type(ast); }
        void eval(const ast::tuple_type& ast)       { visit_type(ast); }
        void eval(const ast::bracket_type& ast)     { visit_type(ast); }
        void eval(const ast::annotated_type& ast)   { visit_type(ast); }
        void eval(const ast::type_application& ast) { visit_type(ast); }
        void eval(const ast::predefined_type& ast)  { visit_type(ast); }
        void eval(const ast::product_of_types& ast) { visit_type(ast); }
        void eval(const ast::type_sequence& ast)    { visit_type(ast); }
        void eval(const ast::int_type& ast)         { visit_type(ast); }
        void eval(const ast::error_type& ast)       { visit_type(ast); }
        void eval(const ast::bool_type& ast)        { visit_type(ast); }
        void eval(const ast::char_type& ast)        { visit_type(ast); }
        void eval(const ast::string_type& ast)      { visit_type(ast); }
        void eval(const ast::type_var& ast)         { visit_type(ast); }
        void eval(const ast::type_kind_scope& ast)  { visit_type(ast); }
        void eval(const ast::type_kind_application& ast){ visit_type(ast); }
        void eval(const ast::type_type_application& ast){ visit_type(ast); }

        void visit_type(const ast::type& t);

        //---------------------------------------------------------------
        //                      KINDS
        //---------------------------------------------------------------
        void eval(const ast::star_kind& ast)        { visit_kind(ast); };
        void eval(const ast::arrow_kind& ast)       { visit_kind(ast); };
        void eval(const ast::star_arrow_kind& ast)  { visit_kind(ast); };
        void eval(const ast::product_kind& ast)     { visit_kind(ast); };
        void eval(const ast::star_tuple_kind& ast)  { visit_kind(ast); };
        void eval(const ast::value_kind& ast)       { visit_kind(ast); };
        void eval(const ast::kind_identifier& ast)  { visit_kind(ast); };
        void eval(const ast::kind_scope& ast)       { visit_kind(ast); };
        void eval(const ast::star_all_kind& ast)    { visit_kind(ast); };
        void eval(const ast::star_some_kind& ast)   { visit_kind(ast); };
        void eval(const ast::star_list_kind& ast)   { visit_kind(ast); };
        void eval(const ast::star_record_kind& ast) { visit_kind(ast); };
        void eval(const ast::star_variant_kind& ast){ visit_kind(ast); };
        void eval(const ast::star_predefined_kind& ast) { visit_kind(ast); };
        void eval(const ast::star_ref_kind& ast)    { visit_kind(ast); };
        void eval(const ast::kind_var& ast)         { visit_kind(ast); };

        void visit_kind(const ast::kind& t);
};

};};};
