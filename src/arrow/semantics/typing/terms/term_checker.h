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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "type_equaler.h"
#include "semantics/typing/types/kind_inference_context.h"

namespace arrow { namespace typing
{

namespace details
{
    class term_check_vis;    
    class close_term_vis;
};

class kind_checker;
class inst_collector;

class term_checker
{
    public:
        using group_item            = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group             = std::vector<group_item>;

        using context_type          = details::kind_inference_context;

    private:
        details::term_check_vis*    m_check;
        type_equaler                m_type_eq;
        kind_checker*               m_kind_check;
        ast::type                   m_type_bool;        

        error_handler_ptr           m_eh;
        module                      m_mod;

        friend details::term_check_vis;
        friend details::close_term_vis;

    public:
        term_checker(const error_handler_ptr& eh, const module& mod, kind_checker* kc);
        ~term_checker();

        term_checker(const term_checker&)               = delete;
        term_checker& operator=(const term_checker&)    = delete;

        /// is_recursive is used only if group has one element
        void                check(rec_group& group, bool is_recursive, bool is_local_func,
                                inst_collector* col);
        ast::term           check(const ast::term& ex, ast::type& t, bool is_local_func,
                                inst_collector* col);

    public:
        void                check_kind(ast::type& t, ast::kind& k, context_type& ctx,
                                inst_collector* collector);
        void                check_subkind_star(ast::type& t, context_type& ctx,
                                inst_collector* collector);
        void                check_subkind_star(const ast::location& loc, const ast::kind& k,
                                details::star_kind_reason reason, const ast::type& current_type);
        ast::kind           infer_kind(ast::type& t, context_type& ctx, inst_collector* collector);

        bool                unify_kinds(const ast::location& loc, const ast::kind& k1, 
                                const ast::kind& k2, bool show_errors);
        bool                is_subkind(const ast::location& loc, const ast::kind& k_spec, 
                                const ast::kind& k_gen, bool show_errors);
        bool                is_subkind_star(const ast::location& loc, const ast::kind& k_spec, 
                                bool show_errors);

        ast::type           get_symbol_type(const ast::identifier_term& id, bool& overloaded,
                                bool& has_errors);

        const ast::type&    get_type_bool() const;
};

};};