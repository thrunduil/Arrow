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

#include "kind_checker_vis.h"
#include "error/error.h"
#include "unifier_kinds.h"
#include "semantics/semantics.h"
#include "computation_on_types.h"
#include "semantics/utils.h"
#include "semantics/typing/terms/term_check_helpers.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "semantics/typing/terms/instantiate_term.h"

#include <iostream>

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              kind_check_vis
//------------------------------------------------------------------

ast::type kind_check_vis::eval(const ast::predefined_type& ast, const ast::kind& k,
                          context_type& ctx, bool& error, bool show_error)
{
    error   = false;

    (void)ctx;

    bool check_star;
    if (unifier_kinds(m_eh).unify_with_star_predefined(k, ast.get_type_code(), check_star) == false)
    {
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k,
                    star_kind_reason::primitive, m_type_root, ast);
        };

        error = true;
        return ast::error_type(ast->get_location());
    };

    return ast;
};

ast::type kind_check_vis::eval(const ast::type_constant& ast, const ast::kind& k,
                          context_type& ctx, bool& error, bool show_error)
{    
    error                   = false;
    const ast::kind& k_ast  = ast.get_return_kind();
    bool is_extern          = ast.is_extern_func();
    (void)ctx;

    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), k_ast, k, show_error) == false)
    {
        error = true;
        if (show_error == true)
            error_kind_mismatch(ast->get_location(), k, k_ast, m_type_root, ast);

        return ast::error_type(ast->get_location());
    };
    
    if (unifier_kinds(m_eh).is_value_kind_any(k_ast) == true && is_extern == false)
    {
        error = true;

        if (show_error == true)
            error_type_constant_ord(ast->get_location(), m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    if (is_extern == false)
        return ast;

    using extern_function_ptr       = std::shared_ptr<ast::extern_type_function_eval>;
    const extern_function_ptr& func = ast.get_function();
    ast::kind func_ret_type         = func->get_return_kind();

    if (unifier_kinds(m_eh).unify_kinds(ast->get_location(), func_ret_type, k_ast, show_error) == false)
    {   
        error = true;
        if (show_error == true)
            error_kind_mismatch_extern_func_ret(ast->get_location(), ast.get_name(),
                                            k_ast, func_ret_type);

        return ast::error_type(ast->get_location());
    };

    long n                          = ast.var_size();
    long n_func                     = func->number_arguments();

    if (n != n_func)
    {
        error = true;
        
        if (show_error == true)
            error_kind_mismatch_extern_func_size(ast->get_location(), ast.get_name(),
                                            n, n_func);

        return ast::error_type(ast->get_location());
    };

    for (long i = 0; i < n; ++i)
    {
        const ast::kind& arg_type       = ast.get_label(i).get_kind();
        ast::kind func_arg_type         = func->get_arg_kind(i);

        if (unifier_kinds(m_eh).unify_kinds(ast->get_location(), arg_type, 
                                            func_arg_type, show_error) == false)
        {
            error = true;
            if (show_error == true)
                error_kind_mismatch_extern_func_arg(ast->get_location(), ast.get_name(),
                                                i, arg_type, func_arg_type);
            return ast::error_type(ast->get_location());
        }
    };

    return ast;
};

ast::type kind_check_vis::eval(const ast::annotated_type& ast, const ast::kind& k, 
                               context_type& ctx, bool& error, bool show_error)
{
    const ast::type& at = ast.get_type();
    const ast::kind& ki = ast.get_kind();    
    error               = false;

    const ast::location& loc = ast->get_location();

    ast::type at2       = visit(at, ki, ctx, error, show_error);

    if (error == true)
        return ast::error_type(loc);

    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), ki, k, show_error) == false)
    {
        error = true;
        
        if (show_error == true)
            error_kind_mismatch(loc, k, ki, m_type_root, ast);

        return ast::error_type(loc);
    }

    if (at2 == at)
        return ast;
    else
        return ast::annotated_type(ast->get_location(), at2, ki);
};

ast::type kind_check_vis::eval(const ast::tuple_type& ast, const ast::kind& k,
                          context_type& ctx, bool& error, bool show_error)
{
    long n              = ast.size();    
    error               = false;
    bool check_star;

    //tuple has * kind
    ast::star_tuple_kind k_tup;
    if (unifier_kinds(m_eh).unify_with_star_tuple(k, n, k_tup, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, 
                    star_kind_reason::tuple, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }
    
    if (check_star == true)
    {
        for (long i = 0; i < n; ++i)
        {
            const ast::type& t  = ast.get(i);
            const ast::kind& ki = k_tup.get(i);

            ast::type t2        = visit(t, ki, ctx, error, false);

            if (unifier_kinds(m_eh).unify_with_star_any(ki) == false)
                error = true;

            if (error == true)
                break;
        };

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_tuple(k, n, k_tup, check_star);
        }
    };

    ast::tuple_type res = ast::tuple_type(ast->get_location(), ast::tuple_type::fixed);
    bool modif          = false;

    for (long i = 0; i < n; ++i)
    {
        const ast::type& t  = ast.get(i);
        const ast::kind& ki = k_tup.get(i);

        ast::type t2        = visit(t, ki, ctx, error, show_error);

        if (error == true)
            return ast::error_type(ast->get_location());

        if (unifier_kinds(m_eh).unify_with_star_any(ki) == false)
        {
            error = true;
            if (show_error == true)
            {
                error_star_kind_required(ast->get_location(), ki, 
                        star_kind_reason::tuple, m_type_root, ast);
            }
            return ast::error_type(ast->get_location());
        }

        res.add(t2);
        modif               |= t2 != t;
    };

    if (modif == false)
        return ast;
    else
        return res;
};

