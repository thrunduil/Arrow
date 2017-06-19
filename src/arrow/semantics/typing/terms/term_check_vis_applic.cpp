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

#include "term_check_vis.h"
#include "semantics/semantics.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "type_errors.h"
#include "error/error.h"
#include "semantics/typing/types/computation_on_types.h"
#include "overload_resolution.h"
#include "instantiate_term.h"

#include <iostream>

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              term_check_vis
//------------------------------------------------------------------
ast::term term_check_vis::eval(const ast::term_type_application& ast, const ast::type& t)
{
    std::vector<applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, t);
};

ast::term term_check_vis::eval(const ast::term_kind_application& ast, const ast::type& t)
{
    std::vector<applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, t);
};

ast::term term_check_vis::eval(const ast::term_application& ast, const ast::type& t)
{
    std::vector<applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, t);
};

ast::term term_check_vis::eval_application(const ast::term& ast, std::vector<applic_arg_term>& args,
                ast::term& head, const ast::type& t)
{
    //standard system F rule for term application and type application:
    //
    //  G |- r : sig -> phi         G |- r : All[a1,...,an] sig
    //  G |- u : sig
    //  --------------------        --------------------------------------------
    //  G |- r u : phi              G |- r T1 ... Tn : {a1->T1, ..., an->Tn} sig
        
    // these rules are combined as in Jay, Jones. "Scrap your type applications"  2008:
    //
    //  G |- r : All[a1,...,an] sig -> phi
    //  G |- u : psi
    //  --------------------------------------------------------- 
    //  G |- r u: All[{a1,...,an} - dom(S)] {S}phi
    //  S: substitution, such {S}sig = psi; dom(S) in {a1,...,an}

    // infer argument types
    // arguments must have full type, i.e. kind variables are not
    // allowed
    for (size_t i = 0; i < args.size(); ++i)
    {
        applic_arg_term& arg   = args[i];

        if (arg.get_item_type() == applic_arg_term::inst_arg)
        {
            continue;
        }
        else if (arg.get_item_type() == applic_arg_term::kind_arg)
        {
            continue;
        }
        else if (arg.get_item_type() == applic_arg_term::function_arg)
        {
            ast::type_var tv_arg    = ast::type_var(arg.get_location());
            arg.set_func_arg_type(tv_arg);

            ast::term t_arg2        = visit(arg.get_func_arg(), tv_arg);
            arg.set_func_arg(t_arg2);
        }
    };

    //infer lambda type
    bool is_overloaded          = false;
    bool has_errors             = false;
    ast::type_var ty_head_var   = ast::type_var(head->get_location());
    ast::type ty_head           = ty_head_var;
    kind_subs_map subs_map;

    ast::term head2             = eval_application_head(head, args, ty_head_var,
                                    subs_map, is_overloaded, has_errors);

    if (has_errors == true)
        return head2;

    //sema::print_type(std::cout, ty_head); std::cout << "\n";

    if (is_overloaded == false)
    {
        // first kind applications could be already processed
        size_t i                = subs_map.number_substitutions();

        for (size_t j = 0; j < i; ++j)
        {
            if (j >= args.size()
                || args[j].get_item_type() != details::applic_arg_term::kind_arg)
            {
                error::error_base_msg(m_eh).internal_error
                        (ast->get_location(), "invalid kind application symbol");
                return ast::error_term(ast->get_location());
            };

            const ast::kind& arg_ty = args[j].get_kind_arg();
            head2               = ast::term_kind_application(ast->get_location(), head2, arg_ty);
        };

        //sema::print_term(std::cout, head2, m_eh, m_mod); std::cout << "\n";
        //sema::print_type(std::cout, ty_head); std::cout << "\n";

        head2                   = subs_map.make_kind_application(head2);
        head2                   = instantiate_kind_application(head2);

        //sema::print_term(std::cout, head2, m_eh, m_mod); std::cout << "\n";
        //sema::print_type(std::cout, ty_head); std::cout << "\n";

        for (; i < args.size(); ++i)
        {
            applic_arg_term& arg   = args[i];

            if (arg.get_item_type() == applic_arg_term::function_arg)
            {
                var_substitution subs(m_mod);
                ast::arrow_type t_lam;

                if (m_type_eq.deduce_arrow_all(ty_head, t_lam, subs, m_ctx) == false)
                {
                    //is_var must be false, otherwise the first check will succeed
                    type_errors(m_eh).error_arrow_type_required_app(head->get_location(), ty_head);
                    return ast::error_term(ast->get_location());
                }

                ast::type tl_left           = t_lam.get_lhs();
                ast::type tl_right          = t_lam.get_rhs();

                //sema::print_type(std::cout, tl_left); std::cout << "\n";

                //form All[a1,...,an] sig for fresh type variables a1, ..., an
                tl_left                     = subs.apply(tl_left);

                //sema::print_type(std::cout, tl_left); std::cout << "\n";
                //sema::print_type(std::cout, arg.get_func_arg_type()); std::cout << "\n";

                //find unification {S}sig = psi
                if (subs.deduce(arg.get_location(), tl_left, arg.get_func_arg_type(),
                                 m_type_eq, m_bound_vars, m_ctx, true) == false)
                {
                    type_errors(m_eh).error_type_mismatch_applic(ast->get_location(), tl_left, 
                                                                arg.get_func_arg_type());
                    return ast::error_term(ast->get_location());
                }

                bool ded_succ               = subs.check_deduction(arg.get_location(), ty_head,
                                                m_type_eq, m_ctx, m_eh, true);

                if (ded_succ == false)
                    return ast::error_term(ast->get_location());

                //find {a1,...,an} - dom(S)
                subs.mark_free_variables();

                //sema::print_type(std::cout, tl_right); std::cout << "\n";

                //form {S}phi
                tl_right                    = subs.apply(tl_right);

                //sema::print_type(std::cout, tl_right); std::cout << "\n";

                //form All[{a1,...,an} - dom(S)] {S}phi
                tl_right                    = subs.generalize_type(tl_right);

                //sema::print_type(std::cout, tl_right); std::cout << "\n";

                //add missing type applications
                ast::term head_subs         = subs.make_type_application(head2);
                head_subs                   = instantiate_type_application(head_subs);

                head_subs                   = ast::term_application(arg.get_location(), 
                                                head_subs, arg.get_func_arg());

                head2                       = subs.generalize(head_subs);
                ty_head                     = tl_right;

                //sema::print_type(std::cout, ty_head); std::cout << "\n";
            }
            else if (arg.get_item_type() == applic_arg_term::inst_arg)
            {
                ast::all_type all_t;
                bool is_var;
                ast::type_var tv_all;

                if (is_all_type(ty_head, all_t, tv_all, is_var) == false)
                {
                    if (is_var == true)
                        type_errors(m_eh).error_missing_type_annotation_all(ast->get_location());
                    else
                        type_errors(m_eh).error_all_type_required_tapp(ast->get_location(), ty_head);

                    return ast::error_term(ast->get_location());
                };

                const ast::type& arg_app    = arg.get_inst_arg();
                ast::type arg_app_2         = arg_app;

                const ast::kind& all_k      = all_t.get_var().get_kind();
                const ast::identifier& id   = all_t.get_var().get_identifier();
                ast::kind arg_app_k         = infer_kind(arg_app_2);

                if (m_owner->is_subkind(arg_app_2->get_location(), arg_app_k, all_k, true) == false)
                {
                    type_errors(m_eh).error_invalid_type_lambda_kind(ast->get_location(), id, all_k, arg_app_k);
                    return ast::error_term(ast->get_location());
                };

                type_substitution ts(m_eh, m_mod);
                ts.add_type(id, arg_app_2, true);
            
                ty_head                     = ts.make(all_t.get_body());

                //sema::print_type(std::cout, ty_head); std::cout << "\n";

                head2                       = ast::term_type_application(ast->get_location(), head2, arg_app_2);
                head2                       = instantiate_type_application(head2);
            }
            else if (arg.get_item_type() == applic_arg_term::kind_arg)
            {
                //kind application is possible for identifier only
                //and should already be processed

                error::type_error_msg(m_eh).error_kind_abstraction_required(arg.get_location(), ty_head);
                return ast::error_term(ast->get_location());
            }
        };
    }
    else
    {
        //get return type and apply arguments
        ty_head                             = get_return_overloaded(head2);

        for (size_t i = 0; i < args.size(); ++i)
        {
            applic_arg_term& arg   = args[i];

            if (arg.get_item_type() == applic_arg_term::function_arg)
            {
                head2                       = ast::term_application(arg.get_location(), 
                                                head2, arg.get_func_arg());
            }
            else if (arg.get_item_type() == applic_arg_term::inst_arg)
            {
                const ast::type& arg_app    = arg.get_inst_arg();
                head2                       = ast::term_type_application(ast->get_location(), 
                                                head2, arg_app);
            }
            else if (arg.get_item_type() == applic_arg_term::kind_arg)
            {
                const ast::kind& arg_app    = arg.get_kind_arg();
                head2                       = ast::term_kind_application(ast->get_location(), 
                                                head2, arg_app);
            }
        };
    };

    //sema::print_type(std::cout, ty_head); std::cout << "\n";
    //sema::print_type(std::cout, t); std::cout << "\n";

    if (m_type_eq.unify_types(ast->get_location(), ty_head, t, 
                              m_bound_vars, m_ctx, true) == false)
    {
        type_errors(m_eh).error_type_mismatch_applic(ast->get_location(), ty_head, t);
        return ast::error_term(ast->get_location());
    };   

    //sema::print_term(std::cout, head2, m_eh, m_mod); std::cout << "\n";
    //sema::print_type(std::cout, ty_head); std::cout << "\n";
    //sema::print_type(std::cout, t); std::cout << "\n";

    if (subs_map.check_all_infered(ast->get_location(), m_eh, true) == false)
        return ast::error_term(ast->get_location());

    return head2;
};

