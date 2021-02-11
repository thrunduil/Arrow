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

#include "type_equaler.h"
#include "semantics/typing/types/kind_checker.h"
#include "scopes/symbol_data.h"
#include "scopes/module.h"
#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "semantics/semantics.h"
#include "semantics/typing/types/unifier_kinds.h"
#include "semantics/typing/types/computation_on_types.h"
#include "unify_types_vis.h"
#include "semantics/typing/types/normal_form_type.h"

#include <iostream>

namespace arrow { namespace typing
{

type_equaler::type_equaler(kind_checker* kc, const module& mod, const error_handler_ptr& eh,
                           inst_collector* col)
    :m_kind_checker(kc), m_mod(mod), m_eh(eh), m_bound_map(nullptr), m_collector(col)
{};

void type_equaler::set_collector(inst_collector* col)
{
    m_collector = col;
}

void type_equaler::start()
{
    static bound_map empty;
    empty.clear();

    m_bound_map = &empty;
};

bool type_equaler::unify_with_value_kind(const ast::type& t0, ast::predefined_type_code& code, 
                                         const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::int_type)
    {
        code = ast::predefined_type_code::type_int;
        return true;
    }
    if (t->get_type_code() == ast::details::type_code::bool_type)
    {
        code = ast::predefined_type_code::type_bool;
        return true;
    }
    if (t->get_type_code() == ast::details::type_code::char_type)
    {
        code = ast::predefined_type_code::type_char;
        return true;
    }
    if (t->get_type_code() == ast::details::type_code::string_type)
    {
        code = ast::predefined_type_code::type_string;
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_value_kind(t, code, ctx);
        else
            return false;
    }

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_value_kind(tv.get(), code, ctx);

    //it is not possible that a type with value kind is given by type variable
    return false;
};

bool type_equaler::is_int_value_kind(const ast::type& t0, ast::int_type& it, 
                                     const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::int_type)
    {
        it = t.static_cast_to<ast::int_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return is_int_value_kind(t, it, ctx);
        else
            return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_int_value_kind(tv.get(), it, ctx);

    //it is not possible that a type with value kind is given by type variable
    return false;
};
bool type_equaler::is_bool_value_kind(const ast::type& t0, ast::bool_type& it, 
                                      const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::bool_type)
    {
        it = t.static_cast_to<ast::bool_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return is_bool_value_kind(t, it, ctx);
        else
            return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_bool_value_kind(tv.get(), it, ctx);

    //it is not possible that a type with value kind is given by type variable
    return false;
};

bool type_equaler::is_char_value_kind(const ast::type& t0, ast::char_type& it, 
                                      const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::char_type)
    {
        it = t.static_cast_to<ast::char_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return is_char_value_kind(t, it, ctx);
        else
            return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_char_value_kind(tv.get(), it, ctx);

    //it is not possible that a type with value kind is given by type variable
    return false;
};

bool type_equaler::is_string_value_kind(const ast::type& t0, ast::string_type& it, 
                                      const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::string_type)
    {
        it = t.static_cast_to<ast::string_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return is_string_value_kind(t, it, ctx);
        else
            return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_string_value_kind(tv.get(), it, ctx);

    //it is not possible that a type with value kind is given by type variable
    return false;
};

bool type_equaler::unify_with_predefined(const ast::type& t0, ast::predefined_type_code tc,
                                         const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::predefined_type)
    {
        ast::predefined_type pt = t.static_cast_to<ast::predefined_type>();
        return tc == pt.get_type_code();
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_predefined(t, tc, ctx);
        else
            return false;
    };


    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_predefined(tv.get(), tc, ctx);

    ast::predefined_type pt  = ast::predefined_type(tv->get_location(), tc);

    tv.set_checked(pt);
    return true;
}

bool type_equaler::unify_with_tuple(const ast::type& t0, long size, ast::tuple_type& ret,
                                    const context_type& ctx)
{
    start();
    ast::type t = simplify_type_rec(t0);
    
    if (t->get_type_code() == ast::details::type_code::tuple_type)
    {
        ast::tuple_type tt = t.static_cast_to<ast::tuple_type>();
        ret = tt;
        return size == tt.size();
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_tuple(t, size, ret, ctx);

        ret = ast::tuple_type();
        return false;
    }

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_tuple(tv.get(), size, ret, ctx);

    ast::tuple_type tt  = ast::tuple_type(tv->get_location(), ast::tuple_type::fixed);

    for (long i = 0; i < size; ++i)
    {
        ast::type_var tvi   = ast::type_var(tv->get_location());
        tt.add(tvi);
    }

    tv.set_checked(tt);
    ret = tt;
    return true;
}

