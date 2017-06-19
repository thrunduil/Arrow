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

#include "unifier_kinds.h"
#include "error/error.h"
#include "bound_map.h"
#include "unify_kinds_vis.h"
#include "unify_subkinds_vis.h"

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              unifier_kinds
//------------------------------------------------------------------
unifier_kinds::unifier_kinds(const error_handler_ptr& eh)
    :m_eh(eh), m_show_errors(true)
{};

bool unifier_kinds::unify_kinds(const ast::location& loc, const ast::kind& k_inf, 
                               const ast::kind& k_req, bool show_errors) const
{
    m_show_errors = show_errors;
    bound_map bm;
    return details::unify_kinds_vis(loc, bm, m_show_errors, m_eh).make(k_inf, k_req);

};

bool unifier_kinds::is_subkind(const ast::location& loc, const ast::kind& k_spec, 
                const ast::kind& k_gen, bool show_errors) const
{
    m_show_errors = show_errors;
    bound_map bm;
    return details::unify_subkinds_vis(loc, bm, m_show_errors, m_eh).make(k_spec, k_gen);

};

bool unifier_kinds::expand_products(ast::product_kind& pk1, ast::product_kind& pk2)
{
    long n1         = pk1.size();
    long n2         = pk2.size();
    long n;

    bool fixed_1    = pk1.is_size_fixed();
    bool fixed_2    = pk2.is_size_fixed();

    if (fixed_1 == true && fixed_2 == true)
    {
        if (n1 != n2)
            return false;
        n = n1;
    }
    else if (fixed_1 == true)
    {
        if (n2 > n1)
            return false;
        n = n1;

        for (long i = n2; i < n; ++i)
            pk2.add(pk1.get(i));
    }
    else if (fixed_2 == true)
    {
        if (n1 > n2)
            return false;
        n = n2;

        for (long i = n1; i < n; ++i)
            pk1.add(pk2.get(i));
    }
    else
    {
        if (n1 < n2)
        {
            n = n2;
            for (long i = n1; i < n; ++i)
                pk1.add(pk2.get(i));
        }
        else
        {
            n = n1;
            for (long i = n2; i < n; ++i)
                pk2.add(pk1.get(i));
        };
    }

    return true;
};

bool unifier_kinds::is_star_kind_any(const ast::kind& k)
{
    return details::is_star_any_vis().make(k);
};

bool unifier_kinds::is_star_star_kind(const ast::kind& k, bool force_star)
{
    return details::is_star_star_vis().make(k, force_star);
};

bool unifier_kinds::unify_with_star_any(const ast::kind& k) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_any(kv1.get());

        kv1.set_star_var();
        return true;
    }

    if (is_star_kind_any(k) == true)
        return true;
    else
        return false;
};

