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
#include "term_checker.h"

namespace arrow { namespace typing { namespace details
{

//abstract over free type variables
//default kind vars to kind *
//normalize types
class close_term
{
    public:
        using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group     = std::vector<group_item>;
        using context_type  = details::kind_inference_context;

    private:
        term_checker*   m_owner;

    public:
        close_term(term_checker* owner);

        void            make(rec_group& group, context_type& ctx, inst_collector& col);
};

}}}
