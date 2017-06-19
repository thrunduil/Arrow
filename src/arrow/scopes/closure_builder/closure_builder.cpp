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
#include "closure_builder.h"
#include "scopes/module.h"
#include "scopes/local_def_substitution.h"
#include "scopes/local_scope.h"
#include "scopes/graph.h"
#include "semantics/semantics.h"
#include "error/error.h"
#include "scopes/module_impl.h"
#include "scopes/overload_set.h"
#include "scopes/symbol_definition.h"

namespace arrow { namespace sema
{

//------------------------------------------------------------------
//              closure_data_collector
//------------------------------------------------------------------
const closure_data_collector::var_set&
closure_data_collector::get_local_functions() const
{
    return m_local_functions;
};

const closure_data_collector::var_set& 
closure_data_collector::get_pattern_vars() const
{
    return m_pattern_vars;
};

void closure_data_collector::add_referred_var(const ast::identifier id)
{
    m_pattern_vars.insert(id);
};
void closure_data_collector::add_referred_function(const ast::identifier& id)
{
    m_local_functions.insert(id);
};

void closure_data_collector::remove_local_symbols(const local_scope& let_scope,
                                                  const module& mod)
{
    using iterator  = var_set::iterator;

    //process patterns

    iterator end    = m_pattern_vars.end();
    iterator it     = m_pattern_vars.begin();

    for (; it != end;)
    {
        bool is_bound   = let_scope.is_local_symbol_defined(mod, *it);

        if (is_bound == true)
        {
            //this symbol is defined in pattern scopes, must add to closure
            ++it;
            continue;
        }

        //this symbol is bound locally => remove;
        it = m_pattern_vars.erase(it);
    };

    //process functions

    iterator f_end      = m_local_functions.end();
    iterator f_it       = m_local_functions.begin();

    for (; it != end;)
    {
        bool is_bound   = let_scope.is_local_symbol_defined(mod, *it);

        if (is_bound == true)
        {
            //this symbol is defined in pattern scopes, must add to closure
            ++it;
            continue;
        }

        //this symbol is defined locally => remove;
        it = m_local_functions.erase(it);
    };
};

void closure_data_collector::add_local(closure_data_collector& col)
{
    {
        auto beg    = col.m_pattern_vars.begin();
        auto end    = col.m_pattern_vars.end();
        m_pattern_vars.insert(beg, end);
    };

    {
        auto beg    = col.m_local_functions.begin();
        auto end    = col.m_local_functions.end();
        m_local_functions.insert(beg, end);
    };
};

void closure_data_collector::set_import(const var_set& collected)
{
    m_pattern_vars = collected;
};

void closure_data_collector::make_import(const import_func& imp_fun,
                                const var_set*& collected_patterns)
{
    for (const auto& func : m_local_functions)
    {
        const var_set& func_pat = imp_fun(func);

        m_pattern_vars.insert(func_pat.begin(), func_pat.end());
    };

    //referred functions need not be imported since we proceed
    //in topological order

    collected_patterns = &m_pattern_vars;
};

void closure_data_collector::build_closure(ast::closure_term& cl) const
{
    for (const ad::var_info& pat : m_pattern_vars)
    {
        ast::identifier ext_id  = pat.make_var_identifier();
        ast::identifier int_id  = sema::copy(ext_id);
        ast::type ty        = ast::type_var(ext_id->get_location());
        ast::term subs      = ast::identifier_term(ext_id->get_location(), ext_id);
        cl.add_item(subs, int_id, ty);
    };
};

//------------------------------------------------------------------
//              closure_info
//------------------------------------------------------------------
closure_info::closure_info()
{};

closure_info::closure_info(const ast::identifier& id, const closure_data_collector& col,
                           const ast::term& body, ast::closure_kind kind)
    :m_id(id), m_data(col)
{
    m_closure = ast::closure_term(id->get_location(), body, kind);
};

void closure_info::make_import(const import_func& imp_func,
                               const var_set*& collected_patterns)
{
    m_data.make_import(imp_func, collected_patterns);
};
void closure_info::set_import(const var_set& collected)
{
    m_data.set_import(collected);
};

void closure_info::build_func_closure()
{
    m_data.build_closure(m_closure);
};

void closure_info::build_var_closure(ast::closure_term& cl)
{
    m_data.build_closure(cl);
};


//------------------------------------------------------------------
//              closure_map
//------------------------------------------------------------------
void closure_map::register_function(const ast::identifier& id, const local_scope& ls)
{
    m_local_functions.insert(id);

    const overload_set* os = ls.get_overload_set(id);

    if (os == nullptr)
        return;
    if (os->is_overload() == false)
        return;

    const ast::identifier& id_over  = os->get_identifier();

    m_local_functions.insert(id_over);
    m_overloads[id_over] = os;
};

void closure_map::unregister_function(const ast::identifier& id, const local_scope& ls)
{
    m_local_functions.erase(id);

    const overload_set* os = ls.get_overload_set(id);

    if (os == nullptr)
        return;
    if (os->is_overload() == false)
        return;

    const ast::identifier& id_over  = os->get_identifier();

    m_local_functions.erase(id_over);
    m_overloads.erase(id_over);
};

bool closure_map::is_local_function(const ast::identifier& id) const
{
    auto pos = m_local_functions.find(id);
    if (pos == m_local_functions.end())
        return false;
    else
        return true;
};

ast::term closure_map::build_function_closure(const ast::identifier& id, 
                const closure_data_collector& col, const ast::term& ex)
{
    closure_info ci(id, col, ex, ast::closure_kind::function);
    m_func_closures[id] = ci;
    return ci.get_closure();
};

void closure_map::build_overload_closures()
{
    for (const auto & over : m_overloads)
    {
        const ad::var_info& vi      = over.first;
        const overload_set* os      = over.second;

        auto pos = m_func_closures.find(vi);
        if (pos != m_func_closures.end())
            continue;

        closure_data_collector col;

        long n  = os->size();

        for (long i = 0; i < n; ++i)
        {
            using symbol_def        = ad::symbol_definition_env;

            ad::def_location loc;
            const symbol_def* def           = os->get_definition(i, loc);
            const ast::identifier& def_id   = def->get_symbol();

            if (is_local_function(def_id) == false)
                continue;

            col.add_referred_function(def_id);
        };

        ast::identifier id  = vi.make_var_identifier();
        ast::term ex        = ast::identifier_term(id->get_location(), id);

        closure_info ci(id, col, ex, ast::closure_kind::function);
        m_func_closures[vi] = ci;
    };
};

ast::term closure_map::build_var_closure(const ast::identifier& id, const ast::term& def)
{
    ast::closure_term cl(def->get_location(), def, ast::closure_kind::identifier);
    m_var_closures.push_back(id_closure(id, cl));
    return cl;
};

void closure_map::finalize_closures()
{
    //build dependency graph
    var_graph<ad::var_info> graph;
    graph.reserve((long)m_func_closures.size());

    for (const auto& item : m_func_closures)
        graph.add_node(item.first);

    using closure_data  = closure_data_collector;

    for (const auto& item : m_func_closures)
    {
        const ad::var_info& func    = item.first;
        const closure_info& cl      = item.second;
        const closure_data& data    = cl.get_data();
        const var_set& refs         = data.get_local_functions();

        for(const auto& ref : refs)
            graph.add_edge(func, ref);
    };

    //build_connected components
    using component_vec = std::vector<std::vector<ad::var_info>>;
    component_vec components;

    graph.strongly_connected_components(components);

    //add imported pattern vars
    for (const auto& group : components)
        fill_closures(group);

    //build final closures
    build_func_closures();
    build_var_closures();
};

void closure_map::fill_closures(const std::vector<ad::var_info>& group)
{
    if (group.size() == 0)
        return;

    const var_set* collected;

    {
        const ad::var_info& vi  = group[0];    
        fill_closures(vi, collected);
    };

    //this is and error
    if (collected == nullptr)
        return;

    for (size_t i = 1; i < group.size(); ++i)
    {
        const ad::var_info& vi  = group[i];
        set_closures(vi, *collected);
    };
};

void closure_map::fill_closures(const ad::var_info& vi, const var_set*& collected)
{
    auto pos    = m_func_closures.find(vi);
    collected   = nullptr;

    //not possible
    if (pos == m_func_closures.end())
        return;

    using import_func   = closure_data_collector::import_func;

    import_func func    = [this](const ad::var_info& vi) -> const var_set&
                          {return this->get_patterns(vi);};

    closure_info& ci    = pos->second;
    ci.make_import(func, collected);
};

const closure_map::var_set& 
closure_map::get_patterns(const ad::var_info& vi) const
{
    auto pos    = m_func_closures.find(vi);

    //not possible
    if (pos == m_func_closures.end())
    {
        static var_set empty_set;
        return empty_set;
    };

    const closure_info& ci    = pos->second;
    return ci.get_data().get_pattern_vars();
};


void closure_map::set_closures(const ad::var_info& vi, const var_set& collected)
{
    auto pos    = m_func_closures.find(vi);

    //not possible
    if (pos == m_func_closures.end())
        return;

    closure_info& ci    = pos->second;
    ci.set_import(collected);
};

void closure_map::build_func_closures()
{
    for (auto& cl_info : m_func_closures)
    {
        closure_info& ci    = cl_info.second;
        ci.build_func_closure();
    };
};

void closure_map::build_var_closures()
{
    for (auto& item : m_var_closures)
    {
        const ast::identifier& id   = item.first;
        ast::closure_term& cl       = item.second;

        auto pos = m_func_closures.find(id);

        //this is impossible
        if (pos == m_func_closures.end())
            continue;

        pos->second.build_var_closure(cl);
    };
};

//------------------------------------------------------------------
//              closure_builder_vis
//------------------------------------------------------------------
closure_builder_vis::closure_builder_vis(const module& mod, 
                              const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{};

ast::term closure_builder_vis::make(const ast::term& ex)
{
    closure_data_collector col;
    ast::term ret = visit(ex, col);

    m_closure_map.finalize_closures();
    return ret;
};

ast::term closure_builder_vis::eval(const ast::identifier_term& ast, 
                            closure_data_collector& col)
{
    term_symbol_kind tk = m_mod.get_term_symbol_kind(ast);

    const ast::identifier& id   = ast.get_identifier();

    if (tk == term_symbol_kind::pattern_var)
    {
        col.add_referred_var(id);
        return ast;
    }

    if (m_closure_map.is_local_function(id) == true)
    {
        col.add_referred_function(id);
        return m_closure_map.build_var_closure(id, ast);
    }

    return ast;
};

ast::term closure_builder_vis::eval(const ast::closure_term& ast, 
                                    closure_data_collector& col)
{
    (void)col;
    error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                                    "unexpected closure");
    return ast::error_term(ast->get_location());
};

ast::term closure_builder_vis::eval(const ast::overloaded_symbol& ast, 
                                    closure_data_collector& col)
{
    //overloaded symbols are created during type checking
    error::error_base_msg(m_eh).internal_error
        (ast->get_location(), "overloaded symbol should not exist during closure building");

    (void)col;
    return ast;
};

