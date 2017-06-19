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

#include "interpreter.h"
#include "interpret_vis.h"
#include "semantics/semantics.h"

#include <iostream>

namespace arrow { namespace codegen
{

//------------------------------------------------------------------
//              interpreter
//------------------------------------------------------------------
interpreter::interpreter(module& mod, const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{};

ast::term interpreter::make(const ast::term& t)
{
    interpreter_vis vis(m_eh, m_mod);

    ast::term eval_t    = t;
    bool progress       = true;

    while (progress == true)
    {
        //sema::print_term(std::cout, eval_t, m_eh, m_mod); std::cout << "\n";

        if (vis.is_value(eval_t) == true)
            return eval_t;

        eval_t = vis.make(eval_t, progress);
    };

    return eval_t;
}

};};