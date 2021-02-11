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

#include "unique_names_provider.h"
#include "root/options.h"
#include "grammar/unicode.h"

#include <sstream>

namespace arrow { namespace details
{

unique_names_provider :: unique_names_provider()
{};

unique_names_provider& unique_names_provider::get()
{
    static unique_names_provider instance;
    return instance;
};

void unique_names_provider::clear()
{
    m_string_count.clear();
    m_string_set.clear();
    m_var_map.clear();
};

std::string unique_names_provider::get_base_name(const std::string& str)
{
    int32_t index_last  = (long)str.size();

    while (index_last > 0)
    {
        int32_t old_index   = index_last;
        int32_t code        = read_previous_code_point_from_utf8(str, index_last);

        if (code < 0)
            return str;

        if (code >= '0' && code <= '9')
            continue;

        index_last          = old_index;
        break;
    }

    return str.substr(0, index_last);
};

ast::identifier unique_names_provider::get_unique_name(const ast::identifier& old, 
                                            const std::string& base)
{
    if (old.get_symbol_data()->has_definition() == false)
        return old;

    long& trial             = get_count(base);
    ast::unique_string name = get_unique_name(base, trial);
    ast::fixity_type ft     = old.get_fixity();
    ast::identifier new_id  = ast::identifier(old->get_location(), name, ft, old.get_symbol_data());

    m_var_map[old]  = new_id;
    return new_id;
};

long& unique_names_provider::get_count(const std::string& name)
{
    auto pos = m_string_count.find(name);
    if (pos == m_string_count.end())
        pos         = m_string_count.insert(string_count::value_type(name, 0)).first;

    long& count = pos->second;
    return count;
}

ast::unique_string unique_names_provider::get_unique_name(const std::string& base, long& trial)
{
    ++trial;
    ast::unique_string new_name = ast::unique_string(base + to_string(trial));
    auto pos = m_string_set.find(new_name);

    if (pos == m_string_set.end())
    {
        m_string_set.insert(pos, new_name);
        return new_name;
    };

    return get_unique_name(base, trial);
};

std::string unique_names_provider::to_string(long val) const
{
    std::ostringstream os;
    os << val;
    return os.str();
}

ast::identifier unique_names_provider::get_mapping(const ast::identifier& old, bool& has)
{
    if (old.get_symbol_data()->has_definition() == false)
    {
        has = false;
        return old;
    };

    auto pos = m_var_map.find(old);
    if (pos == m_var_map.end())
    {
        has = false;
        return old;
    }

    has = true;
    return pos->second;
};

}}