ast::type kind_check_vis::eval(const ast::variant_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    bool check_star;

    //variant type has * kind
    ast::star_variant_kind vk;
    if (unifier_kinds(m_eh).unify_with_star_variant(k, ast, vk, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, 
                        star_kind_reason::variant, m_type_root, ast);
        };

        return ast::error_type(ast->get_location());
    }

    long n                  = ast.size();
    using annot_type        = ast::annotated_type_identifier;

    if (check_star == true)
    {        
        for (long i = 0; i < n; ++i)
        {
            const ast::type& ft     = ast.get_type(i);
            const annot_type& id    = ast.get_identifier(i);

            const ast::kind& id_k   = id.get_kind();

            //it is already checked, that field exists
            ast::kind ki            = vk.get_field(id.get_identifier());

            //field type must have * kind
            ast::type ft2           = visit(ft, ki, ctx, error, false);

            if (error == true)
                break;

            //kind must be *
            if (unifier_kinds(m_eh).is_subkind(id->get_location(), ki, id_k, false) == false)
            {
                error = true;
                break;
            }

            if (unifier_kinds(m_eh).unify_with_star_any(id_k) == false)
            {
                error = true;
                break;
            };
        };

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_variant(k, ast, vk, check_star);
        }
    };

    ast::variant_type res   = ast::variant_type(ast->get_location());
    bool modif              = false;

    using annot_type        = ast::annotated_type_identifier;

    for (long i = 0; i < n; ++i)
    {
        const ast::type& ft     = ast.get_type(i);
        const annot_type& id    = ast.get_identifier(i);

        const ast::kind& id_k   = id.get_kind();

        //it is already checked, that field exists
        ast::kind ki            = vk.get_field(id.get_identifier());

        //field type must have * kind
        ast::type ft2           = visit(ft, ki, ctx, error, show_error);

        if (error == true)
            return ast::error_type(ast->get_location());

        //kind must be *
        if (unifier_kinds(m_eh).is_subkind(id->get_location(), ki, id_k, show_error) == false)
        {
            error = true;

            if (show_error == true)
                error_kind_mismatch(id->get_location(), ki, id_k, m_type_root, ast);

            return ast::error_type(ast->get_location());
        }

        if (unifier_kinds(m_eh).unify_with_star_any(id_k) == false)
        {
            error = true;

            if (show_error == true)
            {
                error_star_kind_required_field(id->get_location(), id_k, 
                    id.get_identifier(), m_type_root, ast);
            };

            return ast::error_type(ast->get_location());
        };

        res.add(id, ft2);
        modif                   |= ft2 != ft;
    };

    if (modif == false)
        return ast;
    else
        return res;
};

ast::type kind_check_vis::eval(const ast::record_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    bool check_star;

    //record type has * kind
    ast::star_record_kind rk;
    if (unifier_kinds(m_eh).unify_with_star_record(k, ast, rk, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, 
                        star_kind_reason::record, m_type_root, ast);
        };

        return ast::error_type(ast->get_location());
    }

    long n              = ast.size();
    using annot_type    = ast::annotated_type_identifier;

    if (check_star == true)
    {
        for (long i = 0; i < n; ++i)
        {
            const ast::type& ft     = ast.get_type(i);
            const annot_type& id    = ast.get_identifier(i);

            const ast::kind& id_k   = id.get_kind();
        
            //it is already checked, that field exists
            ast::kind ki            = rk.get_field(id.get_identifier());

            //field type must have * kind
            ast::type ft2           = visit(ft, ki, ctx, error, false);

            if (error == true)
                break;

            //kind must be *
            if (unifier_kinds(m_eh).is_subkind(id->get_location(), ki, id_k, false) == false)
            {
                error = true;
                break;
            }

            if (unifier_kinds(m_eh).unify_with_star_any(id_k) == false)
            {
                error = true;
                break;
            };
        };

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_record(k, ast, rk, check_star);
        }
    };

    ast::record_type res= ast::record_type(ast->get_location());
    bool modif          = false;    

    for (long i = 0; i < n; ++i)
    {
        const ast::type& ft     = ast.get_type(i);
        const annot_type& id    = ast.get_identifier(i);

        const ast::kind& id_k   = id.get_kind();
        
        //it is already checked, that field exists
        ast::kind ki            = rk.get_field(id.get_identifier());

        //field type must have * kind
        ast::type ft2           = visit(ft, ki, ctx, error, show_error);

        if (error == true)
            return ast::error_type(ast->get_location());

        //kind must be *
        if (unifier_kinds(m_eh).is_subkind(id->get_location(), ki, id_k, show_error) == false)
        {
            error = true;

            if (show_error == true)
                error_kind_mismatch(id->get_location(), ki, id_k, m_type_root, ast);

            return ast::error_type(ast->get_location());
        }


        if (unifier_kinds(m_eh).unify_with_star_any(id_k) == false)
        {
            error = true;

            if (show_error == true)
            {
                error_star_kind_required_field(id->get_location(), id_k, 
                    id.get_identifier(), m_type_root, ast);
            };

            return ast::error_type(ast->get_location());
        };

        res.add(id, ft2);
        modif                   |= ft2 != ft;
    };

    if (modif == false)
        return ast;
    else
        return res;
};