ast::term term_check_vis::eval_application_head(const ast::term& head, 
            const std::vector<applic_arg_term>& args, const ast::type_var& ty_head_var, 
            kind_subs_map& subs_map, bool& is_overloaded, bool& has_errors)
{
    has_errors = false;

    ast::term head2;

    if (head->get_term_code() == ast::details::term_code::identifier_term)
    {
        ast::identifier_term id = head.static_cast_to<ast::identifier_term>();
        head2                   = eval_identifier(id, args, ty_head_var, subs_map, 
                                    is_overloaded, has_errors);

        if (has_errors == true)
            return head2;
    }
    else if (head->get_term_code() == ast::details::term_code::closure_term)
    {
        ast::closure_term cl    = head.static_cast_to<ast::closure_term>();
        head2                   = eval_closure(cl, args, ty_head_var, subs_map, 
                                    is_overloaded, has_errors);

        if (has_errors == true)
            return head2;
    }
    else if (head->get_term_code() == ast::details::term_code::overloaded_symbol)
    {
        ast::overloaded_symbol os = head.static_cast_to<ast::overloaded_symbol>();
        const ast::type& ty     = os.get_type();

        ty_head_var.set_checked(ty);
        is_overloaded           = true;
        head2                   = head;
    }
    else
    {
        head2                   = visit(head, ty_head_var);
    };

    return head2;
};

