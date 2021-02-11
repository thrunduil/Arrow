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
#include "arrow/ast/ast.h"
#include "arrow/error_handler.h"
#include "arrow/details/fwd_decls.h"

#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable:4251)   //needs to have dll-interface to be used by clients

namespace arrow
{

class ARROW_EXPORT modules
{
    private:
        using impl_type     = details::modules_impl;
        using impl_ptr      = std::shared_ptr<impl_type>;

    private:
        impl_ptr            m_impl;

    public:
        modules(const error_handler_ptr& eh, bool in_shell);

        //------------------------------------------------------------------
        //                      PARSING
        //------------------------------------------------------------------
        /// add directory to lib paths
        void                    add_lib_path(const std::string& dir);

        /// parse given file
        ast::module_definition  parse_module(const std::string& path);

        /// parse given stream
        ast::module_definition  parse_module(std::istream& os, const std::string& path_long,
                                    const std::string& path_short);

        //------------------------------------------------------------------
        //                      SHELL
        //------------------------------------------------------------------
        void                    eval_term(const std::string& command, bool& need_input);
        void                    print_type(const std::string& command, bool& need_input);
        void                    print_kind(const std::string& command, bool& need_input);
        void                    print_help(const std::string& command, bool& need_input);
        void                    clear_definitions();
        void                    process_commands(const std::string& command, 
                                    bool& need_more_input);

        //------------------------------------------------------------------
        //                      SEMANTICS
        //------------------------------------------------------------------
        /// link modules, build scopes, type check, evaluate
        void                process_modules(std::vector<ast::term>& evaled);
};

};

#pragma warning(pop)