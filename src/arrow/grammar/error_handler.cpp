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

#include "arrow/error_handler.h"

namespace arrow
{

//-------------------------------------------------------------
//              error_handler_scope
//-------------------------------------------------------------
error_handler_scope::error_handler_scope(error_handler* bm)
    :m_owner(bm)
{};

error_handler_scope::~error_handler_scope()
{
    if (m_owner)
        m_owner->pop_error_scope();
}
error_handler_scope::error_handler_scope(error_handler_scope&& other)
    :m_owner(other.m_owner)
{
    other.m_owner = nullptr;
};

//-------------------------------------------------------------
//              error_handler
//-------------------------------------------------------------
error_handler::error_handler(std::ostream* os)
    :m_os(os), m_has_errors(false)
{};

error_handler_scope error_handler::push_error_scope()
{
    m_error_scope.push_back(m_has_errors);
    m_has_errors = false;
    return error_handler_scope(this);
}

void error_handler::pop_error_scope()
{
    if (m_error_scope.size() > 0)
    {
        m_has_errors = m_has_errors || m_error_scope.back();
        m_error_scope.pop_back();
    };
};

void error_handler::report_parser_error(const location& loc, const std::string& msg)
{
    if (m_os)
    {
        *m_os << "\n";
        *m_os << msg;

        if (loc.has_location())
        {
            if (msg.size() > 0 && msg.back() == '\n')
            {}
            else
            {
                *m_os << "; ";
            }

            *m_os << "in ";
            loc.print(*m_os);
        }

        *m_os << "\n\n";
    }

    m_has_errors    = true;
};

void error_handler::report_parser_warning(const location& loc, const std::string& msg)
{
    if (m_os)
    {
        *m_os << "\n";
        *m_os << "warning: " << msg;

        if (loc.has_location())
        {
            if (msg.size() > 0 && msg.back() == '\n')
            {}
            else
            {
                *m_os << "; ";
            }

            *m_os << "in ";
            loc.print(*m_os);
        }

        *m_os << "\n\n";
    }
};


bool error_handler::has_errors() const
{
    return m_has_errors;
}

void error_handler::add_error()
{
    m_has_errors = true;
}

void error_handler::clear_errors()
{
    m_has_errors = false;
};

};