        //types are not modifies => scopes need not be updated
ast::term closure_builder_vis::eval(const ast::variant_case_term& ast, 
                                    closure_data_collector& col)
{            
    return base_type::eval_case(ast, col);
};
ast::term closure_builder_vis::eval(const ast::unpack_term& ast, 
                                    closure_data_collector& col)
{
    return base_type::eval_unpack(ast, col);
};
ast::term closure_builder_vis::eval(const ast::mu_term& ast, 
                                    closure_data_collector& col)
{
    return base_type::eval_mu(ast, col);
};
ast::term closure_builder_vis::eval(const ast::lambda_term& ast, 
                                    closure_data_collector& col)
{
    return base_type::eval_lambda(ast, col);
};

class closure_builder_caller : public local_def_subs_caller
{
    private:
        closure_builder_vis*    m_owner;
        closure_data_collector& m_col;
        local_scope             m_ls;

    public:
        closure_builder_caller(closure_builder_vis* owner, closure_data_collector& col,
                               const local_scope& ls)
            :m_owner(owner), m_col(col), m_ls(ls)
        {};

        virtual void add_name_substitution(const ast::identifier& id, 
                        const ast::identifier& new_id) override
        {
            m_owner->add_name_substitution(id, new_id, m_ls);
        };
        virtual void remove_name_substitution(const ast::identifier& id) override
        {
            m_owner->remove_name_substitution(id, m_ls);
        };
        virtual ast::term make_term(const ast::identifier& id, const ast::term& t,
                                    bool overloaded) override
        {
            return m_owner->make_term(id, t, m_col, m_ls, overloaded);
        };
        virtual ast::type make_type(const ast::identifier& id, const ast::type& t,
                                    bool overloaded) override
        {
            (void)overloaded;
            return m_owner->make_type(id, t);
        };
        virtual ast::type get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors) override
        {
            return m_owner->get_symbol_type(idt, overloaded, has_errors);
        };
        virtual ast::term get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors)
        {
            return m_owner->get_symbol_term(idt, overloaded, has_errors);
        };

