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

#include "substitute_term_vis.h"
#include "scopes/symbol_data.h"

namespace arrow { namespace codegen
{

//------------------------------------------------------------------
//              substitute_term_vis
//------------------------------------------------------------------
substitute_term_vis::substitute_term_vis(const module& mod, const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{};

substitute_term_vis::substitute_term_vis(const ast::identifier& id, const ast::term& subs,
                const module& mod, const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{
    m_subs_map[id] = subs;
};

void substitute_term_vis::add(const ast::identifier& id, const ast::term& subs)
{
    m_subs_map[id] = subs;
};

ast::term substitute_term_vis::eval(const ast::identifier_term& ast)
{
    auto pos = m_subs_map.find(ast.get_identifier());

    if (pos != m_subs_map.end())
        return pos->second;
    else
        return ast;
};

ast::term substitute_term_vis::eval(const ast::closure_term& ast)
{
    const ast::term& body   = ast.get_body();
    long n                  = ast.size();

    ast::term body2         = visit(body);

    bool modified           = body2 != body;

    ast::closure_term ret(ast->get_location(), body2, ast.get_kind());

    for (long i = 0; i < n; ++i)
    {
        const ast::term& s          = ast.get_substitution(i);
        const ast::identifier& id   = ast.get_internal_ident(i);
        const ast::type& ty         = ast.get_type(i);

        ast::term s2                = visit(s);
        ast::type ty2               = visit(ty);

        modified                    |= s2 != s;
        modified                    |= ty2 != ty;

        ret.add_item(s2, id, ty2);
    };

    if (modified == false)
        return ast;
    else
        return ret;
};

ast::term substitute_term_vis::eval(const ast::overloaded_symbol& ast)
{
    return ast;
}

ast::term substitute_term_vis::eval(const ast::lambda_term& ast)
{
    //default rewrite is OK
    return base_type::eval_lambda(ast);
};

ast::term substitute_term_vis::eval(const ast::mu_term& ast)
{
    //default rewrite is OK
    return base_type::eval_mu(ast);
};

ast::term substitute_term_vis::eval(const ast::unpack_term& ast)
{
    //default rewrite is OK
    return base_type::eval_unpack(ast);
};

ast::variant_case_term substitute_term_vis::eval(const ast::variant_case_term& ast)
{
    //default rewrite is OK
    return base_type::eval_case(ast);
};

ast::term substitute_term_vis::eval(const ast::let_term& ast)
{            
    //definitions need not be modified

    const ast::term& body   = ast.get_body();
    ast::term body2         = visit(body);

    if (body2 == body)
    {
        return ast;
    }
    else
    {
        return ast::let_term(ast->get_location(), ast.get_definitions(), 
                        body2, ast.get_symbol_data());
    };
}

};};