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

#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace ast
{

struct id_rewriter : public rewrite_visitor<id_rewriter>
{
    using base_type = rewrite_visitor<id_rewriter>;

    lambda_term eval(const lambda_term& ast)
    {
        return base_type::eval_lambda(ast);
    };
    mu_term eval(const mu_term& ast)
    {
        return base_type::eval_mu(ast);
    };

    unpack_term eval(const unpack_term& ast)
    {
        return base_type::eval_unpack(ast);
    };

    variant_case_term eval(const variant_case_term& ast)
    {
        return base_type::eval_case(ast);
    };

    let_term eval(const let_term& ast)
    {
        return base_type::eval_let(ast);
    }

    template<class Ast>
    auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
    {
        return base_type::eval(ast);
    }
};

struct id_rewriter2 : public traversal_visitor<id_rewriter2>
{
    using base_type = traversal_visitor<id_rewriter2>;
};

definitions id_rewrite(const definitions& ast)
{
    auto x = id_rewriter().visit(ast);
    id_rewriter2().visit(ast);
    return x;
};

};};
