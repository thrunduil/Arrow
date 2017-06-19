/* 
 *  This file is a part of Hasky Lib.
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

#include "arrow/config.h"
#include "arrow/ast/ast_visitor.h"
#include "arrow/error_handler.h"

namespace arrow { namespace sema
{

//utility class; change value and restore old scope at exit
template<class T>
class scoped_var
{
    private:
        T&          value_ref;
        T           old_value;

    public:
        scoped_var(T& old, const T& new_value)
            :value_ref(old), old_value(old)
        {
            value_ref = new_value;
        };

        ~scoped_var()
        {
            value_ref = old_value;
        };

        void restore_old_value()
        {
            value_ref = old_value;
        };
};

long    cast_to_long(const ast::location& loc, uint64_t val, 
                         const error_handler_ptr& eh, bool& has_error);
int64_t cast_to_int64(const ast::location& loc, uint64_t val, 
                         const error_handler_ptr& eh, bool& has_error);

// helper to disable compiler warnings
template<class T>
const T identity(const T val)   { return val; };

};};
