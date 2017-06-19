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
#include "arrow/ast/ast_visitor.h"
#include "arrow/error_handler.h"
#include "scopes/type_table.h"
#include "semantics/typing/types/kind_inference_context.h"

namespace arrow { namespace typing
{

class kind_checker;
class term_checker;
class inst_collector;

class type_checker
{
    public:
        using group_item_type   = std::tuple<ad::var_info, ast::type, ast::kind>;
        using rec_group_type    = std::vector<group_item_type>;

        using group_item_term   = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group_term    = std::vector<group_item_term>;

        using context_type  = details::kind_inference_context;

    private:
        module              m_mod;
        error_handler_ptr   m_eh;
        kind_checker*       m_kind_checker;
        term_checker*       m_term_checker;        

    public:
        type_checker(module& mod, const error_handler_ptr& eh);
        ~type_checker();

        type_checker(const type_checker&) = delete;
        type_checker& operator=(const type_checker&) = delete;

    public:
        /// type checking
        /// is_recursive is used only if group has one element
        void        check_kind(rec_group_type& group, bool is_recursive,
                        inst_collector* col);
        void        check_subkind_star(ast::type& t, context_type& ctx, 
                        inst_collector* col);

        /// term checking
        /// is_recursive is used only if group has one element
        void        check_type(rec_group_term& group, bool is_recursive, bool is_local_func,
                        inst_collector* col);
        ast::term   check_type(const ast::term& ex, ast::type& t, bool is_local_func,
                        inst_collector* col);
};

};};