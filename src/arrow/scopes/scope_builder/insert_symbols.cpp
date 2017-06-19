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

#include "insert_symbols.h"
#include "semantics/semantics.h"

#include <sstream>

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   insert_operators  
//------------------------------------------------------------------
void insert_operators::eval(const ast::definition&, module&, const error_handler_ptr&)
{};    

void insert_operators::eval(const ast::term_definition&, module&, const error_handler_ptr&)
{};

void insert_operators::eval(const ast::type_definition&, module&, const error_handler_ptr&)
{};

void insert_operators::eval(const ast::type_definition_var&, module&, const error_handler_ptr&)
{};

void insert_operators::eval(const ast::term_use&, module&, const error_handler_ptr&)
{};

void insert_operators::eval(const ast::fixity_definition& def, module& mod, const error_handler_ptr& eh)
{
    const ast::operator_info& fix    = def.get_operator_type();
    const ast::symbol_sequence& syms = def.get_symbols();

    long n_sym                      = syms.size();

    for (long i = 0; i < n_sym; ++i)
    {
        const ast::identifier& v    = syms.get(i);
        mod.add_operator_declaration(v, fix, eh);
    };
};

//------------------------------------------------------------------
//                   insert_symbols  
//------------------------------------------------------------------
//insert global symbols definitions to the module
void insert_symbols::eval(const ast::definition&, module&, const error_handler_ptr&)
{};    

void insert_symbols::get_arg_types(const ast::identifier& id, const ast::type& t, ast::type& body, 
            std::vector<ast::type>& ty_vec, const error_handler_ptr& eh, bool is_extern_func)
{
    body = t;

    for (;;)
    {
        if (body->get_type_code() == ast::details::type_code::type_kind_scope)
        {
            ast::type_kind_scope ks = body.static_cast_to<ast::type_kind_scope>();
            body                    = ks.get_body();
            continue;
        };
        if (body->get_type_code() == ast::details::type_code::arrow_type)
        {
            ast::arrow_type at      = body.static_cast_to<ast::arrow_type>();        
            const ast::type& lhs    = at.get_lhs();
            body                    = at.get_rhs();

            ty_vec.push_back(lhs);            
            continue;
        }
        else if (body->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv        = body.static_cast_to<ast::type_var>();

            if (tv.is_set() == true)
            {
                body                = tv.get();
                continue;
            }

            error::type_error_msg(eh).error_term_constant_not_annotated
                            (id->get_location(), id, t);

            body    = ast::error_type(id->get_location());
            break;
        }
        else if (is_extern_func == true)
        {
            if (body->get_type_code() == ast::details::type_code::all_type)
            {
                error::type_error_msg(eh).error_extern_function_cannot_be_polymorphic
                                (id->get_location(), id, t);
                body    = ast::error_type(id->get_location());
                break;
            }
            else if (body->get_type_code() == ast::details::type_code::some_type)
            {
                error::type_error_msg(eh).error_extern_function_cannot_be_polymorphic
                                (id->get_location(), id, t);
                body    = ast::error_type(id->get_location());
                break;
            }
        };

        break;
    };
};

ast::term insert_symbols::make_extern_def(const ast::term& body0, const ast::term_definition& def,
                            const module& mod, const error_handler_ptr& eh)
{
    //perform decomposition t0 := Kind[args ...] t_body
    std::vector<ast::identifier> args;
    ast::term def_body;
    sema::decompose_term(body0, args, def_body);

    if (def_body->get_term_code() != ast::details::term_code::term_constant)
        return body0;

    ast::term_constant def_tc   = def_body.static_cast_to<ast::term_constant>();

    const ast::identifier& id   = def.get_identifier();
    const ast::type& ty         = def.get_type();

    bool is_extern              = def_tc.is_extern_func();
    ast::term_constant ef       = ast::term_constant(id->get_location(), id, is_extern);        

    ast::type tail;
    std::vector<ast::type> ty_vec;
    get_arg_types(id, ty, tail, ty_vec, eh, is_extern);

    ef.set_return_type(tail);

    if (is_extern == true)
        ef.set_function(mod.get_extern_function_def(eh, id));

    long n          = (long)ty_vec.size();

    ast::term body  = ef;

    using annot_id  = ast::annotated_identifier;

    ast::fixity_type ft         = ast::fixity_type::undefined;

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::ident);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::var);

    //build definition
    for (long i = n-1; i >= 0; --i)
    {
        std::ostringstream os;
        os << "a" << i + 1;

        ast::type t_loc         = ty_vec[i];
        ast::identifier lab_id  = ast::identifier(id->get_location(), ast::unique_string(os.str()),
                                    ft, ik, ast::identifier::fresh());
        annot_id lab            = annot_id(id->get_location(), lab_id, t_loc);
        ast::term ex            = ast::identifier_term(lab_id->get_location(), lab_id);

        ef.add_label(lab, ex);
        
        body     = ast::lambda_term(id->get_location(), lab, body, ast::lambda_term::fresh());
    };

    long nargs  = (long)args.size();

    //add kind scope
    for (long i = nargs-1; i >= 0; --i)
        body     = ast::term_kind_scope(id->get_location(), args[i], body);

    body    = sema::copy_identifiers(body);
    return body;
}