bool unifier_kinds::unify_with_star_arrow(const ast::kind& k, ast::kind& lhs, ast::kind& rhs,
                                          bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_arrow(kv1.get(), lhs, rhs, must_check_star);

        lhs = ast::kind_var(k->get_location(), true);
        rhs = ast::kind_var(k->get_location(), true);

        ast::kind k_subs    = ast::star_arrow_kind(k->get_location(), lhs, rhs, 
                                ast::star_kind_state::flexible);
        must_check_star     = false;

        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_arrow_kind)
    {
        ast::star_arrow_kind ak = k.static_cast_to<ast::star_arrow_kind>();
        lhs             = ak.get_lhs();
        rhs             = ak.get_rhs();
        must_check_star = (ak.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        lhs = ast::kind_var(k->get_location(), true);
        rhs = ast::kind_var(k->get_location(), true);
        must_check_star = false;
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_list(const ast::kind& k, ast::kind& body, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_list(kv1.get(), body, must_check_star);

        body = ast::kind_var(k->get_location(), true);

        ast::kind k_subs    = ast::star_list_kind(k->get_location(), body,
                                ast::star_kind_state::flexible);
        must_check_star     = false;
        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_list_kind)
    {
        ast::star_list_kind ak = k.static_cast_to<ast::star_list_kind>();
        body            = ak.get_body();
        must_check_star = (ak.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        body = ast::kind_var(k->get_location(), true);
        must_check_star = false;
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_ref(const ast::kind& k, ast::kind& body,
                                        bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_ref(kv1.get(), body, must_check_star);

        body = ast::kind_var(k->get_location(), true);

        ast::kind k_subs    = ast::star_ref_kind(k->get_location(), body,
                                ast::star_kind_state::flexible);
        must_check_star     = false;
        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_ref_kind)
    {
        ast::star_ref_kind ak = k.static_cast_to<ast::star_ref_kind>();
        body            = ak.get_body();
        must_check_star = (ak.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        body = ast::kind_var(k->get_location(), true);
        must_check_star = false;
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_predefined(const ast::kind& k, 
                               ast::predefined_type_code code, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_predefined(kv1.get(), code, must_check_star);

        ast::kind k_subs    = ast::star_predefined_kind(k->get_location(), code,
                                ast::star_kind_state::flexible);
        must_check_star     = false;
        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_predefined_kind)
    {
        ast::star_predefined_kind ak = k.static_cast_to<ast::star_predefined_kind>();
        must_check_star     = (ak.is_fixed() == true);
        if (code == ak.get_code())
            return true;
    };

    must_check_star = false;

    if (is_star_star_kind(k, true) == true)
        return true;

    return false;
};

bool unifier_kinds::unify_with_star_all(const ast::kind& k, ast::kind& var, ast::kind& body,
                                        bool star_allowed, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_all(kv1.get(), var, body, star_allowed, must_check_star);

        var     = ast::kind_var(k->get_location(), false);
        body    = ast::kind_var(k->get_location(), true);

        ast::kind k_subs    = ast::star_all_kind(k->get_location(), var, body,
                                ast::star_kind_state::flexible);
        must_check_star     = false;
        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_all_kind)
    {
        ast::star_all_kind ak = k.static_cast_to<ast::star_all_kind>();
        var             = ak.get_var();
        body            = ak.get_body();
        must_check_star = (ak.is_fixed() == false);
        return true;
    };

    if (star_allowed == true && is_star_star_kind(k, true) == true)
    {
        var     = ast::kind_var(k->get_location(), false);
        body    = ast::kind_var(k->get_location(), true);
        must_check_star = false;
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_some(const ast::kind& k, ast::kind& var, ast::kind& body,
                                         bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_some(kv1.get(), var, body, must_check_star);

        var     = ast::kind_var(k->get_location(), false);
        body    = ast::kind_var(k->get_location(), true);

        ast::kind k_subs    = ast::star_some_kind(k->get_location(), var, body,
                                ast::star_kind_state::flexible);
        must_check_star     = false;
        kv1.set_checked(k_subs);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_some_kind)
    {
        ast::star_some_kind ak = k.static_cast_to<ast::star_some_kind>();
        var             = ak.get_var();
        body            = ak.get_body();
        must_check_star = (ak.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        var     = ast::kind_var(k->get_location(), false);
        body    = ast::kind_var(k->get_location(), true);
        must_check_star = false;
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_tuple(const ast::kind& k, long size, 
                ast::star_tuple_kind& tup, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_tuple(kv1.get(), size, tup, must_check_star);

        tup             = ast::star_tuple_kind(k->get_location(), true,
                            ast::star_kind_state::flexible);
        must_check_star = false;

        for (long i = 0; i < size; ++i)
            tup.add(ast::kind_var(k->get_location(), true));

        kv1.set_checked(tup);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_tuple_kind)
    {
        ast::star_tuple_kind st = k.static_cast_to<ast::star_tuple_kind>();
        must_check_star         = (st.is_fixed() == false);

        if (st.size() == size)
        {
            tup = st;
            return true;
        };
    };

    if (is_star_star_kind(k, true) == true)
    {
        tup             = ast::star_tuple_kind(k->get_location(), true,
                            ast::star_kind_state::flexible);
        must_check_star = false;

        for (long i = 0; i < size; ++i)
            tup.add(ast::kind_var(k->get_location(), true));
        
        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_record(const ast::kind& k, const ast::record_type& rec,
                ast::star_record_kind& rec_kind, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_record(kv1.get(), rec, rec_kind, must_check_star);

        rec_kind        = ast::star_record_kind(k->get_location(),
                            ast::star_kind_state::flexible);
        must_check_star = false;

        long n      = rec.size();

        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rec.get_identifier(i).get_identifier();
            ast::kind kv    = ast::kind_var(k->get_location(), true);

            rec_kind.add(id, kv);
        }

        kv1.set_checked(rec_kind);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_record_kind)
    {
        ast::star_record_kind rk = k.static_cast_to<ast::star_record_kind>();
        rec_kind        = rk;
        must_check_star = (rk.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        rec_kind        = ast::star_record_kind(k->get_location(), 
                            ast::star_kind_state::flexible);
        must_check_star = false;
        long n          = rec.size();

        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rec.get_identifier(i).get_identifier();
            ast::kind kv                = ast::kind_var(k->get_location(), true);
            rec_kind.add(id, kv);
        }        

        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_star_variant(const ast::kind& k, const ast::variant_type& rec,
                ast::star_variant_kind& rec_kind, bool& must_check_star) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_star_variant(kv1.get(), rec, rec_kind, must_check_star);

        rec_kind        = ast::star_variant_kind(k->get_location(), 
                            ast::star_kind_state::flexible);
        must_check_star = false;
        long n          = rec.size();

        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rec.get_identifier(i).get_identifier();
            ast::kind kv    = ast::kind_var(k->get_location(), true);

            rec_kind.add(id, kv);
        }

        kv1.set_checked(rec_kind);
        return true;
    }

    if (k->get_kind_code() == ast::details::kind_code::star_variant_kind)
    {
        ast::star_variant_kind vk = k.static_cast_to<ast::star_variant_kind>();
        rec_kind        = vk;
        must_check_star = (vk.is_fixed() == false);
        return true;
    };

    if (is_star_star_kind(k, true) == true)
    {
        rec_kind        = ast::star_variant_kind(k->get_location(),
                            ast::star_kind_state::flexible);
        long n          = rec.size();
        must_check_star = false;

        for (long i = 0; i < n; ++i)
        {
            const ast::identifier& id   = rec.get_identifier(i).get_identifier();
            ast::kind kv                = ast::kind_var(k->get_location(), true);
            rec_kind.add(id, kv);
        }        

        return true;
    }

    must_check_star = false;
    return false;
};

bool unifier_kinds::unify_with_value(const ast::kind& k, ast::value_kind_code code) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_value(kv1.get(), code);

        if (kv1.is_star_var() == true)
            return false;

        kv1.set_checked(ast::value_kind(k->get_location(), code));
        return true;
    }

    if (k->get_kind_code() != ast::details::kind_code::value_kind)
        return false;

    ast::value_kind vk  = k.static_cast_to<ast::value_kind>();
    return vk.get_code() == code;
};

bool unifier_kinds::is_value_kind_any(const ast::kind& k) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return is_value_kind_any(kv1.get());

        return false;
    }

    if (k->get_kind_code() != ast::details::kind_code::value_kind)
        return false;
    else
        return true;
};

bool unifier_kinds::unify_with_arrow(const ast::kind& k, ast::arrow_kind& ak,
                                    ast::polarity_type pol) const
{    
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_arrow(kv1.get(), ak, pol);

        ast::kind_var k1    = ast::kind_var(k->get_location(), false);
        ast::kind_var k2    = ast::kind_var(k->get_location(), false);
        ak                  = ast::arrow_kind(k->get_location(), pol, k1, k2);

        if (kv1.is_star_var() == true)
            return false;

        kv1.set_checked(ak);
        return true;
    }

    if (k->get_kind_code() != ast::details::kind_code::arrow_kind)
        return false;

    ak = k.static_cast_to<ast::arrow_kind>();
    return true;
};

bool unifier_kinds::unify_with_product(const ast::kind& k, ast::product_kind& pk, 
                long size) const
{
    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_product(kv1.get(), pk, size);

        pk                  = ast::product_kind(k->get_location(), true);
        for (long i = 0; i < size; ++i)
        {
            ast::kind_var ki= ast::kind_var(pk->get_location(), false);
            pk.add(ki);
        };

        if (kv1.is_star_var() == true)
            return false;

        kv1.set_checked(pk);
        return true;
    }

    if (k->get_kind_code() != ast::details::kind_code::product_kind)
        return false;

    pk      = k.static_cast_to<ast::product_kind>();
    long n  = pk.size();

    if (pk.is_size_fixed() == true)
    {
        if (n != size)
            return false;
        else
            return true;
    };

    if (n == size)
        return true;
    else if (n > size)
        return false;

    for (long i = n; i < size; ++i)
    {
        ast::kind_var ki= ast::kind_var(pk->get_location(), false);
        pk.add(ki);
    };

    return true;
};

