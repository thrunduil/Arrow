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

#include "unify_subkinds_vis.h"
#include "unify_kinds_vis.h"
#include "unifier_kinds.h"
#include "semantics/semantics.h"

namespace arrow { namespace typing { namespace details
{

bool unify_subkinds_vis::make(const ast::kind& k_spec, const ast::kind& k_gen)
{
    bool is_var_sp  = k_spec->get_kind_code() == ast::details::kind_code::kind_var;
    bool is_var_gen = k_gen->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_sp == true && is_var_gen == true)
    {
        ast::kind_var kv_sp = k_spec.static_cast_to<ast::kind_var>();
        ast::kind_var kv_gn = k_gen.static_cast_to<ast::kind_var>();

        return is_subkind_var(kv_sp, kv_gn);
    }
    else if (is_var_sp == true)
    {
        ast::kind_var kv_sp = k_spec.static_cast_to<ast::kind_var>();

        return is_subkind_var(kv_sp, k_gen);
    }
    else if (is_var_gen == true)
    {
        ast::kind_var kv_gn = k_gen.static_cast_to<ast::kind_var>();

        return is_subkind_var(k_spec, kv_gn);
    };

    bool res = false;
    visit(k_spec, k_gen, res);
    return res;
};

bool unify_subkinds_vis::is_subkind_var(const ast::kind_var& k_spec, const ast::kind& k_gen)
{
    if (k_spec.is_set() == true)
        return make(k_spec.get(), k_gen);

    details::occurs_check_vis vis(k_spec);
    vis.visit(k_gen);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::kind_error_msg(m_eh).error_infinite_kind(m_loc, k_spec, k_gen);

        return false;
    };

    if (k_spec.is_star_var() == true)
    {
        bool is_star    = unifier_kinds::is_star_kind_any(k_gen);

        if (is_star == false)
        {
            if (m_show_errors == true)
                error::kind_error_msg(m_eh).error_star_kind_required_var(m_loc, k_gen);
            
            return false;
        };
    };

    k_spec.set_checked(k_gen);
    return true;
}

bool unify_subkinds_vis::is_subkind_var(const ast::kind& k_spec, const ast::kind_var& k_gen)
{
    if (k_gen.is_set() == true)
        return make(k_spec, k_gen.get());

    details::occurs_check_vis vis(k_gen);
    vis.visit(k_spec);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::kind_error_msg(m_eh).error_infinite_kind(m_loc, k_gen, k_spec);

        return false;
    };

    if (k_gen.is_star_var() == true)
    {
        bool is_star    = unifier_kinds::is_star_kind_any(k_spec);

        if (is_star == false)
        {
            if (m_show_errors == true)
                error::kind_error_msg(m_eh).error_star_kind_required_var(m_loc, k_spec);
            
            return false;
        };
    };

    ast::kind k_spec2   = sema::make_star_kind_flexible(k_spec);
    k_gen.set_checked(k_spec2);
    return true;
}

bool unify_subkinds_vis::is_subkind_var(const ast::kind_var& k_spec, const ast::kind_var& k_gen)
{
    if (k_spec.is_set() == true)
        return make(k_spec.get(), k_gen);

    if (k_gen.is_set() == true)
        return make(k_spec, k_gen.get());

    if (k_spec == k_gen)
        return true;

    ast::kind_var k_gen2    = k_gen;

    if (k_spec.is_star_var() == true)
        k_gen2.set_star_var();

    k_spec.set_checked(k_gen);
    return true;
}

void unify_subkinds_vis::eval(const ast::value_kind& vk1, const ast::kind& k_gen, bool& res)
{
    if (k_gen->get_kind_code() == ast::details::kind_code::value_kind)
    {
        ast::value_kind vk2     = k_gen.static_cast_to<ast::value_kind>();
        res = vk1.get_code() == vk2.get_code();
    }
    else
    {
        res = false;
    }
}

void unify_subkinds_vis::eval(const ast::arrow_kind& ak_sp, const ast::kind& k_gen, bool& res)
{
    if (k_gen->get_kind_code() == ast::details::kind_code::arrow_kind)
    {
        ast::arrow_kind ak_gn   = k_gen.static_cast_to<ast::arrow_kind>();

        const ast::kind& ak1_l  = ak_sp.get_lhs();
        const ast::kind& ak1_r  = ak_sp.get_rhs();
        ast::polarity_type pol1 = ak_sp.get_type();

        const ast::kind& ak2_l  = ak_gn.get_lhs();
        const ast::kind& ak2_r  = ak_gn.get_rhs();
        ast::polarity_type pol2 = ak_gn.get_type();

        res =  make(ak2_l, ak1_l);

        if (res == false)
            return;

        res = make(ak1_r, ak2_r);

        if (res == false)
            return;

        if (is_polarity_less(pol1, pol2) == false)
            res = false;
        else
            res = true;

        return;
    }
    else
    {
        res = false;
        return;
    }
}