        virtual void make_definition(const ast::identifier& id,
                                const ast::term_definition& new_def) override
        {
            return m_owner->make_definition(id, new_def, m_col);
        };
};

ast::term closure_builder_vis::eval(const ast::let_term& ast, closure_data_collector& col)
{
    const ast::term& ex         = ast.get_body();
    const ast::definitions& def = ast.get_definitions();

    local_scope ls              = m_mod.get_scope_for_let(ast);

    closure_builder_caller caller(this, col, ls);
    local_def_substitution loc_defs(&caller, def->get_location());
    ast::definitions def2       = loc_defs.process_definitions(def);

    // we can use the same collector
    ast::term ex2               = make_body(ex, col);

    m_closure_map.build_overload_closures();
    loc_defs.finish();    

    ast::let_term ret(ast->get_location(), def2, ex2, ast.get_symbol_data());
    ret.update_scope(m_mod); 
    return ret;
};

ast::type closure_builder_vis::get_symbol_type(const ast::identifier_term& idt, 
                                bool& overloaded, bool& has_errors)
{
    return m_mod.get_symbol_type(m_eh, idt, overloaded, has_errors);
};

ast::term closure_builder_vis::get_symbol_term(const ast::identifier_term& idt, 
                                bool& overloaded, bool& has_errors)
{
    return m_mod.get_symbol_term(m_eh, idt, overloaded, has_errors);
};

