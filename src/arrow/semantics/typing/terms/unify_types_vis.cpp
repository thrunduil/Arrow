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

#include "unify_types_vis.h"
#include "error/error.h"
#include "semantics/typing/types/unifier_kinds.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/utils.h"
#include "semantics/semantics.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "semantics/typing/types/normal_form_type.h"

#include <iostream>
namespace arrow { namespace typing { namespace details
{

class occurs_check_type_vis : public ast::traversal_visitor<occurs_check_type_vis>
{
    private:
        using base_type = ast::traversal_visitor<occurs_check_type_vis>;

    private:
        const ast::type_var&    m_var;

    public:
        bool found;

    public:
        occurs_check_type_vis(const ast::type_var& var)
            :m_var(var), found(false)
        {};

        void eval(const ast::type_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            if (ast == m_var)
                found = true;
        };

        template<class Ast>
        void eval(const Ast& ast)
        {
            base_type::eval(ast);
        };
};

unify_types_vis::unify_types_vis(const ast::location& loc, const error_handler_ptr& eh, const module& mod,
                bound_map& bm, const context_type& ctx, inst_collector* col)
    :m_loc(loc), m_bound_map(bm), m_eh(eh), m_mod(mod), m_unfolding_state(initial), m_ctx(ctx)
    , m_show_errors(true), m_deduction_mode(false), m_var_subs(nullptr)
    , m_collector(col)
{};

bool unify_types_vis::make(const ast::type& t10, const ast::type& t20, bool show_errors)
{
    m_show_errors       = show_errors;
    m_deduction_mode    = false;
    m_var_subs          = nullptr;
    return make(t10, t20);
};

bool unify_types_vis::deduce(const ast::type& subs_ty, const ast::type& target_ty, var_substitution& subs,
                             bool show_errors)
{
    m_show_errors       = show_errors;
    m_deduction_mode    = true;
    m_var_subs          = &subs;
    return make(subs_ty, target_ty);
}

bool unify_types_vis::make(ast::type t10, ast::type t20)
{    
    if (t10 == t20)
        return true;

    //sema::print_type(std::cout, t10); std::cout<<"\n";
    //sema::print_type(std::cout, t20); std::cout<<"\n";
    
    int num_unfolding;
    ast::type t1_sim = simplify_type(t10, true, false, num_unfolding);
    ast::type t2_sim = simplify_type(t20, false, false, num_unfolding);

    bool is_tv1     = t1_sim->get_type_code() == ast::details::type_code::type_var;
    bool is_tv2     = t2_sim->get_type_code() == ast::details::type_code::type_var;

    if (is_tv1 || is_tv2)
        return unify_type_vars(t1_sim, t2_sim, is_tv1, is_tv2);
    
    //sema::print_type(std::cout, t1_sim); std::cout<<"\n";
    //sema::print_type(std::cout, t2_sim); std::cout<<"\n";

    ast::details::type_code tc1  = sema::get_head_type(t1_sim);
    ast::details::type_code tc2  = sema::get_head_type(t2_sim);

    bool subs1      = m_ctx.make_substitution(t1_sim);
    bool subs2      = m_ctx.make_substitution(t2_sim);

    if (subs1 || subs2)
        return make(t1_sim, t2_sim);

    bool is_mu1     = tc1 == ast::details::type_code::mu_type;
    bool is_mu2     = tc2 == ast::details::type_code::mu_type;
    
    if (is_mu1 || is_mu2)
        return unify_mu(t1_sim, t2_sim, is_mu1, is_mu2);

    bool res;
    visit(t1_sim, t2_sim, res);
    return res;
}

void unify_types_vis::eval(const ast::identifier_type& t1p, const ast::type& t2, bool& res)
{
    res = false;
    if (t2->get_type_code() != ast::details::type_code::identifier_type)
        return;

    ast::identifier_type t2p = t2.static_cast_to<ast::identifier_type>();
    res = unify_identifier_types(t1p.get_identifier(), t2p.get_identifier());
};

void unify_types_vis::eval(const ast::predefined_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::predefined_type)
        return;

    ast::predefined_type t2p = t2.static_cast_to<ast::predefined_type>();

    res = (t1p.get_type_code() == t2p.get_type_code());
};

void unify_types_vis::eval(const ast::ref_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::ref_type)
        return;

