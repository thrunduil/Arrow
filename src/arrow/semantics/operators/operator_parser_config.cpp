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

#include "operator_parser_config.h"
#include "error/error.h"

namespace arrow { namespace oper
{

//---------------------------------------------------------------
//               operator_config_type
//---------------------------------------------------------------
operator_config_type::iterm
operator_config_type::hide_kind_applic(const ast::location& loc, const ikind_vec& args)
{
    ast::product_kind tt(loc, true);
    for (const auto& elem : args)
        tt.add(elem);

    // store type in annotated type
    ast::error_type  et     = ast::error_type(loc);
    ast::annotated_type at  = ast::annotated_type(loc, et, tt);

    return at;
};

operator_config_type::iterm
operator_config_type::hide_type_applic(const ast::location& loc, const itype_vec& args)
{
    ast::product_of_types tt(loc);
    for (const auto& elem : args)
        tt.add(elem);

    return tt;
};

ast::type 
operator_config_type::make_projection(const ast::location& loc, const iterm& lhs, 
                const iterm& path0)
{
    //this is an error
    if (path0->get_type_code() != ast::details::type_code::int_type)
        return path0;

    ast::int_type path      = path0.static_cast_to<ast::int_type>();
    ast::int_literal il     = ast::int_literal(path0->get_location(), path.get_value());
    ast::path_int_type ret  = ast::path_int_type(loc, lhs, il);
    return ret;
};

ast::type operator_config_type::make_dot(const ast::location& loc, const iterm& lhs, 
                    const ast::identifier& path, const error_handler_ptr& eh)
{
    error::type_error_msg(eh).error_unexpected_field_access(loc, lhs, path);
    return ast::error_type(loc);
};

ast::type operator_config_type::make_applic_type(const ast::location& loc,
                const iterm& lhs, const iterm& rhs)
{
    //decode type arguments
    if (rhs->get_type_code() != ast::details::type_code::product_of_types)
        return ast::error_type(loc);

    ast::product_of_types pt= rhs.static_cast_to<ast::product_of_types>();
    long n                  = pt.size();

    ast::type ret           = lhs;

    for (long i = 0; i < n; ++i)                
    {
        ast::type ti_arg    = pt.get(i);
        ret                 = ast::type_type_application(ti_arg->get_location(), ret, ti_arg);
    }

    return ret;
};

ast::type operator_config_type::make_applic_kind(const ast::location& loc,
                const iterm& lhs, const iterm& rhs)
{
    //decode type arguments
    if (rhs->get_type_code() != ast::details::type_code::annotated_type)
        return ast::error_type(loc);

    ast::annotated_type at  = rhs.static_cast_to<ast::annotated_type>();
    ast::kind ki            = at.get_kind();

    if (ki->get_kind_code() != ast::details::kind_code::product_kind)
        return ast::error_type(loc);

    ast::product_kind pk    = ki.static_cast_to<ast::product_kind>();

    long n                  = pk.size();

    ast::type ret           = lhs;

    for (long i = 0; i < n; ++i)                
    {
        ast::kind ki_arg    = pk.get(i);
        ret                 = ast::type_kind_application(ki_arg->get_location(), ret, ki_arg);
    }

    return ret;
};

//---------------------------------------------------------------
//               operator_config_term
//---------------------------------------------------------------
operator_config_term::iterm
operator_config_term::hide_type_applic(const ast::location& loc, const itype_vec& args)
{
    ast::tuple_type tt(loc, ast::tuple_type::fixed);
    for (const auto& elem : args)
        tt.add(elem);

    // store type in val_type node
    ast::val_term vt            = ast::val_term(loc, tt);
    return vt;
};

operator_config_term::iterm
operator_config_term::hide_kind_applic(const ast::location& loc, const ikind_vec& args)
{
    ast::product_kind tt(loc, true);
    for (const auto& elem : args)
        tt.add(elem);

    // store type in annotated term
    ast::error_type  et     = ast::error_type(loc);
    ast::error_term  e_ex   = ast::error_term(loc);
    ast::annotated_type at  = ast::annotated_type(loc, et, tt);
    ast::annotated_term aex = ast::annotated_term(loc, e_ex, at);
    return aex;
};

ast::term 
operator_config_term::make_projection(const ast::location& loc, const iterm& lhs, 
                const iterm& path0)
{
    //this is an error
    if (path0->get_term_code() != ast::details::term_code::int_literal)
        return path0;

    ast::int_literal path   = path0.static_cast_to<ast::int_literal>();
    ast::path_int_term ret  = ast::path_int_term(loc, lhs, path);
    return ret;
};

ast::term 
operator_config_term::make_dot(const ast::location& loc, const iterm& lhs, 
                const ast::identifier& path, const error_handler_ptr& eh)
{
    (void)eh;
    ast::path_term ret  = ast::path_term(loc, lhs, path);
    return ret;
};

ast::term operator_config_term::make_applic_type(const ast::location& loc,
                const iterm& lhs, const iterm& rhs)
{
    //decode type arguments
    if (rhs->get_term_code() != ast::details::term_code::val_term)
        return ast::error_term(loc);

    ast::val_term vt        = rhs.static_cast_to<ast::val_term>();
    ast::type ty            = vt.get_type();

    if (ty->get_type_code() != ast::details::type_code::tuple_type)
        return ast::error_term(loc);

    ast::tuple_type tt      = ty.static_cast_to<ast::tuple_type>();

    long n                  = tt.size();

    ast::term ret           = lhs;

    for (long i = 0; i < n; ++i)                
    {
        ast::type ty_arg    = tt.get(i);
        ret                 = ast::term_type_application(ty_arg->get_location(), ret, ty_arg);
    }

    return ret;
};

ast::term operator_config_term::make_applic_kind(const ast::location& loc,
                const iterm& lhs, const iterm& rhs)
{
    //decode kind arguments
    if (rhs->get_term_code() != ast::details::term_code::annotated_term)
        return ast::error_term(loc);

    ast::annotated_term vt  = rhs.static_cast_to<ast::annotated_term>();
    ast::type ty            = vt.get_type();

    if (ty->get_type_code() != ast::details::type_code::annotated_type)
        return ast::error_term(loc);

    ast::annotated_type aty = ty.static_cast_to<ast::annotated_type>();
    ast::kind ki            = aty.get_kind();

    if (ki->get_kind_code() != ast::details::kind_code::product_kind)
        return ast::error_term(loc);

    ast::product_kind pk    = ki.static_cast_to<ast::product_kind>();

    long n                  = pk.size();

    ast::term ret           = lhs;

    for (long i = 0; i < n; ++i)                
    {
        ast::kind ty_arg    = pk.get(i);
        ret                 = ast::term_kind_application(ty_arg->get_location(), ret, ty_arg);
    }

    return ret;
};

};};
