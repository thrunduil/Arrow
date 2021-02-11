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

#include "store.h"
#include <stdexcept>

namespace arrow { namespace codegen
{

static std::vector<ast::term> gl_store;

long store::make_cell(const ast::term& content)
{
    long index = (long)gl_store.size();
    gl_store.push_back(content);
    return index;
};

ast::term store::deref_cell(long index)
{
    if (index < 0 || index >= (long)gl_store.size())
        throw std::runtime_error("invalid interpret: invalid cell index");
    return gl_store[index];
};

void store::update_cell(long index, const ast::term& new_content)
{
    if (index < 0 || index >= (long)gl_store.size())
        throw std::runtime_error("invalid interpret: invalid cell index");

    gl_store[index] = new_content;
};

void store::free_memory()
{
    gl_store = std::vector<ast::term>();
};

};};