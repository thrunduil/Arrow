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

#include "term_checker.h"
#include "term_check_vis.h"
#include "arrow/ast/ast_visitor.h"
#include "semantics/typing/types/kind_checker.h"
#include "semantics/typing/types/unifier_kinds.h"

#include <iostream>
#include <sstream>

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              term_checker
//------------------------------------------------------------------

term_checker::term_checker(const error_handler_ptr& eh, const module& mod, kind_checker* kc)
    :m_eh(eh), m_mod(mod), m_kind_check(kc), m_type_eq(kc, mod, eh, nullptr)
{
    m_check     = new details::term_check_vis(this);
    m_type_bool = ast::predefined_type(ast::location(), ast::predefined_type_code::type_bool);
};

term_checker::~term_checker()
{
    delete m_check;
}

ast::term term_checker::check(const ast::term& ex, ast::type& t, bool is_local_func,
                              inst_collector* col)
{
    m_check->start();    
    m_type_eq.set_collector(col);
    return m_check->make(ex, t, is_local_func, col);
};

void term_checker::check(rec_group& group, bool is_recursive, bool is_local_func,
                         inst_collector* col)
{
    m_check->start();    
    m_type_eq.set_collector(col);
    return m_check->make(group, is_recursive, is_local_func, col);
};

void term_checker::check_subkind_star(ast::type& t, context_type& ctx, 
                                      inst_collector* collector)
{
    return m_kind_check->check_subkind_star(t, ctx, collector);
}
void term_checker::check_subkind_star(const ast::location& loc, const ast::kind& k,
                        details::star_kind_reason reason, const ast::type& current_type)
{
    return m_kind_check->check_subkind_star(loc, k, reason, current_type);
};

void term_checker::check_kind(ast::type& t, ast::kind& k, context_type& ctx, 
                              inst_collector* collector)
{
    m_kind_check->check_simple(t, k, ctx, collector);
};

ast::kind term_checker::infer_kind(ast::type& t, context_type& ctx, 
                                   inst_collector* collector)
{
    ast::kind_var kv(t->get_location(), false);
    ast::kind k = kv;
    m_kind_check->check_simple(t, k, ctx, collector);

    return k;
}

bool term_checker::unify_kinds(const ast::location& loc, const ast::kind& k1, 
                               const ast::kind& k2, bool show_errors)
{
    return unifier_kinds(m_eh).unify_kinds(loc, k1, k2, show_errors);
}

bool term_checker::is_subkind(const ast::location& loc, const ast::kind& k_spec, 
                        const ast::kind& k_gen, bool show_errors)
{
    return unifier_kinds(m_eh).is_subkind(loc, k_spec, k_gen, show_errors);
}
bool term_checker::is_subkind_star(const ast::location& loc, const ast::kind& k_spec, 
                        bool show_errors)
{
    (void)loc;
    (void)show_errors;
    return unifier_kinds(m_eh).is_star_kind_any(k_spec);
}

ast::type term_checker::get_symbol_type(const ast::identifier_term& id, bool& overloaded,
                                        bool& has_errors)
{
    return m_mod.get_symbol_type(m_eh, id, overloaded, has_errors);
}

const ast::type& term_checker::get_type_bool() const
{
    return m_type_bool;
}

};};