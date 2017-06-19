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

#include "semantics.h"
#include "scopes/scope_builder/scope_builder.h"
#include "semantics/typing/terms/type_checker.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "semantics/utils.h"
#include "root/options.h"
#include "semantics/typing/types/computation_on_types.h"
#include "make_fresh_binders_type.h"
#include "unique_names_provider.h"
#include "semantics/typing/types/normal_form_type.h"

#include "scopes/closure_builder/closure_builder.h"
#include "desugaring.h"
#include "codegen/interpreter.h"

#include <map>
#include <sstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4127)  //C4127: conditional expression is constant

namespace arrow { namespace sema
{

namespace ad = arrow::details;
namespace atd = arrow::typing::details;

//---------------------------------------------------------------
//                  TOP LEVEL FUNCTIONS
//---------------------------------------------------------------
void sema::collect_symbol_definitions(const ast::definitions& ast, const module& mod,
                        const error_handler_ptr& eh)
{
    details::scope_builder dg(mod, eh);
    dg.collect_symbol_definitions(ast);
};

void sema::collect_operator_declarations(const ast::definitions& ast, const module& mod,
                        const error_handler_ptr& eh)
{
    details::scope_builder dg(mod, eh);
    dg.collect_operators(ast);
};

void sema::build_scopes(const module& mod, const error_handler_ptr& eh)
{
    details::scope_builder dg(mod, eh);
    dg.build_scope();
};

void sema::build_type_scopes(const module& mod, const error_handler_ptr& eh, const ast::type& t, 
            const ast::kind& k, const ad::type_var_scope& type_scope,
            const arrow::details::type_var_scope& kind_scope)
{
    details::scope_builder dg(mod, eh);
    dg.build_type_scope(t, k, type_scope, kind_scope);
}

void sema::type_check_types(module& mod, const error_handler_ptr& eh)
{
    typing::type_checker tc(mod, eh);
    mod.type_check_types(tc, eh);
};

void sema::type_check_terms(module& mod, const error_handler_ptr& eh)
{
    using sym_map       = module::fun_sym_map;
    sym_map& defs       = mod.get_function_map_modify();
    const graph& gr     = mod.get_graph();

    using key_type      = ad::var_info;
    using node_set      = std::set<ad::var_info>;
    using component_vec = std::vector<std::vector<key_type>>;

    component_vec comp;
    gr.strongly_connected_components(comp);

    typing::type_checker tc(mod, eh);

    for (const auto& group : comp)
    {
        auto scope      = eh->push_error_scope();

        typing::inst_collector col(eh, mod);

        using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group     = std::vector<group_item>;

        rec_group rec;
        long n              = (long)group.size();
        bool is_rec         = n > 1;

        if (n == 1)
        {
            const key_type& key = group[0];
            is_rec          = gr.is_self_recursive(key);
        };

        //make group
        for(long i = 0; i < n; ++i)
        {
            const key_type& key = group[i];
            auto pos            = defs.find(key);

            if (pos == defs.end())
            {
                //this is overload root
                continue;
            }

            ad::symbol_definition_env& sd = pos->second;

            if (sd.has_error() == true)
            {
                eh->add_error();
                continue;
            };

            const ast::type & t   = sd.get_signature();
            bool has_body;
            const ast::term& ex   = sd.get_body(has_body);

            rec.push_back(group_item(key, ex, t));
        };

        //typecheck
        tc.check_type(rec, is_rec, false, &col);

        bool has_errors = eh->has_errors();

        //update definitions
        long it = 0;
        for(long i = 0; i < n; ++i)
        {
            const key_type& key = group[i];
            auto pos            = defs.find(key);

            if (pos == defs.end())
                continue;

            ad::symbol_definition_env& sd = pos->second;

            if (sd.has_error() == true)
                continue;

            bool has_body;
            const ast::term& ex = sd.get_body(has_body);
            (void)ex;

            if (has_body == true)
                sd.set_body(std::get<1>(rec[it]));

            sd.set_signature(std::get<2>(rec[it]));

            if (has_errors == true)
                sd.set_error_flag();

            ++it;
        }

        //process instantiations
        bool has_errors_inst    = false;

        if (has_errors == false)
        {
            col.instantiate();
            has_errors_inst = eh->has_errors();
        }

        if (has_errors_inst == true)
        {
            for(long i = 0; i < n; ++i)
            {
                const key_type& key = group[i];
                auto pos            = defs.find(key);

                if (pos == defs.end())
                    continue;

                ad::symbol_definition_env& sd = pos->second;
                sd.set_error_flag();
            };
        };

        if (eh->has_errors() == true)
            continue;

        //debugging
        for(long i = 0; i < n; ++i)
        {
            typing::inst_collector col_deb(eh, mod);

            const key_type& key = group[i];
            auto pos            = defs.find(key);

            if (pos == defs.end())
                continue;

            const ad::symbol_definition_env& sd = pos->second;

            if (sd.has_error() == true)
            {
                eh->add_error();
                continue;
            };

            const ast::type & t       = sd.get_signature();

            bool has_body;
            const ast::term& ex       = sd.get_body(has_body);

            if (sema::identity(ad::options::print_in_typecheck) == true)
            {
                std::cout << "\n";
                std::cout << sd.get_symbol().get_name_simple().get_string();
                std::cout << " : ";
                sema::print_type(std::cout, t);

                if (has_body == true)
                {
                    std::cout << "\n = ";
                    sema::print_term(std::cout, ex, eh, mod);
                    std::cout << "\n";
                };
            };

            if (ad::options::debug_term_inference == true 
                && eh->has_errors() == false)
            {
                ast::type t2    = t;
                tc.check_type(ex, t2, false, &col_deb);
            };
        };
    };

    using term_list         = module::term_list;
    term_list& terms        = mod.get_eval_terms_modify();

    for (auto& ex : terms)
    {
        typing::inst_collector col(eh, mod);

        ast::type t         = ast::type_var(ex->get_location());
        ex                  = tc.check_type(ex, t, false, &col);

        col.instantiate();

        if (ad::options::debug_term_inference == true)
        {
            typing::inst_collector col_deb(eh, mod);
            tc.check_type(ex, t, false, &col_deb);
        }
    };
};

void sema::eval(module& mod, const error_handler_ptr& eh, std::vector<ast::term>& evaled)
{
    using term_list         = module::term_list;
    const term_list& terms  = mod.get_eval_terms();

    codegen::interpreter ev(mod, eh);

    for (const auto& ex : terms)
    {
        ast::term res       = ev.make(ex);
        evaled.push_back(res);
    };
};

void sema::build_type_dependency_graph(const module& mod, const error_handler_ptr& eh, 
            const ad::type_table& tt)
{
    details::scope_builder dg(mod, eh);
    dg.build_type_dependecy_graph(tt);
};

void sema::build_dependecy_graph(const module& mod, const error_handler_ptr& eh)
{
    details::scope_builder dg(mod, eh);
    dg.build_dependecy_graph();
};

void sema::type_desugaring(const module& mod, const error_handler_ptr& eh, ast::type& t)
{
    desugaring_visitor dv(eh, mod);
    ast::type ret   = dv.make(t);
    t               = ret;
};

void sema::desugaring(module& mod, const error_handler_ptr& eh)
{
    using sym_map       = module::fun_sym_map;
    sym_map& defs       = mod.get_function_map_modify();

    desugaring_visitor dv(eh, mod);

    for (auto& pos : defs)
    {
        auto scope      = eh->push_error_scope();

        bool has_body;

        if (pos.second.has_error() == true)
        {
            eh->add_error();
            continue;
        };

        const ast::term& ex = pos.second.get_body(has_body);
        const ast::type& ty = pos.second.get_signature();        

        if (has_body)
        {
            ast::term ret   = dv.make(ex);
            pos.second.set_body(ret);
        };        

        ast::type ty2   = dv.make(ty);
        pos.second.set_signature(ty2);

        if (eh->has_errors() == true)
            pos.second.set_error_flag();
    };

    using term_list         = module::term_list;
    term_list& terms        = mod.get_eval_terms_modify();

    for (auto& ex : terms)
    {
        ast::term ret   = dv.make(ex);
        ex              = ret;
    }
};

void sema::build_closures(module& mod, const error_handler_ptr& eh)
{
    using sym_map       = module::fun_sym_map;
    sym_map& defs       = mod.get_function_map_modify();

    closure_builder cb(mod, eh);

    for (auto& pos : defs)
    {
        auto scope      = eh->push_error_scope();

        bool has_body;

        if (pos.second.has_error() == true)
        {
            eh->add_error();
            continue;
        };

        const ast::term& ex = pos.second.get_body(has_body);

        if (has_body)
        {
            ast::term ret   = cb.make(ex);
            pos.second.set_body(ret);
        };        

        if (eh->has_errors() == true)
            pos.second.set_error_flag();
    };

    using term_list         = module::term_list;
    term_list& terms        = mod.get_eval_terms_modify();

    for (auto& ex : terms)
    {
        ast::term ret   = cb.make(ex);
        ex              = ret;
    }
};

void sema::decompose_type(const ast::type& ty, std::vector<ast::identifier>& args, 
            ast::type& body)
{
    body = ty;

    if (ty->get_type_code() == ast::details::type_code::type_kind_scope)
    {
        ast::type_kind_scope tks    = ty.static_cast_to<ast::type_kind_scope>();
        const ast::identifier& id   = tks.get_var();
        const ast::type& tail       = tks.get_body();

        args.push_back(id);
        decompose_type(tail, args, body);
    }
    else if (ty->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv    = ty.static_cast_to<ast::type_var>();

        if (tv.is_set() == true)
            return decompose_type(tv.get(), args, body);
        else
            return;
    }

    return;
};

void sema::decompose_term(const ast::term& ty, std::vector<ast::identifier>& args, 
            ast::term& body)
{
    body = ty;

    if (ty->get_term_code() == ast::details::term_code::term_kind_scope)
    {
        ast::term_kind_scope tks    = ty.static_cast_to<ast::term_kind_scope>();
        const ast::identifier& id   = tks.get_var();
        const ast::term& tail       = tks.get_body();

        args.push_back(id);
        decompose_term(tail, args, body);
    }

    return;
};

ast::term sema::compose_term_kind(const ast::term& ex, const std::vector<ast::identifier>& args)
{
    long n  = (long)args.size();

    if (n == 0)
        return ex;

    ast::term ret   = ex;
    for (long i = n - 1; i >= 0; --i)
        ret = ast::term_kind_scope(ex->get_location(), args[i], ret);

    return ret;
};

void sema::decompose_all_type(const ast::type& ty, std::vector<ast::annotated_type_identifier>& args, 
            ast::type& body)
{
    using annot_id  = ast::annotated_type_identifier;
    body = ty;

    if (ty->get_type_code() == ast::details::type_code::all_type)
    {
        ast::all_type at            = ty.static_cast_to<ast::all_type>();
        const annot_id& id          = at.get_var();
        const ast::type& tail       = at.get_body();

        args.push_back(id);
        decompose_all_type(tail, args, body);
    }
    else if (ty->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv    = ty.static_cast_to<ast::type_var>();

        if (tv.is_set() == true)
            return decompose_all_type(tv.get(), args, body);
        else
            return;
    }

    return;
};

class copy_id_vis : public ast::rewrite_visitor<copy_id_vis>
{    
    private:
        using base_type = ast::rewrite_visitor<copy_id_vis>;

