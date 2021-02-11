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

#include "arrow/config.h"
#include "arrow/error_handler.h"
#include "arrow/ast/ast.h"

#include <string>

namespace arrow
{

class literal_parser
{
    private:
        using location  = ast::location;

    private:
        error_handler_ptr   m_eh;
        location            m_loc;

    private:
        void            report_error(const std::string& msg);
        void            report_warning(const std::string& msg);

        bool            horner_mul(uint64_t& value, unsigned base, char next);
        void            check_code_point(int32_t codePoint, const ast::location& loc);
        void            check_text_literal(const std::string& literal, const ast::location& loc);
        void            unescape_escape_sequence(std::string& literal, int32_t& i, int32_t& o,
                            const ast::location& loc);
        void            unescape_uescape_sequence(std::string &literal, int32_t &i, int32_t &o,
                            const ast::location& loc);

        void            unescape(std::string& literal, const ast::location& loc);

    public:        
        literal_parser(const error_handler_ptr& err_handler, const location& loc);

        int32_t         parse_char(std::string s);
        std::string     parse_string(std::string s);
        double          parse_float(const std::string& literal);
        uint64_t        parse_int(const std::string& s);
};

};
