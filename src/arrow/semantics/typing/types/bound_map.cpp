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

#include "bound_map.h"

namespace arrow { namespace typing
{

bound_map_scope::bound_map_scope(bound_map* bm)
    :m_owner(bm), m_has_1(false), m_has_2(false)
{};

bound_map_scope::~bound_map_scope()
{
    if (!m_owner)
        return;

    if (m_has_1)
        m_owner->restore(m_var_1, m_code_1);

    if (m_has_2)
        m_owner->restore(m_var_2, m_code_2);
}

bound_map_scope::bound_map_scope(bound_map_scope&& other)
    :m_owner(other.m_owner)
    , m_has_1(other.m_has_1), m_var_1(std::move(other.m_var_1)), m_code_1(other.m_code_1)
    , m_has_2(other.m_has_2), m_var_2(std::move(other.m_var_2)), m_code_2(other.m_code_2)
{
    other.m_owner = nullptr;
};

void bound_map_scope::add1(const ad::var_info& v, long code)
{
    m_has_1     = true;
    m_var_1     = v;
    m_code_1    = code;
}
void bound_map_scope::add2(const ad::var_info& v, long code)
{
    m_has_2     = true;
    m_var_2     = v;
    m_code_2    = code;
}

void bound_map::clear()
{
    m_map.clear();
};

void bound_map::restore(const ad::var_info& v, long code)
{
    m_map[v] = code;
}

bound_map_scope bound_map::add(const ast::identifier& id1, const ast::identifier& id2)
{
    long code   = (long)m_map.size();

    ad::var_info v1(id1);
    ad::var_info v2(id2);

    auto pos1   = m_map.find(v1);
    auto pos2   = m_map.find(v2);

    bound_map_scope sc(this);

    if (pos1 == m_map.end())
    {
        m_map.insert(pos1, map_type::value_type(v1, code));
    }
    else
    {
        long old_code   = pos1->second;
        pos1->second    = code;
        sc.add1(v1, old_code);
    }

    if (pos2 == m_map.end())
    {
        m_map.insert(pos2, map_type::value_type(v2, code));
    }
    else
    {
        long old_code   = pos2->second;
        pos2->second    = code;
        sc.add2(v2, old_code);
    }

    return std::move(sc);
}

long bound_map::get_code(const ast::identifier& id) const
{
    auto pos = m_map.find(ad::var_info(id));
    if (pos == m_map.end())
        return -1;
    else
        return pos->second;
}

bool bound_map::is_bound(long code)
{
    return code >= 0;
}

/*
void reverse_bound_map::add(const ast::identifier& id1, const ast::identifier& id2)
{
    ad::var_info v1(id1);
    ad::var_info v2(id2);

    m_map_1[v1] = id1;
    m_map_1[v2] = id1;

    m_map_2[v1] = id2;
    m_map_2[v2] = id2;
};

ast::identifier reverse_bound_map::correct_binding_1(const ast::identifier& id) const
{
    auto pos = m_map_1.find(ad::var_info(id));
    if (pos == m_map_1.end())
        return id;
    else
        return pos->second;
}
ast::identifier reverse_bound_map::correct_binding_2(const ast::identifier& id) const
{
    auto pos = m_map_2.find(ad::var_info(id));
    if (pos == m_map_2.end())
        return id;
    else
        return pos->second;
}

void reverse_bound_map::clear()
{
    m_map_1.clear();
    m_map_2.clear();
};
*/

};};