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

#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"

#include <set>
#include <map>

namespace arrow { namespace details
{

class unique_names_provider
{
    private:
        using string_count  = std::map<std::string, long>;
        using string_set    = std::set<ast::unique_string>;
        using var_map       = std::map<var_info, ast::identifier>;
        
    private:
        string_count        m_string_count;
        string_set          m_string_set;
        var_map             m_var_map;

        unique_names_provider();

    public:
        static unique_names_provider&
                            get();

        std::string         get_base_name(const std::string& str);
        ast::identifier     get_unique_name(const ast::identifier& old, const std::string& base);
        ast::identifier     get_mapping(const ast::identifier& old, bool& has);

        void                clear();

    private:
        ast::unique_string  get_unique_name(const std::string& base, long& trial);
        std::string         to_string(long val) const;
        long&               get_count(const std::string& name);
};

}}
