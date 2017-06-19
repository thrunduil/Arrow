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
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "scopes/module.h"
#include "semantics/typing/terms/substitution.h"
#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace typing
{

namespace details
{

class deduction_context
{
    private:
        bool                m_has_type_vars;
        var_substitution*   m_subs;

    public:
        deduction_context();
        deduction_context(var_substitution* subs);

        bool        can_substitute(const ast::type& ty, const ast::kind& var_k) const; 
        void        add_type_var(const ast::type_var& tv, const ast::type& ty);
        bool        has_type_vars() const;
        void        check_type_var(const ast::type_var& tv);
};

class normal_form_vis : public ast::rewrite_visitor<normal_form_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<normal_form_vis>;
        using kind_vec      = std::vector<ast::kind>;

    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        bool                m_expand_rec;
        int                 m_num_unfolding;
        deduction_context   m_deduction;
        inst_collector*     m_collector;

    public:
        normal_form_vis(const error_handler_ptr& eh, const module& mod, inst_collector* col,
                bool expand_rec);

        int         number_rec_unfolded() const;
        ast::type   make(const ast::type& t);
        ast::type   make_deduce(const ast::type& t, var_substitution* subs);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        ast::type eval(const ast::type_application& ta);
        ast::type eval(const ast::type_type_application& ta);
        ast::type eval(const ast::type_kind_application& ast);
        ast::type eval(const ast::if_type& ast);
        ast::type eval(const ast::path_int_type& ast);
        ast::type eval(const ast::identifier_type& t_id);
        ast::type eval(const ast::type_var& tv);
        ast::type eval(const ast::annotated_type& tv);        
        ast::type eval_extern(const ast::type_constant& ast);

        //do not look inside type
        ast::type eval_constructor(const ast::type& ast);
        ast::type eval(const ast::some_type& ast);
        ast::type eval(const ast::all_type& ast);
        ast::type eval(const ast::lambda_type& ast);
        ast::type eval(const ast::mu_type& ast);
        ast::type eval(const ast::arrow_type& ast);
        ast::type eval(const ast::ref_type& ast);
        ast::type eval(const ast::list_type& ast);
        ast::type eval(const ast::variant_type& ast);
        ast::type eval(const ast::record_type& ast);
        ast::type eval(const ast::bracket_type& ast);
        ast::type eval(const ast::tuple_type& ast);
        ast::type eval(const ast::predefined_type& ast);
        ast::type eval(const ast::type_sequence& ast);
        ast::type eval(const ast::product_of_types& ast);
        ast::type eval(const ast::int_type& ast);
        ast::type eval(const ast::error_type& ast);
        ast::type eval(const ast::bool_type& ast);
        ast::type eval(const ast::char_type& ast);
        ast::type eval(const ast::string_type& ast);
        ast::type eval(const ast::type_constant& ast);

        //use defaults
        //auto eval(const ast::type_kind_scope& ast)                                

        //---------------------------------------------------------------
        //                  KINDS
        //---------------------------------------------------------------
        ast::kind eval(const ast::kind_var& kv);

    private:
        bool        visit_kind_application(const ast::type& applic_ast, const ast::type& ty, 
                        kind_vec& args, ast::identifier_type& id);
        ast::type   make_kind_application(const ast::identifier_type& id, kind_vec& args);        
};

};

/// compute beta normal form of a type
class normal_form_type
{
    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        inst_collector*     m_collector;
        bool                m_expand_rec_types;

    public:
        /// if expand_rec_types is true, then recursive types can be unfolded
        /// at head positions, i.e. (Rec[X] F(X)) Y is unfolded to F(Rec[X] F(X)) Y
        normal_form_type(const error_handler_ptr& eh, const module& mod, 
                    inst_collector* col, bool expand_rec_types);

    public:
        ast::type           make(const ast::type& t, int& num_unfolding);
        ast::type           make_deduce(const ast::type& t, var_substitution* subs, 
                                int& num_unfolding);

        static ast::details::type_code
                            get_head_type(const ast::type& ty);
};

};};