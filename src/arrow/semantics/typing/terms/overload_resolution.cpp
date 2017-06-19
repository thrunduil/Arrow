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

#include "overload_resolution.h"
#include "scopes/symbol_definition.h"
#include "semantics/typing/types/computation_on_types.h"
#include "error/error.h"
#include "semantics/typing/types/unifier_kinds.h"
#include "semantics/semantics.h"
#include "scopes/overload_set.h"
#include "instantiate_term.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "semantics/typing/types/normal_form_type.h"
#include "root/options.h"

#pragma warning(push)
#pragma warning(disable:4127)   // conditional expression is constant

namespace arrow { namespace typing 
{

//------------------------------------------------------------------
//              number_args
//------------------------------------------------------------------
number_args::number_args()
    :m_num(0), m_infinite(true)
{};

number_args::number_args(long num, bool infinite)
    :m_num(num), m_infinite(infinite)
{};

bool number_args::operator<(const number_args& other) const
{
    if (m_infinite == true)
        return false;
    if (other.m_infinite == true)
        return true;

    return m_num < other.m_num;
};

bool number_args::operator>(const number_args& other) const
{
    if (other.m_infinite == true)
        return false;
    if (m_infinite == true)
        return true;

    return m_num > other.m_num;
};

//------------------------------------------------------------------
//              match_type
//------------------------------------------------------------------
match_type::match_type()
    :match_type(-1, true, true, false, number_args(0, true), ad::def_location())
{};

match_type::match_type(long group, bool need_spec, bool is_abstract, 
                       bool is_closed, number_args num_args, ad::def_location loc)
    :m_group_index(group), m_need_spec(need_spec), m_is_abstract(is_abstract)
    ,m_is_closed(is_closed), m_num_args(num_args), m_def_loc(loc)
{};

bool match_type::operator>(const match_type& other) const
{    
    // always prefer overload with fewer number of arguments
    if (m_num_args < other.m_num_args)
        return true;
    if (m_num_args > other.m_num_args)
        return false;

    //closed overloads are best
    if (other.m_is_closed == true)
    {
        if (this->m_is_closed == true)
        {
            //in shell mode later overload is better than the previous one
            if (m_group_index > other.m_group_index)
                return true;
        };

        return false;
    }
    else if (this->m_is_closed == true)
    {
        return true;
    };

    // if in shell mode, then always select the last overload added
    // in regular mode group indices are the same
    if (m_group_index < other.m_group_index)
        return false;

    // select match, that is betten in one of dimensions (need_spec, abstract)
    // and not worse in other dimensions
    if (m_need_spec == true && other.m_need_spec == false)
        return false;
    if (m_is_abstract == true && other.m_is_abstract == false)
        return false;

    if (m_group_index > other.m_group_index)
        return true;

    if (m_need_spec == false && other.m_need_spec == true)
        return true;
    if (m_is_abstract == false && other.m_is_abstract == true)
        return true;

    // if both matches are equivalent, select local one
    if (ad::def_location::more_local(m_def_loc, other.m_def_loc) == true)
        return true;

    return false;
};

bool match_type::operator<(const match_type& other) const
{
    if (m_num_args < other.m_num_args)
        return false;
    if (m_num_args > other.m_num_args)
        return true;

    //closed overloads are best
    if (m_is_closed == true)
    {
        if (other.m_is_closed == true)
        {
            //in shell mode later overload is better than the previous one
            if (m_group_index < other.m_group_index)
                return true;
        };

        return false;
    }
    else if (other.m_is_closed == true)
    {
        return true;
    };

    if (m_group_index > other.m_group_index)
        return false;
    if (m_need_spec == false && other.m_need_spec == true)
        return false;
    if (m_is_abstract == false && other.m_is_abstract == true)
        return false;

    if (m_group_index < other.m_group_index)
        return true;

    if (m_need_spec == true && other.m_need_spec == false)
        return true;
    if (m_is_abstract == true && other.m_is_abstract == false)
        return true;

    if (ad::def_location::more_local(other.m_def_loc, this->m_def_loc) == true)
        return true;

    return false;
};

void infered_substitutions::add_term_subs(const vars_subs& subs)
{
    m_subs_vec.push_back(subs);
}
void infered_substitutions::set_kind_subs(const kind_subs& subs)
{
    m_kind_subs = subs;
};

void infered_substitutions::add_type_subs()
{
    m_subs_vec.push_back(vars_subs());
};

infered_substitutions::vars_subs 
infered_substitutions::get_term_substitution(long pos) const
{
    if (pos < 0 || pos >= (long)m_subs_vec.size())
        return vars_subs();
    else
        return m_subs_vec[pos];
}

infered_substitutions::kind_subs
infered_substitutions::get_kind_substitution() const
{
    return m_kind_subs;
};

overload_info::overload_info()
    :m_def(nullptr), m_need_spec(true), m_def_loc()
{};

overload_info::overload_info(const def_type* def, bool need_spec, 
            const ast::type& func_ty, const ast::type& ret_ty, const subs_ptr& subs,
            ad::def_location def_loc)
    :m_def(def), m_need_spec(need_spec), m_subs(subs), m_func_type(func_ty)
    ,m_return_type(ret_ty), m_def_loc(def_loc)
{};

match_type overload_info::get_match() const
{
    if (!m_def)
        return match_type();

    bool infinite;
    bool type_dep;
    long num_args   = sema::get_number_arguments(m_return_type, infinite, type_dep);
    bool is_abs     = m_def->is_abstract_overload();
    bool is_closed  = m_def->is_closed_overload();

    //ignore type dependent arguments
    return match_type(m_def->get_group_index(), m_need_spec, is_abs, is_closed,
                      number_args(num_args, infinite), m_def_loc);
};

ast::type overload_info::get_function_type() const
{
    if (!m_def)
        return ast::error_type(ast::location());

    return m_func_type;
};

ast::type overload_info::get_return_type() const
{
    if (!m_def)
        return ast::error_type(ast::location());

    return m_return_type;
};


const overload_info::def_type*
overload_info::get_def(const ad::def_index*& di) const
{
    if (!m_def)
    {
        di = nullptr;
        return nullptr;
    }

    di  = &m_def->get_definition_index();
    return m_def;
};

ast::location overload_info::get_location() const
{
    if (!m_def)
        return ast::location();

    return m_def->get_location();
};

const overload_info::subs_ptr& overload_info::get_subst()
{
    return m_subs;
};

overload_resolution::overload_resolution(const overload_set& os, 
                const error_handler_ptr& eh, const module& mod, context_type& ctx, 
                bound_map& bm, inst_collector* col, const ast::overload_index* initial)
    : m_eh(eh), m_mod(mod), m_kind_checker(eh, mod), m_type_eq(&m_kind_checker, mod, eh, col)
    , m_ctx(ctx), m_bound_vars(bm), m_inst_collector(col)
{
    const overload_set* initial_set = nullptr;

    if (initial != nullptr)
        initial_set = m_mod.get_overload_set(initial);

    if (initial_set != nullptr)
    {
        m_overloads = *initial_set;
        m_overloads.import_from_local(os);
    }
    else
    {
        m_overloads = os;
    }
};

void overload_resolution::build_candidates(const arg_vec& args)
{
    return build_candidates(args, ast::type());
};

void overload_resolution::build_candidates(const arg_vec& args, const ast::type& ret_ty_in)
{
    auto scope = m_eh->push_error_scope();
    build_candidates(m_overloads, args, ret_ty_in);
    m_eh->clear_errors();
};

void overload_resolution::build_candidates(const overload_set& os, const arg_vec& args, 
                                           const ast::type& ret_ty_in)
{
    long n      = os.size();
    const ad::symbol_definition_env* def;       

    for (long i = 0; i < n; ++i)
    {
        bool need_spec;
        ast::type func_ty, ret_ty;
        ad::def_location def_loc;
        def             = os.get_definition(i, def_loc);

        if (!def)
            continue;

        subs_ptr subs   = subs_ptr(new infered_substitutions());
        bool is_valid   = check_overload(def, args, ret_ty_in, need_spec, func_ty, ret_ty, subs);

        if (is_valid == true)
            m_candidates.push_back(overload_info(def, need_spec, func_ty, ret_ty, subs, def_loc));
    };    
};

void overload_resolution::select_best()
{
    if (m_candidates.size() <= 1)
        return;

    const overload_info& item0  = m_candidates[0];

    candidates_vec sel;
    sel.push_back(item0);

    match_type match            = item0.get_match();

    for (size_t i = 1; i < m_candidates.size(); ++i)
    {
        const overload_info& item   = m_candidates[i];

        match_type match_loc    = item.get_match();

        if (match_loc > match)
        {
            sel.clear();
            sel.push_back(item);
            match   = match_loc;
        }
        else if (match_loc < match)
        {
            continue;
        }
        else
        {
            sel.push_back(item);
        };
    };

    std::swap(m_candidates, sel);
};

bool overload_resolution::check_overload(const def_type* def, const arg_vec& args, 
            const ast::type& ret_ty_in, bool& need_spec, ast::type& func_ty, ast::type& ret_ty,
            subs_ptr& infered_subs)
{
    if (def->has_error() == true)
        return false;

    const ast::type& type   = def->get_signature();

    using kind_map_ptr      = std::shared_ptr<kind_subs_map>;
    kind_map_ptr subs_map   = kind_map_ptr(new kind_subs_map());

    bool has_errors;
    bool can_local          = true;
    ast::type ty_head       = m_inst_collector->instantiate_and_infer_kind(type, args, false,
                                *subs_map, has_errors, can_local);

    if (has_errors == true)
        return false;

    func_ty                 = ty_head;    

    // first kind applications could be already processed
    size_t i                = subs_map->number_substitutions();
    need_spec               = subs_map->number_instantiations() > 0;

    for (; i < args.size(); ++i)
    {
        const details::applic_arg_term& item   = args[i];

        if (item.get_item_type() == details::applic_arg_term::function_arg)
        {
            vars_subs subs  = vars_subs(new var_substitution(m_mod));

            ast::arrow_type t_lam;

            if (deduce_arrow_all(ty_head, t_lam, *subs) == false)
                return false;

            need_spec                   |= subs->size() > 0;
            ast::type tl_left           = t_lam.get_lhs();
            ast::type tl_right          = t_lam.get_rhs();

            //form All[a1,...,an] sig for fresh type variables a1, ..., an
            tl_left                     = subs->apply(tl_left);

            //find unification {S}sig = psi
            if (subs->deduce(item.get_location(), tl_left, item.get_func_arg_type(),
                             m_type_eq, m_bound_vars, m_ctx, false) == false)
            {
                return false;
            }

            bool ded_succ               = subs->check_deduction(item.get_location(), ty_head,
                                            m_type_eq, m_ctx, m_eh, false);

            if (ded_succ == false)
                return false;

            //find {a1,...,an} - dom(S)
            subs->mark_free_variables();

            //form {S}phi
            tl_right                    = subs->apply(tl_right);

            //form All[{a1,...,an} - dom(S)] {S}phi
            tl_right                    = subs->generalize_type(tl_right);

            infered_subs->add_term_subs(std::move(subs));
            ty_head = tl_right;
        }
        else if (item.get_item_type() == details::applic_arg_term::inst_arg)
        {
            ast::all_type all_t;
            bool is_var;
            ast::type_var tv_all;

            if (is_all_type(ty_head, all_t, tv_all, is_var) == false)
                return false;

            const ast::kind& all_k      = all_t.get_var().get_kind();
            const ast::identifier& id   = all_t.get_var().get_identifier();

            ast::type app_ty            = item.get_inst_arg();
            ast::kind arg_app_k         = infer_kind(app_ty);

            if (is_subkind(item.get_location(), arg_app_k, all_k, false) == false)
                return false;

            type_substitution ts(m_eh, m_mod);
            ts.add_type(id, item.get_inst_arg(), true);

            infered_subs->add_type_subs();
            ty_head                     = ts.make(all_t.get_body());
        }
        else if (item.get_item_type() == details::applic_arg_term::kind_arg)
        {
            return false;
        };
    };

    if (subs_map->check_all_infered(def->get_location(), m_eh, false) == false)
        return false;

    infered_subs->set_kind_subs(subs_map);

    if (!ret_ty_in)
    {
        ret_ty = ty_head;
    }
    else
    {
        if (unify_types(ret_ty_in->get_location(), ty_head, ret_ty_in, false) == false)
            return false;

        ret_ty = ret_ty_in;
    }

    if (ad::options::normalize_types == true)
    {
        normal_form_type nf(m_eh, m_mod, m_inst_collector, false);
        int num_unfold;
        ret_ty = nf.make(ret_ty, num_unfold);
    };

    return true;
};

bool overload_resolution::check_errors(const ast::location& loc, const ast::identifier& id,
                                       const arg_vec& args) const
{
    if (m_candidates.size() == 1)
        return false;

    if (m_candidates.size() > 1)
    {
        error::type_error_msg(m_eh).error_ambiguous_overload(loc, id, args, m_candidates);
        return true;
    };

    error::type_error_msg(m_eh).error_no_valid_overload(loc, id, args, m_overloads);
    return true;
};

const overload_resolution::def_type*
overload_resolution::get_best(const ad::def_index*& di, bool& success, ast::type& best_ty,
                                        ast::type& ret_ty, subs_ptr& subs)
{
    if (m_candidates.size() == 1)
    {
        success = true;
        best_ty = m_candidates[0].get_function_type();
        ret_ty  = m_candidates[0].get_return_type();
        subs    = m_candidates[0].get_subst();
        return m_candidates[0].get_def(di);
    };
    
    success = false;
    subs    = subs_ptr();
    return nullptr;
};

bool overload_resolution::deduce_arrow_all(const ast::type& t, ast::arrow_type& ret, 
                    var_substitution& subs)
{
    return m_type_eq.deduce_arrow_all(t, ret, subs, m_ctx);
};
bool overload_resolution::unify_with_arrow(const ast::type& t, ast::arrow_type& ret)
{
    return m_type_eq.unify_with_arrow(t, ret, m_ctx);
};

bool overload_resolution::unify_types(const ast::location& loc, const ast::type& t1, const ast::type& t2,
                    bool show_errors)
{
    return m_type_eq.unify_types(loc, t1, t2, m_bound_vars, m_ctx, show_errors);
};
bool overload_resolution::is_all_type(const ast::type& t, ast::all_type& ret,  ast::type_var& tv, 
                    bool& is_var)
{
    return m_type_eq.is_all_type(t, ret, tv, is_var, m_ctx);
};
ast::kind overload_resolution::infer_kind(ast::type& t)
{
    return m_type_eq.infer_kind(t, m_ctx, m_inst_collector);
};
bool overload_resolution::is_subkind(const ast::location& loc, const ast::kind& k_spec, const ast::kind& k_gen,
                    bool show_errors)
{
    return unifier_kinds(m_eh).is_subkind(loc, k_spec, k_gen, show_errors);
};

};};

#pragma warning(pop)