void insert_symbols::eval(const ast::term_definition& def, module& mod, const error_handler_ptr& eh)
{    
    const ast::identifier& sym      = def.get_identifier();
    ast::term body                  = def.get_body();
    const ast::type& t              = def.get_type();
    bool is_overload                = def.is_overload();

    if (is_overload == true && !body)
    {
        mod.add_overload(sym, t, eh);
        return;
    };

    body                            = make_extern_def(body, def, mod, eh);

    arrow::details::symbol_definition_env edef(def->get_location(), t, sym, body, is_overload);
    mod.add_definition(sym, edef, eh);
};

void insert_symbols::eval(const ast::type_definition& def, module& mod, const error_handler_ptr& eh)
{
    mod.add_type_definition(def, eh);
};

void insert_symbols::eval(const ast::type_definition_var& def, module& mod, const error_handler_ptr& eh)
{
    mod.add_type_definition(def, eh);
};

void insert_symbols::eval(const ast::term_use& def, module& mod, const error_handler_ptr& eh)
{
    mod.add_eval_term(def, eh);
};

void insert_symbols::eval(const ast::fixity_definition&, module&, const error_handler_ptr&)
{};

//------------------------------------------------------------------
//                   insert_local_operators
//------------------------------------------------------------------
void insert_local_operators::eval(const ast::definition&, ad::module_index, local_scope&, 
            const error_handler_ptr&)
{};    

void insert_local_operators::eval(const ast::term_definition&, ad::module_index, local_scope&, 
            const error_handler_ptr&)
{};

void insert_local_operators::eval(const ast::type_definition&, ad::module_index, local_scope&, 
            const error_handler_ptr&)
{};

void insert_local_operators::eval(const ast::type_definition_var&, ad::module_index, local_scope&, 
            const error_handler_ptr&)
{};

void insert_local_operators::eval(const ast::term_use&, ad::module_index, local_scope&, 
                                  const error_handler_ptr&)
{};

void insert_local_operators::eval(const ast::fixity_definition& def, ad::module_index mi, 
            local_scope& scope, const error_handler_ptr& eh)
{
    const ast::operator_info& fix    = def.get_operator_type();
    const ast::symbol_sequence& syms = def.get_symbols();

    long n_sym                  = syms.size();

    for (long i = 0; i < n_sym; ++i)
    {
        const ast::identifier& v    = syms.get(i);
        scope.add_operator_declaration(mi, v, fix, eh);
    };
};

//------------------------------------------------------------------
//                   insert_local_functions
//------------------------------------------------------------------
void insert_local_functions::eval(const ast::term_definition& ast, ad::module_index mi, 
            local_scope& scope, const error_handler_ptr& eh)
{
    const ast::identifier& v    = ast.get_identifier();

    scope.add_let_var(mi, v, ast, eh);
};

//type symbol definitions are not allowed in let expression
void insert_local_functions::eval(const ast::type_definition&, ad::module_index, 
            local_scope&, const error_handler_ptr&)
{};
void insert_local_functions::eval(const ast::type_definition_var&, ad::module_index, 
            local_scope&, const error_handler_ptr&)
{};
void insert_local_functions::eval(const ast::term_application&, ad::module_index, 
            local_scope&, const error_handler_ptr&)
{};

void insert_local_functions::eval(const ast::definition&, ad::module_index, 
            local_scope&, const error_handler_ptr&)
{};

void insert_local_functions::eval(const ast::fixity_definition&, ad::module_index, 
            local_scope&, const error_handler_ptr&)
{};

};};};
