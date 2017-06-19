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
#include "arrow/ast/ast.h"
#include "arrow/error_handler.h"
#include "arrow/details/fwd_decls.h"

#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable:4251)   //needs to have dll-interface to be used by clients

namespace arrow
{

class ARROW_EXPORT parser
{
    private:
        error_handler_ptr       m_eh;
        int                     m_tab_size;

    public:
        parser(const error_handler_ptr& eh);

        /// parse top level definitions from given stream; path is
        /// used to create ast::location
        ast::module_definition  parse_module(std::istream& os, const std::string& path);
        ast::module_definition  parse_console_input(const std::string& os, const std::string& path,
                                    long& line_num, bool& need_more_input);

        /// parse a term; location loc is the starting position
        ast::term               parse_term(const std::string& syntax, const ast::location& loc);

        /// parse a type; location loc is the starting position
        ast::type               parse_type(const std::string& syntax, const ast::location& loc);

        /// parse a kind; location loc is the starting position
        ast::kind               parse_kind(const std::string& syntax, const ast::location& loc);

        /// parse an identifier
        ast::identifier         parse_identifier(const std::string& syntac, const ast::location& loc);
};

};

#pragma warning(pop)