bool type_equaler::is_tuple_type(const ast::type& t0, ast::tuple_type& ret, 
                                 const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::tuple_type)
    {
        ret = t.static_cast_to<ast::tuple_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return is_tuple_type(t, ret, ctx);

        ret = ast::tuple_type();
        return false;
    }

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_tuple_type(tv.get(), ret, ctx);

    return false;
}
bool type_equaler::unify_with_ref(const ast::type& t0, ast::ref_type& ret, 
                                  const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::ref_type)
    {
        ret = t.static_cast_to<ast::ref_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_ref(t, ret, ctx);

        ret = ast::ref_type();
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_ref(tv.get(), ret, ctx);

    ast::type_var tv1   = ast::type_var(tv->get_location());
    ast::ref_type rt    = ast::ref_type(tv->get_location(), tv1);

    tv.set_checked(rt);
    ret = rt;
    return true;
}

bool type_equaler::unify_with_list(const ast::type& t0, ast::list_type& ret, 
                                   const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::list_type)
    {
        ret = t.static_cast_to<ast::list_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_list(t, ret, ctx);

        ret = ast::list_type();
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_list(tv.get(), ret, ctx);

    ast::type_var tv1   = ast::type_var(tv->get_location());
    ast::list_type rt   = ast::list_type(tv->get_location(), tv1);

    tv.set_checked(rt);
    ret = rt;
    return true;
};

bool type_equaler::unify_with_arrow(const ast::type& t0, ast::arrow_type& ret,
                                    const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::arrow_type)
    {
        ret = t.static_cast_to<ast::arrow_type>();
        return true;
    }

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs   = make_substitution(t, ctx);
        
        if (subs == true)
            return unify_with_arrow(t, ret, ctx);

        ret = ast::arrow_type();
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return unify_with_arrow(tv.get(), ret, ctx);

    ast::type_var tv_l  = ast::type_var(tv->get_location());
    ast::type_var tv_r  = ast::type_var(tv->get_location());

    ast::arrow_type at  = ast::arrow_type(tv->get_location(), tv_l, tv_r);
    tv.set_checked(at);
    ret = at;
    return true;
}