ast::term term_check_vis::eval_identifier(const ast::identifier_term& ast, 
                const std::vector<applic_arg_term>& args, const ast::type_var& t, 
                kind_subs_map& subs_map, bool& is_overloaded, bool& has_errors)
{
    const ast::type& ty0        = m_owner->get_symbol_type(ast, is_overloaded, has_errors);
    const ast::location& loc    = ast->get_location();

    if (has_errors == true)
        return ast::error_term(loc);

    if (is_overloaded == false)
    {        
        ast::type ty            = m_collector->instantiate_and_infer_kind(ty0, args, true, 
                                    subs_map, has_errors, can_instantiate_local());        

        if (has_errors == true)
            return ast::error_term(loc);

        t.set_checked(ty);
        return ast;
    }
        
    return resolve_overload(ast, args, t, subs_map, is_overloaded, has_errors);
};

ast::term term_check_vis::eval(const ast::identifier_term& ast, const ast::type& t)
{
    std::vector<applic_arg_term> args;
    kind_subs_map subs_map;
    bool is_overloaded;
    bool has_errors;
    ast::type_var tv    = ast::type_var(ast->get_location());
    ast::type ty_head   = tv;

    ast::term ret       = eval_identifier(ast, args, tv, subs_map, is_overloaded, has_errors);

    if (has_errors == true)
        return ast::error_term(ast->get_location());

    const ast::location& loc    = ast->get_location();
   
    if (is_overloaded == false)
    {
        // first kind applications could be already processed
        size_t i                = subs_map.number_substitutions();

        for (size_t j = 0; j < i; ++j)
        {
            if (j >= args.size()
                || args[j].get_item_type() != details::applic_arg_term::kind_arg)
            {
                error::error_base_msg(m_eh).internal_error
                        (ast->get_location(), "invalid kind application symbol");
                return ast::error_term(ast->get_location());
            };

            const ast::kind& arg_ty = args[j].get_kind_arg();
            ret                 = ast::term_kind_application(ast->get_location(), ret, arg_ty);
        };

        ret                     = subs_map.make_kind_application(ret);
        ret                     = instantiate_kind_application(ret);
    }
    else
    {
        //get return type
        ty_head                 = get_return_overloaded(ret);
    };

    if (m_type_eq.unify_types(loc, ty_head, t, m_bound_vars, m_ctx, true) == false)
    {
        type_errors(m_eh).error_type_mismatch_for_symbol(loc, ast.get_identifier(), ty_head, t);
        return ast::error_term(ast->get_location());
    }

    if (subs_map.check_all_infered(ast->get_location(), m_eh, true) == false)
        return ast::error_term(ast->get_location());

    return ret;
};

