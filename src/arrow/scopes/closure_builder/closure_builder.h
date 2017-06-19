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

#include "arrow/config.h"
#include "arrow/ast/ast_visitor.h"
#include "scopes/module.h"

namespace arrow { namespace sema
{

//collect all patterns vars (also locally bound) and local function
//symbols
class closure_data_collector
{
    public:
        using var_set   = std::set<ad::var_info>;
        using import_func = std::function<const var_set& (ad::var_info)>;

    private:
        var_set         m_pattern_vars;
        var_set         m_local_functions;

    public:
        void            add_referred_var(const ast::identifier id);
        void            add_referred_function(const ast::identifier& id);

        void            remove_local_symbols(const local_scope& let_scope,
                            const module& mod);
        void            add_local(closure_data_collector& col);

        const var_set&  get_local_functions() const;
        const var_set&  get_pattern_vars() const;

        void            make_import(const import_func& imp_fun, 
                            const var_set*& collected_patterns);
        void            set_import(const var_set& collected);

        void            build_closure(ast::closure_term& cl) const;
};

class closure_info
{
    private:
        using closure_data  = closure_data_collector;
        using import_func   = closure_data_collector::import_func;
        using var_set       = closure_data_collector::var_set;

    private:
        ast::identifier     m_id;
        closure_data        m_data;
        ast::closure_term   m_closure;
                
    public:
        closure_info();
        closure_info(const ast::identifier& id, const closure_data_collector& col,
                     const ast::term& body, ast::closure_kind kind);

        void        make_import(const import_func& imp_func, 
                        const var_set*& collected_patterns);
        void        set_import(const var_set& collected);

        void        build_func_closure();
        void        build_var_closure(ast::closure_term& cl);

        const ast::closure_term&
                    get_closure() const         { return m_closure; };

        const closure_data& get_data() const    { return m_data; };
};

class closure_map
{
    private:
        using var_set       = std::set<ad::var_info>;
        using overload_set  = ad::overload_set<ad::overload_term_config>;
        using overloads     = std::map<ad::var_info, const overload_set*>;
        using cl_map        = std::map<ad::var_info, closure_info>;
        using id_closure    = std::pair<ast::identifier, ast::closure_term>;
        using var_cl_vec    = std::vector<id_closure>;

    private:
        var_set     m_local_functions;
        overloads   m_overloads;
        cl_map      m_func_closures;
        var_cl_vec  m_var_closures;

    public:
        void        register_function(const ast::identifier& id, const local_scope& ls);
        void        unregister_function(const ast::identifier& id, const local_scope& ls);
        bool        is_local_function(const ast::identifier& id) const;

        ast::term   build_function_closure(const ast::identifier& id, 
                        const closure_data_collector& col, const ast::term& ex);
        ast::term   build_var_closure(const ast::identifier& id, const ast::term& def);

        void        build_overload_closures();
        void        finalize_closures();

    private:
        void        fill_closures(const std::vector<ad::var_info>& group);
        void        fill_closures(const ad::var_info& vi, const var_set*& collected);
        void        set_closures(const ad::var_info& vi, const var_set& collected);
        void        build_func_closures();
        void        build_var_closures();        

        const var_set&  get_patterns(const ad::var_info& vi) const;
};

class closure_builder_vis : public ast::rewrite_visitor<closure_builder_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<closure_builder_vis>;

    private:
        module              m_mod;
        error_handler_ptr   m_eh;
        closure_map         m_closure_map;

    public:
        closure_builder_vis(const module& mod, const error_handler_ptr& eh);

        ast::term make(const ast::term& ex);

    public:
        template<class Ast>
        auto eval(const Ast& ast, closure_data_collector& col)
            -> decltype(base_type::eval(ast, col))
        {
            return base_type::eval(ast, col);
        };        

        ast::term eval(const ast::let_term& ast, closure_data_collector& col);
        
        ast::term eval(const ast::identifier_term& ast, closure_data_collector& col);
        ast::term eval(const ast::closure_term& ast, closure_data_collector& col);
        ast::term eval(const ast::overloaded_symbol& ast, closure_data_collector& col);

        ast::term eval(const ast::variant_case_term& ast, closure_data_collector& col);
        ast::term eval(const ast::unpack_term& ast, closure_data_collector& col);
        ast::term eval(const ast::mu_term& ast, closure_data_collector& col);
        ast::term eval(const ast::lambda_term& ast, closure_data_collector& col);

    public:
        void        add_name_substitution(const ast::identifier& id, 
                        const ast::identifier& new_id, const local_scope& ls);
        void        remove_name_substitution(const ast::identifier& id, const local_scope& ls);

        ast::term   make_body(const ast::term& t, closure_data_collector& col);
        ast::term   make_term(const ast::identifier& id, const ast::term& t,
                        closure_data_collector& col, const local_scope& sc, bool overloaded);
        ast::type   make_type(const ast::identifier& id, const ast::type& t);
        void        make_definition(const ast::identifier& id,
                        const ast::term_definition& new_def, closure_data_collector& col);

        ast::type   get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                        bool& has_errors);
        ast::term   get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                        bool& has_errors);
};

class closure_builder
{
    private:
        module              m_mod;
        error_handler_ptr   m_eh;

    public:
        closure_builder(const module& mod, const error_handler_ptr& eh);

    public:
        ast::term   make(const ast::term& ex);
};

};};