    ast::ref_type t2p = t2.static_cast_to<ast::ref_type>();

    res = make(t1p.get_type(), t2p.get_type());
}

void unify_types_vis::eval(const ast::if_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::if_type)
        return;

    ast::if_type t2p    = t2.static_cast_to<ast::if_type>();

    res = make(t1p.get_if(), t2p.get_if());
    if (res == false)
        return;

    res = make(t1p.get_then(), t2p.get_then());
    if (res == false)
        return;

    res = make(t1p.get_else(), t2p.get_else());
};

void unify_types_vis::eval(const ast::list_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::list_type)
        return;

    ast::list_type t2p = t2.static_cast_to<ast::list_type>();
    res = make(t1p.get_type(), t2p.get_type());
}

void unify_types_vis::eval(const ast::arrow_type& t1p, const ast::type& t2, bool& res)
{
    res = false;
    if (t2->get_type_code() != ast::details::type_code::arrow_type)
        return;

    ast::arrow_type t2p = t2.static_cast_to<ast::arrow_type>();

    res = make(t1p.get_lhs(), t2p.get_lhs());
    if (res == false)
        return;

    res = make(t1p.get_rhs(), t2p.get_rhs());
}
        
void unify_types_vis::eval(const ast::type_application& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::type_application)
        return;

    ast::type_application t2p = t2.static_cast_to<ast::type_application>();

    res = make(t1p.get_lhs(), t2p.get_lhs());
    if (res == false)
        return;

    res = make(t1p.get_rhs(), t2p.get_rhs());
}

void unify_types_vis::eval(const ast::type_type_application& t1p, ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::type_type_application)
        return;

    ast::type_type_application t2p = t2.static_cast_to<ast::type_type_application>();

    res = make(t1p.get_type(), t2p.get_type());
    if (res == false)
        return;

    res = make(t1p.get_arg(), t2p.get_arg());
};

void unify_types_vis::eval(const ast::annotated_type& t1p, const ast::type& t2, bool& res)
{
    visit(t1p.get_type(), t2, res);
};

void unify_types_vis::eval(const ast::tuple_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::tuple_type)
        return;

    ast::tuple_type t2p = t2.static_cast_to<ast::tuple_type>();

    if (t1p.size() != t2p.size())
        return;

    long n = t1p.size();
    for (long i = 0; i < n; ++i)
    {
        bool eq = make(t1p.get(i), t2p.get(i));
        if (eq == false)
            return;
    }

    res = true;
}

void unify_types_vis::eval(const ast::bracket_type& ast, const ast::type& t2, bool& equal)
{
    //not allowed at this stage
    (void)ast;
    (void)t2;
    equal = false;
};

void unify_types_vis::eval(const ast::variant_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::variant_type)
        return;

    ast::variant_type t2p = t2.static_cast_to<ast::variant_type>();

    if (t1p.size() != t2p.size())
        return;

    long n = t1p.size();
    for (long i = 0; i < n; ++i)
    {
        const annot_type_id& aid    = t1p.get_identifier(i);
        const ast::type& ti         = t1p.get_type(i);
        const ast::identifier& id   = aid.get_identifier();

        bool has    = t2p.has_field(id);

        if (has == false)
            return;

        ast::type ti2               = t2p.get_field(id);
        annot_type_id aid2          = t2p.get_identifier(id);

        bool eq = make(ti, ti2);
        if (eq == false)
            return;
    }

    res = true;
}

void unify_types_vis::eval(const ast::record_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::record_type)
        return;

    ast::record_type t2p = t2.static_cast_to<ast::record_type>();

    if (t1p.size() != t2p.size())
        return;

    long n = t1p.size();
    for (long i = 0; i < n; ++i)
    {
        const annot_type_id& aid    = t1p.get_identifier(i);
        const ast::type& ti         = t1p.get_type(i);
        const ast::identifier& id   = aid.get_identifier();

        bool has    = t2p.has_field(id);

        if (has == false)
            return;

        ast::type ti2               = t2p.get_field(id);
        annot_type_id aid2          = t2p.get_identifier(id);

        bool eq = make(ti, ti2);
        if (eq == false)
            return;
    }

    res = true;
}

