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
#include "scope_builder.h"
#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   insert_operators  
//------------------------------------------------------------------
//insert global operator declarations to the module
struct insert_operators : public ast::traversal_visitor<insert_operators>
{
    void    eval(const ast::definition&, module&, const error_handler_ptr&);
    void    eval(const ast::term_definition&, module&, const error_handler_ptr&);
    void    eval(const ast::type_definition&, module&, const error_handler_ptr&);
    void    eval(const ast::type_definition_var&, module&, const error_handler_ptr&);
    void    eval(const ast::term_use&, module&, const error_handler_ptr&);
    void    eval(const ast::fixity_definition& def, module& mod, const error_handler_ptr& eh);
};

//------------------------------------------------------------------
//                   insert_symbols  
//------------------------------------------------------------------
//insert global symbols definitions to the module
struct insert_symbols : public ast::traversal_visitor<insert_symbols>
{
    void        eval(const ast::definition&, module&, const error_handler_ptr&);
    void        eval(const ast::term_definition& def, module& mod, const error_handler_ptr& eh);
    void        eval(const ast::type_definition& def, module& mod, const error_handler_ptr& eh);
    void        eval(const ast::type_definition_var& def, module& mod, const error_handler_ptr& eh);
    void        eval(const ast::term_use& def, module& mod, const error_handler_ptr& eh);
    void        eval(const ast::fixity_definition&, module&, const error_handler_ptr&);

    void        get_arg_types(const ast::identifier& id, const ast::type& t, ast::type& body, 
                    std::vector<ast::type>& ty_vec, const error_handler_ptr& eh, 
                    bool is_extern_func);
    ast::term   make_extern_def(const ast::term& body0, const ast::term_definition& def,
                    const module& mod, const error_handler_ptr& eh);

};

//------------------------------------------------------------------
//                   insert_local_operators
//------------------------------------------------------------------
struct insert_local_operators : public ast::traversal_visitor<insert_local_operators>
{
    void eval(const ast::definition&, ad::module_index, local_scope&, 
              const error_handler_ptr&);
    void eval(const ast::term_definition&, ad::module_index, local_scope&, 
              const error_handler_ptr&);
    void eval(const ast::type_definition&, ad::module_index, local_scope&, 
              const error_handler_ptr&);
    void eval(const ast::type_definition_var&, ad::module_index, local_scope&, 
              const error_handler_ptr&);
    void eval(const ast::term_use&, ad::module_index, local_scope&, const error_handler_ptr&);
    void eval(const ast::fixity_definition& def, ad::module_index mi, local_scope& scope,
              const error_handler_ptr& eh);
};

//------------------------------------------------------------------
//                   insert_local_functions
//------------------------------------------------------------------
struct insert_local_functions : public ast::traversal_visitor<insert_local_functions>
{
    void eval(const ast::term_definition& ast, ad::module_index mi, local_scope& scope, 
              const error_handler_ptr& eh);

    void eval(const ast::type_definition&, ad::module_index, local_scope&, const error_handler_ptr&);
    void eval(const ast::type_definition_var&, ad::module_index, local_scope&, const error_handler_ptr&);
    void eval(const ast::term_application&, ad::module_index, local_scope&, const error_handler_ptr&);
    void eval(const ast::definition&, ad::module_index, local_scope&, const error_handler_ptr&);
    void eval(const ast::fixity_definition&, ad::module_index, local_scope&, const error_handler_ptr&);
};

};};};