bool type_equaler::deduce_arrow_all(const ast::type& t0, ast::arrow_type& ret, 
                            var_substitution& subs, const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
 
    if (t->get_type_code() == ast::details::type_code::arrow_type)
    {
        ret = t.static_cast_to<ast::arrow_type>();
        return true;
    }

    if (t->get_type_code() == ast::details::type_code::all_type)
    {
        ast::all_type at    = t.static_cast_to<ast::all_type>();
        ast::type tail;
    
        prepare_deduction_all(subs, at, tail, ctx);
        return unify_with_arrow(tail, ret, ctx);
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
    {
        bool subs_loc   = make_substitution(t, ctx);
        
        if (subs_loc == true)
            return deduce_arrow_all(t, ret, subs, ctx);

        ret = ast::arrow_type();
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return deduce_arrow_all(tv.get(), ret, subs, ctx);

    ast::type_var tv_l  = ast::type_var(tv->get_location());
    ast::type_var tv_r  = ast::type_var(tv->get_location());

    ast::arrow_type at  = ast::arrow_type(tv->get_location(), tv_l, tv_r);
    tv.set_checked(at);
    ret = at;
    return true;
}

bool type_equaler::is_record_type(const ast::type& t0, ast::record_type& ret, ast::type_var& tv_ret,
                                  bool& is_var, const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);

    if (t->get_type_code() == ast::details::type_code::record_type)
    {
        ret     = t.static_cast_to<ast::record_type>();
        is_var  = false;
        return true;
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs_loc   = make_substitution(t, ctx);
        
        if (subs_loc == true)
            return is_record_type(t, ret, tv_ret, is_var, ctx);

        ret     = ast::record_type();
        is_var  = false;
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_record_type(tv.get(), ret, tv_ret, is_var, ctx);

    is_var  = true;
    tv_ret  = tv;
    return false;
}

bool type_equaler::is_variant_type(const ast::type& t0, ast::variant_type& ret, const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);

    if (t->get_type_code() == ast::details::type_code::variant_type)
    {
        ret     = t.static_cast_to<ast::variant_type>();
        return true;
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs_loc   = make_substitution(t, ctx);
        
        if (subs_loc == true)
            return is_variant_type(t, ret, ctx);

        ret     = ast::variant_type();
        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_variant_type(tv.get(), ret, ctx);

    ret     = ast::variant_type();
    return false;
}

bool type_equaler::is_all_type(const ast::type& t0, ast::all_type& ret, ast::type_var& tv_ret, 
                               bool& is_var, const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::all_type)
    {
        ret     = t.static_cast_to<ast::all_type>();
        is_var  = false;
        
        return true;
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs_loc   = make_substitution(t, ctx);
        
        if (subs_loc == true)
            return is_all_type(t, ret, tv_ret, is_var, ctx);

        ret     = ast::all_type();
        is_var  = false;

        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_all_type(tv.get(), ret, tv_ret, is_var, ctx);

    is_var  = true;
    tv_ret  = tv;
    return false;
}

bool type_equaler::is_some_type(const ast::type& t0, ast::some_type& ret, ast::type_var& tv_ret, 
                                bool& is_var, const context_type& ctx)
{
    start();

    ast::type t = simplify_type_rec(t0);
    if (t->get_type_code() == ast::details::type_code::some_type)
    {
        ret     = t.static_cast_to<ast::some_type>();
        is_var  = false;
        
        return true;
    };

    if (t->get_type_code() != ast::details::type_code::type_var)
    {        
        bool subs_loc   = make_substitution(t, ctx);
        
        if (subs_loc == true)
            return is_some_type(t, ret, tv_ret, is_var, ctx);

        ret     = ast::some_type();
        is_var  = false;

        return false;
    };

    ast::type_var tv    = t.static_cast_to<ast::type_var>();

    if (tv.is_set() == true)
        return is_some_type(tv.get(), ret, tv_ret, is_var, ctx);

    is_var  = true;
    tv_ret  = tv;
    return false;
}

bool type_equaler::unify_types(const ast::location& loc, const ast::type& t1_0, const ast::type& t2_0,
                               bound_map& bound, const context_type& ctx, bool show_errors)
{
    start();
    m_bound_map = &bound;
    bool res    = unify_types_impl(loc, t1_0, t2_0, ctx, show_errors);
    m_bound_map = nullptr;

    return res;
};

bool type_equaler::deduce_subs(const ast::location& loc, const ast::type& subs_type, 
                const ast::type& target_type, bound_map& bound, const context_type& ctx, 
                var_substitution& subs, bool show_errors)
{
    start();
    m_bound_map = &bound;

    bool res = details::unify_types_vis(loc, m_eh, m_mod, *m_bound_map, ctx, m_collector)
                        .deduce(subs_type, target_type, subs, show_errors);  

    m_bound_map = nullptr;

    return res;
};

ast::kind type_equaler::infer_kind(ast::type& t, context_type& ctx,
                                   inst_collector* collector)
{
    ast::kind_var kv(t->get_location(), false);
    ast::kind k = kv;
    m_kind_checker->check_simple(t, k, ctx, collector);
    return k;
};

bool type_equaler::unify_types_impl(const ast::location& loc, const ast::type& t1, 
                        const ast::type& t2, const context_type& ctx, bool show_errors)
{
    return details::unify_types_vis(loc, m_eh, m_mod, *m_bound_map, ctx, m_collector)
                        .make(t1, t2, show_errors);    
}

bool type_equaler::make_substitution(ast::type& t, const context_type& ctx)
{
    return ctx.make_substitution(t);
}

ast::type type_equaler::simplify_type_rec(const ast::type& t)
{
    normal_form_type nft(m_eh, m_mod, m_collector, true);
    int num_unfolding;
    ast::type ret = nft.make(t, num_unfolding);
    return ret;
};

void type_equaler::prepare_deduction_all(var_substitution& subs, const ast::type& at0, 
                        ast::type& tail, const context_type& ctx)
{
    ast::all_type at;
    {
        ast::type_var tv;
        bool is_var;

        if (is_all_type(at0, at, tv, is_var, ctx) == false)
        {
            tail = at0;
            return;
        }
    };

    using annot_id              = ast::annotated_type_identifier;
    const annot_id& aid         = at.get_var();
    const ast::type& body       = at.get_body();

    ast::type_var tv    = ast::type_var(aid->get_location());
    subs.add(aid, tv);
    return prepare_deduction_all(subs, body, tail, ctx);
};

};};