    public:
        template<class Ast>
        auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

        auto eval(const ast::identifier& ast) -> ast::identifier
        {
            return ast::identifier(ast->get_location(), ast.get_name_simple(), 
                                   ast.get_fixity(), ast.get_symbol_data());
        };

        auto eval(const ast::variant_case_term& ast) -> ast::variant_case_term
        {
            return base_type::eval_case(ast);
        };
        auto eval(const ast::unpack_term& ast) -> ast::unpack_term
        {
            return base_type::eval_unpack(ast);
        };        
        auto eval(const ast::lambda_term& ast) -> ast::lambda_term
        {
            return base_type::eval_lambda(ast);
        };
        auto eval(const ast::mu_term& ast) -> ast::mu_term
        {
            return base_type::eval_mu(ast);
        }

        auto eval(const ast::let_term& ast) -> ast::let_term
        {
            return base_type::eval_let(ast);
        };
};

ast::term sema::copy_identifiers(const ast::term& t)
{
    copy_id_vis vis;
    return vis.visit(t);
}

ast::type sema::copy_identifiers(const ast::type& t)
{
    copy_id_vis vis;
    return vis.visit(t);
}

ast::kind sema::copy_identifiers(const ast::kind& t)
{
    copy_id_vis vis;
    return vis.visit(t);
}

ast::identifier sema::copy(const ast::identifier& id)
{
    return ast::identifier(id->get_location(), id.get_name_simple(), id.get_fixity(),
                           id.get_symbol_data());
}

class has_type_vars_vis : public ast::traversal_visitor<has_type_vars_vis>
{    
    private:
        using base_type = ast::traversal_visitor<has_type_vars_vis>;