void closure_builder_vis::add_name_substitution(const ast::identifier& id, 
                const ast::identifier& new_id, const local_scope& ls)
{
    (void)new_id;
    m_closure_map.register_function(id, ls);
};

void closure_builder_vis::remove_name_substitution(const ast::identifier& id, 
                                                   const local_scope& ls)
{
    m_closure_map.unregister_function(id, ls);
};

ast::type closure_builder_vis::make_type(const ast::identifier& id, const ast::type& t)
{
    (void)id;
    return t;
}

ast::term closure_builder_vis::make_body(const ast::term& t, closure_data_collector& col)
{
    return visit(t, col);
};

ast::term closure_builder_vis::make_term(const ast::identifier& id, const ast::term& t,
                                closure_data_collector& col, const local_scope& let_scope,
                                bool overloaded)
{
    (void)overloaded;
    closure_data_collector col_loc;
    ast::term ex    = visit(t, col_loc);

    col_loc.remove_local_symbols(let_scope, m_mod);

    ast::term ret   = m_closure_map.build_function_closure(id, col_loc, ex);

    col.add_local(col_loc);

    return ret;
};

void closure_builder_vis::make_definition(const ast::identifier& id,
                const ast::term_definition& new_def, closure_data_collector& col)
{
    (void)col;

    //do not build a new identifier;
    const ast::identifier& new_id = new_def.get_identifier();
    new_id.get_symbol_data()->set_definition(id.get_symbol_data()->get_def_index());    
};

//------------------------------------------------------------------
//              closure_builder
//------------------------------------------------------------------
closure_builder::closure_builder(const module& mod, const error_handler_ptr& eh)
    :m_mod(mod), m_eh(eh)
{};

ast::term closure_builder::make(const ast::term& ex)
{
    closure_builder_vis vis(m_mod, m_eh);
    return vis.make(ex);
};

};};