ast::term term_check_vis::eval_closure(const ast::closure_term& ast, 
                const std::vector<applic_arg_term>& args, const ast::type_var& t, 
                kind_subs_map& subs_map, bool& is_overloaded, bool& has_errors)
{
    const ast::term& body       = ast.get_body();
    ast::closure_kind kind      = ast.get_kind();

    if (kind == ast::closure_kind::overload)
    {
        is_overloaded           = true;
        has_errors              = false;
        return visit(ast, t);
    }
    else if (kind == ast::closure_kind::function)
    {
        error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                                       "unexpected function closure");
        has_errors  = true;
        return ast::error_term(ast->get_location());
    }

    const ast::identifier_term& id  = body.static_cast_to<ast::identifier_term>();
    ast::term body2                 = eval_identifier(id, args, t, subs_map, 
                                        is_overloaded, has_errors);

    if (has_errors == true)
        return ast::error_term(ast->get_location());

    bool modified               = body2 != body;
    return eval_closure(ast, body2, modified);
};

ast::term term_check_vis::resolve_overload(const ast::identifier_term& ast, 
                const std::vector<applic_arg_term>& args, const ast::type_var& ty, 
                kind_subs_map& subs_map, bool& is_overloaded, bool& has_errors)
{
    using overload_set          = ad::overload_set<ad::overload_term_config>;
    const overload_set* os      = m_mod.get_overload_set(m_eh, ast);
    const ast::identifier& id   = ast.get_identifier();
    is_overloaded               = false;
    has_errors                  = false;

    if (os == nullptr)
    {
        has_errors              = true;
        return ast::error_term(ast->get_location());
    };

    const ast::overload_index* initial  = nullptr;

    // types of arguments do not contain kind vars or kind scopes
    overload_resolution or(*os, m_eh, m_mod, m_ctx, m_bound_vars, m_collector, initial);
    or.build_candidates(args);
    or.select_best();

    has_errors  = or.check_errors(ast->get_location(), id, args);

    if (has_errors == true)
        return ast::error_term(ast->get_location());

    using def_type              = ad::symbol_definition_env;

    const ad::def_index* di     = nullptr;
    bool success;    
    ast::type function_ty, ret_ty;
    subs_ptr subs;
    const def_type* def         = or.get_best(di, success, function_ty, ret_ty, subs);

    if (success == false)
        return ast;    

    bool must_resolve           = def->is_closed_overload() || inst_collector::is_closed(args);

    if (must_resolve == true)
    {
        if (def->is_abstract_overload() == true)
        {
            error::type_error_msg(m_eh).error_abstract_overload_not_defined
                        (ast->get_location(), id, function_ty, *os);
            has_errors = true;
            return ast::error_term(ast->get_location());
        };

        //overload is fully resolved and cannot be specialized further
        //convert to ordinary symbol
        ast::identifier id2     = sema::copy(id);

        if (di)
        {
            id2.get_symbol_data()->set_definition(*di);
            id2.get_symbol_data()->get_kind().set_sector_term().set_function_def();
        };

        function_ty             = m_collector->instantiate_and_infer_kind(function_ty, 
                                    args, true, subs_map, has_errors, can_instantiate_local());        

        if (has_errors == true)
            return ast::error_term(ast->get_location());        

        ty.set_checked(function_ty);

        return ast::identifier_term(ast->get_location(), id2);
    };

    is_overloaded = true;
    ast::overload_index oi  = m_mod.get_overload_index(os);
    ast::overloaded_symbol ret(ast->get_location(), ast, function_ty, ret_ty, &oi);

    for (auto elem : args)
    {
        if (elem.get_item_type() == applic_arg_term::function_arg)
            ret.add_term_arg(elem.get_func_arg_type());
        else if (elem.get_item_type() == applic_arg_term::inst_arg)
            ret.add_type_arg(elem.get_inst_arg());
        else if (elem.get_item_type() == applic_arg_term::kind_arg)
            ret.add_kind_arg(elem.get_kind_arg());
    };

    return ret;
};

