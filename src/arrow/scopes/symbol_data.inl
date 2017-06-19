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

#include "symbol_data.h"

namespace arrow { namespace details
{

//---------------------------------------------------------
//                  module_index
//---------------------------------------------------------

inline module_index::module_index(ast::unique_string value)
    : m_value(value) 
{};

inline ast::unique_string module_index::get_value() const
{ 
    return m_value; 
};

inline bool module_index::operator==(const module_index& other) const
{
    return m_value == other.m_value;
}

inline bool module_index::operator!=(const module_index& other) const
{
    return m_value != other.m_value;
}

inline bool module_index::operator<(const module_index& other) const
{
    return m_value < other.m_value;
}

inline bool module_index::operator>(const module_index& other) const
{
    return m_value > other.m_value;
}

//---------------------------------------------------------
//                  def_index
//---------------------------------------------------------
inline def_index::def_index(fresh)
    : m_mod(ast::unique_string()), m_scope(scope_index::fresh()), m_value(-1) 
{};

inline def_index::def_index(module_index mod, scope_index scope, int value) 
    : m_mod(mod), m_scope(scope), m_value(value) 
{};

inline bool def_index::operator==(const def_index& other) const
{
    return m_value == other.m_value && m_mod == other.m_mod 
            && m_scope == other.m_scope;
}

inline bool def_index::operator!=(const def_index& other) const
{
    return !operator==(other);
}

inline bool def_index::operator<(const def_index& other) const
{
    if (m_value < other.m_value)
        return true;
    if (m_value > other.m_value)
        return false;

    if (m_scope < other.m_scope)
        return true;
    if (m_scope > other.m_scope)
        return false;

    return m_mod < other.m_mod;
}

inline bool def_index::operator>(const def_index& other) const
{
    if (m_value < other.m_value)
        return false;
    if (m_value > other.m_value)
        return true;

    if (m_scope < other.m_scope)
        return false;
    if (m_scope > other.m_scope)
        return true;

    return m_mod > other.m_mod;
}

//---------------------------------------------------------
//                  scope_index
//---------------------------------------------------------

inline scope_index::scope_index(fresh)
    : m_value(-1), m_type(undefined_scope)
{};

inline scope_index::scope_index(int value, type t) 
    : m_value(value), m_type(t)
{};

inline bool scope_index::operator==(const scope_index& other) const
{
    return (m_value == other.m_value) && m_type == other.m_type;
}

inline bool scope_index::operator!=(const scope_index& other) const
{
    return !operator==(other);
}

inline bool scope_index::operator<(const scope_index& other) const
{
    if (m_value < other.m_value)
        return true;
    if (m_value > other.m_value)
        return false;

    return m_type < other.m_type;
}

inline bool scope_index::operator>(const scope_index& other) const
{
    if (m_value > other.m_value)
        return true;
    if (m_value < other.m_value)
        return false;

    return m_type > other.m_type;
}

//-----------------------------------------------------------------
//                      var_info
//-----------------------------------------------------------------
inline var_info::var_info()
    : m_def(def_index::fresh()), m_kind(ast::identifier_kind())
{};

inline var_info::var_info(const ast::identifier& vi)
    : m_def(vi.get_symbol_data()->get_def_index())
    , m_name(vi.get_name(m_fix_type)), m_loc(vi->get_location())
    , m_kind(vi.get_symbol_data()->get_kind())
{
    check_def_index(m_def.get_value());
};

inline bool var_info::operator<(const var_info& other) const
{
    return (m_def < other.m_def);
}
inline bool var_info::operator==(const var_info& other) const
{
    return (m_def == other.m_def);
}

inline bool var_info::operator!=(const var_info& other) const
{
    return (m_def != other.m_def);
}

inline void var_info::check_def_index(int def) const
{
    if (def < 0)
        throw std::runtime_error("internal error: invalid definition index");
};

inline const def_index& var_info::get_def() const
{
    return m_def;
}

inline ast::unique_string var_info::get_name(ast::fixity_type& ft) const
{
    ft = m_fix_type;
    return m_name;
}
inline ast::unique_string var_info::get_name_simple() const
{
    return m_name;
};

inline const ast::location& var_info::get_location() const
{
    return m_loc;
}

inline ast::identifier var_info::make_var_identifier() const
{
    ast::fixity_type ft;
    ast::unique_string str  = get_name(ft);
    ast::identifier id      = ast::identifier(get_location(), str, ft, m_kind,  
                                          ast::identifier::fresh());

    id.get_symbol_data()->set_module(m_def.get_module());
    id.get_symbol_data()->set_local_scope(m_def.get_scope());
    id.get_symbol_data()->set_definition(m_def);    

    return id;
};

}}

namespace arrow { namespace ast
{

//---------------------------------------------------------
//                  symbol_data
//---------------------------------------------------------
inline symbol_data::symbol_data(identifier_kind ik)
    : m_module(arrow::details::module_index::fresh())
    , m_scope(arrow::details::scope_index::fresh())
    , m_def(arrow::details::def_index::fresh())
    , m_kind(ik)
{};

}};