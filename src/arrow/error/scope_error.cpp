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

#include "error.h"
#include "error_formatter.h"
#include "scopes/module.h"

#include <sstream>

namespace arrow { namespace error
{

scope_error_msg::scope_error_msg(const error_handler_ptr& eh)
    :error_base_msg(eh)
{};

void scope_error_msg::error_undefined_import(const ast::location& loc, ast::unique_string name)
{
    error_formatter ef;
    ef.head() << "undefined import module " << name.get_string();
    m_eh->report_parser_error(loc, ef.str());
}
void scope_error_msg::error_cyclical_dependencies_module(const std::vector<ast::unique_string>& cycle)
{
    error_formatter ef;
    ef.head() << "cyclical dependencies between modules:";

    ef.new_line();
        size_t k = cycle.size();

        for (size_t i = 0; i < k; ++i)
        {
            if (i > 0)
                ef.line() << ", ";
            ef.line() << cycle[i].get_string();
        }

    m_eh->report_parser_error(ast::location(), ef.str());
}

void scope_error_msg::error_symbol_defined(ast::unique_string name, const ast::location& loc, 
                        const ast::location& old_loc)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string() << " is already defined in " << old_loc.to_string(false);
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_type_defined(ast::unique_string name, const ast::location& loc, 
            const ast::location& old_loc)
{
    error_formatter ef;
    ef.head() << "type " << name.get_string() << " is already defined in " << old_loc.to_string(false);
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_global_symbol(ast::unique_string name, const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string() << " has global definition";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_undefined_type_symbol(const ast::location& loc, ast::unique_string name)
{
    error_formatter ef;
    ef.head() << "undefined type symbol ";
    ef.head() << name.get_string();

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_undefined_kind_symbol(const ast::location& loc, ast::unique_string name)
{
    error_formatter ef;
    ef.head() << "undefined kind symbol ";
    ef.head() << name.get_string();

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_ambiguous_type_symbol(const ast::location& loc, ast::unique_string name,
            std::vector<ast::location>& declarations)
{
    error_formatter ef;
    ef.head() << "type symbol " << name.get_string();
    ef.head() << " is ambiguous";
        
    auto l1 = ef.new_info(); 
        ef.line() << "existing definitions:";

        print_locations(ef, declarations);

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_ambiguous_symbol(const ast::location& loc, ast::unique_string name,
            std::vector<ast::location>& declarations)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string();
    ef.head() << " is ambiguous";
        
    auto l1 = ef.new_info(); 
        ef.line() << "existing definitions:";

        print_locations(ef, declarations);

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_type_symbol_already_bound(const ast::location& loc, ast::unique_string name,
            const ast::location& prev_bind)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string();
    ef.head() << " is already bound in " << prev_bind.to_string(false);

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_undefined_variable(ast::unique_string name, const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string();
    
    ef.head() << " is undefined";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::warning_symbol_hidden(ast::unique_string name, const ast::location& new_pos, 
                                      const ast::location& old_pos)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string() << " is already defined in " << old_pos.to_string(true);
    ef.new_info();
        ef.line() << "old definition will be inaccessible";
    m_eh->report_parser_warning(new_pos, ef.str());
};

void scope_error_msg::error_function_definition_without_body(const ast::location& loc, ast::unique_string name)
{
    error_formatter ef;
    ef.head() << "symbol " << name.get_string() << " must have definition";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_kind_scope_not_allowed(const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "kind scope is allowed only at top level of definitions";
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_closure_term_not_allowed(const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "closure term is not allowed in parse tree";
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_overloaded_symbol_not_allowed(const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "overloaded symbol node not allowed in parse tree";
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_module_in_expression(const ast::location& loc, const ast::identifier& id)
{
    error_formatter ef;
    ef.head()   << "module symbol " << id.get_name_simple().get_string() 
                << " cannot be used in expressions";
    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_module_symbol_used(const ast::location& loc, const ast::identifier& id)
{
    error_formatter ef;
    ef.head()   << "symbol " << id.get_name_simple().get_string() 
                << " cannot be used";
    ef.new_info();
    ef.line()   << "symbol is a name of module or an import alias";

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_invalid_imported_module_name(const ast::location& loc, ast::unique_string mod_name,
            const import_map& imports)
{
    error_formatter ef;
    ef.head() << "invalid imported module name: " << get_module_name(mod_name);

    auto l1 = ef.new_info();
        ef.line() << "imported modules:";

        ef.new_line(l1);
        print_imports(ef.line(), imports);

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::warning_unused_let_definition(ast::unique_string name, const ast::location& pos)
{
    error_formatter ef;
    ef.head() << "unused definition of " << name.get_string() << " in let expression";
    m_eh->report_parser_warning(pos, ef.str());
};

void scope_error_msg::error_cell_term_not_allowed_parse(const ast::location& loc)
{
    error_formatter ef;

    ef.head() << "cell is not allowed in a parse tree";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_unable_find_import_def(const ast::location& loc, const std::string& mod_name)
{
    error_formatter ef;

    ef.head() << "unable to load module " << mod_name;

    ef.new_info();
    ef.line() << "module definition not found";

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_underscore_not_allowed(const ast::location& loc)
{
    error_formatter ef;

    ef.head() << "underscore is not allowed";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_type_var_not_allowed(const ast::location& loc)
{
    error_formatter ef;

    ef.head() << "full type annotation is required";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_type_constant_outside_type_definition(const ast::location& loc)
{
    error_formatter ef;

    ef.head() << "type constant is outside of type definition in a parse tree";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_term_constant_outside_term_definition(const ast::location& loc)
{
    error_formatter ef;

    ef.head() << "term constant is outside of term definition in a parse tree";
    m_eh->report_parser_error(loc, ef.str());
}

void scope_error_msg::error_extern_function_cannot_be_local(const ast::location& loc, const ast::identifier& id)
{
    error_formatter ef;

    ef.head() << "local function " << id.get_name_simple().get_string() << " cannot be extern";

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_extern_function_not_defined(const ast::location& loc, const ast::identifier& id)
{
    error_formatter ef;

    ef.head() << "extern function " << id.get_name_simple().get_string() << " not defined";

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_extern_type_function_not_defined(const ast::location& loc, const ast::identifier& id)
{
    error_formatter ef;

    ef.head() << "extern type function " << id.get_name_simple().get_string() << " not defined";

    m_eh->report_parser_error(loc, ef.str());
};

void scope_error_msg::error_unexpected_filled_type_var(const ast::location& loc, 
                            const ast::type_var& ty)
{
    error_formatter ef;

    ef.head() << "type variable is filled; expecting a fresh type variable or a type";

    auto l1 = ef.new_info();
        ef.line() << "type is:";
        ef.new_line(l1);
            print_type(ef.line(), ty);

    m_eh->report_parser_error(loc, ef.str());
};

}}
