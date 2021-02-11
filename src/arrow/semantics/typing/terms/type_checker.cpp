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
#include "type_checker.h"
#include "semantics/typing/types/kind_checker.h"
#include "term_checker.h"

namespace arrow { namespace typing
{

type_checker::type_checker(module& mod, const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{
    m_kind_checker = new typing::kind_checker(m_eh, m_mod);
    m_term_checker = new typing::term_checker(m_eh, m_mod, m_kind_checker);
};

type_checker::~type_checker()
{
    delete m_kind_checker;
}

void type_checker::check_kind(rec_group_type& group, bool is_recursive, 
                              inst_collector* col)
{
    m_kind_checker->check(group, is_recursive, col);
};

void type_checker::check_subkind_star(ast::type& t, context_type& ctx,
                                      inst_collector* col)
{
    return m_kind_checker->check_subkind_star(t, ctx, col);
};

ast::term type_checker::check_type(const ast::term& ex, ast::type& t, bool is_local_func,
                                   inst_collector* col)
{
    return m_term_checker->check(ex, t, is_local_func, col);
};

void type_checker::check_type(rec_group_term& group, bool is_recursive, bool is_local_func,
                              inst_collector* col)
{
    return m_term_checker->check(group, is_recursive, is_local_func, col);
};

};};
