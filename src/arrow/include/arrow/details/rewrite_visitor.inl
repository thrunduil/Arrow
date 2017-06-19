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

#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                  VALUES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(int32_t val, Args&& ...)
    -> int32_t
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const std::string& val, Args&& ...)  
    -> std::string
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(int64_t val, Args&& ...)
    -> int64_t
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(bool val, Args&& ...)
    -> bool
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(double val, Args&& ...)
    -> double
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(unique_string val, Args&& ...)
    -> unique_string
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const operator_info& val, Args&& ...)
    -> operator_info
{
    return val;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(star_kind_state val, Args&& ...)
    -> star_kind_state
{
    return val;
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(predefined_type_code val, Args&& ...)
    -> predefined_type_code
{
    return val;
};

//---------------------------------------------------------------
//                  OTHER
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const module_definition& ast, Args&& ... args)
            -> module_definition
{
    bool changed    = false;

    using string_loc        = std::pair<unique_string, location>;
    using string_vec        = std::vector<string_loc>;

    const std::string& name = ast.get_module_name() const;
    const string_vec& imp   = ast.get_imports() const;
    const definitions& defs = ast.get_definitions() const;

    std::string name2       = visit(name, std::forward<Args>(args)...);
    definitions defs2       = visit(defs, std::forward<Args>(args)...);

    changed                 |= name2 != name;
    changed                 |= defs2 != defs;

    string_vec imp2;
    imp2.reserve(imp.size());

    for (const string_loc& pos : imp)
    {
        unique_string us    = visit(pos.first, std::forward<Args>(args)...);
        location loc        = visit(pos.second, std::forward<Args>(args)...);

        imp2.push_back(string_loc(us, loc));

        changed             |= us != pos.first;
        changed             |= loc != pos.second;
    };

    if (changed == false)
        return ast;
    else
        return module_definition(name2, imp2, defs2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const definitions& ast, Args&& ... args)
            -> definitions
{
    long size       = ast.size();
    bool changed    = false;

    definitions ret(ast->get_location());

    for (long i = 0; i < size; ++i)
    {
        const definition& def   = ast.get_definition(i);
        definition def2         = visit(def, std::forward<Args>(args)...);
        changed                 |= def2 != def;

        ret.add(def2);
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const identifier& ast, Args&& ... args)
            -> identifier
{
    fixity_type ft;
    unique_string id    = ast.get_name(ft);
    symbol_data* sd     = ast.get_symbol_data();
    unique_string id2   = visit(id, std::forward<Args>(args)...);    

    if (id2 == id)
        return ast;
    else
        return identifier(ast->get_location(), id2, ft, sd);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const annotated_type_identifier& ast, Args&& ... args)
            -> annotated_type_identifier
{
    const identifier& id    = ast.get_identifier();
    const kind& ki          = ast.get_kind();

    identifier id2          = visit(id, std::forward<Args>(args)...);
    kind ki2                = visit(ki, std::forward<Args>(args)...);

    if (id2 == id && ki2 == ki)
        return ast;
    else
        return annotated_type_identifier(ast->get_location(), id2, ki2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const annotated_identifier& ast, Args&& ... args)
            -> annotated_identifier
{
    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_type();

    identifier id2          = visit(id, std::forward<Args>(args)...);
    type t2                 = visit(t, std::forward<Args>(args)...);

    if (id2 == id && t2 == t)
        return ast;
    else
        return annotated_identifier(ast->get_location(), id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const symbol_sequence& ast, Args&& ... args)
            -> symbol_sequence
{
    long s          = ast.size();
    bool changed    = false;

    symbol_sequence ret(ast->get_location());

    for (int i = 0; i < s; ++i)
    {
        const identifier& v = ast.get(i);
        identifier v2       = visit(v, std::forward<Args>(args)...);

        if (v2 != v)
            changed  = true;

        ret.add(v2);
    };
        
    if (changed == true)
        return ret;
    else
        return ast;
};

//---------------------------------------------------------------
//                  DEFINITIONS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_definition& ast, Args&& ... args)
            -> term_definition
{
    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_type();
    const term& ex          = ast.get_body();
    bool is_over            = ast.is_overload();

    identifier id2          = visit(id, std::forward<Args>(args)...);
    type t2                 = t? visit(t, std::forward<Args>(args)...) : t;
    term ex2                = ex ? visit(ex, std::forward<Args>(args)...) : ex;

    if (id2 == id && t2 == t && ex2 == ex)
        return ast;
    else
        return term_definition(ast->get_location(), id2, t2, ex2, is_over);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const fixity_definition& ast, Args&& ... args)
            -> fixity_definition
{
    const operator_info& decl   = ast.get_operator_type();
    const symbol_sequence& seq  = ast.get_symbols();

    operator_info decl2         = visit(decl, std::forward<Args>(args)...);
    symbol_sequence seq2        = visit(seq, std::forward<Args>(args)...);

    if (decl == decl2 && seq == seq2)
        return ast;
    else
        return fixity_definition(ast->get_location(), decl2, seq2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_definition& ast, Args&& ... args)
            -> type_definition
{
    const identifier& id    = ast.get_identifier();
    const kind& k           = ast.get_kind();
    const type& t           = ast.get_body();

    identifier id2          = visit(id, std::forward<Args>(args)...);
    kind k2                 = k ? visit(k, std::forward<Args>(args)...) : k;
    type t2                 = t ? visit(t, std::forward<Args>(args)...) : t;

    if (id2 == id && t2 == t && k2 == k)
        return ast;
    else
        return type_definition(ast->get_location(), id2, k2, t2, ast.is_overload());
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_definition_var& ast, Args&& ... args)
            -> type_definition_var
{
    using arg_type          = annotated_type_identifier;
    using id_vec            = std::vector<ast::identifier>;

    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_body();
    const kind& k           = ast.get_return_kind();
    const id_vec& kind_vars = ast.get_kind_vars();

    bool changed            = false;

    id_vec kind_vars2;
    kind_vars2.reserve(kind_vars.size());

    for (const auto& elem : kind_vars)
    {
        ast::identifier id2 = visit(elem, std::forward<Args>(args)...);

        changed             |= id2 != elem;

        kind_vars2.push_back(id2);
    };

    identifier id2          = visit(id, std::forward<Args>(args)...);
    type t2                 = visit(t, std::forward<Args>(args)...);
    kind k2                 = k ? visit(k, std::forward<Args>(args)...) : k;

    type_definition_var ret(ast->get_location(), id2, kind_vars2, ast.is_overload());
    ret.set_body(t2);

    if (k2)
        ret.set_return_kind(k2);
    
    long size               = ast.var_size();

    for (long i = 0; i < size; ++i)
    {
        const arg_type& arg = ast.get_arg(i);
        polarity_type pol   = ast.get_polarity(i);

        arg_type arg2       = visit(arg, std::forward<Args>(args)...);
        ret.add_var(arg2, pol);

        changed             |= arg2 != arg;
    };

    if (changed == false && id2 == id && t2 == t && k2 == k)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_use& ast, Args&& ... args)
            -> term_use
{
    const term& x           = ast.get_term();
    term x2                 = visit(x, std::forward<Args>(args)...);

    if (x2 == x)
        return ast;
    else
        return term_use(ast->get_location(), x2);
};

//---------------------------------------------------------------
//                  KINDS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_kind& ast, Args&& ...)
            -> star_kind
{
    return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const value_kind& ast, Args&& ...)
            -> value_kind
{
    return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const kind_identifier& ast, Args&& ... args)
            -> kind_identifier
{
    const identifier& i     = ast.get_identifier();
    identifier i2           = visit(i, std::forward<Args>(args)...);

    if (i2 == i)
        return ast;
    else
        return kind_identifier(ast->get_location(), i2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const kind_scope& ast, Args&& ... args)
            -> kind_scope
{
    const kind& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    kind t2                         = visit(t, std::forward<Args>(args)...);
    identifier id2                  = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return kind_scope(ast->get_location(), id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_all_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    const kind& t                   = ast.get_body();
    const kind& id                  = ast.get_var();
    star_kind_state st              = ast.get_state();

    star_kind_state st2             = visit(st, std::forward<Args>(args)...);
    kind t2                         = visit(t, std::forward<Args>(args)...);
    kind id2                        = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id && st2 == st)
        return ast;
    else
        return star_all_kind(ast->get_location(), id2, t2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_some_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    const kind& t                   = ast.get_body();
    const kind& id                  = ast.get_var();
    star_kind_state st              = ast.get_state();

    star_kind_state st2             = visit(st, std::forward<Args>(args)...);
    kind t2                         = visit(t, std::forward<Args>(args)...);
    kind id2                        = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id && st2 == st)
        return ast;
    else
        return star_some_kind(ast->get_location(), id2, t2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_list_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    const kind& t                   = ast.get_body();
    star_kind_state st              = ast.get_state();

    star_kind_state st2             = visit(st, std::forward<Args>(args)...);
    kind t2                         = visit(t, std::forward<Args>(args)...);

    if (t2 == t && st2 == st)
        return ast;
    else
        return star_list_kind(ast->get_location(), t2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_record_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());
    
    long size               = ast.size();
    star_kind_state st      = ast.get_state();

    star_kind_state st2     = visit(st, std::forward<Args>(args)...);
    star_record_kind ret    = star_record_kind(ast->get_location(), st2);

    bool changed            = st2 != st;

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_kind(i);
        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ty2            = visit(ty, std::forward<Args>(args)...);

        changed             |= id2 != id;
        changed             |= ty2 != ty;
        ret.add(id2, ty2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_variant_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());
    
    long size               = ast.size();
    star_kind_state st      = ast.get_state();

    star_kind_state st2     = visit(st, std::forward<Args>(args)...);
    star_variant_kind ret   = star_variant_kind(ast->get_location(), st2);

    bool changed            = st2 != st;

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_kind(i);
        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ty2            = visit(ty, std::forward<Args>(args)...);

        changed             |= id2 != id;
        changed             |= ty2 != ty;
        ret.add(id2, ty2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_predefined_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    star_kind_state st              = ast.get_state();
    star_kind_state st2             = visit(st, std::forward<Args>(args)...);

    if (st2 == st)
        return ast;
    else
        return star_predefined_kind(ast->get_location(), ast.get_code(), st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_ref_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    const kind& t                   = ast.get_body();
    star_kind_state st              = ast.get_state();

    star_kind_state st2             = visit(st, std::forward<Args>(args)...);
    kind t2                         = visit(t, std::forward<Args>(args)...);

    if (t2 == t && st2 == st)
        return ast;
    else
        return star_ref_kind(ast->get_location(), t2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const product_kind& ast, Args&& ... args)
            -> product_kind
{
    bool changed            = false;
    long size               = ast.size();
    bool fixed              = ast.is_size_fixed();

    product_kind ret        = product_kind(ast->get_location(), fixed);

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        auto arg2           = visit(arg, std::forward<Args>(args)...);

        changed             |= arg2 != arg;
        ret.add(arg2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_tuple_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());
    
    long size               = ast.size();
    bool fixed              = ast.is_size_fixed();
    star_kind_state st      = ast.get_state();

    star_kind_state st2     = visit(st, std::forward<Args>(args)...);
    star_tuple_kind ret     = star_tuple_kind(ast->get_location(), fixed, st2);

    bool changed            = st2 != st;

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        auto arg2           = visit(arg, std::forward<Args>(args)...);

        changed             |= arg2 != arg;
        ret.add(arg2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const arrow_kind& ast, Args&& ... args)
            -> arrow_kind
{
    const kind& kl          = ast.get_lhs();
    const kind& kr          = ast.get_rhs();
    polarity_type akt       = ast.get_type();

    kind kl2                = visit(kl, std::forward<Args>(args)...);
    kind kr2                = visit(kr, std::forward<Args>(args)...);

    if (kl2 == kl && kr2 == kr)
        return ast;
    else
        return arrow_kind(ast->get_location(), akt, kl2, kr2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const star_arrow_kind& ast, Args&& ... args)
            -> kind
{
    if (ast.is_star())
        return star_kind(ast->get_location());

    const kind& kl          = ast.get_lhs();
    const kind& kr          = ast.get_rhs();
    star_kind_state st      = ast.get_state();

    kind kl2                = visit(kl, std::forward<Args>(args)...);
    kind kr2                = visit(kr, std::forward<Args>(args)...);
    star_kind_state st2     = visit(st, std::forward<Args>(args)...);

    if (kl2 == kl && kr2 == kr && st2 == st)
        return ast;
    else
        return star_arrow_kind(ast->get_location(), kl2, kr2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const kind_var& ast, Args&& ... args)
            -> kind
{
    if (ast.is_set() == true)
        return visit(ast.get(), std::forward<Args>(args)...);
    else
        return ast;
};

//---------------------------------------------------------------
//                  TYPES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const arrow_type& ast, Args&& ... args)
            -> arrow_type
{
    const type& tl          = ast.get_lhs();
    const type& tr          = ast.get_rhs();
    type tl2                = visit(tl, std::forward<Args>(args)...);
    type tr2                = visit(tr, std::forward<Args>(args)...);

    if (tl2 == tl && tr2 == tr)
        return ast;
    else
        return arrow_type(ast->get_location(), tl2, tr2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const error_type& ast, Args&& ...)
            -> error_type
{
    return ast;
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const int_type& ast, Args&& ... args)
            -> int_type
{
    auto val        = ast.get_value();
    auto val2       = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return int_type(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const bool_type& ast, Args&& ... args)
            -> bool_type
{
    auto val        = ast.get_value();
    auto val2       = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return bool_type(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const char_type& ast, Args&& ... args)
            -> char_type
{
    auto val        = ast.get_value();
    auto val2       = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return char_type(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const string_type& ast, Args&& ... args)
            -> string_type
{
    auto val        = ast.get_value();
    auto val2       = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return string_type(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const path_int_type& ast, Args&& ... args)
            -> path_int_type
{
    const type& ty                  = ast.get_type();
    const int_literal& id           = ast.get_index();

    type ty2                        = visit(ty, std::forward<Args>(args)...);
    int_literal id2                 = visit(id, std::forward<Args>(args)...);

    if (ty2 == ty && id2 == id)
        return ast;
    else
        return path_int_type(ast->get_location(), ty2, id2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const ref_type& ast, Args&& ... args)
            -> ref_type
{
    const type& t           = ast.get_type();
    type t2                 = visit(t, std::forward<Args>(args)...);

    if (t2 == t)
        return ast;
    else
        return ref_type(ast->get_location(), t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const if_type& ast, Args&& ... args)
            -> if_type
{
    const type& e_if    = ast.get_if();
    const type& e_then  = ast.get_then();
    const type& e_else  = ast.get_else();

    type if2            = visit(e_if, std::forward<Args>(args)...);
    type then2          = visit(e_then, std::forward<Args>(args)...);
    type else2          = visit(e_else, std::forward<Args>(args)...);

    if (if2 == e_if && then2 == e_then && else2 == e_else)
        return ast;
    else
        return if_type(ast->get_location(), if2, then2, else2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const list_type& ast, Args&& ... args)
            -> list_type
{
    const type& t           = ast.get_type();
    type t2                 = visit(t, std::forward<Args>(args)...);

    if (t2 == t)
        return ast;
    else
        return list_type(ast->get_location(), t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const identifier_type& ast, Args&& ... args)
            -> identifier_type
{
    const identifier& i     = ast.get_identifier();
    identifier i2           = visit(i, std::forward<Args>(args)...);

    if (i2 == i)
        return ast;
    else
        return identifier_type(ast->get_location(), i2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_application& ast, Args&& ... args)
            -> type_application
{
    const type& tl          = ast.get_lhs();
    const type& tr          = ast.get_rhs();
    type tl2                = visit(tl, std::forward<Args>(args)...);
    type tr2                = visit(tr, std::forward<Args>(args)...);

    if (tl2 == tl && tr2 == tr)
        return ast;
    else
        return type_application(ast->get_location(), tl2, tr2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_kind_application& ast, Args&& ... args)
            -> type_kind_application
{
    const type& tl          = ast.get_lhs();
    const kind& tr          = ast.get_rhs();
    type tl2                = visit(tl, std::forward<Args>(args)...);
    kind tr2                = visit(tr, std::forward<Args>(args)...);

    if (tl2 == tl && tr2 == tr)
        return ast;
    else
        return type_kind_application(ast->get_location(), tl2, tr2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_type_application& ast, Args&& ... args)
            -> type_type_application
{
    const type& tl          = ast.get_type();
    const type& tr          = ast.get_arg();
    type tl2                = visit(tl, std::forward<Args>(args)...);
    type tr2                = visit(tr, std::forward<Args>(args)...);

    if (tl2 == tl && tr2 == tr)
        return ast;
    else
        return type_type_application(ast->get_location(), tl2, tr2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const predefined_type& ast, Args&& ... args)
            -> predefined_type
{
    predefined_type_code tc = ast.get_type_code();
    predefined_type_code tc2= visit(tc, std::forward<Args>(args)...);

    if (tc2 == tc)
        return ast;
    else
        return predefined_type(ast->get_location(), tc2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_var& ast, Args&& ... args)
            -> type
{
    if (ast.is_set() == false)
        return ast;
    else
        return visit(ast.get(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const variant_type& ast, Args&& ... args)
            -> variant_type
{
    bool changed            = false;
    long size               = ast.size();

    variant_type ret        = variant_type(ast->get_location());

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_type(i);
        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ty2            = visit(ty, std::forward<Args>(args)...);

        changed             |= id2 != id;
        changed             |= ty2 != ty;
        ret.add(id2, ty2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const lambda_type& ast, Args&& ... args)
            -> lambda_type
{
    const type& t                       = ast.get_body();
    const annotated_type_identifier& id = ast.get_var();
    polarity_type pt                    = ast.get_polarity();

    type t2                         = visit(t, std::forward<Args>(args)...);
    annotated_type_identifier id2   = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return lambda_type(ast->get_location(), pt, id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const mu_type& ast, Args&& ... args)
            -> mu_type
{
    const type& t                       = ast.get_body();
    const annotated_type_identifier& id = ast.get_var();

    type t2                         = visit(t, std::forward<Args>(args)...);
    annotated_type_identifier id2   = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return mu_type(ast->get_location(), id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const some_type& ast, Args&& ... args)
            -> some_type
{
    const type& t                       = ast.get_body();
    const annotated_type_identifier& id = ast.get_var();

    type t2                         = visit(t, std::forward<Args>(args)...);
    annotated_type_identifier id2   = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return some_type(ast->get_location(), id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const all_type& ast, Args&& ... args)
            -> all_type
{
    const type& t                       = ast.get_body();
    const annotated_type_identifier& id = ast.get_var();

    type t2                         = visit(t, std::forward<Args>(args)...);
    annotated_type_identifier id2   = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return all_type(ast->get_location(), id2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const product_of_types& ast, Args&& ... args)
            -> product_of_types
{
    bool changed            = false;
    long size               = ast.size();

    product_of_types ret    = product_of_types(ast->get_location());

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        auto arg2           = visit(arg, std::forward<Args>(args)...);

        changed             |= arg2 != arg;
        ret.add(arg2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_sequence& ast, Args&& ... args)
            -> type_sequence
{
    using item_type = type_sequence_item::item_type;
    long s          = ast.size();
    bool changed    = false;

    type_sequence ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const type& ex          = e.get_basic();
                type ex2                = visit(ex, std::forward<Args>(args)...);
                changed                 |= ex2 != ex;
                ret.add(e.get_location(), ex2);
                break;
            };
            case item_type::proj_item:
            {
                const int_literal& il   = e.get_projection();
                auto il2                = visit(il, std::forward<Args>(args)...);
                changed                 |= il2 != il;
                ret.add_projection(e.get_location(), il2);
                break;
            };
            case item_type::dot_item:
            {
                const identifier& id    = e.get_dot();
                auto id2                = visit(id, std::forward<Args>(args)...);
                changed                 |= id2 != id;
                ret.add_dot(e.get_location(), id2);
                break;
            };

            case item_type::type_applic_item:
            {
                using type_vec          = std::vector<type>;
                const type_vec& args    = e.get_type_applic_args();
                type_vec arg2;

                for (const type& elem: args)
                {
                    type elem2          = visit(elem, std::forward<Args>(args)...);
                    changed             |= elem2 != elem;
                    arg2.push_back(elem2);
                };

                ret.add_type_application(e.get_location(), arg2);
                break;
            }

            case item_type::kind_applic_item:
            {
                using kind_vec          = std::vector<kind>;
                const kind_vec& args    = e.get_kind_applic_args();
                kind_vec arg2;

                for (const kind& elem: args)
                {
                    kind elem2          = visit(elem, std::forward<Args>(args)...);
                    changed             |= elem2 != elem;
                    arg2.push_back(elem2);
                };

                ret.add_kind_application(e.get_location(), arg2);
                break;
            }
        };
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const annotated_type& ast, Args&& ... args)
            -> annotated_type
{
    const type& t           = ast.get_type();
    const kind& ki          = ast.get_kind();    

    type t2         = visit(t, std::forward<Args>(args)...);
    kind k2         = visit(ki, std::forward<Args>(args)...);    

    if (k2 == ki && t2 == t)
        return ast;
    else
        return annotated_type(ast->get_location(), t2, k2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const tuple_type& ast, Args&& ... args)
            -> tuple_type
{
    bool changed            = false;
    long size               = ast.size();

    tuple_type ret          = tuple_type(ast->get_location(), ast.get_state());

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        auto arg2           = visit(arg, std::forward<Args>(args)...);

        changed             |= arg2 != arg;
        ret.add(arg2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const bracket_type& ast, Args&& ... args)
            -> bracket_type
{
    long s          = ast.size();
    bool changed    = false;

    bracket_type ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        auto e2         = visit(e, std::forward<Args>(args)...);
        changed         |= e2 != e;

        ret.add(e2);
    };

    const identifier& vo    = ast.get_opening_bracket();
    const identifier& vc    = ast.get_closing_bracket();

    identifier vo2          = visit(vo, std::forward<Args>(args)...);
    identifier vc2          = visit(vc, std::forward<Args>(args)...);

    changed                 |= vo2 != vo;
    changed                 |= vc2 != vc;

    ret.set_brackets(ast.is_dot(), vo2, vc2);

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const record_type& ast, Args&& ... args)
            -> record_type
{
    bool changed            = false;
    long size               = ast.size();

    record_type ret         = record_type(ast->get_location());

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_type(i);
        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ty2            = visit(ty, std::forward<Args>(args)...);

        changed             |= id2 != id;
        changed             |= ty2 != ty;
        ret.add(id2, ty2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_constant& ast, Args&& ... args)
            -> type_constant
{
    bool changed            = false;
    long size               = ast.var_size();
    bool is_extern          = ast.is_extern_func();

    const identifier& n     = ast.get_name();
    identifier n2           = visit(n, std::forward<Args>(args)...);

    const kind& ki          = ast.get_return_kind();
    kind ki2                = ki ? visit(ki, std::forward<Args>(args)...) : ki;
    
    changed                 |= n2 != n;
    changed                 |= ki2 != ki;

    type_constant ret       = type_constant(ast->get_location(), n2, ki2, is_extern);
    ret.set_function(ast.get_function());

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_label(i);
        const auto& ty      = ast.get_type(i);

        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ty2            = ty ? visit(ty, std::forward<Args>(args)...) : ty;

        changed             |= id2 != id;
        changed             |= ty2 != ty;

        ret.add_label(id2, ty2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type_kind_scope& ast, Args&& ... args)
            -> type_kind_scope
{
    const type& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    type t2                         = visit(t, std::forward<Args>(args)...);
    identifier id2                  = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return type_kind_scope(ast->get_location(), id2, t2);
};

//---------------------------------------------------------------
//                  TERMS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const char_literal& ast, Args&& ... args)
            -> char_literal
{
    auto val    = ast.get_value();
    auto val2   = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return char_literal(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const string_literal& ast, Args&& ... args)
            -> string_literal
{
    auto val    = ast.get_value();
    auto val2   = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return string_literal(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const int_literal& ast, Args&& ... args)
            -> int_literal
{
    auto val        = ast.get_value();
    auto val2       = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return int_literal(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const bool_literal& ast, Args&& ... args)
            -> bool_literal
{
    auto val    = ast.get_value();
    auto val2   = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return bool_literal(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const float_literal& ast, Args&& ... args)
            -> float_literal
{
    auto val    = ast.get_value();
    auto val2   = visit(val, std::forward<Args>(args)...);

    if (val2 == val)
        return ast;
    else
        return float_literal(ast->get_location(), val2);
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const identifier_term& ast, Args&& ... args)
            -> identifier_term
{
    const identifier& var       = ast.get_identifier();
    identifier var2             = visit(var, std::forward<Args>(args)...);

    if (var == var2)
        return ast;
    else
        return identifier_term(ast->get_location(), var2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const overloaded_symbol& ast, Args&& ... args)
            -> overloaded_symbol
{
    using arg_class             = overloaded_symbol::arg_class;

    const identifier_term& var  = ast.get_identifier();
    const type& ty              = ast.get_type();
    const type& ret_ty          = ast.get_return_type();
    long size                   = ast.arg_size();
    bool modified               = false;

    identifier_term var2        = visit(var, std::forward<Args>(args)...);
    type ty2                    = visit(ty, std::forward<Args>(args)...);
    type ret_ty2                = visit(ret_ty, std::forward<Args>(args)...);
    
    overloaded_symbol ret(ast->get_location(), var2, ty2, ret_ty2, ast.get_index());

    for (long i = 0; i < size; ++i)
    {
        arg_class cl            = ast.get_arg_class(i);

        if (cl == arg_class::class_kind)
        {
            const ast::kind& arg_ty = ast.get_arg_kind(i);
            ast::kind arg_ty2       = visit(arg_ty, std::forward<Args>(args)...);
            modified                |= arg_ty2 != arg_ty;

            ret.add_kind_arg(arg_ty);
        }
        else
        {
            const ast::type& arg_ty = ast.get_arg_type(i);
            ast::type arg_ty2       = visit(arg_ty, std::forward<Args>(args)...);
            modified                |= arg_ty2 != arg_ty;

            if (cl == arg_class::class_term)
                ret.add_term_arg(arg_ty);
            else
                ret.add_type_arg(arg_ty);
        }
    };

    if (var == var2 && ty2 == ty && ret_ty2 == ret_ty, modified == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval_lambda(const lambda_term& ast, Args&& ... args)
            -> lambda_term
{
    const term& ex                  = ast.get_body();
    const annotated_identifier& id  = ast.get_var();
    const symbol_data* sd           = ast.get_symbol_data();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    annotated_identifier id2        = visit(id, std::forward<Args>(args)...);

    if (id2 == id && ex2 == ex)
        return ast;
    else
        return lambda_term(ast->get_location(), id2, ex2, sd);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval_mu(const mu_term& ast, Args&& ... args)
            -> mu_term
{
    const term& ex                  = ast.get_body();
    const annotated_identifier& id  = ast.get_var();
    const symbol_data* sd           = ast.get_symbol_data();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    annotated_identifier id2        = visit(id, std::forward<Args>(args)...);

    if (id2 == id && ex2 == ex)
        return ast;
    else
        return mu_term(ast->get_location(), id2, ex2, sd);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const all_term& ast, Args&& ... args)
            -> all_term
{
    const term& ex                      = ast.get_body();
    const annotated_type_identifier& id = ast.get_var();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    annotated_type_identifier id2   = visit(id, std::forward<Args>(args)...);

    if (ex2 == ex && id2 == id)
        return ast;
    else
        return all_term(ast->get_location(), id2, ex2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_type_application& ast, Args&& ... args)
            -> term_type_application
{
    const term& ex                  = ast.get_term();
    const type& t                   = ast.get_type();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    type t2                         = visit(t, std::forward<Args>(args)...);

    if (ex2 == ex && t2 == t)
        return ast;
    else
        return term_type_application(ast->get_location(), ex2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_kind_application& ast, Args&& ... args)
            -> term_kind_application
{
    const term& ex                  = ast.get_term();
    const kind& t                   = ast.get_kind();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    kind t2                         = visit(t, std::forward<Args>(args)...);

    if (ex2 == ex && t2 == t)
        return ast;
    else
        return term_kind_application(ast->get_location(), ex2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const annotated_term& ast, Args&& ... args)
            -> annotated_term
{
    const term& ex          = ast.get_term();
    const type& t           = ast.get_type();

    term ex2        = visit(ex, std::forward<Args>(args)...);
    type t2         = visit(t, std::forward<Args>(args)...);

    if (ex2 == ex && t2 == t)
        return ast;
    else
        return annotated_term(ast->get_location(), ex2, t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const tuple_term& ast, Args&& ... args)
            -> tuple_term
{
    long s          = ast.size();
    bool changed    = false;

    tuple_term ret(ast->get_location(), ast.get_state());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        auto e2         = visit(e, std::forward<Args>(args)...);
        changed         |= e2 != e;

        ret.add(e2);
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_sequence& ast, Args&& ... args)
            -> term_sequence
{
    using item_type = term_sequence_item::item_type;
    long s          = ast.size();
    bool changed    = false;

    term_sequence ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const term& ex          = e.get_basic();
                term ex2                = visit(ex, std::forward<Args>(args)...);
                changed                 |= ex2 != ex;
                ret.add(e.get_location(), ex2);
                break;
            };
            case item_type::proj_item:
            {
                const int_literal& il   = e.get_projection();
                auto il2                = visit(il, std::forward<Args>(args)...);
                changed                 |= il2 != il;
                ret.add_projection(e.get_location(), il2);
                break;
            };
            case item_type::dot_item:
            {
                const identifier& id    = e.get_dot();
                auto id2                = visit(id, std::forward<Args>(args)...);
                changed                 |= id2 != id;
                ret.add_dot(e.get_location(), id2);
                break;
            };

            case item_type::type_applic_item:
            {
                using type_vec          = std::vector<type>;
                const type_vec& args    = e.get_type_applic_args();
                type_vec arg2;

                for (const type& elem: args)
                {
                    type elem2          = visit(elem, std::forward<Args>(args)...);
                    changed             |= elem2 != elem;
                    arg2.push_back(elem2);
                };

                ret.add_type_application(e.get_location(), arg2);
                break;
            }

            case item_type::kind_applic_item:
            {
                using kind_vec          = std::vector<kind>;
                const kind_vec& args    = e.get_kind_applic_args();
                kind_vec arg2;

                for (const kind& elem: args)
                {
                    kind elem2          = visit(elem, std::forward<Args>(args)...);
                    changed             |= elem2 != elem;
                    arg2.push_back(elem2);
                };

                ret.add_kind_application(e.get_location(), arg2);
                break;
            }
        };
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const list_term& ast, Args&& ... args)
            -> list_term
{
    long s          = ast.size();
    bool changed    = false;

    list_term ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        auto e2         = visit(e, std::forward<Args>(args)...);
        changed         |= e2 != e;

        ret.add(e2);
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const bracket_term& ast, Args&& ... args)
            -> bracket_term
{
    long s          = ast.size();
    bool changed    = false;

    bracket_term ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        auto e2         = visit(e, std::forward<Args>(args)...);
        changed         |= e2 != e;

        ret.add(e2);
    };

    const identifier& vo    = ast.get_opening_bracket();
    const identifier& vc    = ast.get_closing_bracket();

    identifier vo2          = visit(vo, std::forward<Args>(args)...);
    identifier vc2          = visit(vc, std::forward<Args>(args)...);

    changed                 |= vo2 != vo;
    changed                 |= vc2 != vc;

    ret.set_brackets(ast.is_dot(), vo2, vc2);

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const record_term& ast, Args&& ... args)
            -> record_term
{
    long s          = ast.size();
    bool changed    = false;

    record_term ret(ast->get_location());

    for (long i = 0; i < s; ++i)
    {
        const term& e       = ast.get_term(i);
        const auto& l       = ast.get_label(i);
        auto e2             = visit(e, std::forward<Args>(args)...);
        auto l2             = visit(l, std::forward<Args>(args)...);

        changed             |= e2 != e;
        changed             |= l2 != l;

        ret.add(l2, e2);
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const closure_term& ast, Args&& ... args)
            -> closure_term
{
    const term& ex  = ast.get_body();
    term ex2        = visit(ex, std::forward<Args>(args)...);

    long n          = ast.size();
    bool changed    = ex2 != ex;

    closure_term ret(ast->get_location(), ex2, ast.get_kind());

    for (long i = 0; i < n; ++i)
    {
        const term& s           = ast.get_substitution(i);
        const identifier& id    = ast.get_internal_ident(i);
        const type& ty          = ast.get_type(i);

        auto s2                 = visit(s, std::forward<Args>(args)...);
        auto id2                = visit(id, std::forward<Args>(args)...);
        auto ty2                = visit(ty, std::forward<Args>(args)...);

        changed             |= s2 != s;
        changed             |= id2 != id;
        changed             |= ty2 != ty;

        ret.add_item(s2, id2, ty2);
    };

    if (changed == true)
        return ret;
    else
        return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_constant& ast, Args&& ... args)
            -> term_constant
{
    bool changed            = false;
    long size               = ast.var_size();
    bool is_extern          = ast.is_extern_func();

    const identifier& n     = ast.get_name();
    identifier n2           = visit(n, std::forward<Args>(args)...);

    const type& ret_ty      = ast.get_return_type();
    type ret_ty2            = visit(ret_ty, std::forward<Args>(args)...);
    
    changed                 |= n2 != n;
    changed                 |= ret_ty2 != ret_ty;

    term_constant ret       = term_constant(ast->get_location(), n2, is_extern);
    ret.set_return_type(ret_ty2);
    ret.set_function(ast.get_function());

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_label(i);
        const auto& ex      = ast.get_term(i);

        auto id2            = visit(id, std::forward<Args>(args)...);
        auto ex2            = ex ? visit(ex, std::forward<Args>(args)...) : ex;

        changed             |= id2 != id;
        changed             |= ex2 != ex;

        ret.add_label(id2, ex2);
    };

    if (changed == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const error_term& ast, Args&& ...)
            -> error_term
{
    return ast;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const if_term& ast, Args&& ... args)
            -> if_term
{
    const term& e_if    = ast.get_if();
    const term& e_then  = ast.get_then();
    const term& e_else  = ast.get_else();

    term if2            = visit(e_if, std::forward<Args>(args)...);
    term then2          = visit(e_then, std::forward<Args>(args)...);
    term else2          = visit(e_else, std::forward<Args>(args)...);

    if (if2 == e_if && then2 == e_then && else2 == e_else)
        return ast;
    else
        return if_term(ast->get_location(), if2, then2, else2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval_let(const let_term& ast, Args&& ... args)
            -> let_term
{
    const term& ex                  = ast.get_body();
    const definitions& defs         = ast.get_definitions();
    symbol_data* sd                 = ast.get_symbol_data();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    definitions defs2               = visit(defs, std::forward<Args>(args)...);

    if (ex2 == ex && defs2 == defs)
        return ast;
    else
        return let_term(ast->get_location(), defs2, ex2, sd);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const ref_term& ast, Args&& ... args)
            -> ref_term
{
    const term& ex                  = ast.get_term();
    term ex2                        = visit(ex, std::forward<Args>(args)...);

    if (ex2 == ex)
        return ast;
    else
        return ref_term(ast->get_location(), ex2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const val_term& ast, Args&& ... args)
            -> val_term
{
    const type& t                   = ast.get_type();
    type t2                         = visit(t, std::forward<Args>(args)...);

    if (t2 == t)
        return ast;
    else
        return val_term(ast->get_location(), t2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const assign_term& ast, Args&& ... args)
            -> assign_term
{
    const term& lhs                 = ast.get_lhs();
    const term& rhs                 = ast.get_rhs();
    term lhs2                       = visit(lhs, std::forward<Args>(args)...);
    term rhs2                       = visit(rhs, std::forward<Args>(args)...);

    if (lhs2 == lhs && rhs2 == rhs)
        return ast;
    else
        return assign_term(ast->get_location(), lhs2, rhs2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const deref_term& ast, Args&& ... args)
            -> deref_term
{
    const term& ex                  = ast.get_term();
    term ex2                        = visit(ex, std::forward<Args>(args)...);

    if (ex2 == ex)
        return ast;
    else
        return deref_term(ast->get_location(), ex2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const cell_term& ast, Args&& ... args)
            -> cell_term
{
    long index  = ast.get_index();
    long index2 = visit(index, std::forward<Args>(args)...);

    if (index2 == index)
        return ast;
    else
        return cell_term(ast->get_location(), index2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const path_term& ast, Args&& ... args)
            -> path_term
{
    const term& ex                  = ast.get_term();
    const identifier& id            = ast.get_identifier();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    identifier id2                  = visit(id, std::forward<Args>(args)...);

    if (ex2 == ex && id2 == id)
        return ast;
    else
        return path_term(ast->get_location(), ex2, id2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const path_int_term& ast, Args&& ... args)
            -> path_int_term
{
    const term& ex                  = ast.get_term();
    const int_literal& id           = ast.get_index();

    term ex2                        = visit(ex, std::forward<Args>(args)...);
    int_literal id2                 = visit(id, std::forward<Args>(args)...);

    if (ex2 == ex && id2 == id)
        return ast;
    else
        return path_int_term(ast->get_location(), ex2, id2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_application& ast, Args&& ... args)
            -> term_application
{
    const term& el                  = ast.get_lhs();
    const term& er                  = ast.get_rhs();

    term el2                        = visit(el, std::forward<Args>(args)...);
    term er2                        = visit(er, std::forward<Args>(args)...);

    if (el2 == el && er2 == er)
        return ast;
    else
        return term_application(ast->get_location(), el2, er2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const variant_term& ast, Args&& ... args)
            -> variant_term
{
    using annot_id                  = ast::annotated_identifier;

    const term& e                   = ast.get_term();
    const type& t                   = ast.get_type();
    const annot_id& id              = ast.get_label();

    term e2                         = visit(e, std::forward<Args>(args)...);
    type t2                         = visit(t, std::forward<Args>(args)...);
    annot_id id2                    = visit(id, std::forward<Args>(args)...);

    if (e2 == e && t2 == t && id2 == id)
        return ast;
    else
        return variant_term(ast->get_location(), t2, id2, e2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval_case(const variant_case_term& ast, Args&& ... args)
            -> variant_case_term
{
    const term& e                   = ast.get_term();
    bool modif                      = false;
    const symbol_data* sd           = ast.get_symbol_data();

    term e2                         = visit(e, std::forward<Args>(args)...);
    modif                           |= e2 != e;

    variant_case_term ret           = variant_case_term(ast->get_location(), sd);
    ret.set_term(e2);

    long n                          = ast.size();

    for (long i = 0; i < n; ++i)
    {
        const term& b1                  = ast.get_bind(i);
        const annotated_identifier& i1  = ast.get_label(i);
    
        term b2                         = visit(b1, std::forward<Args>(args)...);
        annotated_identifier i2         = visit(i1, std::forward<Args>(args)...);

        ret.add_pattern(i2, b2);
        modif                           |= b2 != b1;
        modif                           |= i2 != i1;
    };

    if (modif == false)
        return ast;
    else
        return ret;
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval_unpack(const unpack_term& ast, Args&& ... args)
            -> unpack_term
{
    const term& e1                  = ast.get_package();
    const term& e2                  = ast.get_term();
    const auto& i1                  = ast.get_type_identifier();
    const auto& i2                  = ast.get_term_identifier();
    const symbol_data* sd           = ast.get_symbol_data();

    term e12                        = visit(e1, std::forward<Args>(args)...);
    term e22                        = visit(e2, std::forward<Args>(args)...);
    auto i12                        = visit(i1, std::forward<Args>(args)...);
    auto i22                        = visit(i2, std::forward<Args>(args)...);

    if (e12 == e1 && e22 == e2 && i12 == i1 && i22 == i2)
        return ast;
    else
        return unpack_term(ast->get_location(), e12, i12, i22, e22, sd);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const pack_term& ast, Args&& ... args)
            -> pack_term
{
    using annot_id                  = annotated_type_identifier;
    const annot_id& id              = ast.get_var();
    const term& e                   = ast.get_body();
    const type& t                   = ast.get_var_type();
    const type& st                  = ast.get_some_type();

    annot_id id2                    = visit(id, std::forward<Args>(args)...);
    type t2                         = visit(t, std::forward<Args>(args)...);
    term e2                         = visit(e, std::forward<Args>(args)...);    
    type st2                        = visit(st, std::forward<Args>(args)...);    

    if (t2 == t && e2 == e && id2 == id && st2 == st)
        return ast;
    else
        return pack_term(ast->get_location(), id2, t2, e2, st2);
};

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term_kind_scope& ast, Args&& ... args)
            -> term_kind_scope
{
    const term& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    term t2                         = visit(t, std::forward<Args>(args)...);
    identifier id2                  = visit(id, std::forward<Args>(args)...);

    if (t2 == t && id2 == id)
        return ast;
    else
        return term_kind_scope(ast->get_location(), id2, t2);
};

//---------------------------------------------------------------
//                  NULL VALUES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const type& ast, Args&& ...)
            -> type
{
    if (!ast)
        throw std::runtime_error("uninitialized type");

    return ast;
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const term& ast, Args&& ...)
            -> term
{
    if (!ast)
        throw std::runtime_error("uninitialized term");

    return ast;
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const definition& ast, Args&& ...)
            -> definition
{
    if (!ast)
        throw std::runtime_error("uninitialized definition");

    return ast;
}

template<class Derived>
template<class ... Args>
auto rewrite_visitor<Derived>::eval(const kind& ast, Args&& ...)
            -> kind
{
    if (!ast)
        throw std::runtime_error("uninitialized kind");

    return ast;
}

};};