    public:
        template<class Ast>
        void eval(const Ast& ast, bool& val)
        {
            if (val == true)
                return;

            return base_type::eval(ast, val);
        };

        void eval(const ast::type_var& ast, bool& val)
        {
            if (ast.is_set() == true)
                return visit(ast.get(), val);

            val = true;
        };

        bool make(const ast::type& t)
        {
            bool val = false;
            visit(t, val);
            return val;
        }
};

bool sema::has_type_vars(const ast::type& t)
{
    return has_type_vars_vis().make(t);
};

bool sema::is_type_var(const ast::type& t)
{
    if (t->get_type_code() == ast::details::type_code::type_kind_scope)
    {
        ast::type_kind_scope ts = t.static_cast_to<ast::type_kind_scope>();
        return is_type_var(ts.get_body());
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
        return false;

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == false)
        return true;

    return is_type_var(tv.get());
};

ast::type sema::expand_type_var(const ast::type& tv)
{
    if (tv->get_type_code() != ast::details::type_code::type_var)
        return tv;

    ast::type_var var   = tv.static_cast_to<ast::type_var>();
    if (var.is_set())
        return var.get();
    else
        return var;
}

//------------------------------------------------------------------
//              has_kind_vars
//------------------------------------------------------------------
class has_kind_vars_vis : public ast::traversal_visitor<has_kind_vars_vis>
{
    private:
        using base_type     = ast::traversal_visitor<has_kind_vars_vis>;

