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

#include "instantiation_table.h"
#include "scopes/symbol_data.h"
#include "scopes/symbol_definition.h"
#include "scopes/type_table.h"

namespace arrow { namespace details
{

template<class Conf>
void instantiation_table<Conf>::clear()
{
    m_defs.clear();
};

template<class Conf>
typename instantiation_table<Conf>::iterator_type
instantiation_table<Conf>::register_local_function_subst(const ast::identifier& id, 
                        def_type&& def)
{
    auto pos    = m_defs.insert(fun_sym_map::value_type(id, std::move(def)));
    return pos.first;
};

template<class Conf>
typename instantiation_table<Conf>::iterator_type 
instantiation_table<Conf>::add_type(const ident_type& id_base, 
            const ast::identifier& new_id, def_type&& def, 
            const std::vector<ast::type>& args, bool true_inst)
{
    if (true_inst == true)
    {
        const ast::identifier& base = id_base.get_identifier();

        auto pos    = m_type_ident_map.find(base);

        if (pos == m_type_ident_map.end())
            pos     = m_type_ident_map.insert(pos, type_id_map::value_type(base, type_inst_vec()));

        pos->second.push_back(type_inst_info(args, new_id));
    };

    auto pos    = m_defs.insert(fun_sym_map::value_type(new_id, std::move(def)));
    return pos.first;
};

template<class Conf>
typename instantiation_table<Conf>::iterator_type
instantiation_table<Conf>::add_kind(const ident_type& id_base, 
            const ast::identifier& new_id, def_type&& def, 
            const std::vector<ast::kind>& args, bool true_inst)
{
    if (true_inst == true)
    {
        const ast::identifier& base = id_base.get_identifier();

        auto pos    = m_kind_ident_map.find(base);

        if (pos == m_kind_ident_map.end())
            pos     = m_kind_ident_map.insert(pos, kind_id_map::value_type(base, kind_inst_vec()));

        pos->second.push_back(kind_inst_info(args, new_id));
    };

    auto pos    = m_defs.insert(fun_sym_map::value_type(new_id, std::move(def)));
    return pos.first;
};

template<class Conf>
ast::identifier instantiation_table<Conf>::get_existing_type(const ident_type& base_id, 
                    const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                    long& num_matched)
{
    num_matched = 0;

    auto pos = m_type_ident_map.find(base_id.get_identifier());
    if (pos == m_type_ident_map.end())
    {
        has = false;
        return ast::identifier();
    };

    const type_inst_vec& inst       = pos->second;
    long longest_match              = 0;
    ast::identifier best;

    long arg_size                   = (long)args.size();

    for (const auto& item : inst)
    {
        const type_vec& it_args = item.first;
        long it_size            = (long)it_args.size();

        if (it_size > arg_size)
            continue;
        if (it_size <= longest_match)
            continue;

        for (long i = 0; i < it_size; ++i)
        {
            if (comp.test(args[i], it_args[i]) == false)
                goto lab_exit;
        };

        if (it_size == arg_size)
        {
            has         = true;
            num_matched = arg_size;
            return item.second;
        };

        longest_match       = it_size;
        best                = item.second;

      lab_exit:
        ;
    };

    num_matched = longest_match;
    has         = false;
    return best;
};

template<class Conf>
ast::identifier instantiation_table<Conf>::get_existing_kind(const ident_type& base_id, 
                    const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                    long& num_matched)
{
    num_matched = 0;

    auto pos = m_kind_ident_map.find(base_id.get_identifier());
    if (pos == m_kind_ident_map.end())
    {
        has = false;
        return ast::identifier();
    };

    const kind_inst_vec& inst       = pos->second;
    long longest_match              = 0;
    ast::identifier best;

    long arg_size                   = (long)args.size();

    for (const auto& item : inst)
    {
        const kind_vec& it_args = item.first;
        long it_size            = (long)it_args.size();

        if (it_size > arg_size)
            continue;
        if (it_size <= longest_match)
            continue;

        for (long i = 0; i < it_size; ++i)
        {
            if (comp.test(args[i], it_args[i]) == false)
                goto lab_exit;
        };

        if (it_size == arg_size)
        {
            has         = true;
            num_matched = arg_size;
            return item.second;
        };

        longest_match       = it_size;
        best                = item.second;

      lab_exit:
        ;
    };

    num_matched = longest_match;
    has         = false;
    return best;
};

template class instantiation_table<inst_term_conf>;
template class instantiation_table<inst_type_conf>;

};}
