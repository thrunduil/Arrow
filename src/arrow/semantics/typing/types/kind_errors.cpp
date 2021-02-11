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

#include "kind_checker_vis.h"
#include "error/error.h"

namespace arrow { namespace typing { namespace details
{

void kind_check_vis::error_star_kind_required(const ast::location& loc, const ast::kind& given,
                details::star_kind_reason reason, const ast::type& root_type, const ast::type& cur_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, cur_type);

    error::kind_error_msg(m_owner->m_eh).error_star_kind_required(loc, given, reason, stack);
}

void kind_check_vis::error_product_kind_required(const ast::location& loc, const ast::kind& given, long size, 
            const ast::type& root_type, const ast::type& cur_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, cur_type);

    error::kind_error_msg(m_owner->m_eh).error_product_kind_required(loc, given, size, stack);
};

void kind_check_vis::error_star_kind_required_field(const ast::location& loc, const ast::kind& given,
            const ast::identifier& field_name, const ast::type& root_type, 
            const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_star_kind_field_required(loc, given, field_name, stack);
};

void kind_check_vis::error_value_kind_required(const ast::location& loc, ast::value_kind_code vc,
            const ast::kind& given, const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_value_kind_required(loc, vc, given, stack);
};

void kind_check_vis::error_poly_kind_required(const ast::location& loc, const ast::kind& given,
                        const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_poly_kind_required(loc, given, stack);
};

void kind_check_vis::error_arrow_kind_required(const ast::location& loc, const ast::kind& given,
                details::arrow_kind_reason reason, const ast::type& root_type, 
                const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_arrow_kind_required(loc, given, reason, stack);
}

void kind_check_vis::error_all_kind_required(const ast::location& loc, const ast::kind& given, 
            const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_all_kind_required(loc, given, stack);
}

void kind_check_vis::error_kind_mismatch(const ast::location& loc, const ast::kind& required, 
                const ast::kind& given, const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_kind_mismatch(loc, required, given, stack);
}

void kind_check_vis::error_type_constant_ord(const ast::location& loc, const ast::type& root_type, 
                        const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_type_constant_cannot_be_ord(loc, stack);
};

void kind_check_vis::error_kind_mismatch_for_symbol(const ast::location& loc, 
                ast::unique_string id, const ast::kind& id_kind, const ast::kind& req,
                const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_kind_mismatch_id(loc, id, id_kind, req, stack);
};

void kind_check_vis::error_kind_polarity_mismatch_for_symbol(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, ast::polarity_type exp_pol, 
            const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_kind_polarity_mismatch_for_symbol
                    (loc, id, pol, exp_pol, stack);
};

void kind_check_vis::error_kind_polarity_mismatch_for_symbol_rec(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, ast::polarity_type exp_pol, 
            const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_kind_polarity_mismatch_for_symbol_rec
                    (loc, id, pol, exp_pol, stack);
};


void kind_check_vis::error_polarized_symbol_on_neutral_pos(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, const ast::type& root_type,
            const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_polarized_symbol_on_neutral_pos
                    (loc, id, pol, stack);
};

void kind_check_vis::error_invalid_kind_application(const ast::location& loc, const ast::type& root_type, 
            const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_invalid_kind_application(loc, stack);
};

void kind_check_vis::error_invalid_kind_application_star(const ast::location& loc, const ast::type& root_type, 
            const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_invalid_kind_application_star(loc, stack);
};

void kind_check_vis::error_invalid_kind_application_sym(const ast::location& loc, const ast::identifier& id,
            const ast::kind& ki, const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_invalid_kind_application_sym
                        (loc, id, ki, stack);
};

void kind_check_vis::error_kind_var_not_allowed_polykind(const ast::location& loc, const ast::identifier& id, 
                const ast::kind& ki, const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_kind_var_not_allowed_polykind(loc, id, ki, stack);
};

void kind_check_vis::error_too_many_kind_args(const ast::location& loc, const ast::identifier& id, 
                const ast::kind& ki, const ast::type& root_type, const ast::type& current_type)
{
    std::vector<ast::type> stack;
    make_type_stack(stack, root_type, current_type);

    error::kind_error_msg(m_owner->m_eh).error_too_many_kind_args(loc, id, ki, stack);
};

void kind_check_vis::error_kind_mismatch_extern_func_ret(const ast::location& loc, 
        const ast::identifier& name, const ast::kind& ret_type, const ast::kind& func_ret_type)
{
    error::kind_error_msg(m_owner->m_eh).error_kind_mismatch_extern_func_ret
                                    (loc, name, ret_type, func_ret_type);
};
void kind_check_vis::error_kind_mismatch_extern_func_size(const ast::location& loc, 
        const ast::identifier& name, long req_arg_size, long func_arg_size)
{
    error::kind_error_msg(m_owner->m_eh).error_kind_mismatch_extern_func_size
                                    (loc, name, req_arg_size, func_arg_size);
};
void kind_check_vis::error_kind_mismatch_extern_func_arg(const ast::location& loc, 
        const ast::identifier& name, long arg_num, const ast::kind& ret_type, 
        const ast::kind& func_ret_type)
{
    error::kind_error_msg(m_owner->m_eh).error_kind_mismatch_extern_func_arg
                            (loc, name, arg_num, ret_type, func_ret_type);
};

}}}
