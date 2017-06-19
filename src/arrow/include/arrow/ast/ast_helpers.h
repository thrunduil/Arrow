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
#include "arrow/details/fwd_decls.h"
#include "arrow/details/ast_base.h"

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      UTILS
//---------------------------------------------------------------

class ARROW_EXPORT unique_string
{
    private:
        const std::string*  m_value;

    public:
        inline unique_string();

        explicit unique_string(const std::string& str);

        inline unique_string(const unique_string&);

        inline unique_string&
                            operator=(const unique_string&);

        explicit inline     operator bool() const;
        const std::string&  get_string() const;

        //pointer based comparison
        inline bool         operator==(unique_string other) const;
        inline bool         operator!=(unique_string other) const;
        inline bool         operator>=(unique_string other) const;
        inline bool         operator<=(unique_string other) const;
        inline bool         operator>(unique_string other) const;
        inline bool         operator<(unique_string other) const;
};

class ARROW_EXPORT location
{
    private:
        long                m_line;
        long                m_column;
        unique_string       m_file;

    public:
        location();
        location(long line, long col, unique_string file);

        long                get_line() const        { return m_line; };
        long                get_column() const      { return m_column; };
        unique_string       get_file() const        { return m_file; };

        inline bool         has_location() const;

        void                print(std::ostream& os, bool disp_file = true) const;
        std::string         to_string(bool disp_file = true) const;

        bool                operator==(const location& other) const;
        bool                operator!=(const location& other) const;
        bool                operator<(const location& other) const;
};

//---------------------------------------------------------------
//                      ENUMS
//---------------------------------------------------------------
enum class predefined_type_code
{
    type_bool,      type_int,   type_float,
    type_string,    type_char,
};

enum class value_kind_code
{
    int_kind,       bool_kind,  char_kind,  string_kind
};

enum class polarity_type
{
    positive,   negative,   neutral
};

};};

#pragma warning(pop)
