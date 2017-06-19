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

#include "term_checker.h"

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              term_or_type
//------------------------------------------------------------------
template<class Func_arg_type>
class applic_arg
{
    public:
        enum item_type
        {
            function_arg, inst_arg, kind_arg
        };

    private:
        Func_arg_type       m_func_arg;
        ast::type           m_inst_arg;
        ast::kind           m_kind_arg;
        item_type           m_item_type;

    public:
        applic_arg();
        static applic_arg   make_func(const Func_arg_type& arg);
        static applic_arg   make_func_arg_type(const ast::type& arg);
        static applic_arg   make_inst(const ast::type& arg);
        static applic_arg   make_kind(const ast::kind& arg);

        item_type           get_item_type() const;
        const ast::location& get_location() const;

        const Func_arg_type&get_func_arg() const;
        const ast::type&    get_func_arg_type() const;
        const ast::type&    get_inst_arg() const;
        const ast::kind&    get_kind_arg() const;

        void                set_func_arg_type(const ast::type& ty);
        void                set_func_arg_kind(const ast::kind& k);
        void                set_inst_arg_kind(const ast::kind& k);
        void                set_func_arg(const Func_arg_type& ty);
        void                set_inst_arg(const ast::type& ty);
};

using applic_arg_term   = applic_arg<ast::term>;
using applic_arg_type   = applic_arg<ast::type>;

};};};