    private:
        bool        m_value;

    public:
        has_kind_vars_vis()
            :m_value(false)
        {};

        bool make(const ast::kind& k)
        {
            m_value = false;
            visit(k);
            return m_value;
        };

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            if (m_value == true)
                return;

            return base_type::eval(ast);
        };

    public:
        //-----------------------------------------------------------
        //                      KINDS
        //-----------------------------------------------------------
        void eval(const ast::kind_var& ast)
        {
            if (ast.is_set())
                return visit(ast.get());

            m_value = true;
        }
};

bool sema::has_kind_vars(const ast::kind& k)
{
    has_kind_vars_vis vis;
    return vis.make(k);
};

/// remove top level kind variables, return empty kind if
/// k is uninitialized kind variable
ast::kind sema::expand_kind_vars(const ast::kind& k)
{
    if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        ast::kind_var kv    = k.static_cast_to<ast::kind_var>();

        if (kv.is_set() == true)
            return expand_kind_vars(kv.get());
        else
            return ast::kind();
    };

    return k;
};

int sema::measure_height(const ast::type& mu0)
{
    int height = 0;

    ast::type mu    = mu0;
    for (;;)
    {
        if (mu->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv    = mu.static_cast_to<ast::type_var>();

            if (tv.is_set() == false)
                return height;

            mu                  = tv.get();
            continue;
        };
        if (mu->get_type_code() == ast::details::type_code::mu_type)
        {
            ast::mu_type mt     = mu.static_cast_to<ast::mu_type>();
            ++height;
            mu                  = mt.get_body();
            continue;
        }

        return height;
    };
};

ast::details::type_code sema::get_head_type(const ast::type& ty)
{
    return typing::normal_form_type::get_head_type(ty);
};

//------------------------------------------------------------------
//              get_number_arguments
//------------------------------------------------------------------
class get_number_arguments_vis : public ast::traversal_visitor<get_number_arguments_vis>
{
    private:
        using base_type     = ast::traversal_visitor<get_number_arguments_vis>;
        using var_set       = std::set<ad::var_info>;

    private:
        long        n_args;
        bool        m_infinite;
        bool        m_type_dep;
        var_set     m_mu_args;