void unify_subkinds_vis::eval(const ast::product_kind& pk0, const ast::kind& k_gen, bool& res)
{
    if (k_gen->get_kind_code() == ast::details::kind_code::product_kind)
    {
        ast::product_kind pk1 = pk0;
        ast::product_kind pk2 = k_gen.static_cast_to<ast::product_kind>();

        res = unifier_kinds::expand_products(pk1, pk2);
        if (res == false)
            return;

        long n = pk1.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::kind& ki1    = pk1.get(i);
            const ast::kind& ki2    = pk2.get(i);

            res = make(ki1, ki2);
            if (res == false)
                return;
        };

        res = true;
    }
    else
    {
        res = false;
    }
};

void unify_subkinds_vis::eval(const ast::kind_scope& ks1, const ast::kind& k_gen, bool& res)
{
    if (k_gen->get_kind_code() == ast::details::kind_code::kind_scope)
    {
        ast::kind_scope ks2 = k_gen.static_cast_to<ast::kind_scope>();

        const ast::kind& k1 = ks1.get_body();
        const ast::kind& k2 = ks2.get_body();

        const ast::identifier& id1  = ks1.get_var();
        const ast::identifier& id2  = ks2.get_var();

        auto scope  = m_bm.add(id1, id2);

        res = make(k1, k2);
        return;
    }
    else
    {
        res = false;
        return;
    }
}

void unify_subkinds_vis::eval(const ast::kind_identifier& ki1, const ast::kind& k_gen, bool& res)
{
    if (k_gen->get_kind_code() == ast::details::kind_code::kind_identifier)
    {
        ast::kind_identifier ki2    = k_gen.static_cast_to<ast::kind_identifier>();

        const ast::identifier& id1  = ki1.get_identifier();
        const ast::identifier& id2  = ki2.get_identifier();

        res = unifier_kinds::unify_identifiers(m_loc, id1, id2, m_bm);
        return;
    }
    else
    {
        res = false;
        return;
    }
}
void unify_subkinds_vis::eval(const ast::kind_var& k_spec, const ast::kind& k_gen, bool& res)
{
    //kind vars are already processed
    (void)k_spec;
    (void)k_gen;
    res = false;
};

void unify_subkinds_vis::eval(const ast::star_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    (void)k_spec;
    res = is_star_star_kind(k_gen, true);
}

