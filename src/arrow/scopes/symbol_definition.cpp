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

#include "symbol_definition.h"

namespace arrow { namespace details
{

//-------------------------------------------------------------------
//                   symbol_definition_env   
//-------------------------------------------------------------------
symbol_definition_env::symbol_definition_env()
    :m_def_index(def_index::fresh()), m_group_index(-1),m_has_error(true)
{};

symbol_definition_env::symbol_definition_env(const ast::location& loc, const ast::type& sig, 
        const ast::identifier& id, const ast::term& body, bool is_overload)
    : m_location(loc), m_type(sig), m_symbol(id), m_body(body)
    , m_def_index(def_index::fresh()), m_group_index(-1), m_has_error(false)
{
    if (is_overload == false)
        m_symbol_type   = function;
    else if (body)
        m_symbol_type   = closed_overload;
    else
        m_symbol_type   = abstract_overload;
};

const ast::type& symbol_definition_env::get_signature() const
{
    return m_type;
}
void symbol_definition_env::set_signature(const ast::type& ty)
{
    if (ty)
    {
        if (ty->get_type_code() == ast::details::type_code::error_type)
            m_has_error = true;
    }

    m_type = ty;
}
const ast::identifier& symbol_definition_env::get_symbol() const
{
    return m_symbol;
};
const ast::term& symbol_definition_env::get_body(bool& has_body) const
{
    has_body = m_body != ast::term();
    return m_body;
}
void symbol_definition_env::set_body(const ast::term& ex)
{
    if (ex)
    {
        if (ex->get_term_code() == ast::details::term_code::error_term)
            m_has_error = true;
    };

    m_body = ex;
}
ast::term_definition symbol_definition_env::get_term_definition() const
{
    bool is_over    = m_symbol_type != function;
    bool has_body;
    ast::term_definition ret = ast::term_definition(get_location(), get_symbol(), 
                                    get_signature(), get_body(has_body), is_over);
    return ret;
}
void symbol_definition_env::update_term_definition(const ast::term_definition& def)
{
    set_signature(def.get_type());
    set_body(def.get_body());
};

bool symbol_definition_env::has_error() const
{
    return m_has_error;
}
void symbol_definition_env::set_error_flag() const
{
    m_has_error = true;
};

const ast::location& symbol_definition_env::get_location() const
{
    return m_location;
}
const def_index& symbol_definition_env::get_definition_index() const
{
    return m_def_index;
}
long symbol_definition_env::get_group_index() const
{
    return m_group_index;
}

symbol_definition_env::symbol_type 
symbol_definition_env::get_symbol_type() const
{
    return m_symbol_type;    
}

bool symbol_definition_env::is_overload() const
{    
    return m_symbol_type != function;
};
bool symbol_definition_env::is_abstract_overload() const
{
    return m_symbol_type == abstract_overload;
}
bool symbol_definition_env::is_closed_overload() const
{
    return m_symbol_type == closed_overload;
}

void symbol_definition_env::set_def_index(def_index di, long group)
{
    m_def_index     = di;
    m_group_index   = group;
    m_symbol.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    m_symbol.get_symbol_data()->set_definition(di);    
};

}}