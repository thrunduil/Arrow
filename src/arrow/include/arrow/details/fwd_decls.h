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
#include "fwd_decls_ast.h"

#include <memory>

namespace arrow 
{
    class error_handler;    
    using error_handler_ptr = std::shared_ptr<error_handler>;

    class module;
    class local_scope;
    class graph;

    template<class Key>
    class var_graph;    

    enum class term_symbol_kind : int;
    enum class type_symbol_kind : int;
};

namespace arrow { namespace typing
{
    class type_checker;
}};

namespace arrow { namespace typing { namespace details
{
    enum class star_kind_reason : int;
    enum class arrow_kind_reason : int;
}}}


namespace arrow { namespace details
{
    class module_impl;
    class modules_impl;
    
    template<class Key>
    class var_graph_impl;

    template<class Config>
    class overload_set;

    struct overload_term_config;
    struct overload_type_config;

    class graph_impl;
    class module_index;
    class modules_provider;
    class symbol_definition_env;    
    class local_scope_impl;
    class type_var_scope;
    class type_var_scope_impl;
    class type_alias_info;
    struct def_index;
    class scope_index;
    class var_info;
    class type_table;
    class type_comparer;
    class kind_comparer;
}};

namespace arrow { namespace typing { namespace details
{
    template<class Func_arg_type>
    class applic_arg;

    using applic_arg_term   = applic_arg<ast::term>;
    using applic_arg_type   = applic_arg<ast::type>;

    class term_check_vis;
    class kind_inference_context;
}}};

namespace arrow { namespace typing 
{
    class overload_info;
    class type_equaler;
    class bound_map;
    class infered_substitutions;
    class kind_subs_map;
    class inst_collector;
}}

namespace arrow { namespace oper
{
    class operator_declaration;
}}