    public:
        long make(const ast::type& t, bool& infinite, bool& type_dep)
        {
            n_args      = 0;
            m_infinite  = false;
            m_type_dep  = false;

            visit(t);

            infinite    = m_infinite;
            type_dep    = m_type_dep;
            return n_args;
        };

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        void eval(const ast::error_type&)           { return; }
        void eval(const ast::int_type&)             { return; }
        void eval(const ast::bool_type&)            { return; }
        void eval(const ast::char_type&)            { return; }
        void eval(const ast::string_type&)          { return; }
        void eval(const ast::variant_type&)         { return; }
        void eval(const ast::record_type&)          { return; }
        void eval(const ast::product_of_types&)     { return; }
        void eval(const ast::type_sequence&)        { return; }
        void eval(const ast::predefined_type&)      { return; }
        void eval(const ast::tuple_type&)           { return; }
        void eval(const ast::bracket_type&)         { return; }
        void eval(const ast::list_type&)            { return; };
        void eval(const ast::if_type&)              { return; };
        void eval(const ast::path_int_type&)        { return; };
        void eval(const ast::ref_type&)             { return; };
        void eval(const ast::type_constant&)        { return; };
        void eval(const ast::lambda_type&)          { return; };
        void eval(const ast::some_type&)            { return; };

        //type applications should be reduced to normal form if possible
        void eval(const ast::type_application&)
        {
            m_type_dep  = true;
            return; 
        };
        void eval(const ast::type_type_application&)
        { 
            m_type_dep  = true;
            return; 
        };

        void eval(const ast::annotated_type& ast)
        {
            visit(ast.get_type());
        };
        void eval(const ast::type_kind_scope& ast)
        {
            visit(ast.get_body());
        };
        void eval(const ast::type_kind_application& ast)
        {
            visit(ast.get_lhs());
        };        

        void eval(const ast::arrow_type& ast)
        {
            ++n_args;
            visit(ast.get_rhs());
        };

        void eval(const ast::all_type& ast)
        {
            visit(ast.get_body());
        };
        
        void eval(const ast::mu_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            add_mu_var(id);
            visit(ty);
        };

        void eval(const ast::type_var& ast)
        {
            if (ast.is_set())
                visit(ast.get());
        };

        void eval(const ast::identifier_type& ast)
        {
            if (m_mu_args.size() == 0)
                return;

            auto pos = m_mu_args.find(ast.get_identifier());

            if (pos == m_mu_args.end())
                return;

            m_infinite = true;
        };

    private:
        void add_mu_var(const ast::identifier& id)
        {
            m_mu_args.insert(id);
        };
};

long sema::get_number_arguments(const ast::type& ast, bool& infinite, 
                                bool& type_dependent)
{
    return get_number_arguments_vis().make(ast, infinite, type_dependent);
};

//------------------------------------------------------------------
//              make_fresh_binders_vis
//------------------------------------------------------------------

ast::type sema::make_fresh_binders(const ast::type& t, const error_handler_ptr& eh,
                                   const module& mod)
{
    make_fresh_binders_vis vis(eh, mod);
    return vis.make(t);
};

//------------------------------------------------------------------
//              make_new_binder
//------------------------------------------------------------------
ast::identifier sema::make_new_binder(const ast::identifier& id, const module& mod,
                                      bool can_change)
{
    ast::identifier id2;

    if (ad::options::debug_term_inference == true && can_change == true)
    {
        ad::unique_names_provider& unique_names
                            = ad::unique_names_provider::get();
        std::string base    = unique_names.get_base_name(id.get_name_simple().get_string());            
        id2                 = unique_names.get_unique_name(id, base);
    }
    else
    {
        ast::identifier_kind ik = id.get_symbol_data()->get_kind();
        id2                 = ast::identifier(id->get_location(), id.get_name_simple(),
                                id.get_fixity(), ik, ast::identifier::fresh());
    };
    
    module mm(mod);
    mm.build_fresh_type_variable(id2);
    return id2;
};

//------------------------------------------------------------------
//              is_free_var
//------------------------------------------------------------------
class is_free_var_vis : public ast::traversal_visitor<is_free_var_vis>
{    
    private:
        using base_type = ast::traversal_visitor<is_free_var_vis>;
        using var_set   = std::set<ad::var_info>;

    private:
        const var_set&  m_vars;

    public:
        is_free_var_vis(const std::set<ad::var_info>& vars)
            :m_vars(vars)
        {};

        bool make(const ast::type& t)
        {
            bool val = false;
            visit(t, val);
            return val;
        }

    public:
        template<class Ast>
        void eval(const Ast& ast, bool& val)
        {
            if (val == true)
                return;

            return base_type::eval(ast, val);
        };

