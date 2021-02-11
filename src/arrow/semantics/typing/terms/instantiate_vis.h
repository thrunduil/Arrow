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
#include "arrow/ast/ast_visitor.h"
#include "semantics/typing/types/computation_on_types.h"
#include "instantiate_term.h"

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              instantiate_vis
//------------------------------------------------------------------
class instantiate_vis : public ast::rewrite_visitor<instantiate_vis>
{
    private:
        using base_type = ast::rewrite_visitor<instantiate_vis>;
        using type_subs = typing::type_substitution;
        using name_subs = std::map<ad::var_info, ast::identifier>;

    private:
        type_subs               m_subs;
        name_subs               m_name_subs;
        error_handler_ptr       m_eh;
        module                  m_mod;
        inst_collector*         m_owner;
        bool                    m_can_inst_local;

    public:
        instantiate_vis(const error_handler_ptr& eh, const module& mod,
                            inst_collector* owner);

        void        add_type(const ad::var_info& var, const ast::type& ty, 
                        bool make_fresh_binders);
        void        add_kind(const ad::var_info& var, const ast::kind& ty);

        ast::term   make_term(const ast::term& body, bool can_instantiate_local);
        ast::type   make_type(const ast::type& body, bool can_instantiate_local);
        ast::kind   make_kind(const ast::kind& body, bool can_instantiate_local);

        void        clear();
        long        size_all() const;

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        ast::term eval(const ast::overloaded_symbol& ast);
        ast::term eval(const ast::closure_term& ast);
        ast::term eval(const ast::term_type_application& ast);
        ast::term eval(const ast::term_kind_application& ast);
        ast::term eval(const ast::term_application& ast);
        ast::term eval(const ast::identifier_term& ast);
        ast::term eval(const ast::let_term& ast);
        ast::term eval(const ast::mu_term& ast);
        ast::term eval(const ast::lambda_term& ast);
        ast::term eval(const ast::unpack_term& ast);
        ast::term eval(const ast::term_kind_scope& ast);
        ast::term eval(const ast::all_term& ast);
        ast::term eval(const ast::pack_term& ast);
        ast::term eval(const ast::variant_case_term& ast);

        //---------------------------------------------------------------
        //               TYPES
        //---------------------------------------------------------------
        ast::type eval_type(const ast::type& ty)
        {
            return m_subs.make(ty);
        }

        ast::type eval(const ast::arrow_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::ref_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::if_type& ast)          { return eval_type(ast); }
        ast::type eval(const ast::path_int_type& ast)    { return eval_type(ast); }
        ast::type eval(const ast::list_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::identifier_type& ast)  { return eval_type(ast); }
        ast::type eval(const ast::some_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::all_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::lambda_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::mu_type& ast)          { return eval_type(ast); }
        ast::type eval(const ast::variant_type& ast)     { return eval_type(ast); }
        ast::type eval(const ast::record_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_constant& ast)    { return eval_type(ast); }
        ast::type eval(const ast::tuple_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::bracket_type& ast)     { return eval_type(ast); }
        ast::type eval(const ast::annotated_type& ast)   { return eval_type(ast); }
        ast::type eval(const ast::type_application& ast) { return eval_type(ast); }
        ast::type eval(const ast::predefined_type& ast)  { return eval_type(ast); }
        ast::type eval(const ast::type_var& ast)         { return eval_type(ast); }
        ast::type eval(const ast::product_of_types& ast) { return eval_type(ast); }
        ast::type eval(const ast::type_sequence& ast)    { return eval_type(ast); }
        ast::type eval(const ast::error_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::int_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::bool_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::char_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::string_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_kind_scope& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_kind_application& ast){ return eval_type(ast); }
        ast::type eval(const ast::type_type_application& ast){ return eval_type(ast); }

        //---------------------------------------------------------------
        //               KINDS
        //---------------------------------------------------------------
        ast::kind eval_kind(const ast::kind& ty)
        {
            return m_subs.make_kind(ty);
        }

        ast::kind eval(const ast::star_kind& ast)       { return eval_kind(ast); }
        ast::kind eval(const ast::product_kind& ast)    { return eval_kind(ast); }
        ast::kind eval(const ast::star_tuple_kind& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::arrow_kind& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::star_arrow_kind& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::value_kind& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::kind_identifier& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::kind_scope& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::star_all_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::star_some_kind& ast)  { return eval_kind(ast); }
        ast::kind eval(const ast::star_list_kind& ast)  { return eval_kind(ast); }
        ast::kind eval(const ast::star_record_kind& ast){ return eval_kind(ast); }
        ast::kind eval(const ast::star_ref_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::kind_var& ast)        { return eval_kind(ast); }

        ast::kind eval(const ast::star_variant_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::star_predefined_kind& ast){ return eval_kind(ast); }

    private:
        ast::term   eval_overloaded(const ast::overloaded_symbol& ast, 
                        const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos);
        ast::term   eval_overloaded(const ast::overloaded_symbol& ast, 
                        const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos,
                        const ast::closure_term& ct);
        ast::term   eval_closure(const ast::closure_term& ast, 
                        const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos,
                        bool& success);
        ast::term   eval_application(const ast::term& ast, std::vector<details::applic_arg_term>& args,
                        ast::term& head);
        bool        can_instantiate_local() const;

        void        instantiate_overload_arguments(std::vector<details::applic_arg_term>& ret,
                        bool& modified, const ast::overloaded_symbol& os);
        bool        check_must_resolve(const std::vector<details::applic_arg_term>& subs_args,
                        bool is_local_func) const;
        ast::definitions    make_fresh_function_names(const ast::definitions& defs);

    public:
        void        add_name_substitution(const ast::identifier& old_id, 
                        const ast::identifier& new_id);
        void        remove_name_substitution(const ast::identifier& old_id);
        void        make_definition(const ast::identifier& old_id,
                        const ast::term_definition& new_def);
        ast::type   get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                        bool& has_errors);
        ast::term   get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                        bool& has_errors);
};

};};