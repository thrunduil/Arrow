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

#include "arrow/ast/ast.h"

namespace arrow 
{

class extern_func_provider
{
    public:
        using extern_function_ptr   = std::shared_ptr<ast::extern_function_eval>;
        using extern_type_ptr       = std::shared_ptr<ast::extern_type_function_eval>;

    public:
        static extern_function_ptr  error_function();
        static extern_type_ptr      error_type_function();

        extern_function_ptr         get(ast::unique_string id) const;
        extern_type_ptr             get_type(ast::unique_string id) const;
};

};
