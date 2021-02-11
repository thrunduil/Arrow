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

#include "operator_declaration.h"
#include "error/error.h"

#include <stdexcept>
#include <iostream>

namespace arrow { namespace oper
{

namespace ae = arrow::error;

//-------------------------------------------------------------------
//                   operator_declaration   
//-------------------------------------------------------------------
operator_declaration::operator_declaration()
{};

const ast::location& operator_declaration::get_location(ast::fixity_type fix) const
{
    switch(fix)
    {
        case ast::fixity_type::infix:
            return m_location_infix;
        case ast::fixity_type::prefix:
            return m_location_prefix;
        case ast::fixity_type::postfix:
            return m_location_postfix;
        case ast::fixity_type::indexing:
            return m_location_indexing;
        case ast::fixity_type::surrounding:
            return m_location_surrounding;
        default:
            throw std::runtime_error("invalid case");
    };
}

const ast::operator_info& operator_declaration::get_info(ast::fixity_type fix) const
{
    switch(fix)
    {
        case ast::fixity_type::infix:
            return m_info_infix;
        case ast::fixity_type::prefix:
            return m_info_prefix;
        case ast::fixity_type::postfix:
            return m_info_postfix;
        case ast::fixity_type::indexing:
            return m_info_indexing;
        case ast::fixity_type::surrounding:
            return m_info_surrounding;
        default:
            throw std::runtime_error("invalid case");
    };
}

operator_declaration operator_declaration::select_info(ast::fixity_type fix) const
{
    switch(fix)
    {
        case ast::fixity_type::infix:
        {
            operator_declaration ret;
            ret.m_location_infix    = m_location_infix;
            ret.m_info_infix        = m_info_infix;
            return ret;
        }
        case ast::fixity_type::prefix:
        {
            operator_declaration ret;
            ret.m_location_prefix    = m_location_prefix;
            ret.m_info_prefix        = m_info_prefix;
            return ret;
        }
        case ast::fixity_type::postfix:
        {
            operator_declaration ret;
            ret.m_location_postfix    = m_location_postfix;
            ret.m_info_postfix        = m_info_postfix;
            return ret;
        }
        case ast::fixity_type::surrounding:
        {
            operator_declaration ret;
            ret.m_location_surrounding  = m_location_surrounding;
            ret.m_info_surrounding      = m_info_surrounding;
            return ret;
        }
        case ast::fixity_type::indexing:
        {
            operator_declaration ret;
            ret.m_location_indexing     = m_location_indexing;
            ret.m_info_indexing         = m_info_indexing;
            return ret;
        }
        default:
            throw std::runtime_error("invalid case");
    };
}

bool operator_declaration::has(ast::fixity_type fix) const
{
    switch(fix)
    {
        case ast::fixity_type::undefined:
            return false;
        case ast::fixity_type::infix:
            return m_info_infix.m_fixity != 0;
        case ast::fixity_type::prefix:
            return m_info_prefix.m_fixity != 0;
        case ast::fixity_type::postfix:
            return m_info_postfix.m_fixity != 0;
        case ast::fixity_type::indexing:
            return m_info_indexing.m_fixity != 0;
        case ast::fixity_type::surrounding:
            return m_info_surrounding.m_fixity != 0;
        default:
            throw std::runtime_error("invalid case");
    };
}

bool operator_declaration::is_defined() const
{
    return m_info_infix.m_fixity != 0 
        || m_info_prefix.m_fixity != 0
        || m_info_postfix.m_fixity != 0
        || m_info_surrounding.m_fixity != 0
        || m_info_indexing.m_fixity != 0
        ;
}

bool operator_declaration::all_operators_defined() const
{
    return m_info_infix.m_fixity != 0 
        && m_info_prefix.m_fixity != 0
        && m_info_postfix.m_fixity != 0
        && m_info_surrounding.m_fixity != 0
        && m_info_indexing.m_fixity != 0
        ;
}

void operator_declaration::add(const ast::location& loc, const ast::operator_info& info, ast::fixity_type fix)
{
    switch(fix)
    {
        case ast::fixity_type::infix:
            m_location_infix    = loc;
            m_info_infix        = info;
            return;
        case ast::fixity_type::prefix:
            m_location_prefix    = loc;
            m_info_prefix        = info;
            return;
        case ast::fixity_type::postfix:
            m_location_postfix    = loc;
            m_info_postfix        = info;
            return;
        case ast::fixity_type::indexing:
            m_location_indexing   = loc;
            m_info_indexing       = info;
            return;
        case ast::fixity_type::surrounding:
            m_location_surrounding   = loc;
            m_info_surrounding       = info;
            return;

        default:
            throw std::runtime_error("invalid case");
    };
}

operator_declaration
operator_declaration::make_single(const ast::location& loc, const ast::operator_info& decl)
{
    operator_declaration ret;
    ret.add_single(loc, decl);
    return ret;
};

void operator_declaration::add_single(const ast::location& loc, const ast::operator_info& info)
{
    if (info.is_infix())
    {
        m_location_infix    = loc;
        m_info_infix        = info;
        return;
    }
    else if (info.is_prefix())
    {
        m_location_prefix    = loc;
        m_info_prefix        = info;
        return;
    }
    else if (info.is_postfix())
    {
        m_location_postfix    = loc;
        m_info_postfix        = info;
        return;
    }
    else if (info.is_indexing())
    {
        m_location_indexing   = loc;
        m_info_indexing       = info;
        return;
    }
    else if (info.is_surrounding())
    {
        m_location_surrounding   = loc;
        m_info_surrounding       = info;
        return;
    };
};

void operator_declaration::add_definitions(const operator_declaration& defs)
{
    for (auto ft : {ast::fixity_type::infix, ast::fixity_type::prefix, ast::fixity_type::postfix,
                        ast::fixity_type::indexing, ast::fixity_type::surrounding})
    {
        if (this->has(ft) == false)
            this->add(defs.get_location(ft), defs.get_info(ft), ft);
    }
};

bool operator_declaration::equal_defined(const operator_declaration& other, 
                            std::vector<ast::location>& different_locations)
{
    bool equal = true;

    for (auto ft : {ast::fixity_type::infix, ast::fixity_type::prefix, ast::fixity_type::postfix,
                        ast::fixity_type::indexing, ast::fixity_type::surrounding})
    {
        if (this->has(ft) == true && other.has(ft) == true)
        {
            if (this->get_info(ft) != other.get_info(ft))
            {
                equal = false;
                different_locations.push_back(this->get_location(ft));
                different_locations.push_back(other.get_location(ft));
            };
        };
    }

    return equal;
};

size_t operator_declaration::get_all_fixity_flag() const
{
    size_t ret = 0;
    ret     |= m_info_infix.m_fixity;
    ret     |= m_info_prefix.m_fixity;
    ret     |= m_info_postfix.m_fixity;
    ret     |= m_info_indexing.m_fixity;
    ret     |= m_info_surrounding.m_fixity;

    return ret;
};

void operator_declaration::print(std::ostream& os) const
{
    bool need_nl    = false;

    if (this->has(ast::fixity_type::infix))
    {
        if (need_nl == true)
            os << "\n";

        m_info_infix.print(os);
        need_nl     = true;
    };
    if (this->has(ast::fixity_type::prefix))
    {
        if (need_nl == true)
            os << "\n";

        m_info_prefix.print(os);
        need_nl     = true;
    };
    if (this->has(ast::fixity_type::postfix))
    {
        if (need_nl == true)
            os << "\n";

        m_info_postfix.print(os);
        need_nl     = true;
    };
    if (this->has(ast::fixity_type::indexing))
    {
        if (need_nl == true)
            os << "\n";

        m_info_indexing.print(os);
        need_nl     = true;
    };
    if (this->has(ast::fixity_type::surrounding))
    {
        if (need_nl == true)
            os << "\n";

        m_info_surrounding.print(os);
        need_nl     = true;
    };
};

};};
