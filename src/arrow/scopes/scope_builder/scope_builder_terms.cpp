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
#include "scope_builder_terms.h"
#include "scope_builder_types.h"
#include "semantics/semantics.h"
#include "semantics/utils.h"
#include "insert_symbols.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_scope_visitor   
//------------------------------------------------------------------
build_scope_visitor::build_scope_visitor(const error_handler_ptr& eh, const ast::location& loc, 
                    const module& mod)
    : m_mod(mod), m_eh(eh), m_bound(mod.get_this_local_scope()), m_loc(loc)
    , m_type_var_scope(mod.new_typevar_scope(loc))
    , m_kind_var_scope(mod.new_kindvar_scope(loc))
{};

void build_scope_visitor::start_expression()
{};

void build_scope_visitor::process_toplevel_signature(const ast::type& sig)
{
    build_type_scope_visitor vis(m_eh, m_mod, m_type_var_scope, m_kind_var_scope);
    vis.visit(sig);
};

//---------------------------------------------------------------
//                      TERMS
//---------------------------------------------------------------
void build_scope_visitor::eval(const ast::term_sequence& ast)
{
    process_sequence<ast::term_sequence, build_scope_visitor>
        vis(m_mod, m_eh, this);

    vis.make(ast);
};

void build_scope_visitor::build_qualified_identifier(const ast::identifier& id, 
                        const module& root_mod)
{
    ast::symbol_data* sym_data  = id.get_symbol_data();
            
    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    std::vector<ast::location> ambig;
    if (root_mod.is_symbol_defined(id, true, ambig) == true)
    {
        if (ambig.size() > 0)
        {
            error::scope_error_msg(m_eh).error_ambiguous_symbol
                    (id->get_location(), id.get_name_simple(), ambig);
        };

        return;
    }

    ast::unique_string vs       = id.get_name_simple();
    error::scope_error_msg(m_eh).error_undefined_variable(vs, id->get_location());
    return;
};

void build_scope_visitor::build_identifier(const ast::identifier& id)
{
    ast::symbol_data* sym_data  = id.get_symbol_data();
            
    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());
}

void build_scope_visitor::eval(const ast::lambda_term& ast)
{
    const ast::term& ex         = ast.get_body();
    ad::module_index mi         = m_mod.get_module_index();
    ast::symbol_data* sym_data  = ast.get_symbol_data();

    //set m_bound to m_bound.new_scope(); at exit restore old scope
    scoped_local_scope sls(m_bound, m_bound.new_scope(m_mod));

    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    const ast::annotated_identifier& v  = ast.get_var();
    const ast::type& t                  = v.get_type();

    m_bound.add_pattern_var(mi, v, m_eh);

    visit_type(t);
    visit(ex);
};

void build_scope_visitor::eval(const ast::mu_term& ast)
{
    const ast::term& ex         = ast.get_body();
    ad::module_index mi         = m_mod.get_module_index();
    ast::symbol_data* sym_data  = ast.get_symbol_data();

    //set m_bound to m_bound.new_scope(); at exit restore old scope
    scoped_local_scope sls(m_bound, m_bound.new_scope(m_mod));

    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    const ast::annotated_identifier& v  = ast.get_var();
    const ast::type& t                  = v.get_type();

    m_bound.add_pattern_var(mi, v, m_eh);

    visit_type(t);
    visit(ex);
};

void build_scope_visitor::eval(const ast::term_kind_scope& ast)
{
    const ast::term& ex         = ast.get_body();
    const ast::identifier& id   = ast.get_var();

    m_kind_var_scope.add_var(m_eh, id);

    visit(ex);
}

void build_scope_visitor::eval(const ast::all_term& ast)
{
    const ast::term& ex     = ast.get_body();
    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);

    const ast::annotated_type_identifier& v  = ast.get_var();
    const ast::identifier& id           = v.get_identifier();
    const ast::kind& ki                 = v.get_kind();

    m_type_var_scope.add_var(m_eh, id);

    visit(ki);
    visit(ex);
};