void unify_types_vis::eval(const ast::all_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::all_type)
        return;

    ast::all_type t2p = t2.static_cast_to<ast::all_type>();

    const ast::type& b1         = t1p.get_body();
    const ast::type& b2         = t2p.get_body();

    const annot_type_id& aid1   = t1p.get_var();
    const annot_type_id& aid2   = t2p.get_var();

    const ast::kind& k1         = aid1.get_kind();
    const ast::kind& k2         = aid2.get_kind();

    if (unify_kinds(k1, k2) == false)
        return;

    const ast::identifier& id1  = aid1.get_identifier();
    const ast::identifier& id2  = aid2.get_identifier();

    auto scope  = m_bound_map.add(id1, id2);

    res = make(b1, b2);
}

void unify_types_vis::eval(const ast::some_type& t1p, const ast::type& t2, bool& res)
{
    res = false;
    if (t2->get_type_code() != ast::details::type_code::some_type)
        return;

    ast::some_type t2p = t2.static_cast_to<ast::some_type>();

    const ast::type& b1         = t1p.get_body();
    const ast::type& b2         = t2p.get_body();

    const annot_type_id& aid1   = t1p.get_var();
    const annot_type_id& aid2   = t2p.get_var();

    const ast::kind& k1         = aid1.get_kind();
    const ast::kind& k2         = aid2.get_kind();

    if (unify_kinds(k1, k2) == false)
        return;

    const ast::identifier& id1  = aid1.get_identifier();
    const ast::identifier& id2  = aid2.get_identifier();

    auto scope = m_bound_map.add(id1, id2);

    res = make(b1, b2);
}

void unify_types_vis::eval(const ast::lambda_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::lambda_type)
        return;

    ast::lambda_type t2p = t2.static_cast_to<ast::lambda_type>();

    const ast::type& b1         = t1p.get_body();
    const ast::type& b2         = t2p.get_body();

    const annot_type_id& aid1   = t1p.get_var();
    const annot_type_id& aid2   = t2p.get_var();

    const ast::kind& k1         = aid1.get_kind();
    const ast::kind& k2         = aid2.get_kind();

    if (unify_kinds(k1, k2) == false)
        return;

    const ast::identifier& id1  = aid1.get_identifier();
    const ast::identifier& id2  = aid2.get_identifier();

    auto scope = m_bound_map.add(id1, id2);

    res = make(b1, b2);
}

void unify_types_vis::eval(const ast::mu_type& t1p, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::mu_type)
        return;

    ast::mu_type t2p            = t2.static_cast_to<ast::mu_type>();

    const ast::type& b1         = t1p.get_body();
    const ast::type& b2         = t2p.get_body();

    const annot_type_id& aid1   = t1p.get_var();
    const annot_type_id& aid2   = t2p.get_var();

    const ast::kind& k1         = aid1.get_kind();
    const ast::kind& k2         = aid2.get_kind();

    if (unify_kinds(k1, k2) == false)
        return;

    const ast::identifier& id1  = aid1.get_identifier();
    const ast::identifier& id2  = aid2.get_identifier();

    auto scope = m_bound_map.add(id1, id2);

    res = make(b1, b2);
};

void unify_types_vis::eval(const ast::type_constant& nt1, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::type_constant)
        return;

    ast::type_constant nt2 = t2.static_cast_to<ast::type_constant>();

    const ast::identifier& id1  = nt1.get_name();
    const ast::identifier& id2  = nt2.get_name();

    if (ad::var_info(id1) != ad::var_info(id2))
        return;

    if (nt1.var_size() != nt2.var_size())
        return;

    long n  = nt1.var_size();

    for (long i = 0; i < n; ++i)
    {
        const ast::type& b1 = nt1.get_type(i);
        const ast::type& b2 = nt2.get_type(i);

        if (b1 == b2)
            continue;

        bool is_set_1   = (bool)b1;
        bool is_set_2   = (bool)b2;

        if (is_set_1 != is_set_2)
            return;

        res = make(b1, b2);

        if (res == false)
            return;
    };

    res = true;
}

void unify_types_vis::eval(const ast::type_kind_scope& tks1, const ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::type_kind_scope)
        return;

    ast::type_kind_scope tks2   = t2.static_cast_to<ast::type_kind_scope>();

    const ast::type& t1_b       = tks1.get_body();
    const ast::type& t2_b       = tks2.get_body();

    const ast::identifier& id1  = tks1.get_var();
    const ast::identifier& id2  = tks2.get_var();

    auto scope = m_bound_map.add(id1, id2);

    res = make(t1_b, t2_b);
}

