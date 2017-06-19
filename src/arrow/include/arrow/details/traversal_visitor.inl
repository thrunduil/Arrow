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

#include "arrow/ast/ast_visitor.h"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                  VALUES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(int32_t, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const std::string&, Args&& ...)  
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(int64_t, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(bool, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(double, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(unique_string, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const operator_info&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(predefined_type_code, Args&& ...)
{};

//---------------------------------------------------------------
//                  OTHER
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const module_definition& ast, Args&& ... args)
{
    using string_loc        = std::pair<unique_string, location>;
    using string_vec        = std::vector<string_loc>;

    const std::string& name = ast.get_module_name() const;
    const string_vec& imp   = ast.get_imports() const;
    const definitions& defs = ast.get_definitions() const;

    visit(name, std::forward<Args>(args)...);
    visit(defs, std::forward<Args>(args)...);

    for (const string_loc& pos : imp)
    {
        visit(pos.first, std::forward<Args>(args)...);
        visit(pos.second, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const definitions& ast, Args&& ... args)
{
    long size       = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const definition& def   = ast.get_definition(i);
        visit(def, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const identifier& ast, Args&& ... args)
{
    fixity_type ft;
    unique_string id    = ast.get_name(ft);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const annotated_type_identifier& ast, Args&& ... args)
{
    const identifier& id    = ast.get_identifier();
    const kind& ki          = ast.get_kind();

    visit(id, std::forward<Args>(args)...);
    visit(ki, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const annotated_identifier& ast, Args&& ... args)
{
    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_type();

    visit(id, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const symbol_sequence& ast, Args&& ... args)
{
    long s          = ast.size();

    for (int i = 0; i < s; ++i)
    {
        const identifier& v = ast.get(i);
        visit(v, std::forward<Args>(args)...);
    };
};

//---------------------------------------------------------------
//                  DEFINITIONS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_definition& ast, Args&& ... args)
{
    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_type();
    const term& ex          = ast.get_body();

    visit(id, std::forward<Args>(args)...);  

    if (t)
        visit(t, std::forward<Args>(args)...);    
    if (ex)
        visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const fixity_definition& ast, Args&& ... args)
{
    const operator_info& decl   = ast.get_operator_type();
    const symbol_sequence& seq  = ast.get_symbols();

    visit(decl, std::forward<Args>(args)...);
    visit(seq, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_definition& ast, Args&& ... args)
{
    const identifier& id    = ast.get_identifier();
    const kind& k           = ast.get_kind();
    const type& t           = ast.get_body();

    visit(id, std::forward<Args>(args)...);

    if (k)
        visit(k, std::forward<Args>(args)...);

    if (t)
        visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_definition_var& ast, Args&& ... args)
{
    using arg_type          = annotated_type_identifier;
    using id_vec            = std::vector<ast::identifier>;

    const identifier& id    = ast.get_identifier();
    const type& t           = ast.get_body();
    const kind& k           = ast.get_return_kind();
    const id_vec& kind_vars = ast.get_kind_vars();

    visit(id, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);

    if (k)
        visit(k, std::forward<Args>(args)...);

    for (const auto& elem : kind_vars)
        visit(elem, std::forward<Args>(args)...);

    long size               = ast.var_size();

    for (long i = 0; i < size; ++i)
    {
        const arg_type& arg = ast.get_arg(i);
        visit(arg, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_use& ast, Args&& ... args)
{
    const term& x           = ast.get_term();
    visit(x, std::forward<Args>(args)...);
};

//---------------------------------------------------------------
//                  KINDS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_kind&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const value_kind&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const kind_identifier& ast, Args&& ... args)
{
    const identifier& i     = ast.get_identifier();
    visit(i, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const kind_scope& ast, Args&& ... args)
{
    const kind& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_all_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    const kind& t                   = ast.get_body();
    const kind& id                  = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_some_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    const kind& t                   = ast.get_body();
    const kind& id                  = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_list_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    const kind& t                   = ast.get_body();
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_record_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_kind(i);
        visit(id, std::forward<Args>(args)...);
        visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_variant_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_identifier(i);
        const auto& ty      = ast.get_kind(i);
        visit(id, std::forward<Args>(args)...);
        visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_predefined_kind& ast, Args&& ...)
{
    (void)ast;
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_ref_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    const kind& t                   = ast.get_body();
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const product_kind& ast, Args&& ... args)
{
    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        visit(arg, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_tuple_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        visit(arg, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const arrow_kind& ast, Args&& ... args)
{
    const kind& kl          = ast.get_lhs();
    const kind& kr          = ast.get_rhs();
    visit(kl, std::forward<Args>(args)...);
    visit(kr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const star_arrow_kind& ast, Args&& ... args)
{
    if (ast.is_star() == true)
        return;

    const kind& kl          = ast.get_lhs();
    const kind& kr          = ast.get_rhs();
    visit(kl, std::forward<Args>(args)...);
    visit(kr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const kind_var& ast, Args&& ... args)
{
    if (ast.is_set() == true)
        visit(ast.get(), std::forward<Args>(args)...);
};

//---------------------------------------------------------------
//                  TYPES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const error_type&, Args&& ...)
{
    return;
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const int_type& ast, Args&& ... args)
{
    auto val        = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const bool_type& ast, Args&& ... args)
{
    auto val        = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const char_type& ast, Args&& ... args)
{
    auto val        = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const string_type& ast, Args&& ... args)
{
    auto val        = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}


template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const arrow_type& ast, Args&& ... args)
{
    const type& tl          = ast.get_lhs();
    const type& tr          = ast.get_rhs();
    visit(tl, std::forward<Args>(args)...);
    visit(tr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const path_int_type& ast, Args&& ... args)
{
    const type& ty                  = ast.get_type();
    const int_literal& id           = ast.get_index();

    visit(ty, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ref_type& ast, Args&& ... args)
{
    const type& t           = ast.get_type();
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const if_type& ast, Args&& ... args)
{
    const type& e_if    = ast.get_if();
    const type& e_then  = ast.get_then();
    const type& e_else  = ast.get_else();

    visit(e_if, std::forward<Args>(args)...);
    visit(e_then, std::forward<Args>(args)...);
    visit(e_else, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const list_type& ast, Args&& ... args)
{
    const type& t           = ast.get_type();
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const identifier_type& ast, Args&& ... args)
{
    const identifier& i     = ast.get_identifier();
    visit(i, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_application& ast, Args&& ... args)
{
    const type& tl          = ast.get_lhs();
    const type& tr          = ast.get_rhs();
    visit(tl, std::forward<Args>(args)...);
    visit(tr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_kind_application& ast, Args&& ... args)
{
    const type& tl          = ast.get_lhs();
    const kind& tr          = ast.get_rhs();
    visit(tl, std::forward<Args>(args)...);
    visit(tr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_type_application& ast, Args&& ... args)
{
    const type& tl          = ast.get_type();
    const type& tr          = ast.get_arg();
    visit(tl, std::forward<Args>(args)...);
    visit(tr, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const predefined_type& ast, Args&& ... args)
{
    predefined_type_code tc = ast.get_type_code();
    visit(tc, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_var& ast, Args&& ...  args)
{
    if (ast.is_set() == true)
        return visit(ast.get(), std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const variant_type& ast, Args&& ... args)
{
    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& id     = ast.get_identifier(i);
        const auto& ty     = ast.get_type(i);

        visit(id, std::forward<Args>(args)...);
        visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const lambda_type& ast, Args&& ... args)
{
    const type& t           = ast.get_body();
    const auto& id          = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const mu_type& ast, Args&& ... args)
{
    const type& t           = ast.get_body();
    const auto& id          = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const some_type& ast, Args&& ... args)
{
    const type& t           = ast.get_body();
    const auto& id          = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const all_type& ast, Args&& ... args)
{
    const type& t           = ast.get_body();
    const auto& id          = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const annotated_type& ast, Args&& ... args)
{
    const type& t           = ast.get_type();
    const kind& ki          = ast.get_kind();    

    visit(t, std::forward<Args>(args)...);
    visit(ki, std::forward<Args>(args)...);    
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const tuple_type& ast, Args&& ... args)
{
    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        visit(arg, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const bracket_type& ast, Args&& ... args)
{
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        visit(e, std::forward<Args>(args)...);
    };

    const identifier& vo    = ast.get_opening_bracket();
    const identifier& vc    = ast.get_closing_bracket();

    visit(vo, std::forward<Args>(args)...);
    visit(vc, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const product_of_types& ast, Args&& ... args)
{
    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& arg     = ast.get(i);
        visit(arg, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_sequence& ast, Args&& ... args)
{
    using item_type = type_sequence_item::item_type;
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const type& ex          = e.get_basic();
                visit(ex, std::forward<Args>(args)...);
                break;
            };
            case item_type::proj_item:
            {
                const int_literal& il   = e.get_projection();
                visit(il, std::forward<Args>(args)...);
                break;
            };
            case item_type::dot_item:
            {
                const identifier& id    = e.get_dot();
                visit(id, std::forward<Args>(args)...);
                break;
            };

            case item_type::type_applic_item:
            {
                using type_vec          = std::vector<type>;
                const type_vec& args    = e.get_type_applic_args();

                for (const type& elem: args)
                    visit(elem, std::forward<Args>(args)...);

                break;
            }

            case item_type::kind_applic_item:
            {
                using kind_vec          = std::vector<kind>;
                const kind_vec& args    = e.get_kind_applic_args();

                for (const kind& elem: args)
                    visit(elem, std::forward<Args>(args)...);

                break;
            }

        };
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const record_type& ast, Args&& ... args)
{
    long size               = ast.size();

    for (long i = 0; i < size; ++i)
    {
        const auto& id     = ast.get_identifier(i);
        const auto& ty     = ast.get_type(i);

        visit(id, std::forward<Args>(args)...);
        visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_constant& ast, Args&& ... args)
{
    long size               = ast.var_size();

    const identifier& n     = ast.get_name();
    const kind& ki          = ast.get_return_kind();

    visit(n, std::forward<Args>(args)...);
    visit(ki, std::forward<Args>(args)...);
    
    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_label(i);
        const auto& ty      = ast.get_type(i);
        
        visit(id, std::forward<Args>(args)...);

        if (ty)
            visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type_kind_scope& ast, Args&& ... args)
{
    const type& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

//---------------------------------------------------------------
//                  TERMS
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const char_literal& ast, Args&& ... args)
{
    auto val    = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const string_literal& ast, Args&& ... args)
{
    auto val    = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const int_literal& ast, Args&& ... args)
{
    auto val        = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const bool_literal& ast, Args&& ... args)
{
    auto val    = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const float_literal& ast, Args&& ... args)
{
    auto val    = ast.get_value();
    visit(val, std::forward<Args>(args)...);
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const identifier_term& ast, Args&& ... args)
{
    const identifier& var       = ast.get_identifier();
    visit(var, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const overloaded_symbol& ast, Args&& ... args)
{
    using arg_class             = overloaded_symbol::arg_class;

    const identifier_term& var  = ast.get_identifier();
    const type& ty              = ast.get_type();
    const type& ret_ty          = ast.get_return_type();
    long size                   = ast.arg_size();

    visit(var, std::forward<Args>(args)...);
    visit(ty, std::forward<Args>(args)...);
    visit(ret_ty, std::forward<Args>(args)...);

    for (long i = 0; i < size; ++i)
    {
        arg_class cl            = ast.get_arg_class(i);

        if (cl == arg_class::class_kind)
        {
            const ast::kind& arg_ty = ast.get_arg_kind(i);
            visit(arg_ty, std::forward<Args>(args)...);
        }
        else
        {
            const ast::type& arg_ty = ast.get_arg_type(i);
            visit(arg_ty, std::forward<Args>(args)...);
        }
    }
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const lambda_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_body();
    const auto& id                  = ast.get_var();

    visit(id, std::forward<Args>(args)...);
    visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const mu_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_body();
    const auto& id                  = ast.get_var();

    visit(id, std::forward<Args>(args)...);
    visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const all_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_body();
    const auto& id                  = ast.get_var();

    visit(id, std::forward<Args>(args)...);
    visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_type_application& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    const type& t                   = ast.get_type();
    
    visit(ex, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_kind_application& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    const kind& t                   = ast.get_kind();
    
    visit(ex, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const annotated_term& ast, Args&& ... args)
{
    const term& ex          = ast.get_term();
    const type& t           = ast.get_type();

    visit(ex, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const tuple_term& ast, Args&& ... args)
{
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        visit(e, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_sequence& ast, Args&& ... args)
{
    using item_type = term_sequence_item::item_type;
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const term& ex          = e.get_basic();
                visit(ex, std::forward<Args>(args)...);
                break;
            };
            case item_type::proj_item:
            {
                const int_literal& il   = e.get_projection();
                visit(il, std::forward<Args>(args)...);
                break;
            };
            case item_type::dot_item:
            {
                const identifier& id    = e.get_dot();
                visit(id, std::forward<Args>(args)...);
                break;
            };

            case item_type::type_applic_item:
            {
                using type_vec          = std::vector<type>;
                const type_vec& args    = e.get_type_applic_args();

                for (const type& elem: args)
                    visit(elem, std::forward<Args>(args)...);

                break;
            }

            case item_type::kind_applic_item:
            {
                using kind_vec          = std::vector<kind>;
                const kind_vec& args    = e.get_kind_applic_args();

                for (const kind& elem: args)
                    visit(elem, std::forward<Args>(args)...);

                break;
            }

        };
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const list_term& ast, Args&& ... args)
{
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        visit(e, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const bracket_term& ast, Args&& ... args)
{
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        visit(e, std::forward<Args>(args)...);
    };

    const identifier& vo    = ast.get_opening_bracket();
    const identifier& vc    = ast.get_closing_bracket();

    visit(vo, std::forward<Args>(args)...);
    visit(vc, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const record_term& ast, Args&& ... args)
{
    long s          = ast.size();

    for (long i = 0; i < s; ++i)
    {
        const term& e       = ast.get_term(i);
        const auto& l       = ast.get_label(i);

        visit(e, std::forward<Args>(args)...);
        visit(l, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const closure_term& ast, Args&& ... args)
{
    const term& ex  = ast.get_body();
    long n          = ast.size();

    visit(ex, std::forward<Args>(args)...);    

    for (long i = 0; i < n; ++i)
    {
        const term& s           = ast.get_substitution(i);
        const identifier& id    = ast.get_internal_ident(i);
        const type& ty          = ast.get_type(i);

        visit(s, std::forward<Args>(args)...);
        visit(id, std::forward<Args>(args)...);
        visit(ty, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_constant& ast, Args&& ... args)
{
    long size               = ast.var_size();

    const identifier& n     = ast.get_name();
    const type& ret_ty      = ast.get_return_type();

    visit(n, std::forward<Args>(args)...);
    visit(ret_ty, std::forward<Args>(args)...);
    
    for (long i = 0; i < size; ++i)
    {
        const auto& id      = ast.get_label(i);
        const auto& ex      = ast.get_term(i);
        
        visit(id, std::forward<Args>(args)...);

        if (ex)
            visit(ex, std::forward<Args>(args)...);
    };
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const error_term&, Args&& ...)
{};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const if_term& ast, Args&& ... args)
{
    const term& e_if    = ast.get_if();
    const term& e_then  = ast.get_then();
    const term& e_else  = ast.get_else();

    visit(e_if, std::forward<Args>(args)...);
    visit(e_then, std::forward<Args>(args)...);
    visit(e_else, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const let_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_body();
    const definitions& defs         = ast.get_definitions();

    visit(ex, std::forward<Args>(args)...);
    visit(defs, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const ref_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const val_term& ast, Args&& ... args)
{
    const type& ty                  = ast.get_type();
    visit(ty, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const assign_term& ast, Args&& ... args)
{
    const term& lhs                 = ast.get_lhs();
    const term& rhs                 = ast.get_rhs();
    visit(lhs, std::forward<Args>(args)...);
    visit(rhs, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const deref_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    visit(ex, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const cell_term& ast, Args&& ... args)
{
    long index = ast.get_index();
    visit(index, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const path_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    const identifier& id            = ast.get_identifier();

    visit(ex, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const path_int_term& ast, Args&& ... args)
{
    const term& ex                  = ast.get_term();
    const int_literal& id           = ast.get_index();

    visit(ex, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_application& ast, Args&& ... args)
{
    const term& el                  = ast.get_lhs();
    const term& er                  = ast.get_rhs();

    visit(el, std::forward<Args>(args)...);
    visit(er, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const variant_term& ast, Args&& ... args)
{
    const term& e                   = ast.get_term();
    const type& t                   = ast.get_type();
    const auto& lab                 = ast.get_label();

    visit(e, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
    visit(lab, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const variant_case_term& ast, Args&& ... args)
{
    const term& e                   = ast.get_term();
    long n                          = ast.size();

    visit(e, std::forward<Args>(args)...);

    for (long i = 0; i < n; ++i)
    {
        const annotated_identifier& id  = ast.get_label(i);
        const term& b                   = ast.get_bind(i);

        visit(id, std::forward<Args>(args)...);
        visit(b, std::forward<Args>(args)...);
    }
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const unpack_term& ast, Args&& ... args)
{
    const term& e1                  = ast.get_package();
    const term& e2                  = ast.get_term();
    const auto& i1                  = ast.get_type_identifier();
    const auto& i2                  = ast.get_term_identifier();

    visit(e1, std::forward<Args>(args)...);
    visit(e2, std::forward<Args>(args)...);

    visit(i1, std::forward<Args>(args)...);
    visit(i2, std::forward<Args>(args)...);
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const pack_term& ast, Args&& ... args)
{
    using annot_id                  = annotated_type_identifier;
    const annot_id& id              = ast.get_var();
    const term& e                   = ast.get_body();
    const type& t                   = ast.get_var_type();
    const type& st                  = ast.get_some_type();

    visit(id, std::forward<Args>(args)...);
    visit(t, std::forward<Args>(args)...);
    visit(e, std::forward<Args>(args)...);    
    visit(st, std::forward<Args>(args)...);    
};

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term_kind_scope& ast, Args&& ... args)
{
    const term& t                   = ast.get_body();
    const identifier& id            = ast.get_var();

    visit(t, std::forward<Args>(args)...);
    visit(id, std::forward<Args>(args)...);
};

//---------------------------------------------------------------
//                  NULL VALUES
//---------------------------------------------------------------
template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const type& ast, Args&& ...)
{
    if (!ast)
        throw std::runtime_error("uninitialized type");
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const term& ast, Args&& ...)
{
    if (!ast)
        throw std::runtime_error("uninitialized term");
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const definition& ast, Args&& ...)
{
    if (!ast)
        throw std::runtime_error("uninitialized definition");
}

template<class Derived>
template<class ... Args>
void traversal_visitor<Derived>::eval(const kind& ast, Args&& ...)
{
    if (!ast)
        throw std::runtime_error("uninitialized kind");
}

};};
