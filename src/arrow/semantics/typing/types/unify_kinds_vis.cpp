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

#include "unify_kinds_vis.h"
#include "unifier_kinds.h"

namespace arrow { namespace typing { namespace details
{

template<class Ast1, class Ast2>
static void make_fixed(const Ast1& ast1, const Ast2& ast2, bool fixed)
{
    if (fixed == false)
        return;

    ast1.make_fixed();
    ast2.make_fixed();
}

bool unify_kinds_vis::make(const ast::kind& k_inf, const ast::kind& k_req)
{
    bool is_var_1   = k_inf->get_kind_code() == ast::details::kind_code::kind_var;
    bool is_var_2   = k_req->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true && is_var_2 == true)
    {
        ast::kind_var kv1   = k_inf.static_cast_to<ast::kind_var>();
        ast::kind_var kv2   = k_req.static_cast_to<ast::kind_var>();

        return unify_kinds_var(kv1, kv2);
    }
    else if (is_var_1 == true)
    {
        ast::kind_var kv1   = k_inf.static_cast_to<ast::kind_var>();

        return unify_kinds_var(kv1, k_req);
    }
    else if (is_var_2 == true)
    {
        ast::kind_var kv2   = k_req.static_cast_to<ast::kind_var>();

        return unify_kinds_var(k_inf, kv2);
    };

    bool res = false;
    visit(k_inf, k_req, res);
    return res;
};

bool unify_kinds_vis::unify_kinds_var(const ast::kind_var& k_inf, const ast::kind& k_req)
{
    if (k_inf.is_set() == true)
        return make(k_inf.get(), k_req);

    details::occurs_check_vis vis(k_inf);
    vis.visit(k_req);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::kind_error_msg(m_eh).error_infinite_kind(m_loc, k_inf, k_req);

        return false;
    };

    if (k_inf.is_star_var() == true)
    {
        bool is_star    = unifier_kinds::is_star_kind_any(k_req);

        if (is_star == false)
        {
            if (m_show_errors == true)
                error::kind_error_msg(m_eh).error_star_kind_required_var(m_loc, k_req);
            
            return false;
        };
    };
    