void unify_types_vis::eval(const ast::type_var& ast, ast::type& t2, bool& equal)
{
    //should be already processed
    (void)ast;
    (void)t2;
    equal = false;
};

void unify_types_vis::eval(const ast::type_sequence& ast, ast::type& t2, bool& equal)
{
    //not allowed at this stage
    (void)ast;
    (void)t2;
    equal = false;
};

void unify_types_vis::eval(const ast::product_of_types& pt1, ast::type& t2, bool& equal)
{
    equal = false;

    if (t2->get_type_code() != ast::details::type_code::product_of_types)
        return;

    ast::product_of_types pt2   = t2.static_cast_to<ast::product_of_types>();

    if (pt1.size() != pt2.size())
        return;

    long n = pt1.size();

    for (long i = 0; i < n; ++i)
    {        
        const ast::type& t1i    = pt1.get(i);
        const ast::type& t2i    = pt2.get(i);

        bool eq_loc = make(t1i, t2i);

        if (eq_loc == false)
            return;
    };

    equal = true;
    return;
};

void unify_types_vis::eval(const ast::path_int_type& pt1, const ast::type& t2, bool& equal)
{
    //projections are already expanded if this is possible at this point
    equal = false;

    if (t2->get_type_code() != ast::details::type_code::path_int_type)
        return;

    ast::path_int_type pt2  = t2.static_cast_to<ast::path_int_type>();

    const ast::type& ty1        = pt1.get_type();
    const ast::type& ty2        = pt2.get_type();
    const ast::int_literal& v1  = pt1.get_index();
    const ast::int_literal& v2  = pt2.get_index();

    if (v1.get_value() != v2.get_value())
        return;   

    equal = make(ty1, ty2);
    return;
};

void unify_types_vis::eval(const ast::error_type& ast, ast::type& t2, bool& equal)
{
    (void)ast;
    (void)t2;
    equal = true;
};

void unify_types_vis::eval(const ast::int_type& tp1, ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::int_type)
        return;

    ast::int_type tp2   = t2.static_cast_to<ast::int_type>();

    res = tp1.get_value() == tp2.get_value();
};

void unify_types_vis::eval(const ast::bool_type& tp1, ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::bool_type)
        return;

    ast::bool_type tp2   = t2.static_cast_to<ast::bool_type>();

    res = tp1.get_value() == tp2.get_value();
};

void unify_types_vis::eval(const ast::char_type& tp1, ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::char_type)
        return;

    ast::char_type tp2   = t2.static_cast_to<ast::char_type>();

    res = tp1.get_value() == tp2.get_value();
};

void unify_types_vis::eval(const ast::string_type& tp1, ast::type& t2, bool& res)
{
    res = false;

    if (t2->get_type_code() != ast::details::type_code::string_type)
        return;

    ast::string_type tp2    = t2.static_cast_to<ast::string_type>();

    res = tp1.get_value() == tp2.get_value();
};

void unify_types_vis::eval(const ast::type_kind_application& ast, ast::type& t2, bool& res)
{
    //kind applications should be expanded in simplify_types function
    (void)ast;
    (void)t2;
    res = false;
};

bool unify_types_vis::unify_type_vars(const ast::type& t1, const ast::type& t2, bool is_tv1, bool is_tv2)
{
    if (is_tv1 && is_tv2)
    {
        ast::type_var t1p   = t1.static_cast_to<ast::type_var>();
        ast::type_var t2p   = t2.static_cast_to<ast::type_var>();

        return unify_type_vars(t1p, t2p);
    }
    else if (is_tv1)
    {
        ast::type_var t1p = t1.static_cast_to<ast::type_var>();
        return unify_type_vars(t1p, t2);
    }
    else
    {
        ast::type_var t2p = t2.static_cast_to<ast::type_var>();

        return unify_type_vars(t1, t2p);
    }
};

bool unify_types_vis::unify_type_vars(const ast::type_var& t1, const ast::type_var& t2)
{
    if (t1.is_set())
        return unify_type_vars(t1.get(), t2);

    if (t2.is_set())
        return unify_type_vars(t1, t2.get());

    if (t1 == t2)
        return true;

    t1.set_checked(t2);
    return true;
}

