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

#include "kind_checker.h"
#include "kind_checker_vis.h"
#include "unifier_kinds.h"

namespace arrow { namespace typing { namespace details
{

class occurs_check_vis : public ast::traversal_visitor<occurs_check_vis>
{
    private:
        using base_type = ast::traversal_visitor<occurs_check_vis>;

    private:
        const ast::kind_var&    m_var;

    public:
        bool found;

    public:
        occurs_check_vis(const ast::kind_var& var)
            :m_var(var), found(false)
        {};

        void eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            if (ast == m_var)
                found = true;
        };

        template<class Ast>
        void eval(const Ast& ast)
        {
            base_type::eval(ast);
        };
};

};};};

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              kind_checker
//------------------------------------------------------------------
kind_checker::kind_checker(const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod)
{
    m_check = new details::kind_check_vis(this);
};

kind_checker::~kind_checker()
{
    delete m_check;
}

void kind_checker::check(rec_group& group, bool is_recursive, inst_collector* collector)
{
    m_check->make(group, is_recursive, collector);
}
void kind_checker::check_simple(ast::type& t, ast::kind& k, 
                                context_type& ctx, inst_collector* collector)
{
    m_check->make_from_term_checker(t, k, ctx, collector, false);
}

void kind_checker::check_subkind_star(ast::type& t, context_type& ctx, 
                                      inst_collector* collector)
{
    ast::kind_var kv(t->get_location(), true);
    ast::kind k = kv;
    return m_check->make_from_term_checker(t, k, ctx, collector, true);
}

void kind_checker::check_subkind_star(const ast::location& loc, const ast::kind& k,
                    details::star_kind_reason reason, const ast::type& current_type)
{
    if (unifier_kinds(m_eh).is_star_kind_any(k) == false)
    {
        m_check->error_star_kind_required(loc, k, reason, current_type, current_type);
    };
};

ast::kind kind_checker::get_type_constructor_kind(const ast::identifier_type& id,
                                        bool& overloaded, bool& has_error) const
{
    return m_mod.get_type_constructor_kind(m_eh, id, overloaded, has_error);
};

};};