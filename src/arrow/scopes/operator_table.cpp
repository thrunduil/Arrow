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

#include "operator_table.h"

namespace arrow { namespace details
{

bool operator_table::get_all_operator_declaration(ast::unique_string str, 
                        oper::operator_declaration& ret) const
{
    auto pos    = m_oper_decl_map.find(str);

    if (pos != m_oper_decl_map.end())
    {
        ret.add_definitions(pos->second);
        return true;
    }
    else
    {
        return false;
    };
};

bool operator_table::check_operator_type(ast::unique_string str, oper::operator_declaration& od) const
{
    auto pos    = m_oper_decl_map.find(str);

    if (pos != m_oper_decl_map.end())
    {
        od.add_definitions(pos->second);

        if (od.all_operators_defined() == true)
            return true;
    }

    return false;
};

void operator_table::add_operator_declaration(const ast::identifier& vi, 
                        const ast::operator_info& fixity)
{
    ast::unique_string str      = vi.get_name_simple();
      
    auto pos_old   = m_oper_decl_map.find(vi.get_name_simple());

    if (pos_old == m_oper_decl_map.end())
    {
        m_oper_decl_map[vi.get_name_simple()]  
            = oper::operator_declaration::make_single(vi->get_location(), fixity);
    }
    else
    {
        pos_old->second.add_single(vi->get_location(), fixity);
    }
};

bool operator_table::is_any_operator(ast::unique_string vi) const
{
    auto pos = m_oper_decl_map.find(vi);
    if (pos != m_oper_decl_map.end())
        return true;
    else
        return false;
};

bool operator_table::is_operator_declared(ast::unique_string vi, 
                        ast::fixity_type ft) const
{
    auto pos = m_oper_decl_map.find(vi);
    if (pos != m_oper_decl_map.end())
    {
        if (pos->second.has(ft) == true)
            return true;
    };

    return false;
};

oper::operator_declaration operator_table::get_operator_declaration(ast::unique_string str, 
                ast::fixity_type ft, bool& found) const
{
    found       = false;
    auto pos    = m_oper_decl_map.find(str);

    // operator defined in given module hides definitions of an operator
    // of the same kind in other modules
    if (pos!= m_oper_decl_map.end())
    {
        if (pos->second.has(ft))
        {
            found = true;
            return pos->second.select_info(ft);
        }
    }

    return oper::operator_declaration();
};

};}