bool unify_types_vis::unify_type_vars(const ast::type_var& t1, const ast::type& t2)
{
    if (t1.is_set())
        return make(t1.get(), t2);

    details::occurs_check_type_vis vis(t1);
    vis.visit(t2);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::type_error_msg(m_eh).error_infinite_type(m_loc, t1, t2);

        return false;
    };

    t1.set_checked(t2);
    return true;
}

bool unify_types_vis::unify_type_vars(const ast::type& t1, const ast::type_var& t2)
{
    if (t2.is_set())
        return make(t1, t2.get());

    details::occurs_check_type_vis vis(t2);
    vis.visit(t1);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::type_error_msg(m_eh).error_infinite_type(m_loc, t2, t1);

        return false;
    };

    t2.set_checked(t1);
    return true;
}

bool unify_types_vis::unify_identifier_types(const ast::identifier& id1, const ast::identifier& id2)
{
    ast::symbol_data* sd1   = id1.get_symbol_data();
    ast::symbol_data* sd2   = id2.get_symbol_data();

    ast::identifier_kind ik1    = sd1->get_kind();
    ast::identifier_kind ik2    = sd2->get_kind();
    
    if (ik1.is_type_def() == true)
    {
        //we should also check local instantiations, but currently
        //type definitions cannot have local instantiations (as well as
        //any instantiations and local definitions)
        if (ik2.is_type_def() == false)
            return false;

        return ad::var_info(id1) == ad::var_info(id2);
    }
    else if (ik1.is_type_var() == true)
    {
        if (ik2.is_type_var() == false)
            return false;
    }
    else if (ik1.is_kind_var() == true)
    {
        if (ik2.is_kind_var() == false)
            return false;
    }
    else
    {
        return false;
    };

    long code1  = m_bound_map.get_code(id1);
    long code2  = m_bound_map.get_code(id2);

    if (bound_map::is_bound(code1) == true && bound_map::is_bound(code2) == true)
        return code1 == code2;
    else if (bound_map::is_bound(code1) == false && bound_map::is_bound(code2) == false)
        return ad::var_info(id1) == ad::var_info(id2);
    else
        return false;
};

bool unify_types_vis::unify_kinds(const ast::kind& k_inf, const ast::kind& k_req)
{
    return unifier_kinds(m_eh).unify_kinds(m_loc, k_inf, k_req, m_show_errors);
}

bool unify_types_vis::is_subkind(const ast::kind& k_spec, const ast::kind& k_gen)
{
    return unifier_kinds(m_eh).is_subkind(m_loc, k_spec, k_gen, m_show_errors);
}

ast::type unify_types_vis::simplify_type(const ast::type& t, bool first, bool expand_mu, 
                                        int& num_unfolding)
{
    normal_form_type nft(m_eh, m_mod, m_collector, expand_mu);

    if (first == true && m_deduction_mode == true)
    {
        ast::type ret   = nft.make_deduce(t, m_var_subs, num_unfolding);
        m_var_subs->finalize_normalization(m_bound_map);
        return ret;
    }
    else
    {        
        ast::type ret   = nft.make(t, num_unfolding);
        return ret;
    };
};