void build_scope_visitor::eval(const ast::pack_term& ast)
{
    using annot_id      = ast::annotated_type_identifier;
    const annot_id& var = ast.get_var();
    const ast::term& ex = ast.get_body();
    const ast::type& ty = ast.get_var_type();
    const ast::type& st = ast.get_some_type();

    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);

    const ast::identifier& id   = var.get_identifier();
    const ast::kind& ki         = var.get_kind();

    m_type_var_scope.add_var(m_eh, id);

    visit(ki);
    visit(ex);
    visit(ty);
    visit(st);
};

void build_scope_visitor::eval(const ast::overloaded_symbol& ast)
{
    error::scope_error_msg(m_eh).error_overloaded_symbol_not_allowed(ast->get_location());
};

void build_scope_visitor::eval(const ast::identifier& id)
{
    ast::symbol_data* sym_data  = id.get_symbol_data();
            
    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());
};

void build_scope_visitor::eval(const ast::identifier_term& ast)
{
    const ast::identifier& id   = ast.get_identifier();            
    ast::symbol_data* sym_data  = id.get_symbol_data();
            
    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    return;
};

void build_scope_visitor::eval(const ast::variant_case_term& ast)
{
    using annot_id  = ast::annotated_identifier;

    const ast::term& ex         = ast.get_term();
    long n                      = ast.size();
    visit(ex);

    ast::symbol_data* sym_data  = ast.get_symbol_data();
    ad::module_index mi         = m_mod.get_module_index();

    //set m_bound to m_bound.new_scope(); at exit restore old scope
    scoped_local_scope sls(m_bound, m_bound.new_scope(m_mod));

    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    for (long i = 0; i < n; ++i)
    {
        const annot_id& aid1    = ast.get_label(i);
        const ast::term& b1     = ast.get_bind(i);

        m_bound.add_pattern_var(mi, aid1, m_eh);

        visit(aid1);
        visit(b1);
    };
};

void build_scope_visitor::eval(const ast::let_term& ast)
{
    const ast::definitions& defs    = ast.get_definitions();
    const ast::term& ex             = ast.get_body();
    ast::symbol_data* sym_data      = ast.get_symbol_data();
    ad::module_index mi             = m_mod.get_module_index();

    //set m_bound to m_bound.new_scope(); at exit restore old scope
    scoped_local_scope sls(m_bound, m_bound.new_scope(m_mod));

    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    long s          = defs.size();            

    for (long i = 0; i < s; ++i)
    {
        const ast::definition& d = defs.get_definition(i);
        insert_local_operators().visit(d, mi, m_bound, m_eh);
    }

    for (long i = 0; i < s; ++i)
    {
        const ast::definition& d = defs.get_definition(i);
        insert_local_functions().visit(d, mi, m_bound, m_eh);
    }

    visit(ex);

    using fun_sym_map       = local_scope::fun_sym_map;
    const fun_sym_map& loc  = m_bound.get_local_vars();

    for (auto& pos : loc)
    {
        const ast::term_definition& def = pos.second.get_term_definition();
        visit(def);
    };
};

void build_scope_visitor::eval(const ast::term_constant& ast)
{
    long k              = ast.var_size();

    using annot_id  = ast::annotated_identifier;

    for (long i = 0; i < k; ++i)
    {
        const ast::term& t  = ast.get_term(i);
        if (t)
            visit(t);

        const annot_id& aid         = ast.get_label(i);
        const ast::identifier& id   = aid.get_identifier();
        const ast::type& ty         = aid.get_type();

        ast::symbol_data* sym_data  = id.get_symbol_data();            
        sym_data->set_local_scope(m_bound.get_scope_index());
        sym_data->set_module(m_mod.get_module_index());

        visit(ty);
    }

    const ast::type& ret_ty         = ast.get_return_type();
    visit(ret_ty);
};

