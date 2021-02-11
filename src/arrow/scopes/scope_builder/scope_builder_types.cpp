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
#include "scope_builder_types.h"
#include "scopes/type_var_scope.h"
#include "semantics/semantics.h"
#include "semantics/utils.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_type_scope_visitor   
//------------------------------------------------------------------
build_type_scope_visitor::build_type_scope_visitor(const error_handler_ptr& eh, 
            const module& mod, const ad::type_var_scope& type_scope, 
            const ad::type_var_scope& kind_scope)
    : m_mod(mod), m_eh(eh), m_type_var_scope(type_scope)
    , m_kind_var_scope(kind_scope)
{};

//---------------------------------------------------------------
//                      TYPES
//---------------------------------------------------------------
void build_type_scope_visitor::eval(const ast::type_kind_scope& ast)
{
    using annot_ident       = ast::annotated_type_identifier;

    const ast::identifier& id   = ast.get_var();
    const ast::type& body       = ast.get_body();

    m_kind_var_scope.add_var(m_eh, id);
    visit(body);
}

void build_type_scope_visitor::eval(const ast::all_type& ast)
{
    using annot_ident       = ast::annotated_type_identifier;
    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);

    const annot_ident& id       = ast.get_var();
    const ast::identifier& var  = id.get_identifier();
    const ast::type& body       = ast.get_body();
    const ast::kind& ki         = id.get_kind();

    m_type_var_scope.add_var(m_eh, var);

    visit(ki);
    visit(body);
};

void build_type_scope_visitor::eval(const ast::some_type& ast)
{
    using annot_ident       = ast::annotated_type_identifier;
    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);

    const annot_ident& id       = ast.get_var();
    const ast::identifier& var  = id.get_identifier();
    const ast::type& body       = ast.get_body();
    const ast::kind& ki         = id.get_kind();

    m_type_var_scope.add_var(m_eh, var);
            
    visit(ki);
    visit(body);
};

void build_type_scope_visitor::eval(const ast::mu_type& ast)
{
    using annot_ident       = ast::annotated_type_identifier;
    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);
    const annot_ident& id       = ast.get_var();

    const ast::identifier& var  = id.get_identifier();
    const ast::type& body       = ast.get_body();
    const ast::kind& ki         = id.get_kind();

    m_type_var_scope.add_var(m_eh, var);
            
    visit(ki);
    visit(body);
};

void build_type_scope_visitor::eval(const ast::lambda_type& ast)
{
    using annot_ident       = ast::annotated_type_identifier;
    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);
    const annot_ident& id       = ast.get_var();

    const ast::identifier& var  = id.get_identifier();
    const ast::type& body       = ast.get_body();
    const ast::kind& ki         = id.get_kind();

    m_type_var_scope.add_var(m_eh, var);
            
    visit(ki);
    visit(body);
};

void build_type_scope_visitor::eval(const ast::identifier_type& ast)
{
    const ast::identifier& ti   = ast.get_identifier();
    eval_type_identifier(ti);
}

void build_type_scope_visitor::eval(const ast::type_constant& ast)
{
    const ast::identifier& id   = ast.get_name();
    m_mod.build_fresh_type_variable(id);

    base_type::eval(ast);
};

void build_type_scope_visitor::eval(const ast::type_sequence& ast)
{
    process_sequence<ast::type_sequence, build_type_scope_visitor> 
        vis(m_mod, m_eh, this);

    vis.make(ast);
};

void build_type_scope_visitor::build_qualified_identifier(const ast::identifier& id,
        const module& qualified_mod)
{
    if (id.get_symbol_data()->has_definition())
        return;

    // qualified identifier must be fully resolved
    ast::fixity_type ft;
    ast::unique_string name = id.get_name(ft);
    ast::location loc;
    bool found;
    std::vector<ast::location> ambig;

    ad::def_index di    = qualified_mod.get_type_definition_index
                            (name, true, loc, found, ambig);

    if (found == true && ambig.size() == 0)
    {
        id.get_symbol_data()->get_kind().set_sector_type().set_function_def();
        id.get_symbol_data()->set_definition(di);
        return;
    };

    if (found == true)
    {
        error::scope_error_msg(m_eh).error_ambiguous_type_symbol
                                (id->get_location(), name, ambig);
        return;
    }
    else
    {
        error::scope_error_msg(m_eh).error_undefined_type_symbol
                                (id->get_location(), name);
        return;
    }
};

void build_type_scope_visitor::build_identifier(const ast::identifier& id)
{
    eval_type_identifier(id);
}

void build_type_scope_visitor::eval_type_identifier(const ast::identifier& ti)
{
    if (ti.get_symbol_data()->has_definition())
        return;

    bool set = m_type_var_scope.set_definition_info(ti);

    if (set == true)
        return;
};

//---------------------------------------------------------------
//                      KINDS
//---------------------------------------------------------------        
void build_type_scope_visitor::eval(const ast::kind_scope& ast)
{
    const ast::identifier& id   = ast.get_var();
    const ast::kind& body       = ast.get_body();

    m_kind_var_scope.add_var(m_eh, id);

    visit(body);
};

void build_type_scope_visitor::eval(const ast::kind_identifier& ast)
{
    const ast::identifier& ki   = ast.get_identifier();

    if (ki.get_symbol_data()->has_definition())
        return;

    bool set = m_kind_var_scope.set_definition_info(ki);

    if (set == true)
        return;

    error::scope_error_msg(m_eh).error_undefined_kind_symbol
                            (ki->get_location(), ki.get_name_simple());
    return;
};

};};};
