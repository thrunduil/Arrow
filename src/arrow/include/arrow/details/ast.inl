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
#include "arrow/ast/ast.h"

namespace arrow { namespace ast
{

inline bool unique_string::operator==(unique_string other) const
{
    return this->m_value == other.m_value;
}

inline bool unique_string::operator!=(unique_string other) const
{
    return this->m_value != other.m_value;
}

inline bool unique_string::operator>=(unique_string other) const
{
    return this->m_value >= other.m_value;
}

inline bool unique_string::operator<=(unique_string other) const
{
    return this->m_value <= other.m_value;
}

inline bool unique_string::operator>(unique_string other) const
{
    return this->m_value > other.m_value;
}

inline bool unique_string::operator<(unique_string other) const
{
    return this->m_value < other.m_value;
}

inline unique_string::unique_string(const unique_string& other)
    : m_value(other.m_value)
{};

inline unique_string& unique_string::operator=(const unique_string& other)
{
    m_value = other.m_value;
    return *this;
};

inline unique_string::operator bool() const
{
    return m_value != nullptr;
}

inline unique_string::unique_string()
    :m_value(nullptr)
{};

inline bool location::has_location() const 
{ 
    return m_line > 0 && m_column > 0; 
};

inline bool location::operator==(const location& other) const
{
    return m_line == other.m_line && m_column == other.m_column 
            && m_file == other.m_file;
}
inline bool location::operator!=(const location& other) const
{
    return !operator==(other);
}
inline bool location::operator<(const location& other) const
{
    if (m_line < other.m_line)
        return true;
    if (m_line > other.m_line)
        return false;

    if (m_column < other.m_column)
        return true;
    if (m_column > other.m_column)
        return false;

    return m_file < other.m_file;
};

//----------------------------------------------------------------
//                  ast_atom
//----------------------------------------------------------------
inline ast_atom::ast_atom(const location& loc)
    :m_location(loc), m_refcount(0)
{};

inline void ast_atom::increase_refcount()
{
    ++m_refcount;
}

inline bool ast_atom::decrease_refcount()
{
    return (--m_refcount) == 0;
}

};};
