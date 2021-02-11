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

#include "overload_table.h"
#include "module_impl.h"
#include "type_table.h"
#include "scopes/overload_set.h"
#include "semantics/semantics.h"

namespace arrow { namespace details
{

template<class Config>
void overload_table<Config>::clear()
{
    m_overload_map.clear();
};

template<class Config>
typename overload_table<Config>::iterator 
overload_table<Config>::get_or_build(const ast::identifier& vi, 
        ast::fixity_type ft,bool is_overload, const error_handler_ptr& eh, module_index mi,
        scope_index si, overload_finalizator<Config>* fin)
{
    ast::unique_string name = vi.get_name_simple();
    key_type_over key(name, ft);

    auto pos_over   = m_overload_map.find(key);
    if (pos_over == m_overload_map.end())
        pos_over    = build_new_overload(vi, ft, is_overload, eh, mi, si, fin);

    return pos_over;
};

template<class Config>
bool overload_table<Config>::is_overloaded(const ast::unique_string id) const
{
    auto pos = m_overload_map.find(key_type_over(id, ast::fixity_type::undefined));

    if (pos == m_overload_map.end())
        return false;
    else
        return pos->second.is_overload();
};

template<class Config>
typename overload_table<Config>::const_iterator 
overload_table<Config>::get(ast::unique_string id, ast::fixity_type ft, bool& has) const
{
    auto pos    = m_overload_map.find(key_type_over(id, ft));

    if (pos == m_overload_map.end())
        has = false;
    else
        has = true;

    return pos;
};

template<class Config>
typename overload_table<Config>::over_iter2 
overload_table<Config>::build_new_overload(const ast::identifier& id, ast::fixity_type ft, 
                                bool is_overload, const error_handler_ptr& eh,
                                module_index mi, scope_index si, overload_finalizator<Config>* fin)
{
    if (is_overload == false)
    {
        key_type_over   key_over(id.get_name_simple(), ft);
        overload_set os(mi, si, id, ft, false);
        return m_overload_map.insert(overload_map::value_type(key_over, os)). first;
    }

    if (ft == ast::fixity_type::undefined)
    {
        ast::identifier id2 = sema::copy(id);
        this->add_overload(id2, ft, func_type(), eh, mi, si, fin);

        key_type_over   key_over(id.get_name_simple(), ft);
        return m_overload_map.find(key_over);
    };

    ast::fixity_type ft_und = ast::fixity_type::undefined;
    key_type_over   key_over(id.get_name_simple(), ft_und);
    key_type_over   key_over2(id.get_name_simple(), ft);

    auto pos_over   = m_overload_map.find(key_over);

    if (pos_over == m_overload_map.end())
        pos_over    = build_new_overload(id, ft_und, is_overload, eh, mi, si, fin);

    const ast::identifier& id2  = pos_over->second.get_identifier();
    overload_set os = initialize_overload_set(id2, ft, is_overload, mi, si, fin);
    return m_overload_map.insert(overload_map::value_type(key_over2, os)). first;
};

template<class Config>
overload_set<Config> 
overload_table<Config>::initialize_overload_set(const ast::identifier& id, ast::fixity_type ft,
            bool is_overload, module_index mi, scope_index si, overload_finalizator<Config>* fin)
{
    ast::unique_string name = id.get_name_simple();

    if (is_overload == false)
    {
        key_type_over   key_over(name, ft);
        overload_set os(mi, si, id, ft, false);
        return os;
    }

    overload_set os(mi, si, id, ft, true);
    if (fin)
        fin->make(os, name, ft);

    return os;
};

template<class Config>
void overload_table<Config>::remove_failed_definition(const ast::identifier& id)
{
    key_type_over key(id.get_name_simple(), ast::fixity_type::undefined);

    auto pos = m_overload_map.find(key);

    if (pos == m_overload_map.end())
        return;

    if (pos->second.is_overload() == false)
        return;

    m_overload_map.erase(pos);
};

template<class Config>
typename overload_table<Config>::iterator 
overload_table<Config>::add_overload(const ast::identifier& vi, ast::fixity_type ft, 
        const func_type& ty, const error_handler_ptr& eh, module_index mi, scope_index si, 
        overload_finalizator<Config>* fin)
{
    (void)eh;
    bool has_def        = ty != func_type();

    ast::unique_string name = vi.get_name_simple();
    key_type_over key_string(name, ft);

    auto pos_over = m_overload_map.find(key_string);

    if (pos_over != m_overload_map.end())
    {
        if (has_def == false)
        {}
        else
        {
            pos_over->second    = initialize_overload_set(vi, ft, true, mi, si, fin);
        };
    }
    else
    {
        overload_set os = initialize_overload_set(vi, ft, true, mi, si, fin);
        pos_over        = m_overload_map.insert(overload_map::value_type(key_string, os)).first;
    };

    return pos_over;
};

template class overload_table<ad::overload_term_config>;
template class overload_table<ad::overload_type_config>;

};}
