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

#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "root/options.h"

#include <map>

#pragma warning (push)
#pragma warning(disable: 4127)  // conditional expression is constant

namespace arrow { namespace ast
{

//--------------------------------------------------------------------------
//                          ast_base
//--------------------------------------------------------------------------
template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>::ast_base()
{};

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>::ast_base(Impl* ptr)
    :m_value(ptr)
{};

template<class Derived, class Impl, class Mark>
template<class S>
ast_base<Derived,Impl,Mark>::ast_base(const details::refcount<S>& ptr)
    :m_value(ptr)
{}

template<class Derived, class Impl, class Mark>
template<class S>
ast_base<Derived,Impl,Mark>::ast_base(details::refcount<S>&& ptr)
    :m_value(ptr)
{}

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>::ast_base(const ast_base& other)
    :m_value(other.m_value)
{};

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>::ast_base(ast_base&& other)
    :m_value(std::move(other.m_value))
{};

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>& ast_base<Derived,Impl,Mark>::operator=(const ast_base& other)
{
    m_value = other.m_value;
    return *this;
}

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>& ast_base<Derived,Impl,Mark>::operator=(ast_base&& other)
{
    m_value = std::move(other.m_value);
    return *this;
}

template<class Derived, class Impl, class Mark>
ast_base<Derived,Impl,Mark>::~ast_base()
{};

//---------------------------------------------------------------
//                      utils
//---------------------------------------------------------------
template<class Ast>
void check_null(const Ast& ast)
{
    if (arrow::details::options::debug_ast == true)
    {
        if (!ast)
            throw std::runtime_error("ast not initialized");
    };
}

void check_null(unique_string str)
{
    if (arrow::details::options::debug_ast == true)
    {
        if (!str)
            throw std::runtime_error("unique_string not initialized");
    };
}

template<class Container>
void check_pos(const Container& cont, long pos)
{
    if (arrow::details::options::debug_ast == true)
    {
        if (pos >= (long)cont.size() || pos < 0)
            throw std::runtime_error("invalid index");
    };
}

};};

#pragma warning (pop)