ast::term term_check_vis::resolve_overload(const ast::identifier_term& ast, 
            const std::vector<details::applic_arg_term>& args, const ast::type& ret_ty_in,
            const module& mod, const error_handler_ptr& eh, subs_ptr& subs, 
            const ast::overload_index* oi, inst_collector* col, bool& success)
{
    using overload_set          = ad::overload_set<ad::overload_term_config>;
    const overload_set* os      = mod.get_overload_set(eh, ast);
    const ast::identifier& id   = ast.get_identifier();
    success                     = false;

    if (os == nullptr)
        return ast::error_term(ast->get_location());

    //this is overload resolution for arguments with closed types
    // we may use empty context
    context_type ctx;
    bound_map bm;
    overload_resolution or(*os, eh, mod, ctx, bm, col, oi);
    or.build_candidates(args, ret_ty_in);
    or.select_best();

    bool has_errors = or.check_errors(ast->get_location(), id, args);

    if (has_errors == true)
        return ast::error_term(ast->get_location());

    const ad::def_index* di     = nullptr;
    ast::type function_ty, ret_ty;

    using def_type              = ad::symbol_definition_env;
    const def_type* def         = or.get_best(di, success, function_ty, ret_ty, subs);

    if (success == false)
        return ast;    

    if (def->is_abstract_overload() == true)
    {
        error::type_error_msg(eh).error_abstract_overload_not_defined
                    (ast->get_location(), id, function_ty, *os);
        return ast::error_term(ast->get_location());
    };

    //overload is fully resolved and cannot be specialized further
    //convert to ordinary symbol
    ast::identifier id2         = sema::copy(id);

    if (di)
    {
        id2.get_symbol_data()->set_definition(*di);
        id2.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    };

    ast::identifier_term ret    = ast::identifier_term(ast->get_location(), id2);
    
    //kinds will be applied lated

    success                     = true;
    return ret;
};

}}}