void unify_subkinds_vis::eval(const ast::star_tuple_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_tuple_kind)
    {
        ast::star_tuple_kind tk_sp  = k_spec;
        ast::star_tuple_kind tk_gen = k_gen.static_cast_to<ast::star_tuple_kind>();
        bool fixed                  = tk_gen.is_fixed();

        if (tk_gen.is_star() == true)
        {
            res = true;
            return;
        };

        if (tk_sp.size() != tk_gen.size())
            return is_star(k_gen, res);

        long n = tk_gen.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::kind& ki_sp  = tk_sp.get(i);
            const ast::kind& ki_gen = tk_gen.get(i);

            res = make(ki_sp, ki_gen);
            if (res == false)
                return is_star(k_gen, res);
        };

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_arrow_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_arrow_kind)
    {
        ast::star_arrow_kind ak_sp   = k_spec;
        ast::star_arrow_kind ak_gn   = k_gen.static_cast_to<ast::star_arrow_kind>();
        bool fixed                   = ak_gn.is_fixed();

        if (ak_gn.is_star() == true)
        {
            res = true;
            return;
        };

        const ast::kind& ak1_l  = ak_sp.get_lhs();
        const ast::kind& ak1_r  = ak_sp.get_rhs();

        const ast::kind& ak2_l  = ak_gn.get_lhs();
        const ast::kind& ak2_r  = ak_gn.get_rhs();

        res  = make(ak2_l, ak1_l);

        if (res == false)
            return is_star(k_gen, res);

        res = make(ak1_r, ak2_r);

        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_all_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_all_kind)
    {
        ast::star_all_kind sk_sp    = k_spec;
        ast::star_all_kind sk_gen   = k_gen.static_cast_to<ast::star_all_kind>();
        bool fixed                  = sk_gen.is_fixed();

        if (sk_gen.is_star() == true)
        {
            res = true;
            return;
        };

        const ast::kind& v_sp       = sk_sp.get_var();
        const ast::kind& v_gen      = sk_gen.get_var();

        const ast::kind& b_sp       = sk_sp.get_body();
        const ast::kind& b_gen      = sk_gen.get_body();

        // All[var] body is covariant in var as in case of arrow kind var -> body
        res  = make(v_gen, v_sp);

        if (res == false)
            return is_star(k_gen, res);

        res = make(b_sp, b_gen);
        
        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_some_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_some_kind)
    {
        ast::star_some_kind sk_sp    = k_spec;
        ast::star_some_kind sk_gen   = k_gen.static_cast_to<ast::star_some_kind>();
        bool fixed                   = sk_gen.is_fixed();

        if (sk_gen.is_star() == true)
        {
            res = true;
            return;
        };

        const ast::kind& v_sp       = sk_sp.get_var();
        const ast::kind& v_gen      = sk_gen.get_var();

        const ast::kind& b_sp       = sk_sp.get_body();
        const ast::kind& b_gen      = sk_gen.get_body();

        // Some[var] body is covariant in var as in case of arrow kind var -> body
        res = make(v_gen, v_sp);

        if (res == false)
            return is_star(k_gen, res);

        res = make(b_sp, b_gen);

        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_list_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_list_kind)
    {
        ast::star_list_kind sk_sp    = k_spec;
        ast::star_list_kind sk_gen   = k_gen.static_cast_to<ast::star_list_kind>();
        bool fixed                   = sk_gen.is_fixed();

        if (sk_gen.is_star() == true)
        {
            res = true;
            return;
        };

        const ast::kind& b_sp       = sk_sp.get_body();
        const ast::kind& b_gen      = sk_gen.get_body();

        res = make(b_sp, b_gen);
        
        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_predefined_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_predefined_kind)
    {
        ast::star_predefined_kind sk_sp    = k_spec;
        ast::star_predefined_kind sk_gen   = k_gen.static_cast_to<ast::star_predefined_kind>();
        bool fixed                         = sk_gen.is_fixed();

        res = sk_sp.get_code() == sk_gen.get_code();        
        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_ref_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_ref_kind)
    {
        ast::star_ref_kind sk_sp    = k_spec;
        ast::star_ref_kind sk_gen   = k_gen.static_cast_to<ast::star_ref_kind>();
        bool fixed                  = sk_gen.is_fixed();

        const ast::kind& b_sp       = sk_sp.get_body();
        const ast::kind& b_gen      = sk_gen.get_body();

        res = make(b_sp, b_gen);
        if (res == false)
            return is_star(k_gen, res);

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_record_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_record_kind)
    {
        ast::star_record_kind rk_sp     = k_spec;
        ast::star_record_kind rk_gen    = k_gen.static_cast_to<ast::star_record_kind>();
        bool fixed                      = rk_gen.is_fixed();

        if (rk_sp.size() != rk_gen.size())
            return is_star(k_gen, res);

        long n  = rk_sp.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rk_sp.get_identifier(i);
            const ast::kind& ki_sp      = rk_sp.get_kind(i);

            if (rk_gen.has_field(id) == false)
                return is_star(k_gen, res);

            const ast::kind& ki_gen     = rk_gen.get_field(id);

            res = make(ki_sp, ki_gen);

            if (res == false)
                return is_star(k_gen, res);
        };

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

void unify_subkinds_vis::eval(const ast::star_variant_kind& k_spec, const ast::kind& k_gen, bool& res)
{
    if (k_spec.is_star() == true)
    {
        res = is_star_star_kind(k_gen, true);
        return;
    };

    if (k_gen->get_kind_code() == ast::details::kind_code::star_variant_kind)
    {
        ast::star_variant_kind rk_sp    = k_spec;
        ast::star_variant_kind rk_gen   = k_gen.static_cast_to<ast::star_variant_kind>();
        bool fixed                      = rk_gen.is_fixed();

        if (rk_sp.size() != rk_gen.size())
            return is_star(k_gen, res);

        long n  = rk_sp.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rk_sp.get_identifier(i);
            const ast::kind& ki_sp      = rk_sp.get_kind(i);

            if (rk_gen.has_field(id) == false)
                return is_star(k_gen, res);

            const ast::kind& ki_gen     = rk_gen.get_field(id);

            res = make(ki_sp, ki_gen);

            if (res == false)
                return is_star(k_gen, res);
        };

        if (fixed)
            k_spec.make_fixed();

        res = true;
        return;
    }

    return is_star(k_gen, res);
};

bool unify_subkinds_vis::is_polarity_less(ast::polarity_type pol_spec, ast::polarity_type pol_gen)
{
    if (pol_gen == ast::polarity_type::neutral)
        return true;
    else
        return pol_spec == pol_gen;
}
bool unify_subkinds_vis::is_star_star_kind(const ast::kind& k, bool force_star)
{
    return is_star_star_vis().make(k, force_star);
};

void unify_subkinds_vis::is_star(const ast::kind& k_gen, bool& res)
{
    res = is_star_star_kind(k_gen, true);
};

};};};