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

#include "semantics/utils.h"
#include "error/error.h"

namespace arrow { namespace sema
{

long sema::cast_to_long(const ast::location& loc, uint64_t val, 
                         const error_handler_ptr& eh, bool& has_error)
{
    long ret    = (long)val;
    has_error   = false;

    if ((uint64_t) ret != val || ret < 0)
    {
        error::literal_error_msg(eh).error_integer_literal_is_too_big(loc, val);
        has_error = true;
        return 0;
    }

    return ret;
}

int64_t sema::cast_to_int64(const ast::location& loc, uint64_t val, 
                         const error_handler_ptr& eh, bool& has_error)
{
    int64_t ret = (int64_t)val;
    has_error   = false;

    if ((uint64_t) ret != val || ret < 0)
    {
        error::literal_error_msg(eh).error_integer_literal_is_too_big(loc, val);
        has_error = true;
        return 0;
    }

    return ret;
}

};};
