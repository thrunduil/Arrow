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

#include "desugaring.h"
#include "scopes/scope_builder/scope_builder.h"
#include "semantics/utils.h"

namespace arrow { namespace sema
{

desugaring_visitor::desugaring_visitor(const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod), m_expand_tuples(false), m_ls(mod.get_this_local_scope())
    ,m_make_star_kind(false)
{};

ast::term desugaring_visitor::make(const ast::term& in)
{
    m_expand_tuples = false;
    ast::term ret   = visit(in);

    m_expand_tuples = true;
    ret             = visit(ret);
    return ret;
};

ast::type desugaring_visitor::make(const ast::type& in)
{
    m_expand_tuples = false;
    ast::type ret   = visit(in);

    m_expand_tuples = true;
    ret             = visit(ret);
    return ret;
};

ast::term desugaring_visitor::eval(const ast::lambda_term& ast)
{
    ast::lambda_term lam2   = base_type::eval_lambda(ast);

    if (ast == lam2)
        return ast;

    const ast::type& ty1        = ast.get_var().get_type();
    const ast::type& ty2        = lam2.get_var().get_type();

    if (ty1 == ty2)
        return lam2;

    lam2.update_scope(m_mod);
    return lam2;
};

ast::term desugaring_visitor::eval(const ast::mu_term& ast)
{
    ast::mu_term lam2           = base_type::eval_mu(ast);

    if (ast == lam2)
        return ast;

    const ast::type& ty1        = ast.get_var().get_type();
    const ast::type& ty2        = lam2.get_var().get_type();

    if (ty1 == ty2)
        return lam2;

    lam2.update_scope(m_mod);
    return lam2;
};

ast::variant_case_term desugaring_visitor::eval(const ast::variant_case_term& ast)
{
    ast::variant_case_term lam2 = base_type::eval_case(ast);

    if (ast == lam2)
        return ast;

    lam2.update_scope(m_mod);
    return lam2;
};

ast::term desugaring_visitor::eval(const ast::let_term& ast)
{
    const ast::term& ex             = ast.get_body();
    const local_scope& ls           = m_mod.get_scope_for_let(ast);
    const ast::definitions& defs    = ast.get_definitions();
    ast::definitions defs2          = ast::definitions(defs->get_location());

    details::scoped_local_scope sls(m_ls, ls);

    ast::term ex2                   = visit(ex);

    using fun_sym_map               = local_scope::fun_sym_map;
    fun_sym_map& loc                = ls.get_local_vars_modify();

    for (auto& pos : loc)
    {
        ast::term_definition sd     = visit(pos.second.get_term_definition());
        pos.second.update_term_definition(sd);
        defs2.add(sd);
    }
            
    return ast::let_term(ast->get_location(), defs2, ex2, ast.get_symbol_data());
};

ast::term desugaring_visitor::eval(const ast::term_sequence& ast_in)
{            
    ast::term_sequence ast  = base_type::eval(ast_in);
    using operator_parser   = oper::operator_parser<oper::operator_config_term>;

    operator_parser op(ast->get_location(), m_mod, m_ls, m_eh);
            
    using item_type     = ast::term_sequence_item::item_type;
    using type_vec      = std::vector<ast::type>;
    using kind_vec      = std::vector<ast::kind>;
    long s              = ast.size();

    //making operator sequence
    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const ast::term& ex     = e.get_basic();
                op.push_basic(ex);
                break;
            };
            case item_type::proj_item:
            {
                const ast::int_literal il= e.get_projection();
                op.push_projection(il);
                break;
            };
            case item_type::dot_item:
            {
                const ast::identifier& id   = e.get_dot();
                op.push_dot(id);
                break;
            };

            case item_type::type_applic_item:
            {
                const type_vec& args    = e.get_type_applic_args();
                op.push_type_application(e.get_location(), args);
                break;
            }

            case item_type::kind_applic_item:
            {
                const kind_vec& args    = e.get_kind_applic_args();
                op.push_kind_application(e.get_location(), args);
                break;
            }
        };
    };

    ast::term ret   = op.parse();
    return ret;
};

ast::term desugaring_visitor::eval(const ast::tuple_term& ast_in)
{
    if (m_expand_tuples == false)
        return base_type::eval(ast_in);

    bool can_expand = ast_in.size() == 1 
                    && ast_in.get_state() != ast::tuple_term::fixed;

    if (can_expand == false)
        return base_type::eval(ast_in);
    else
        return visit(ast_in.get(0));
};

ast::term desugaring_visitor::eval(const ast::unpack_term& ast)
{
    // kind of unpack type variable can be infered
    ast::unpack_term lam2   = base_type::eval_unpack(ast);

    if (ast == lam2)
        return ast;

    const ast::type& ty1    = ast.get_term_identifier().get_type();
    const ast::type& ty2    = lam2.get_term_identifier().get_type();

    if (ty1 == ty2)
        return lam2;

    lam2.update_scope(m_mod);
    return lam2;
};

