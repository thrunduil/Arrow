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

#include "arrow/ast/ast.h"
#include "semantics/operators/operator_declaration.h"

#include <map>

namespace arrow { namespace details
{

//-------------------------------------------------------------------
//                   operator_table   
//-------------------------------------------------------------------

class operator_table
{
    private:
        using oper_map      = std::map<ast::unique_string, oper::operator_declaration>;

    private:
        oper_map            m_oper_decl_map;

    public:
        void                add_operator_declaration(const ast::identifier& vi, 
                                const ast::operator_info& fix);

        bool                is_any_operator(ast::unique_string vi) const;
        bool                is_operator_declared(ast::unique_string vi, 
                                ast::fixity_type ft) const;
        bool                get_all_operator_declaration(ast::unique_string str, 
                                oper::operator_declaration& ret) const;
        oper::operator_declaration 
                            get_operator_declaration(ast::unique_string str, ast::fixity_type ft,
                                bool& found) const;

        bool                check_operator_type(ast::unique_string vi, 
                                oper::operator_declaration& oi) const;
};

};}