void build_scope_visitor::eval(const ast::record_term& ast)
{
    //labels are not visible inside record
    long k              = ast.size();

    using annot_id  = ast::annotated_identifier;

    for (long i = 0; i < k; ++i)
    {
        const ast::term& t  = ast.get_term(i);
        visit(t);

        const annot_id& aid         = ast.get_label(i);
        const ast::identifier& id   = aid.get_identifier();
        const ast::type& id_t       = aid.get_type();

        ast::symbol_data* sym_data  = id.get_symbol_data();            
        sym_data->set_local_scope(m_bound.get_scope_index());
        sym_data->set_module(m_mod.get_module_index());

        visit(id_t);
    }
};
        
void build_scope_visitor::eval(const ast::unpack_term& ast)
{
    ad::module_index mi             = m_mod.get_module_index();

    const ast::term& pack           = ast.get_package();
    const ast::term& t              = ast.get_term();
    const auto& aid_type            = ast.get_type_identifier();
    const auto& aid_term            = ast.get_term_identifier();
    ast::symbol_data* sym_data      = ast.get_symbol_data();

    const ast::identifier& id_type  = aid_type.get_identifier();
    const ast::identifier& id_term  = aid_term.get_identifier();

    const ast::kind& id_type_k      = aid_type.get_kind();
    const ast::type& id_term_t      = aid_term.get_type();

    ast::symbol_data* sym_type      = id_type.get_symbol_data();
    ast::symbol_data* sym_term      = id_term.get_symbol_data();

    visit(pack);
    visit(id_type_k);
    visit(id_term_t);

    //set m_bound to m_bound.new_scope(); at exit restore old scope
    scoped_local_scope sls(m_bound, m_bound.new_scope(m_mod));

    sym_type->set_local_scope(m_bound.get_scope_index());
    sym_type->set_module(m_mod.get_module_index());

    sym_term->set_local_scope(m_bound.get_scope_index());
    sym_term->set_module(m_mod.get_module_index());

    sym_data->set_local_scope(m_bound.get_scope_index());
    sym_data->set_module(m_mod.get_module_index());

    m_bound.add_pattern_var(mi, aid_term, m_eh);

    ad::type_var_scope ts2  = m_mod.make_new_var_scope
                                (ast->get_location(), m_type_var_scope);

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, ts2);
    m_type_var_scope.add_var(m_eh, id_type);

    visit(t);
};

//---------------------------------------------------------------
//                      DEFINITIONS
//---------------------------------------------------------------
void build_scope_visitor::eval(const ast::term_definition& ast)
{
    const ast::identifier& v    = ast.get_identifier();
    const ast::type& sig        = ast.get_type();    
    const ast::term& body       = ast.get_body();

    sema::scoped_var<ad::type_var_scope> tvc(m_type_var_scope, 
                            m_type_var_scope.new_var_scope(ast->get_location(), m_mod));
    sema::scoped_var<ad::type_var_scope> kvc(m_kind_var_scope, 
                            m_kind_var_scope.new_var_scope(ast->get_location(), m_mod));

    if (sig)
    {
        build_type_scope_visitor vis(m_eh, m_mod, m_type_var_scope, m_kind_var_scope);
        vis.visit(sig);
    }

    visit(v);

    if (body)
        visit(body);
}

//---------------------------------------------------------------
//                      TYPES
//---------------------------------------------------------------
void build_scope_visitor::visit_type(const ast::type& t)
{
    build_type_scope_visitor vis(m_eh, m_mod, m_type_var_scope, m_kind_var_scope);
    vis.visit(t);
};

//---------------------------------------------------------------
//                      KINDS
//---------------------------------------------------------------
void build_scope_visitor::visit_kind(const ast::kind& t)
{
    build_type_scope_visitor vis(m_eh, m_mod, m_type_var_scope, m_kind_var_scope);
    vis.visit(t);
};

};};};
