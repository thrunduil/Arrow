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

#include "term_check_vis.h"
#include "semantics/typing/types/kind_checker.h"
#include "scopes/module_impl.h"
#include "scopes/overload_set.h"

namespace arrow { namespace typing 
{

class number_args
{
    private:
        long        m_num;
        bool        m_infinite;

    public:
        number_args();
        number_args(long num, bool infinite);

        bool        operator<(const number_args& other) const;
        bool        operator>(const number_args& other) const;
};

class match_type
{
    private:
        long                m_group_index;
        number_args         m_num_args;
        bool                m_need_spec;
        bool                m_is_abstract;
        bool                m_is_closed;
        ad::def_location    m_def_loc;

    public:
        match_type();
        match_type(long group, bool need_spec, bool is_asbstract, bool is_closed,
                   number_args num_args, ad::def_location def_loc);

        /// if m1 > m2, then m1 is a better match
        bool        operator<(const match_type& other) const;
        bool        operator>(const match_type& other) const;
};

class infered_substitutions
{
    private:
        using vars_subs = std::shared_ptr<var_substitution>;
        using kind_subs = std::shared_ptr<kind_subs_map>;
        using subs_vec  = std::vector<vars_subs>;        

    private:
        subs_vec    m_subs_vec;
        kind_subs   m_kind_subs;

    public:
        void        add_term_subs(const vars_subs& subs);
        void        set_kind_subs(const kind_subs& subs);
        void        add_type_subs();

        vars_subs   get_term_substitution(long pos) const;
        kind_subs   get_kind_substitution() const;
};

class overload_info
{
    private:
        using def_type      = ad::symbol_definition_env;
        using subs_ptr      = std::shared_ptr<infered_substitutions>;
        using vars_subs     = std::shared_ptr<var_substitution>;

    private:
        const def_type*     m_def;
        ast::type           m_func_type;
        ast::type           m_return_type;
        bool                m_need_spec;
        ad::def_location    m_def_loc;
        subs_ptr            m_subs;        

    public:
        overload_info();
        overload_info(const def_type*, bool need_spec, const ast::type& func_ty,
                      const ast::type& ret_ty, const subs_ptr& subs, ad::def_location loc);
            
        match_type          get_match() const;
        ast::type           get_function_type() const;
        ast::type           get_return_type() const;
        ast::location       get_location() const;
        const subs_ptr&     get_subst();
        const def_type*     get_def(const ad::def_index*& di) const;
};

class overload_resolution
{
    private:
        using def_type          = ad::symbol_definition_env;
        using overload_set      = ad::overload_set<ad::overload_term_config>;
        using candidates_vec    = std::vector<overload_info>;
        using arg_vec           = std::vector<details::applic_arg_term>;
        using context_type      = details::kind_inference_context;
        using subs_ptr          = std::shared_ptr<infered_substitutions>;
        using vars_subs         = std::shared_ptr<var_substitution>;

    private:
        overload_set            m_overloads;
        candidates_vec          m_candidates;
        error_handler_ptr       m_eh;
        module                  m_mod;

        kind_checker            m_kind_checker;
        type_equaler            m_type_eq;
        context_type&           m_ctx;
        inst_collector*         m_inst_collector;
        bound_map&              m_bound_vars;

    public:
        overload_resolution(const overload_set& os, const error_handler_ptr& eh, 
            const module& mod, context_type& ctx, bound_map& bm, inst_collector* col,
            const ast::overload_index* initial);

        void            build_candidates(const arg_vec& args);
        void            build_candidates(const arg_vec& args, const ast::type& ret_ty);

        void            select_best();

        bool            check_errors(const ast::location& loc, const ast::identifier& id,
                            const arg_vec& args) const;
        
        const def_type* get_best(const ad::def_index*& di, bool& success, ast::type& funct_ty,
                            ast::type& ret_ty, subs_ptr& subs);

    private:
        void            build_candidates(const overload_set& os, const arg_vec& args, 
                            const ast::type& ret_ty);
        bool            check_overload(const def_type* def, const arg_vec& args, const ast::type& ret_ty_in,
                            bool& need_spec, ast::type& func_ty, ast::type& ret_ty, subs_ptr& subs);

        bool            deduce_arrow_all(const ast::type& t, ast::arrow_type& ret, 
                            var_substitution& subs);
        bool            unify_with_arrow(const ast::type& t, ast::arrow_type& ret);
        bool            unify_types(const ast::location& loc, const ast::type& t1, const ast::type& t2,
                            bool show_errors);
        bool            is_all_type(const ast::type& t, ast::all_type& ret,  ast::type_var& tv, 
                            bool& is_var);
        ast::kind       infer_kind(ast::type& t);
        bool            is_subkind(const ast::location& loc, const ast::kind& k_spec, const ast::kind& k_gen,
                            bool show_errors);
};

};};