ast::term desugaring_visitor::eval(const ast::all_term& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::term& ty     = ast.get_body();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::term ty2           = visit(ty);

    if (aid2 == aid && ty2 == ty)
        return ast;
    else
        return ast::all_term(ast->get_location(), aid2, ty2);
};

ast::term desugaring_visitor::eval(const ast::pack_term& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::term& ex     = ast.get_body();
    const ast::type& ty     = ast.get_var_type();
    const ast::type& st     = ast.get_some_type();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::term ex2           = visit(ex);
    ast::type ty2           = visit(ty);
    ast::type st2           = visit(st);

    if (aid2 == aid && ex2 == ex && ty2 == ty && st2 == st)
        return ast;
    else
        return ast::pack_term(ast->get_location(), aid2, ty2, ex2, st2);
};

ast::type desugaring_visitor::eval(const ast::tuple_type& ast_in)
{
    if (m_expand_tuples == false)
        return base_type::eval(ast_in);

    bool can_expand = ast_in.size() == 1 
                    && ast_in.get_state() != ast::tuple_term::fixed;

    if (can_expand == false)
        return base_type::eval(ast_in);
    else
        return visit(ast_in.get(0));
};

ast::type desugaring_visitor::eval(const ast::type_sequence& ast_in)
{     
    ast::type_sequence ast  = base_type::eval(ast_in);
    using operator_parser   = oper::operator_parser<oper::operator_config_type>;

    operator_parser op(ast->get_location(), m_mod, m_ls, m_eh);
            
    using item_type     = ast::type_sequence_item::item_type;
    using kind_vec      = std::vector<ast::kind>;
    using type_vec      = std::vector<ast::type>;
    long s              = ast.size();

    //making operator sequence
    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const ast::type& ex     = e.get_basic();
                op.push_basic(ex);
                break;
            };
            case item_type::proj_item:
            {
                const ast::int_literal il= e.get_projection();
                op.push_projection(il);
                break;
            };
            case item_type::dot_item:
            {
                const ast::identifier& id   = e.get_dot();
                op.push_dot(id);
                break;
            };

            case item_type::type_applic_item:
            {
                const type_vec& args    = e.get_type_applic_args();
                op.push_type_application(e.get_location(), args);
                break;
            }

            case item_type::kind_applic_item:
            {
                const kind_vec& args    = e.get_kind_applic_args();
                op.push_kind_application(e.get_location(), args);
                break;
            }
        };
    };

    ast::type ret   = op.parse();
    return ret;
};

ast::type desugaring_visitor::eval(const ast::some_type& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::type& ty     = ast.get_body();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::type ty2           = visit(ty);

    if (aid2 == aid && ty2 == ty)
        return ast;
    else
        return ast::some_type(ast->get_location(), aid2, ty2);
};

ast::type desugaring_visitor::eval(const ast::all_type& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::type& ty     = ast.get_body();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::type ty2           = visit(ty);

    if (aid2 == aid && ty2 == ty)
        return ast;
    else
        return ast::all_type(ast->get_location(), aid2, ty2);
};

ast::type desugaring_visitor::eval(const ast::lambda_type& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::type& ty     = ast.get_body();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::type ty2           = visit(ty);

    if (aid2 == aid && ty2 == ty)
        return ast;
    else
        return ast::lambda_type(ast->get_location(), ast.get_polarity(), aid2, ty2);
};

ast::type desugaring_visitor::eval(const ast::mu_type& ast)
{
    using annot_id          = ast::annotated_type_identifier;

    const ast::type& ty     = ast.get_body();
    const annot_id& aid     = ast.get_var();

    annot_id aid2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        aid2                = visit(aid);
    }

    ast::type ty2           = visit(ty);

    if (aid2 == aid && ty2 == ty)
        return ast;
    else
        return ast::mu_type(ast->get_location(), aid2, ty2);
};

ast::type desugaring_visitor::eval(const ast::type_kind_application& ast)
{
    const ast::type& ty     = ast.get_lhs();
    const ast::kind& ki     = ast.get_rhs();

    ast::type ty2           = visit(ty);

    ast::kind ki2;

    {
        sema::scoped_var<bool> sc(m_make_star_kind, true);
        ki2                = visit(ki);
    }

    if (ty2 == ty && ki2 == ki)
        return ast;
    else
        return ast::type_kind_application(ast->get_location(), ty2, ki2);
};

ast::kind desugaring_visitor::eval(const ast::kind_var& ast)
{
    if (ast.is_set() == true)
        return visit(ast.get());

    if (m_make_star_kind == false)
        return ast;

    ast::star_kind sk   = ast::star_kind(ast->get_location());
    ast.set_checked(sk);

    return sk;
};

};};