        void eval(const ast::identifier_type& ast, bool& val)
        {
            const ast::identifier& id   = ast.get_identifier();

            auto pos = m_vars.find(id);

            if (pos != m_vars.end())
                val = true;

            return;
        };
};

bool sema::is_free_var(const std::set<ad::var_info>& vars, const ast::type& ty)
{
    is_free_var_vis vis(vars);
    return vis.make(ty);
};

//------------------------------------------------------------------
//              free_vars
//------------------------------------------------------------------
class free_vars_vis : public ast::traversal_visitor<free_vars_vis>
{    
    private:
        using base_type = ast::traversal_visitor<free_vars_vis>;
        using var_set   = std::set<ad::var_info>;

    private:
        var_set&        m_vars;
        bool            m_is_kind_closed;
        bool            m_free_vars;

    public:
        free_vars_vis(std::set<ad::var_info>& vars)
            :m_vars(vars), m_is_kind_closed(true), m_free_vars(false)
        {};

        void make(const ast::type& t)
        {
            visit(t);
        }

        void make_kind(const ast::kind& k)
        {
            visit(k);
        }

        bool is_kind_closed() const
        {
            return m_is_kind_closed;
        };

        bool has_free_vars() const
        {
            return m_free_vars;
        }

    public:
        template<class Ast>
        void eval(const Ast& ast)
        {
            return base_type::eval(ast);
        };

        void eval(const ast::identifier_type& ast)
        {
            const ast::identifier& id   = ast.get_identifier();
            m_vars.insert(id);
        };

        void eval(const ast::some_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            visit(ty);
            m_vars.erase(id);
        };

        void eval(const ast::all_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            visit(ty);
            m_vars.erase(id);
        };

        void eval(const ast::lambda_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            visit(ty);
            m_vars.erase(id);
        };

        void eval(const ast::mu_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            visit(ty);
            m_vars.erase(id);
        };

        void eval(const ast::type_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            m_free_vars = true;
        }

        //------------------------------------------------------------------
        //                      KINDS
        //------------------------------------------------------------------
        void eval(const ast::kind_identifier& ast)
        {
            (void)ast;
            m_is_kind_closed = false;
        }

        void eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            m_free_vars = true;
        }
};

void sema::free_vars(const ast::type& ty, std::set<ad::var_info>& vars)
{
    free_vars_vis vis(vars);
    return vis.make(ty);
};

bool sema::is_closed_type(const ast::type& ty, bool& closed_kind, bool& unset_vars)
{
    std::set<ad::var_info> free_vars;

    free_vars_vis vis(free_vars);
    vis.make(ty);

    closed_kind = vis.is_kind_closed();
    unset_vars  = vis.has_free_vars();

    if (free_vars.size() == 0)
        return true;
    else
        return false;
};

bool sema::is_closed_kind(const ast::kind& k, bool& unset_vars)
{
    std::set<ad::var_info> free_vars;

    free_vars_vis vis(free_vars);
    vis.make_kind(k);

    unset_vars  = vis.has_free_vars();
    return vis.is_kind_closed();
};

//------------------------------------------------------------------
//              uncurry_application
//------------------------------------------------------------------

void sema::uncurry_application(const ast::term& ast, 
        std::vector<atd::applic_arg_term>& args, ast::term& head)
{
    if (ast->get_term_code() == ast::details::term_code::term_application)
    {
        ast::term_application app   = ast.static_cast_to<ast::term_application>();

        const ast::term& lam        = app.get_lhs();
        const ast::term& arg        = app.get_rhs();

        uncurry_application(lam, args, head);
        args.push_back(atd::applic_arg_term::make_func(arg));
        return;
    }
    else if (ast->get_term_code() == ast::details::term_code::term_type_application)
    {
        ast::term_type_application app  = ast.static_cast_to<ast::term_type_application>();
        const ast::term& all            = app.get_term();
        const ast::type& arg            = app.get_type();

        uncurry_application(all, args, head);
        args.push_back(atd::applic_arg_term::make_inst(arg));
        return;
    }
    else if (ast->get_term_code() == ast::details::term_code::term_kind_application)
    {
        ast::term_kind_application app  = ast.static_cast_to<ast::term_kind_application>();
        const ast::term& all            = app.get_term();
        const ast::kind& arg            = app.get_kind();

        uncurry_application(all, args, head);
        args.push_back(atd::applic_arg_term::make_kind(arg));
        return;
    };

    head    = ast;
    return;
};