bool unifier_kinds::unify_with_any_product(const ast::kind& k, ast::product_kind& pk, 
                long req_index) const
{
    if (req_index < 0)
        return false;

    bool is_var_1   = k->get_kind_code() == ast::details::kind_code::kind_var;

    if (is_var_1 == true)
    {
        ast::kind_var kv1   = k.static_cast_to<ast::kind_var>();

        if (kv1.is_set() == true)
            return unify_with_any_product(kv1.get(), pk, req_index);

        pk                  = ast::product_kind(k->get_location(), false);
        for (long i = 0; i <= req_index; ++i)
        {
            ast::kind_var ki= ast::kind_var(pk->get_location(), false);
            pk.add(ki);
        };

        if (kv1.is_star_var() == true)
            return false;

        kv1.set_checked(pk);
        return true;
    }

    if (k->get_kind_code() != ast::details::kind_code::product_kind)
        return false;

    pk      = k.static_cast_to<ast::product_kind>();
    long n  = pk.size();

    if (pk.is_size_fixed() == true)
    {
        if (req_index >= n)
            return false;
        else
            return true;
    };

    if (req_index < n)
        return true;

    for (long i = n; i <= req_index; ++i)
    {
        ast::kind_var ki= ast::kind_var(pk->get_location(), false);
        pk.add(ki);
    };

    return true;
};

bool unifier_kinds::unify_identifiers(const ast::location& loc, const ast::identifier& id1, 
                const ast::identifier& id2, const bound_map& bm)
{
    (void)loc;

    ast::symbol_data* sd1       = id1.get_symbol_data();
    ast::symbol_data* sd2       = id2.get_symbol_data();

    ast::identifier_kind ik1    = sd1->get_kind();
    ast::identifier_kind ik2    = sd2->get_kind();

    if (ik1.is_sector_kind() == false)
        return false;
    if (ik2.is_sector_kind() == false)
        return false;

    long code1  = bm.get_code(id1);
    long code2  = bm.get_code(id2);

    if (bound_map::is_bound(code1) == true && bound_map::is_bound(code2) == true)
        return code1 == code2;
    else if (bound_map::is_bound(code1) == false && bound_map::is_bound(code2) == false)
        return ad::var_info(id1) == ad::var_info(id2);
    else
        return false;
};

bool unifier_kinds::is_polarity_less(ast::polarity_type pol_spec, ast::polarity_type pol_gen)
{
    return details::unify_subkinds_vis::is_polarity_less(pol_spec, pol_gen);
};

};};