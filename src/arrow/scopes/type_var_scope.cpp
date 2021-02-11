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

#include "type_var_scope.h"

namespace arrow { namespace details
{

//-------------------------------------------------------------------
//                   type_var_scope_impl   
//-------------------------------------------------------------------
type_var_scope_impl::type_var_scope_impl(const ast::location& loc, module_index mi, 
                        scope_index si, bool is_kind_scope)
    :m_mod_index(mi), m_scope_index(si), m_parent(nullptr), m_loc(loc)
    ,m_last_def_index(0), m_kind_scope(is_kind_scope)
{};

type_var_scope_impl::type_var_scope_impl(const ast::location& loc, module_index mi, scope_index si, 
                        const type_var_scope& parent)
    :m_mod_index(mi), m_scope_index(si), m_parent(nullptr), m_loc(loc)
    ,m_last_def_index(0), m_kind_scope(parent.is_kind_scope())
{
    m_parent = parent.m_impl->shared_from_this();
};

bool type_var_scope_impl::set_definition_info(const ast::identifier& v)
{
    ast::symbol_data* sd    = v.get_symbol_data();

    sd->set_local_scope(m_scope_index);
    sd->set_module(m_mod_index);

    bool has;
    ast::location loc;
    def_index di            = find_definition(v.get_name_simple(), loc, has);

    if (has == true)
    {
        if (m_kind_scope == true)
            sd->get_kind().set_sector_kind().set_function_var();
        else
            sd->get_kind().set_sector_type().set_function_var();

        sd->set_definition(di);
        return true;
    }
    else
    {
        return false;
    };
};

def_index type_var_scope_impl::find_definition(ast::unique_string name, 
                        ast::location& loc, bool& has) const
{    
    auto pos = m_set.find(name);

    if (pos != m_set.end())
    {
        has             = true;
        long index      = pos->second;
        loc             = this->m_loc;
        return def_index(m_mod_index, m_scope_index, index);
    }

    if (m_parent)
        return m_parent->find_definition(name, loc, has);

    has = false;
    loc = ast::location();
    return def_index(def_index::fresh());
};

bool type_var_scope_impl::is_kind_scope() const
{
    return m_kind_scope;
}

def_index type_var_scope_impl::add_var(const error_handler_ptr& eh, const ast::identifier& vi)
{
    if (m_kind_scope == true)
    {
        bool has;
        ast::location loc;
        def_index def = find_definition(vi.get_name_simple(), loc, has);

        if (has == false)
        {
            def_index di = set_def_index(vi);
            m_set.insert(var_map::value_type(vi.get_name_simple(), code_kind(di.get_value())));
            return di;
        }
        else
        {
            set_def_index(vi, def);
            return def;
        }
    };

    //check type variables hiding
    if (true)
    {
        bool has;
        ast::location loc;
        def_index def = find_definition(vi.get_name_simple(), loc, has);

        if (has == true)
        {
            error::scope_error_msg(eh).error_type_symbol_already_bound
                            (vi->get_location(), vi.get_name_simple(), loc);
        };
    };

    def_index di = set_def_index(vi);
    m_set.insert(var_map::value_type(vi.get_name_simple(), code_kind(di.get_value())));
    return di;
}

def_index type_var_scope_impl::set_def_index(const ast::identifier& vi)
{
    long index  = m_last_def_index++;    

    def_index di            = def_index(m_mod_index, m_scope_index, index);    

    return set_def_index(vi, di);
}
def_index type_var_scope_impl::set_def_index(const ast::identifier& vi, const def_index& di)
{
    ast::symbol_data* sd    = vi.get_symbol_data();

    if (m_kind_scope == true)
        sd->get_kind().set_sector_kind().set_function_var();
    else
        sd->get_kind().set_sector_type().set_function_var();

    sd->set_local_scope(m_scope_index);
    sd->set_module(m_mod_index);    
    sd->set_definition(di);

    return di;
}

//-------------------------------------------------------------------
//                   type_var_scope   
//-------------------------------------------------------------------
type_var_scope::type_var_scope()
{};

type_var_scope::type_var_scope(const impl_ptr& im)
    :m_impl(im)
{};

type_var_scope::~type_var_scope()
{};

bool type_var_scope::set_definition_info(const ast::identifier& vi) const
{
    return m_impl->set_definition_info(vi);
};

bool type_var_scope::is_kind_scope() const
{
    return m_impl->is_kind_scope();
}

void type_var_scope::add_var(const error_handler_ptr& eh, const ast::identifier& vi)
{
    m_impl->add_var(eh, vi);
};

type_var_scope type_var_scope::new_var_scope(const ast::location& loc, const module& mod) const
{
    return mod.make_new_var_scope(loc, *this);
}

def_index type_var_scope::set_def_index(const ast::identifier& vi)
{
    return m_impl->set_def_index(vi);
}

}}