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

#include "term_check_helpers.h"

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              applic_arg
//------------------------------------------------------------------
template<class Func_arg_type>
applic_arg<Func_arg_type>::applic_arg()
    :m_item_type(function_arg)
{};

template<class Func_arg_type>
applic_arg<Func_arg_type> 
applic_arg<Func_arg_type>::make_func(const Func_arg_type& arg)
{
    applic_arg ret;
    ret.m_item_type = function_arg;
    ret.m_func_arg  = arg;
    return ret;
};

template<class Func_arg_type>
applic_arg<Func_arg_type> 
applic_arg<Func_arg_type>::make_func_arg_type(const ast::type& arg)
{
    applic_arg ret;
    ret.m_item_type = function_arg;
    ret.m_inst_arg  = arg;
    return ret;
}

template<class Func_arg_type>
applic_arg<Func_arg_type> 
applic_arg<Func_arg_type>::make_inst(const ast::type& arg)
{
    applic_arg ret;
    ret.m_item_type = inst_arg;
    ret.m_inst_arg  = arg;
    return ret;
};

template<class Func_arg_type>
applic_arg<Func_arg_type> 
applic_arg<Func_arg_type>::make_kind(const ast::kind& arg)
{
    applic_arg ret;
    ret.m_item_type = kind_arg;
    ret.m_kind_arg  = arg;
    return ret;
};

template<class Func_arg_type>
typename applic_arg<Func_arg_type>::item_type
applic_arg<Func_arg_type>::get_item_type() const
{
    return m_item_type;
};

template<class Func_arg_type>
const ast::location& applic_arg<Func_arg_type>::get_location() const
{
    if (m_func_arg)
        return m_func_arg->get_location();
    else if (m_inst_arg)
        return m_inst_arg->get_location();
    else
        return m_kind_arg->get_location();
};

template<class Func_arg_type>
const Func_arg_type& applic_arg<Func_arg_type>::get_func_arg() const
{
    return m_func_arg;
}

template<class Func_arg_type>
const ast::type& applic_arg<Func_arg_type>::get_func_arg_type() const
{
    return m_inst_arg;
}

template<class Func_arg_type>
const ast::type& applic_arg<Func_arg_type>::get_inst_arg() const
{
    return m_inst_arg;
}

template<class Func_arg_type>
const ast::kind& applic_arg<Func_arg_type>::get_kind_arg() const
{
    return m_kind_arg;
}

template<class Func_arg_type>
void applic_arg<Func_arg_type>::set_func_arg_type(const ast::type& ty)
{
    m_inst_arg = ty;
}

template<class Func_arg_type>
void applic_arg<Func_arg_type>::set_func_arg(const Func_arg_type& ty)
{
    m_func_arg = ty;
}

template<class Func_arg_type>
void applic_arg<Func_arg_type>::set_inst_arg(const ast::type& ty)
{
    m_inst_arg = ty;
}

template<class Func_arg_type>
void applic_arg<Func_arg_type>::set_func_arg_kind(const ast::kind& k)
{
    m_kind_arg = k;
}

template<class Func_arg_type>
void applic_arg<Func_arg_type>::set_inst_arg_kind(const ast::kind& k)
{
    m_kind_arg = k;
}

template class applic_arg<ast::type>;
template class applic_arg<ast::term>;

}}}
