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
#include "arrow/ast/ast_visitor.h"
#include "semantics/make_fresh_binders_type.h"

namespace arrow { namespace codegen
{

ast::term   make_fresh_binders(const ast::term& t, const error_handler_ptr& eh,
                const module& mod);

//------------------------------------------------------------------
//              is_value_vis
//------------------------------------------------------------------
class is_value_vis : public ast::traversal_visitor<is_value_vis>
{
    private:
        using base_type = ast::traversal_visitor<is_value_vis>;

    public:
        bool    m_value;

    public:
        is_value_vis()
            :m_value(false)
        {};

    public:

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            base_type::eval(ast);
        }

    public:
        void eval(const ast::error_term& ast)       { (void)ast; m_value = true; }

        void eval(const ast::int_literal& ast)      { (void)ast; m_value = true; }
        void eval(const ast::bool_literal& ast)     { (void)ast; m_value = true; }
        void eval(const ast::float_literal& ast)    { (void)ast; m_value = true; }
        void eval(const ast::char_literal& ast)     { (void)ast; m_value = true; }
        void eval(const ast::string_literal& ast)   { (void)ast; m_value = true; }
        void eval(const ast::lambda_term& ast)      { (void)ast; m_value = true; }        
        void eval(const ast::all_term& ast)         { (void)ast; m_value = true; }
        void eval(const ast::cell_term& ast)        { (void)ast; m_value = true; }
        void eval(const ast::overloaded_symbol& ast){ (void)ast; m_value = true; }        
        void eval(const ast::val_term& ast)         { (void)ast; m_value = true; }
        void eval(const ast::term_kind_scope& ast)  { (void)ast; m_value = true; }

        // not allowed at this stage
        void eval(const ast::term_sequence& ast)    { (void)ast; m_value = true; }
        void eval(const ast::bracket_term& ast)     { (void)ast; m_value = true; }

        void eval(const ast::closure_term& ast);
        void eval(const ast::tuple_term& ast);
        void eval(const ast::list_term& ast);
        void eval(const ast::record_term& ast);
        void eval(const ast::term_constant& ast);
        void eval(const ast::pack_term& ast);
        void eval(const ast::variant_term& ast);
        
        void eval(const ast::unpack_term& ast)          { (void)ast; m_value = false; }        
        void eval(const ast::ref_term& ast)             { (void)ast; m_value = false; }
        void eval(const ast::assign_term& ast)          { (void)ast; m_value = false; }
        void eval(const ast::deref_term& ast)           { (void)ast; m_value = false; }        
        void eval(const ast::identifier_term& ast)      { (void)ast; m_value = false; }        
        void eval(const ast::annotated_term& ast)       { (void)ast; m_value = false; }
        void eval(const ast::if_term& ast)              { (void)ast; m_value = false; }
        void eval(const ast::term_application& ast)     { (void)ast; m_value = false; }
        void eval(const ast::let_term& ast)             { (void)ast; m_value = false; }
        void eval(const ast::path_term& ast)            { (void)ast; m_value = false; }
        void eval(const ast::path_int_term& ast)        { (void)ast; m_value = false; }
        void eval(const ast::variant_case_term& ast)    { (void)ast; m_value = false; }        
        void eval(const ast::term_type_application& ast){ (void)ast; m_value = false; }
        void eval(const ast::term_kind_application& ast){ (void)ast; m_value = false; }
        void eval(const ast::mu_term& ast)              { (void)ast; m_value = false; }
};

//------------------------------------------------------------------
//              make_fresh_binders_term_vis
//------------------------------------------------------------------
class make_fresh_binders_term_vis : public ast::rewrite_visitor<make_fresh_binders_term_vis>
{    
    private:
        using base_type     = ast::rewrite_visitor<make_fresh_binders_term_vis>;
        using annot_term    = ast::annotated_identifier;
        using annot_type    = ast::annotated_type_identifier;
        using type_subs     = sema::make_fresh_binders_vis;
        using subs_map      = std::map<ad::var_info, ast::identifier>;

    private:
        type_subs           m_type_subs;
        subs_map            m_subs_map;
        error_handler_ptr   m_eh;
        module              m_mod;

    public:
        make_fresh_binders_term_vis(const error_handler_ptr& eh, const module& mod);

        ast::term make(const ast::term& ty);

    public:
        template<class Ast>
        auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