void sema::uncurry_application(const ast::type& ast, 
        std::vector<atd::applic_arg_type>& args, ast::type& head)
{
    if (ast->get_type_code() == ast::details::type_code::type_application)
    {
        ast::type_application app   = ast.static_cast_to<ast::type_application>();

        const ast::type& lam        = app.get_lhs();
        const ast::type& arg        = app.get_rhs();

        uncurry_application(lam, args, head);
        args.push_back(atd::applic_arg_type::make_func(arg));
        return;
    }
    else if (ast->get_type_code() == ast::details::type_code::type_type_application)
    {
        ast::type_type_application app  = ast.static_cast_to<ast::type_type_application>();
        const ast::type& all            = app.get_type();
        const ast::type& arg            = app.get_arg();

        uncurry_application(all, args, head);
        args.push_back(atd::applic_arg_type::make_inst(arg));
        return;
    }
    else if (ast->get_type_code() == ast::details::type_code::type_kind_application)
    {
        ast::type_kind_application app  = ast.static_cast_to<ast::type_kind_application>();
        const ast::type& all            = app.get_lhs();
        const ast::kind& arg            = app.get_rhs();

        uncurry_application(all, args, head);
        args.push_back(atd::applic_arg_type::make_kind(arg));
        return;
    }
    else if (ast->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var app  = ast.static_cast_to<ast::type_var>();

        if (app.is_set() == true)
            return uncurry_application(app.get(), args, head);

        head    = ast;
        return;
    }

    head    = ast;
    return;
};

//------------------------------------------------------------------
//              close_type
//------------------------------------------------------------------
class close_type_vis : public ast::rewrite_visitor<close_type_vis>
{    
    private:
        using base_type     = ast::rewrite_visitor<close_type_vis>;

    private:
        error_handler_ptr   m_eh;
        bool                m_kindvar_allowed;

    public:
        close_type_vis(const error_handler_ptr& eh, bool kindvar_allowed)
            :m_eh(eh), m_kindvar_allowed(kindvar_allowed)
        {};

        ast::type make(const ast::type& t)
        {
            return visit(t);
        }

        ast::kind make_kind(const ast::kind& k)
        {
            return visit(k);
        }

    public:
        template<class Ast>
        auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

        ast::type eval(const ast::type_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            error::error_base_msg(m_eh).internal_error(ast->get_location(), "unexpected type var");
            return ast::error_type(ast->get_location());
        }

        //------------------------------------------------------------------
        //                      KINDS
        //------------------------------------------------------------------
        ast::kind eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            if (m_kindvar_allowed == false)
            {
                error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                                        "unexpected kind var");
            };
            return ast;
        }

        ast::kind eval(const ast::star_arrow_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::product_kind& k)
        {
            ast::product_kind k2 = k;
            k2.fix_size();
            return base_type::eval(k2);
        };

        ast::kind eval(const ast::star_tuple_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_all_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_some_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_list_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_record_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_variant_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_predefined_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };

        ast::kind eval(const ast::star_ref_kind& k)
        {
            k.make_fixed();
            return base_type::eval(k);
        };
};

ast::type sema::close_type(const ast::type& ty, const error_handler_ptr& eh)
{
    return close_type_vis(eh, false).make(ty);
};

ast::kind sema::close_kind(const ast::kind& ki, const error_handler_ptr& eh,
                           bool kindvar_allowed)
{
    return close_type_vis(eh, kindvar_allowed).make_kind(ki);
};

//------------------------------------------------------------------
//              make_star_kind_flexible
//------------------------------------------------------------------
class make_star_kind_flexible_vis : public ast::rewrite_visitor<make_star_kind_flexible_vis>
{    
    private:
        using base_type = ast::rewrite_visitor<make_star_kind_flexible_vis>;

    public:
        ast::kind make(const ast::kind& k)
        {
            return visit(k);
        }

    public:
        template<class Ast>
        auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

        //------------------------------------------------------------------
        //                      KINDS
        //------------------------------------------------------------------
        ast::kind eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            return ast;
        }

        ast::star_kind_state eval(ast::star_kind_state)
        {
            return ast::star_kind_state::flexible;
        }
};

ast::kind sema::make_star_kind_flexible(const ast::kind& k)
{
    return make_star_kind_flexible_vis().make(k);
};

}}

#pragma warning(pop)