bool unify_types_vis::unify_mu(const ast::type& t1, const ast::type& t2, bool is_mu1, bool is_mu2)
{
    // strong equivalence of recursive types can be undecidable for
    // F-omega; there exists an efficient algorithm for king * recursion,
    // and there exists a double exponential type algorithm for kind *->*
    // recursion; weak equivalence as in Cardone, F., & Coppo, M. 
    // "Decidability properties of recursive types", 2003, can be also 
    // undecidable for F-omega.

    // therefore we use a very weak equivalence relation ~, which does not
    // allow for any unfolding inside mu type:
    //      equivalence == inside mu types:
    //      1. s == s' imply (Rec[t] s) == (Rec[t] s')
    //      2. s1 == s1' & s2 == s2' imply (s1->s2) == (s1'->s2')
    //      3. other rules like 2
    //
    //      equivalence of types:
    //      1'. s == s' imply (Rec[t] s) ~ (Rec[t] s')
    //      2'. (Rec[t] s) ~ (s [t = Rec[t] s]) (unfolding)
    //      3'. s1 ~ s1' & s2 ~ s2' imply (s1->s2) ~ (s1'->s2')
    //      4'. other rules like 3'
    //
    // and ==, ~ are the smallest equivalence relations having these
    // properties

    //sema::print_type(std::cout, t1); std::cout<<"\n";
    //sema::print_type(std::cout, t2); std::cout<<"\n";

    if (is_mu1 == true && is_mu2 == false)
    {
        // t1 : Rec[X] F(X), t2 = Cons (a1, ..., an)
        // according to rule (1) t2 is unrolled t1; since we do not allow for 
        // any unrolling inside mu types, we cannot do any unrolling in t2 type;
        // if t2 is already unrolled, then t2 must also be a mu type (otherwise
        // some unrolling must have been performed in t2 due to contractiveness
        // of t1 and t2)

        if (m_unfolding_state == unrolled_second)
            return false;
        if (m_unfolding_state == unrolled_both)
            return false;

        int num_unfolding_1;
        ast::type t1_ret    = simplify_type(t1, true, true, num_unfolding_1);
        ast::type t2_ret    = t2;

        unfolding_state ns  = num_unfolding_1 > 0 ? unrolled_first : m_unfolding_state;
        sema::scoped_var<unfolding_state> sv(m_unfolding_state, ns);

        bool res;
        visit(t1_ret, t2_ret, res);
        return res;
    }

    if (is_mu1 == false && is_mu2 == true)
    {
        // t1 = Cons (a1, ..., an), t2 : Rec[X] F(X)
        // according to rule (1) t1 is unrolled t2; since we do not allow for 
        // any unrolling inside mu types, we cannot do any unrolling in t1 type;
        // if t1 is already unrolled, then t1 must also be a mu type (otherwise
        // some unrolling must have been performed in t1 due to contractiveness
        // of t1 and t2)

        if (m_unfolding_state == unrolled_first)
            return false;
        if (m_unfolding_state == unrolled_both)
            return false;

        int num_unfolding_2;
        ast::type t1_ret    = t1;
        ast::type t2_ret    = simplify_type(t2, false, true, num_unfolding_2);

        unfolding_state ns  = num_unfolding_2 > 0 ? unrolled_second : m_unfolding_state;
        sema::scoped_var<unfolding_state> sv(m_unfolding_state, ns);

        bool res;
        visit(t1_ret, t2_ret, res);
        return res;
    }

    if (is_mu1 == true && is_mu2 == true)
    {
        // t1 = Rec[X] F(X), t2 : Rec[Y] G(X)
        // it is still possible, that t1 is unrolled t2 or t2 is unrolled t1
        // since our contractiveness definition allows for types like 
        // Rec[X] Rec[Y] F(X,Y)
        
        if (m_unfolding_state == unrolled_both)
        {
            // now we are in the rule 1'; no further unrolling is possible
            bool res;
            visit(t1, t2, res);
            return res;
        }

        int num_unfolding_1;
        int num_unfolding_2;
        ast::type t1_ret    = simplify_type(t1, true, true, num_unfolding_1);
        ast::type t2_ret    = simplify_type(t2, false, true, num_unfolding_2);

        unfolding_state ns  = m_unfolding_state;

        if (m_unfolding_state == unrolled_first)
        {
            if (num_unfolding_2 > num_unfolding_1)
                return false;
            if (num_unfolding_2 > 0)
                ns  = unfolding_state::unrolled_both;
        }
        else if (m_unfolding_state == unrolled_second)
        {
            if (num_unfolding_1 > num_unfolding_2)
                return false;
            if (num_unfolding_1 > 0)
                ns  = unfolding_state::unrolled_both;
        }
        else
        {
            if (num_unfolding_1 > 0 && num_unfolding_2 > 0)
                ns  = unfolding_state::unrolled_both;
            else if (num_unfolding_1 > 0)
                ns  = unfolding_state::unrolled_first;
            else if (num_unfolding_2 > 0)
                ns  = unfolding_state::unrolled_second;
        };

        sema::scoped_var<unfolding_state> sv(m_unfolding_state, ns);

        bool res;
        visit(t1_ret, t2_ret, res);
        return res;
    }

    return false;
};

};};}