        //------------------------------------------------------------------
        //                      TERMS
        //------------------------------------------------------------------
        ast::term eval(const ast::lambda_term& ast);
        ast::term eval(const ast::mu_term& ast);
        ast::term eval(const ast::unpack_term& ast);
        ast::term eval(const ast::variant_case_term& ast);
        ast::term eval(const ast::let_term& ast);
        ast::term eval(const ast::all_term& ast);
        ast::term eval(const ast::pack_term& ast);
        ast::identifier_term eval(const ast::identifier_term& ast);
        ast::term eval(const ast::closure_term& ast);

        //use default rewriting
        //ast::term eval(const ast::overloaded_symbol& ast);
        //ast::term eval(const ast::path_term& ast);
        //ast::term eval(const ast::term_type_application& ast)
        //ast::term eval(const ast::term_kind_application& ast);
        //ast::term eval(const ast::tuple_term& ast)
        //ast::term eval(const ast::term_sequence& ast)
        //ast::term eval(const ast::list_term& ast)
        //ast::term eval(const ast::bracket_term& ast)
        //ast::term eval(const ast::record_term& ast)
        //ast::term eval(const ast::term_constant& ast)
        //ast::term eval(const ast::int_literal& ast)
        //ast::term eval(const ast::bool_literal& ast)
        //ast::term eval(const ast::float_literal& ast)
        //ast::term eval(const ast::char_literal& ast)
        //ast::term eval(const ast::string_literal& ast)
        //ast::term eval(const ast::error_term& ast)
        //ast::term eval(const ast::assign_term& ast)
        //ast::term eval(const ast::val_term& ast)
        //ast::term eval(const ast::ref_term& ast)
        //ast::term eval(const ast::deref_term& ast)
        //ast::term eval(const ast::cell_term& ast)
        //ast::term eval(const ast::annotated_term& ast)
        //ast::term eval(const ast::if_term& ast)
        //ast::term eval(const ast::term_application& ast)        
        //ast::term eval(const ast::path_int_term& ast)        
        //ast::term eval(const ast::variant_term& ast)
        //ast::term eval(const ast::term_kind_scope& ast)

        //------------------------------------------------------------------
        //                      TYPES
        //------------------------------------------------------------------
        ast::type eval_type(const ast::type& ty);

        ast::type eval(const ast::arrow_type& ast)      { return eval_type(ast); };
        ast::type eval(const ast::path_int_type& ast)   { return eval_type(ast); };
        ast::type eval(const ast::ref_type& ast)        { return eval_type(ast); };
        ast::type eval(const ast::if_type& ast)         { return eval_type(ast); };
        ast::type eval(const ast::list_type& ast)       { return eval_type(ast); };
        ast::type eval(const ast::identifier_type& ast) { return eval_type(ast); };
        ast::type eval(const ast::some_type& ast)       { return eval_type(ast); };
        ast::type eval(const ast::all_type& ast)        { return eval_type(ast); };
        ast::type eval(const ast::lambda_type& ast)     { return eval_type(ast); };
        ast::type eval(const ast::mu_type& ast)         { return eval_type(ast); };
        ast::type eval(const ast::variant_type& ast)    { return eval_type(ast); };
        ast::type eval(const ast::record_type& ast)     { return eval_type(ast); };
        ast::type eval(const ast::type_constant& ast)   { return eval_type(ast); };
        ast::type eval(const ast::annotated_type& ast)  { return eval_type(ast); };
        ast::type eval(const ast::tuple_type& ast)      { return eval_type(ast); };
        ast::type eval(const ast::bracket_type& ast)    { return eval_type(ast); };
        ast::type eval(const ast::type_application& ast){ return eval_type(ast); };
        ast::type eval(const ast::predefined_type& ast) { return eval_type(ast); };
        ast::type eval(const ast::type_var& ast)        { return eval_type(ast); };
        ast::type eval(const ast::product_of_types& ast){ return eval_type(ast); };
        ast::type eval(const ast::type_sequence& ast)   { return eval_type(ast); };
        ast::type eval(const ast::error_type& ast)      { return eval_type(ast); };
        ast::type eval(const ast::int_type& ast)        { return eval_type(ast); };
        ast::type eval(const ast::bool_type& ast)       { return eval_type(ast); };
        ast::type eval(const ast::char_type& ast)       { return eval_type(ast); };
        ast::type eval(const ast::string_type& ast)     { return eval_type(ast); };
        ast::type eval(const ast::type_kind_scope& ast) { return eval_type(ast); };
        ast::type eval(const ast::type_kind_application& ast)   { return eval_type(ast); };
        ast::type eval(const ast::type_type_application& ast)   { return eval_type(ast); };

    private:
        annot_term              make_substitution(const annot_term& id, 
                                    bool can_change_name);
        void                    remove_substitution(const annot_term& id);
        ast::identifier_term    substitute(const ast::identifier_term& id);
};

};};
