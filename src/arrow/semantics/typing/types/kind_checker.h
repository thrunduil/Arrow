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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/module.h"
#include "kind_inference_context.h"

namespace arrow { namespace typing
{

namespace ad = arrow::details;

namespace details
{
    class kind_check_vis;

    enum class star_kind_reason : int
    {
        primitive,  tuple,      arrow,  field_type, list,
        variant,    record,     ref,    all,        some,  
        type_var,   type_constant
    };

    enum class arrow_kind_reason : int
    {
        type_application,   type_lambda
    };

};

class kind_checker
{
    public:
        using group_item        = std::tuple<ad::var_info, ast::type, ast::kind>;
        using rec_group         = std::vector<group_item>;

        using context_type      = details::kind_inference_context;

    private:
        details::kind_check_vis*    m_check;
        error_handler_ptr           m_eh;
        module                      m_mod;

        friend details::kind_check_vis;

    public:
        kind_checker(const error_handler_ptr& eh, const module& mod);
        ~kind_checker();

        kind_checker(const kind_checker&)               = delete;
        kind_checker& operator=(const kind_checker&)    = delete;
        
        /// is_recursive is used only if group has one element
        void        check(rec_group& group, bool is_recursive, inst_collector* collector);

        void        check_simple(ast::type& t, ast::kind& k, 
                        context_type& ctx, inst_collector* collector);

        void        check_subkind_star(ast::type& t, context_type& ctx, 
                        inst_collector* collector);
        void        check_subkind_star(const ast::location& loc, const ast::kind& k,
                        details::star_kind_reason reason, const ast::type& current_type);

    private:
        ast::kind   get_type_constructor_kind(const ast::identifier_type& id,
                        bool& is_overload, bool& has_error) const;
};

};};