ast::type kind_check_vis::eval(const ast::ref_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    bool check_star;

    //ref type has * kind
    ast::kind k_body;
    if (unifier_kinds(m_eh).unify_with_star_ref(k, k_body, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, 
                        star_kind_reason::ref, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    const ast::type& t  = ast.get_type();

    if (check_star == true)
    {
        //base type must have * kind
        ast::type t2        = visit(t, k_body, ctx, error, false);

        if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
            error = true;

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_ref(k, k_body, check_star);
        }
    };

    //base type must have * kind
    ast::type t2        = visit(t, k_body, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_body, 
                    star_kind_reason::ref, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (t2 == t)
        return ast;
    else
        return ast::ref_type(ast->get_location(), t2);
};

ast::type kind_check_vis::eval(const ast::if_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    const ast::type& t_if   = ast.get_if();
    const ast::type& t_then = ast.get_then();
    const ast::type& t_else = ast.get_else();
    error                   = false;

    ast::kind k_bool        = ast::value_kind(t_if->get_location(), ast::value_kind_code::bool_kind);
    ast::type t_if2         = visit(t_if, k_bool, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    ast::type t_then2       = visit(t_then, k, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    ast::type t_else2       = visit(t_else, k, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (t_if2 == t_if && t_then2 == t_then && t_else2 == t_else)
        return ast;
    else
        return ast::if_type(ast->get_location(), t_if2, t_then2, t_else2);
};

ast::type kind_check_vis::eval(const ast::list_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    bool check_star;

    //list type has * kind
    ast::kind k_body;
    if (unifier_kinds(m_eh).unify_with_star_list(k, k_body, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, 
                    star_kind_reason::list, m_type_root, ast);
        };

        return ast::error_type(ast->get_location());
    }

    const ast::type& t  = ast.get_type();

    if (check_star == true)
    {
        ast::type t2        = visit(t, k_body, ctx, error, false);

        if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
            error = true;

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_list(k, k_body, check_star);
        }
    };

    //base type must have * kind
    ast::type t2        = visit(t, k_body, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_body, 
                    star_kind_reason::list, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (t2 == t)
        return ast;
    else
        return ast::list_type(ast->get_location(), t2);
};

ast::type kind_check_vis::eval(const ast::int_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    (void)ctx;
    ast::value_kind_code vkc = ast::value_kind_code::int_kind;

    if (unifier_kinds(m_eh).unify_with_value(k, vkc) == false)
    {
        error = true;
        if (show_error == true)
            error_value_kind_required(ast->get_location(), vkc, k, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    return ast;
}

ast::type kind_check_vis::eval(const ast::bool_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    (void)ctx;
    ast::value_kind_code vkc = ast::value_kind_code::bool_kind;

    if (unifier_kinds(m_eh).unify_with_value(k, vkc) == false)
    {
        error = true;
        if (show_error == true)
            error_value_kind_required(ast->get_location(), vkc, k, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    return ast;
}

ast::type kind_check_vis::eval(const ast::char_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;
    (void)ctx;
    ast::value_kind_code vkc = ast::value_kind_code::char_kind;

    if (unifier_kinds(m_eh).unify_with_value(k, vkc) == false)
    {
        error = true;
        if (show_error == true)
            error_value_kind_required(ast->get_location(), vkc, k, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    return ast;
}

ast::type kind_check_vis::eval(const ast::string_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = false;

    (void)ctx;
    ast::value_kind_code vkc = ast::value_kind_code::string_kind;

    if (unifier_kinds(m_eh).unify_with_value(k, vkc) == false)
    {
        error = true;
        if (show_error == true)
            error_value_kind_required(ast->get_location(), vkc, k, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    return ast;
}

ast::type kind_check_vis::eval(const ast::type_var& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    if (ast.is_set() == true)
        return visit(ast.get(), k, ctx, error, show_error);

    if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        const ast::kind_var& kv = k.static_cast_to<ast::kind_var>();
        if (kv.is_set() == true)
            return visit(ast, kv.get(), ctx, error, show_error);
    };

    error::error_base_msg(m_eh).internal_error(ast->get_location(), "unexpected type var");
    error = true;
    return ast;
}

ast::type kind_check_vis::eval(const ast::all_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //  G, X^{ok}   |- A : *
    //---------------------------------
    //  G           |- All[X^{k}] A : *

    error                           = false;
    const auto& var                 = ast.get_var();
    const ast::identifier& var_id   = var.get_identifier();
    const ast::kind& var_kind       = var.get_kind();
    const ast::type& t              = ast.get_body();
    bool check_star;

    //All[..] t has * kind
    ast::kind k_var, k_body;
    
    if (unifier_kinds(m_eh).unify_with_star_all(k, k_var, k_body, true, check_star) == false)
    {
        error = true;
        if (show_error == true)
            error_star_kind_required(ast->get_location(), k, star_kind_reason::all, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }
    
    auto scope = ctx.add_var(var_id, var_kind, ast::polarity_type::neutral);

    if (check_star == true)
    {
        // All kind is contravariant in var
        if (unifier_kinds(m_eh).is_subkind(k->get_location(), k_var, var_kind, false) == false)
            error = true;

        //body must have * kind
        ast::type t2    = visit(t, k_body, ctx, error, false);

        if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
            error = true;

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_all(k, k_var, k_body, true, check_star);
        }
    };

    // All kind is contravariant in var
    if (unifier_kinds(m_eh).is_subkind(k->get_location(), k_var, var_kind, show_error) == false)
    {
        error = true;
        if (show_error == true)
            error_kind_mismatch(ast->get_location(), k_var, var_kind, m_type_root, ast);
        return ast::error_type(ast->get_location());
    }

    //body must have * kind
    ast::type t2    = visit(t, k_body, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_body, star_kind_reason::all, 
                            m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (t2 == t)
        return ast;
    else
        return ast::all_type(ast->get_location(), var, t2);
};

ast::type kind_check_vis::eval(const ast::some_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //  G, X^{ok}   |- A : *
    //---------------------------------
    //  G           |- Some[X^{k}] A : *

    error                           = false;
    const auto& var                 = ast.get_var();
    const ast::identifier& var_id   = var.get_identifier();
    const ast::kind& var_kind       = var.get_kind();
    const ast::type& t              = ast.get_body();
    bool check_star;

    //Some[..] t has * kind
    ast::kind k_var, k_body;
    if (unifier_kinds(m_eh).unify_with_star_some(k, k_var, k_body, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, star_kind_reason::some, 
                                     m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    auto scope          = ctx.add_var(var_id, var_kind, ast::polarity_type::neutral);

    if (check_star == true)
    {
        // Some kind is contravariant in var
        if (unifier_kinds(m_eh).is_subkind(k->get_location(), k_var, var_kind, false) == false)
            error = true;

        //body must have * kind
        ast::type t2    = visit(t, k_body, ctx, error, false);

        if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
            error = true;

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_some(k, k_var, k_body, check_star);
        }
    };

    // Some kind is contravariant in var
    if (unifier_kinds(m_eh).is_subkind(k->get_location(), k_var, var_kind, show_error) == false)
    {
        error = true;
        if (show_error == true)
            error_kind_mismatch(ast->get_location(), k_var, var_kind, m_type_root, ast);
        return ast::error_type(ast->get_location());
    }

    //body must have * kind
    ast::type t2    = visit(t, k_body, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (unifier_kinds(m_eh).unify_with_star_any(k_body) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_body, star_kind_reason::some, 
                            m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (t2 == t)
        return ast;
    else
        return ast::some_type(ast->get_location(), var, t2);
};

ast::type kind_check_vis::eval(const ast::path_int_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //  G   |- T : {k0, ..., ki, ...}
    //------------------------------
    //  G   |- T[i] : ki

    error                       = false;
    const ast::type& ty         = ast.get_type();
    const ast::int_literal& ind = ast.get_index();

    ast::kind_var kv_T  = ast::kind_var(ast->get_location(), false);
    ast::type ty2       = visit(ty, kv_T, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    long n          = sema::cast_to_long(ind->get_location(), ind.get_value(), m_eh, error);

    if (error == true)
        return ast::error_type(ast->get_location());

    ast::product_kind pk;
    if (unifier_kinds(m_eh).unify_with_any_product(kv_T, pk, n) == false)
    {
        error = true;
        if (show_error == true)
            error_product_kind_required(ast->get_location(), kv_T, n+1, m_type_root, ast);
        return ast::error_type(ast->get_location());
    };

    ast::kind elem_k    = pk.get(n);

    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), elem_k, k, show_error) == false)
    {
        error = true;
        if (show_error == true)
            error_kind_mismatch(ast->get_location(), k, elem_k, m_type_root, ast);

        return ast::error_type(ast->get_location());
    }

    if (ty2 == ty)
        return ast;
    else
        return ast::path_int_type(ast->get_location(), ty2, ind);
};

ast::type kind_check_vis::eval(const ast::product_of_types& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //  G   |- Ti : ki
    //------------------------------
    //  G   |- {T1, ..., Tn} : {k1, ..., kn}

    error       = false;
    long n      = ast.size();

    ast::product_kind pk;
    if (unifier_kinds(m_eh).unify_with_product(k, pk, n) == false)
    {
        error = true;
        if (show_error == true)
            error_product_kind_required(ast->get_location(), k, n, m_type_root, ast);
        return ast::error_type(ast->get_location());
    };

    ast::product_of_types res   = ast::product_of_types(ast->get_location());
    bool modif                  = false;

    for (long i = 0; i < n; ++i)
    {
        const ast::type& ti = ast.get(i);
        const ast::kind& ki = pk.get(i);

        ast::type ti2       = visit(ti, ki, ctx, error, show_error);

        if (error == true)
            return ast::error_type(ast->get_location());

        res.add(ti2);
        modif               |= ti2 != ti;
    };

    if (modif == false)
        return ast;
    else
        return res;
};

ast::type kind_check_vis::eval(const ast::arrow_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    // -G   |- A : *
    //  G   |- B: *
    //------------------------------
    //  G   |- A -> B : *

    error = false;
    bool check_star;

    //arrow type has * kind
    ast::kind k_lhs, k_rhs;
    if (unifier_kinds(m_eh).unify_with_star_arrow(k, k_lhs, k_rhs, check_star) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k, star_kind_reason::arrow, 
                                     m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    const ast::type& t1 = ast.get_lhs();
    const ast::type& t2 = ast.get_rhs();

    ast::type t12, t22;

    if (check_star == true)
    {
        for(;;)
        {
            {
                auto scope  = ctx.negate_polarities();
                t12         = visit(t1, k_lhs, ctx, error, false);
            }

            if (error == true)
                break;

            t22             = visit(t2, k_rhs, ctx, error, false);

            if (error == true)
                break;

            if (unifier_kinds(m_eh).unify_with_star_any(k_lhs) == false)
            {
                error = true;
                break;
            }
    
            if (unifier_kinds(m_eh).unify_with_star_any(k_rhs) == false)
                error = true;

            break;
        };

        if (error == true)
        {
            error = false;
            make_star_star(k);
            unifier_kinds(m_eh).unify_with_star_arrow(k, k_lhs, k_rhs, check_star);
        };
    };    

    {
        auto scope  = ctx.negate_polarities();
        t12         = visit(t1, k_lhs, ctx, error, show_error);
    }

    if (error == true)
        return ast::error_type(ast->get_location());

    t22             = visit(t2, k_rhs, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (unifier_kinds(m_eh).unify_with_star_any(k_lhs) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_lhs, star_kind_reason::arrow,
                                     m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }
    
    if (unifier_kinds(m_eh).unify_with_star_any(k_rhs) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_star_kind_required(ast->get_location(), k_rhs, star_kind_reason::arrow, 
                                     m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (t12 == t1 && t22 == t2)
        return ast;
    else
        return ast::arrow_type(ast->get_location(), t12, t22);
};

ast::type kind_check_vis::eval(const ast::lambda_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    // G, X^{pk} |- F : k2
    //-----------------------------------------
    // G         |- Fun{X^{pk}} => F : pk -> k2

    using type_annot = ast::annotated_type_identifier;

    error                       = false;
    const type_annot& var       = ast.get_var();
    const ast::type& body       = ast.get_body();
    const ast::kind& vk         = var.get_kind();
    const ast::identifier& id   = var.get_identifier();
    ast::polarity_type pol_in   = ast.get_polarity();

    ast::arrow_kind ak1;
    if (unifier_kinds(m_eh).unify_with_arrow(k, ak1, pol_in) == false)
    {
        error   = true;
        if (show_error == true)
        {
            error_arrow_kind_required(ast->get_location(), k, arrow_kind_reason::type_lambda,
                                  m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    };
    
    const ast::kind& kl         = ak1.get_lhs();
    ast::kind k_body            = ak1.get_rhs();
    ast::polarity_type pol      = ak1.get_type();

    // infered kind (pol_in vk -> ?) must be more specialized than required (i.e. k = pol kl -> ?)
    // thus kl <= vk
    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), kl, vk, show_error) == false)
    {
        error   = true;
        if (show_error == true)
        {
            error_kind_mismatch_for_symbol(ast->get_location(), id.get_name_simple(), 
                                           vk, kl, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    };

    if (unifier_kinds::is_polarity_less(pol_in, pol) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_kind_polarity_mismatch_for_symbol(ast->get_location(), id, pol_in, pol,
                                                m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    };

    auto scope      = ctx.add_var(id, vk, pol_in);
    ast::type body2 = visit(body, k_body, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (body2 == body)
        return ast;
    else
        return ast::lambda_type(ast->get_location(), pol_in, var, body2);
};

ast::type kind_check_vis::eval(const ast::mu_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    // G, X :+k |- F : k
    //-----------------------------------------
    // G        |- Rec[X:+k] F : k

    using type_annot = ast::annotated_type_identifier;

    error                       = false;
    const type_annot& var       = ast.get_var();
    const ast::type& body       = ast.get_body();
    const ast::kind& vk         = var.get_kind();
    const ast::identifier& id   = var.get_identifier();
    ast::polarity_type pol_in   = ast::polarity_type::positive;
    
    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), vk, k, show_error) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_kind_mismatch_for_symbol(ast->get_location(), id.get_name_simple(), 
                                       vk, k, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    };

    // we assume, that mu types must be contractive; i.e. reduction of this type
    // to weak head normal form always terminates
    check_mu_contractive(ast, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    auto scope      = ctx.add_var(id, vk, pol_in);
    ast::type body2 = visit(body, k, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (body2 == body)
        return ast;
    else
        return ast::mu_type(ast->get_location(), var, body2);
};

ast::type kind_check_vis::eval(const ast::type_kind_scope& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    std::vector<ast::identifier> args;
    ast::type ty;
    sema::decompose_type(ast, args, ty);

    error                       = false;
    ast::kind_var kv            = ast::kind_var(ty->get_location(), false);
    ast::type ty2               = visit(ty, kv, ctx, error, show_error);

    if (error == true)
        return ast::error_type(ast->get_location());

    //do not generalize kind yet; we must process all elements in the recursive group first
    ctx.add_generalize_kind(args, kv, k, m_current_symbol);

    if (ty2 == ty)
        return ast;
    
    long n          = (long)args.size();
    ast::type_kind_scope res(ast->get_location(), args.back(), ty2);

    for (long i = n - 2; i >= 0; --i)
        res         = ast::type_kind_scope(ast->get_location(), args[i], res);

    return res;
}

ast::type kind_check_vis::eval(const ast::type_application& ast, const ast::kind& k, 
                          context_type& ctx, bool& error, bool show_error)
{
    std::vector<applic_arg_type> args;
    ast::type head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, k, ctx, error, show_error);
};

ast::type kind_check_vis::eval(const ast::type_type_application& ast, const ast::kind& k, 
                          context_type& ctx, bool& error, bool show_error)
{
    std::vector<applic_arg_type> args;
    ast::type head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, k, ctx, error, show_error);
};

ast::type kind_check_vis::eval(const ast::type_kind_application& ast, const ast::kind& k, 
                               context_type& ctx, bool& error, bool show_error)
{
    std::vector<applic_arg_type> args;
    ast::type head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head, k, ctx, error, show_error);
};

ast::type kind_check_vis::eval_application(const ast::type& ast, std::vector<applic_arg_type>& args, 
                const ast::type& head, const ast::kind& k, context_type& ctx, bool& error,
                bool show_error)
{
    //infer lambda kind
    bool is_overloaded          = false;
    error                       = false;
    ast::kind_var ki_head_var   = ast::kind_var(head->get_location(), false);
    ast::kind ki_head           = ki_head_var;
    kind_subs_map subs_map;

    //sema::print_type(std::cout, head); std::cout << "\n";

    ast::type head2             = eval_application_head(head, args, ki_head_var,
                                    subs_map, is_overloaded, error, show_error, ctx);

    //sema::print_type(std::cout, head2); std::cout << "\n";
    //sema::print_kind(std::cout, ki_head); std::cout << "\n";

    if (error == true)
        return head2;

    if (is_overloaded == false)
    {
        // first kind applications could be already processed
        size_t i                = subs_map.number_substitutions();

        for (size_t j = 0; j < i; ++j)
        {
            if (j >= args.size()
                || args[j].get_item_type() != details::applic_arg_term::kind_arg)
            {
                error = true;
                if (show_error == true)
                {
                    error::error_base_msg(m_eh).internal_error
                        (ast->get_location(), "invalid kind application");
                };
                return ast::error_type(ast->get_location());
            };

            const ast::kind& arg_ty = args[j].get_kind_arg();
            head2               = ast::type_kind_application(ast->get_location(), head2, arg_ty);
        };

        //sema::print_type(std::cout, head2); std::cout << "\n";

        head2                   = subs_map.make_kind_application(head2);
        head2                   = instantiate_kind_application(head2);       

        //sema::print_type(std::cout, head2); std::cout << "\n";

        for (; i < args.size(); ++i)
        {
            applic_arg_type& arg   = args[i];

            if (arg.get_item_type() == applic_arg_type::function_arg)
            {
                // G    |- F : p k1 -> k2
                // pG   |- H : k1
                //--------------------------
                // G    |- F H : k2

                ast::arrow_kind ak1;

                //only neutral polarities can be infered
                ast::polarity_type pol_loc = ast::polarity_type::neutral;

                if (unifier_kinds(m_eh).unify_with_arrow(ki_head, ak1, pol_loc) == false)
                {
                    error = true;
                    if (show_error == true)
                    {
                        error_arrow_kind_required(head2->get_location(), ki_head, 
                                        arrow_kind_reason::type_application, m_type_root, head2);
                    };
                    return ast::error_type(ast->get_location());
                };

                const ast::kind& kl_left    = ak1.get_lhs();
                const ast::kind& kl_right   = ak1.get_rhs();
                ast::polarity_type pol      = ak1.get_type();
                const ast::type& type_rhs   = arg.get_func_arg();
                ast::type type_rhs2;

                {
                    auto scope  = ctx.mult_polarities(pol);
                    type_rhs2   = visit(type_rhs, kl_left, ctx, error, show_error);
                }

                if (error == true)
                    return ast::error_type(ast->get_location());

                arg.set_func_arg(type_rhs2);

                head2                       = ast::type_application(arg.get_location(), 
                                                head2, arg.get_func_arg());
                ki_head                     = kl_right;
            }
            else if (arg.get_item_type() == applic_arg_type::inst_arg)
            {
                // G    |- F : All[k1] k2
                // 0G   |- H : k1
                //--------------------------
                // G    |- F<H> : k2

                ast::star_all_kind ak1;
                ast::kind kl_body;
                ast::kind kl_arg;
                bool check_star;
                if (unifier_kinds(m_eh).unify_with_star_all(ki_head, kl_arg, kl_body, 
                                                false, check_star) == false)
                {
                    error = true;
                    if (show_error == true)
                        error_all_kind_required(head2->get_location(), ki_head, m_type_root, head2);

                    return ast::error_type(ast->get_location());
                };

                //checking conversion of ki_head to * is not needed

                const ast::type& type_rhs   = arg.get_inst_arg();
                ast::polarity_type pol      = ast::polarity_type::neutral;
                ast::type type_rhs2;

                {
                    auto scope  = ctx.mult_polarities(pol);
                    type_rhs2   = visit(type_rhs, kl_arg, ctx, error, show_error);
                }

                if (error == true)
                    return ast::error_type(ast->get_location());

                arg.set_inst_arg(type_rhs2);

                head2                       = ast::type_type_application(arg.get_location(), 
                                                head2, arg.get_inst_arg());
                head2                       = instantiate_type_application(head2);

                ki_head                     = kl_body;
            }
            else if (arg.get_item_type() == applic_arg_type::kind_arg)
            {
                //kind application is possible for identifier only
                //and should already be processed

                error = true;
                if (show_error == true)
                {
                    error::type_error_msg(m_eh).error_kind_abstraction_required
                                                (arg.get_location(), ki_head);
                };
                return ast::error_type(ast->get_location());
            }
        };
    }
    else
    {
        //get return type and apply arguments
        ki_head                             = get_return_overloaded(head2, error);

        if (error == true)
            return ast::error_type(ast->get_location());

        for (size_t i = 0; i < args.size(); ++i)
        {
            applic_arg_type& arg   = args[i];

            if (arg.get_item_type() == applic_arg_type::function_arg)
            {
                head2                       = ast::type_application(arg.get_location(), 
                                                head2, arg.get_func_arg());
            }
            else if (arg.get_item_type() == applic_arg_type::inst_arg)
            {
                const ast::type& arg_app    = arg.get_inst_arg();
                head2                       = ast::type_type_application(ast->get_location(), 
                                                head2, arg_app);
            }
            else if (arg.get_item_type() == applic_arg_type::kind_arg)
            {
                const ast::kind& arg_app    = arg.get_kind_arg();
                head2                       = ast::type_kind_application(ast->get_location(), 
                                                head2, arg_app);
            }
        };
    };

    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), ki_head, k, show_error) == false)
    {
        error = true;
        if (show_error == true)
            error_kind_mismatch(ast->get_location(), k, ki_head, m_type_root, ast);    

        return ast::error_type(ast->get_location());
    }

    if (subs_map.check_all_infered(ast->get_location(), m_eh, show_error) == false)
    {
        error = true;
        return ast::error_type(ast->get_location());
    };

    return head2;
};

ast::type kind_check_vis::eval_application_head(const ast::type& head, 
            const std::vector<applic_arg_type>& args, const ast::kind_var& ki_head_var, 
            kind_subs_map& subs_map, bool& is_overloaded, bool& error, bool show_error,
            context_type& ctx)
{
    error = false;

    ast::type head2;

    if (head->get_type_code() == ast::details::type_code::identifier_type)
    {
        ast::identifier_type id = head.static_cast_to<ast::identifier_type>();
        head2                   = eval_identifier(id, args, ki_head_var, subs_map, 
                                    is_overloaded, error, show_error, ctx);

        if (error == true)
            return head2;
    }
    /*
    TODO: overload
    else if (head->get_type_code() == ast::details::type_code::overloaded_symbol)
    {
        ast::overloaded_symbol os = head.static_cast_to<ast::overloaded_symbol>();
        const ast::type& ty     = os.get_type();

        ty_head_var.set_checked(ty);
        is_overloaded           = true;
        head2                   = head;
    }
    */
    else if (head->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv = head.static_cast_to<ast::type_var>();
        if (tv.is_set() == true)
        {
            return eval_application_head(tv.get(), args, ki_head_var, subs_map, 
                                         is_overloaded, error, show_error, ctx);
        }

        head2                   = visit(head, ki_head_var, ctx, error, show_error);
    }
    else
    {
        head2                   = visit(head, ki_head_var, ctx, error, show_error);
    };

    return head2;
};

ast::type kind_check_vis::eval_identifier(const ast::identifier_type& ast, 
                const std::vector<applic_arg_type>& args, const ast::kind_var& kv, 
                kind_subs_map& subs_map, bool& is_overloaded, bool& error, bool show_error,
                context_type& ctx)
{
    // X^{pk} in G, p = (+ | 0)
    //------------------------------------
    // G |- X : k

    // G, X :+k |- F : k
    //-----------------------------------------
    // G        |- Rec[X:+k] F : k

    //(recursive symbols are allowed if their occurences are
    //positive (but not necessary strictly positive)

    const ast::identifier& id   = ast.get_identifier();
    is_overloaded               = false;

    bool is_binder;
    bool is_rec;
    bool is_visible;
    ast::polarity_type orig_pol;
    ast::kind ki;
    ast::polarity_type pol      = ctx.get_symbol_polarity(id, ki, orig_pol, 
                                    is_binder, is_rec, is_visible);

    if (is_binder == false)
    {
        ki                      = m_owner->get_type_constructor_kind(ast, is_overloaded, error);

        if (error == true)
            return ast::error_type(ast->get_location());
    }

    if (is_binder == true || is_rec == true)
    {
        if (is_visible == false)
        {
            error = true;
            if (show_error == true)
            {
                error_polarized_symbol_on_neutral_pos
                    (ast->get_location(), id, orig_pol, m_type_root, ast);
            };
            return ast::error_type(ast->get_location());
        }
        else if (pol == ast::polarity_type::negative)
        {    
            ast::polarity_type exp_pol  = ast::polarity_type::positive;
            exp_pol                     = ctx.mult_by_state(exp_pol);

            if (is_rec == true)
            { 
                error = true;
                if (show_error == true)
                {
                    error_kind_polarity_mismatch_for_symbol_rec(ast->get_location(), id, orig_pol, 
                                                    exp_pol, m_type_root, ast);
                };
                return ast::error_type(ast->get_location());
            }
            else
            {
                error = true;
                if (show_error == true)
                {
                    error_kind_polarity_mismatch_for_symbol(ast->get_location(), id, orig_pol, 
                                                    exp_pol, m_type_root, ast);
                }
                return ast::error_type(ast->get_location());
            }
        };
    };

    if (is_overloaded == false)
    {
        ast::kind ki_ins        = m_collector->instantiate_and_infer_kind(ki, args, show_error, 
                                    subs_map, error);        

        if (error == true)
            return ast::error_type(ast->get_location());

        if (kv.is_star_var() == true)
        {
            bool is_star    = unifier_kinds::is_star_kind_any(ki_ins);

            if (is_star == false)
            {
                if (show_error == true)
                {
                    error::kind_error_msg(m_eh).error_star_kind_required_var
                            (ast->get_location(), ki_ins);
                };

                error = true;
                return ast::error_type(ast->get_location());
            };
        };

        kv.set_checked(ki_ins);
        return ast;
    }; 

    return resolve_overload(ast, args, kv, subs_map, is_overloaded, error, show_error);
};

ast::type kind_check_vis::eval(const ast::identifier_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    std::vector<applic_arg_type> args;
    kind_subs_map subs_map;
    bool is_overloaded;
    ast::kind_var kv    = ast::kind_var(ast->get_location(), false);
    ast::kind ki_head   = kv;

    ast::type ret       = eval_identifier(ast, args, kv, subs_map, is_overloaded, 
                            error, show_error, ctx);

    if (error == true)
        return ast::error_type(ast->get_location());

    if (is_overloaded == false)
    {
        // first kind applications could be already processed
        size_t i                = subs_map.number_substitutions();

        for (size_t j = 0; j < i; ++j)
        {
            if (j >= args.size()
                || args[j].get_item_type() != details::applic_arg_term::kind_arg)
            {
                error = true;
                if (show_error == true)
                {
                    error::error_base_msg(m_eh).internal_error
                        (ast->get_location(), "invalid kind application");
                };
                return ast::error_type(ast->get_location());
            };

            const ast::kind& arg_ty = args[j].get_kind_arg();
            ret                 = ast::type_kind_application(ast->get_location(), ret, arg_ty);
        };

        ret                     = subs_map.make_kind_application(ret);
        ret                     = instantiate_kind_application(ret);
    }
    else
    {
        ki_head                 = get_return_overloaded(ret, error);

        if (error == true)
            return ast::error_type(ast->get_location());
    };

    if (unifier_kinds(m_eh).is_subkind(ast->get_location(), ki_head, k, show_error) == false)
    {
        error = true;
        if (show_error == true)
        {
            error_kind_mismatch_for_symbol(ast->get_location(), ast.get_identifier().get_name_simple(), 
                                    ki_head, k, m_type_root, ast);
        };
        return ast::error_type(ast->get_location());
    }

    if (subs_map.check_all_infered(ast->get_location(), m_eh, show_error) == false)
    {
        error = true;
        return ast::error_type(ast->get_location());
    };

    return ret;
}

ast::kind kind_check_vis::get_return_overloaded(const ast::type& ty, bool& error)
{
    /*
    //TODO: overload
    if (ty->get_type_code() == ast::details::type_code::overloaded_symbol)
    {
        ast::overloaded_symbol os   = ex.static_cast_to<ast::overloaded_symbol>();
        return os.get_return_type();
    }
    */
    if (ty->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv   = ty.static_cast_to<ast::type_var>();
        if (tv.is_set() == true)
            return get_return_overloaded(tv.get(), error);
    };

    error = true;
    error::error_base_msg(m_eh).internal_error(ty->get_location(), 
                                 "expecting overloaded symbol");
    return ast::kind();
};

ast::type kind_check_vis::resolve_overload(const ast::identifier_type& id, 
            const std::vector<applic_arg_type>& args, const ast::kind_var& kv, 
            kind_subs_map& subs_map, bool& is_overloaded, bool& error, bool show_error)
{
    (void)subs_map;
    (void)kv;
    (void)args;
    (void)show_error;

    //TODO: overload
    error           = true;
    is_overloaded   = false;
    return ast::error_type(id->get_location());
};

ast::type kind_check_vis::eval(const ast::error_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    error = true;
    (void)k;
    (void)ctx;
    (void)show_error;
    return ast;
};

ast::type kind_check_vis::eval(const ast::type_sequence& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //not allowed at this stage
    (void)k;
    (void)ctx;
    (void)show_error;
    error = true;
    return ast;
};

ast::type kind_check_vis::eval(const ast::bracket_type& ast, const ast::kind& k, context_type& ctx,
                               bool& error, bool show_error)
{
    //not allowed at this stage
    (void)k;
    (void)ctx;
    (void)show_error;
    error = true;
    return ast;
};

}}}