    k_inf.set_checked(k_req);
    return true;
}

bool unify_kinds_vis::unify_kinds_var(const ast::kind& k_inf, const ast::kind_var& k_req)
{
    if (k_req.is_set() == true)
        return make(k_inf, k_req.get());

    details::occurs_check_vis vis(k_req);
    vis.visit(k_inf);

    if (vis.found == true)
    {
        if (m_show_errors == true)
            error::kind_error_msg(m_eh).error_infinite_kind(m_loc, k_req, k_inf);

        return false;
    };

    if (k_req.is_star_var() == true)
    {
        bool is_star    = unifier_kinds::is_star_kind_any(k_inf);

        if (is_star == false)
        {
            if (m_show_errors == true)
                error::kind_error_msg(m_eh).error_star_kind_required_var(m_loc, k_inf);
            
            return false;
        };
    };

    k_req.set_checked(k_inf);
    return true;
}

bool unify_kinds_vis::unify_kinds_var(const ast::kind_var& k_inf, const ast::kind_var& k_req)
{
    if (k_inf.is_set() == true)
        return make(k_inf.get(), k_req);

    if (k_req.is_set() == true)
        return make(k_inf, k_req.get());

    if (k_inf == k_req)
        return true;

    bool is_star        = k_req.is_star_var() || k_inf.is_star_var();
    ast::kind_var k_inf2= k_inf;

    if (is_star == true)
        k_inf2.set_star_var();

    k_req.set_checked(k_inf2);
    return true;
}

void unify_kinds_vis::eval(const ast::star_kind& ast, const ast::kind& k_req, bool& res)
{
    (void)ast;
    res = is_star_star_kind(k_req, true);
}

void unify_kinds_vis::eval(const ast::value_kind& vk1, const ast::kind& k_req, bool& res)
{
    if (k_req->get_kind_code() == ast::details::kind_code::value_kind)
    {
        ast::value_kind vk2     = k_req.static_cast_to<ast::value_kind>();
        res = (vk1.get_code() == vk2.get_code());
    }
    else
    {
        res = false;
    }
};
void unify_kinds_vis::eval(const ast::arrow_kind& ak1, const ast::kind& k_req, bool& res)
{
    if (k_req->get_kind_code() != ast::details::kind_code::arrow_kind)
    {
        res = false;
        return;
    }

    ast::arrow_kind ak2     = k_req.static_cast_to<ast::arrow_kind>();

    const ast::kind& ak1_l  = ak1.get_lhs();
    const ast::kind& ak1_r  = ak1.get_rhs();
    ast::polarity_type pol1 = ak1.get_type();

    const ast::kind& ak2_l  = ak2.get_lhs();
    const ast::kind& ak2_r  = ak2.get_rhs();
    ast::polarity_type pol2 = ak2.get_type();

    res = make(ak1_l, ak2_l);

    if (res == false)
        return;

    res = make(ak1_r, ak2_r);
        
    if (pol1 != pol2)
        res = false;
};

void unify_kinds_vis::eval(const ast::kind_identifier& ki1, const ast::kind& k_req, bool& res)
{
    if (k_req->get_kind_code() != ast::details::kind_code::kind_identifier)
    {
        res = false;
        return;
    }

    ast::kind_identifier ki2    = k_req.static_cast_to<ast::kind_identifier>();
    const ast::identifier& id1  = ki1.get_identifier();
    const ast::identifier& id2  = ki2.get_identifier();

    res = unifier_kinds::unify_identifiers(m_loc, id1, id2, m_bm);
};

void unify_kinds_vis::eval(const ast::kind_var& ast, const ast::kind& k_req, bool& res)
{
    //kind vars are already processed
    (void)ast;
    (void)k_req;
    res = false;
}

void unify_kinds_vis::eval(const ast::kind_scope& ks1, const ast::kind& k_req, bool& res)
{
    if (k_req->get_kind_code() != ast::details::kind_code::kind_scope)
    {
        res = false;
        return;
    }

    ast::kind_scope ks2 = k_req.static_cast_to<ast::kind_scope>();

    const ast::kind& k1 = ks1.get_body();
    const ast::kind& k2 = ks2.get_body();

    const ast::identifier& id1  = ks1.get_var();
    const ast::identifier& id2  = ks2.get_var();

    auto scope  = m_bm.add(id1, id2);

    res = make(k1, k2);
}

void unify_kinds_vis::eval(const ast::product_kind& pk0, const ast::kind& k_req, bool& res)
{
    if (k_req->get_kind_code() != ast::details::kind_code::product_kind)
    {
        res = false;
        return;
    }

    ast::product_kind pk1 = pk0;
    ast::product_kind pk2 = k_req.static_cast_to<ast::product_kind>();

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

void unify_kinds_vis::eval(const ast::star_tuple_kind& tk1, const ast::kind& k_req, bool& res)
{
    if (tk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = tk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_tuple_kind)
    {
        ast::star_tuple_kind tk2    = k_req.static_cast_to<ast::star_tuple_kind>();
        fixed                       = fixed || tk2.is_fixed();

        if (tk2.is_star() == true)
        {
            res = tk1.make_star();
            return;
        };

        if (tk1.size() != tk2.size())
            return unify_stars(tk1, k_req, fixed, res);

        long n = tk1.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::kind& ki1    = tk1.get(i);
            const ast::kind& ki2    = tk2.get(i);

            res = make(ki1, ki2);
            if (res == false)
                return unify_stars(tk1, k_req, fixed, res);
        };

        make_fixed(tk1, tk2, fixed);
        res = true;
        return;
    }

    return unify_stars(tk1, k_req, fixed, res);
};

void unify_kinds_vis::eval(const ast::star_arrow_kind& ak1, const ast::kind& k_req, bool& res)
{
    if (ak1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = ak1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_arrow_kind)
    {
        ast::star_arrow_kind ak2    = k_req.static_cast_to<ast::star_arrow_kind>();
        fixed                       = fixed || ak2.is_fixed();

        if (ak2.is_star() == true)
        {
            res = ak1.make_star();
            return;
        };

        const ast::kind& ak1_l  = ak1.get_lhs();
        const ast::kind& ak1_r  = ak1.get_rhs();

        const ast::kind& ak2_l  = ak2.get_lhs();
        const ast::kind& ak2_r  = ak2.get_rhs();

        res = make(ak1_l, ak2_l);

        if (res == false)
            return unify_stars(ak1, k_req, fixed, res);

        res = make(ak1_r, ak2_r);

        if (res == false)
            return unify_stars(ak1, k_req, fixed, res);

        make_fixed(ak1, ak2, fixed);
        res = true;
        return;
    }

    return unify_stars(ak1, k_req, fixed, res);
};

void unify_kinds_vis::eval(const ast::star_all_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_all_kind)
    {
        ast::star_all_kind sk2  = k_req.static_cast_to<ast::star_all_kind>();
        fixed                   = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        const ast::kind& v1     = sk1.get_var();
        const ast::kind& v2     = sk2.get_var();

        const ast::kind& b1     = sk1.get_body();
        const ast::kind& b2     = sk2.get_body();

        res = make(v1, v2);

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        res  = make(b1, b2);

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_some_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_some_kind)
    {
        ast::star_some_kind sk2  = k_req.static_cast_to<ast::star_some_kind>();
        fixed                   = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        const ast::kind& v1     = sk1.get_var();
        const ast::kind& v2     = sk2.get_var();

        const ast::kind& b1     = sk1.get_body();
        const ast::kind& b2     = sk2.get_body();

        res = make(v1, v2);

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        res = make(b1, b2);

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_list_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_list_kind)
    {
        ast::star_list_kind sk2  = k_req.static_cast_to<ast::star_list_kind>();
        fixed                   = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        const ast::kind& b1     = sk1.get_body();
        const ast::kind& b2     = sk2.get_body();

        res = make(b1, b2);        

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_predefined_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_predefined_kind)
    {
        ast::star_predefined_kind sk2  = k_req.static_cast_to<ast::star_predefined_kind>();
        fixed                           = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        res = sk1.get_code() == sk2.get_code();

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_ref_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_ref_kind)
    {
        ast::star_ref_kind sk2  = k_req.static_cast_to<ast::star_ref_kind>();
        fixed                   = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        const ast::kind& b1     = sk1.get_body();
        const ast::kind& b2     = sk2.get_body();

        res = make(b1, b2);

        if (res == false)
            return unify_stars(sk1, k_req, fixed, res);

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_record_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_record_kind)
    {
        ast::star_record_kind sk2  = k_req.static_cast_to<ast::star_record_kind>();
        fixed                      = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        if (sk1.size() != sk2.size())
            return unify_stars(sk1, k_req, fixed, res);

        long n  = sk1.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = sk1.get_identifier(i);
            const ast::kind& ki1        = sk1.get_kind(i);

            if (sk2.has_field(id) == false)
                return unify_stars(sk1, k_req, fixed, res);

            const ast::kind& ki2        = sk2.get_field(id);

            res = make(ki1, ki2);

            if (res == false)
                return unify_stars(sk1, k_req, fixed, res);
        };

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    }

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::eval(const ast::star_variant_kind& sk1, const ast::kind& k_req, bool& res)
{
    if (sk1.is_star() == true)
    {
        res = is_star_star_kind(k_req, true);
        return;
    };

    bool fixed  = sk1.is_fixed();

    if (k_req->get_kind_code() == ast::details::kind_code::star_variant_kind)
    {
        ast::star_variant_kind sk2  = k_req.static_cast_to<ast::star_variant_kind>();
        fixed                      = fixed || sk2.is_fixed();

        if (sk2.is_star() == true)
        {
            res = sk1.make_star();
            return;
        };

        if (sk1.size() != sk2.size())
            return unify_stars(sk1, k_req, fixed, res);

        long n  = sk1.size();
        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = sk1.get_identifier(i);
            const ast::kind& ki1        = sk1.get_kind(i);

            if (sk2.has_field(id) == false)
                return unify_stars(sk1, k_req, fixed, res);

            const ast::kind& ki2        = sk2.get_field(id);

            res = make(ki1, ki2);

            if (res == false)
                return unify_stars(sk1, k_req, fixed, res);
        };

        make_fixed(sk1, sk2, fixed);
        res = true;
        return;
    };

    return unify_stars(sk1, k_req, fixed, res);
}

void unify_kinds_vis::unify_stars(const ast::kind& k_inf, const ast::kind& k_req, 
                                  bool fixed, bool& res)
{
    if (fixed == true)
    {
        res = false;
        return;
    }

    res = is_star_star_kind(k_inf, true);

    if (res == false)
        return;

    res = is_star_star_kind(k_req, true);
};

bool unify_kinds_vis::is_star_star_kind(const ast::kind& k, bool force_star)
{
    return is_star_star_vis().make(k, force_star);
};

};};};