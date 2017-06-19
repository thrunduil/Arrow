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
#include "error.h"
#include "semantics/semantics.h"
#include "error_formatter.h"

#include <sstream>

namespace arrow { namespace error
{

error_base_msg::error_base_msg(const error_handler_ptr& eh)
    :m_eh(eh)
{};

void error_base_msg::internal_error(const ast::location& loc, const std::string& reason)
{
    error_formatter ef;
    ef.head() << "internal error: " << reason;

    m_eh->report_parser_error(loc, ef.str());
};

void error_base_msg::print_locations(error_formatter& ef, const std::vector<ast::location>& locs)
{
    for (const auto& loc : locs)
    {
        ef.new_line();
        ef.line() << loc.to_string() << "\n";
    }
};

void error_base_msg::print_kind(std::ostream& os, const ast::kind& k)
{
    sema::print_kind(os, k);
};

void error_base_msg::print_type(std::ostream& os, const ast::type& t)
{
    sema::print_type(os, t);
};

void error_base_msg::print_predefined_type(std::ostream& os, ast::predefined_type_code tc)
{
    sema::print_predefined_type(os, tc);
};

void error_base_msg::print_identifiers(std::ostream& os, const std::vector<ast::identifier>& id_vec)
{
    for (size_t i = 0; i < id_vec.size(); ++i)
    {
        if (i > 0)
            os << ", ";
        
        os << id_vec[i].get_name_simple().get_string();
    }
}

void error_base_msg::print_vector(std::ostream& os, const std::vector<std::string>& vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        os << vec[i];
    }
}

std::string error_base_msg::get_module_name(ast::unique_string mod_name)
{
    if (mod_name.get_string() == "")
        return "[unnamed]";
    else
        return mod_name.get_string();
}

void error_base_msg::print_imports(std::ostream& os, const import_map& imports)
{
    if (imports.size() == 0)
    {
        os << "[empty]";
        return;
    };

    bool add_sep = false;

    for (const auto& elem : imports)
    {
        if (add_sep == true)
            os << ", ";

        os << get_module_name(elem.first);
        add_sep = true;
    };
};

}}
