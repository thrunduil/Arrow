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

#include "type_errors.h"
#include "error/error.h"

namespace arrow { namespace typing { namespace details
{

void type_errors::error_type_has_not_value_kind(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_type_has_not_value_kind(loc, t);
};

void type_errors::error_predefined_type_required(const ast::location& loc, const ast::type& t, 
        ast::predefined_type_code tc)
{
    error::type_error_msg(eh).error_predefined_type_required(loc, t, tc);
};

void type_errors::error_tuple_type_required(const ast::location& loc, const ast::type& t, long size)
{
    error::type_error_msg(eh).error_tuple_type_required(loc, t, size);
};

void type_errors::error_unit_type_required(const ast::location& loc, const ast::type& t)
{
    auto tc = error::type_error_msg::term_type::assign_term;
    error::type_error_msg(eh).error_unit_type_required(loc, t, tc);
};

void type_errors::error_tuple_type_required_unknown_size(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_some_tuple_type_required(loc, t);
};

void type_errors::error_ref_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_ref_type_required(loc, t);
};

void type_errors::error_list_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_list_type_required(loc, t);
};

void type_errors::error_invalid_type_annotation(const ast::location& loc, const ast::type& annnot,
            const ast::type& req)
{
    error::type_error_msg(eh).error_invalid_type_annotation(loc, annnot, req);
};
void type_errors::error_record_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_record_type_required(loc, t);
};

void type_errors::error_arrow_type_required_lambda(const ast::location& loc, const ast::type& t)
{
    auto tc = error::type_error_msg::term_type::lambda_term;
    error::type_error_msg(eh).error_arrow_type_required(loc, t, tc);
};
void type_errors::error_arrow_type_required_app(const ast::location& loc, const ast::type& t)
{
    auto tc = error::type_error_msg::term_type::application;
    error::type_error_msg(eh).error_arrow_type_required(loc, t, tc);
};

void type_errors::error_arrow_type_not_valid_for_fix(const ast::location& loc, const ast::type& lhs, 
        const ast::type& rhs)
{
    error::type_error_msg(eh).error_arrow_type_error_fix(loc, lhs, rhs);
};
void type_errors::error_label_not_found_in_type(const ast::location& loc, const ast::identifier& lab, 
        const ast::type& rt, bool record)
{
    using rec_var = error::type_error_msg::record_or_variant;
    rec_var rv = (record == true) ? rec_var::record : rec_var::variant;

    error::type_error_msg(eh).error_label_not_found_in_type(loc, lab, rt, rv);
};
void type_errors::error_label_not_set_in_term(const ast::location& loc, const ast::identifier& lab, 
        const ast::record_type& rt)
{
    error::type_error_msg(eh).error_label_not_set_in_term(loc, lab, rt);
};

void type_errors::error_invalid_field_type(const ast::location& loc, const ast::identifier& label,
            const ast::type& field_type, const ast::type& req_type)
{
    error::type_error_msg(eh).error_invalid_field_type(loc, label, field_type, req_type);
};
void type_errors::error_invalid_tuple_index(const ast::location& loc, uint64_t pos, long tuple_size)
{
    error::type_error_msg(eh).error_invalid_tuple_index(loc, pos, tuple_size);
};
void type_errors::error_invalid_tuple_elem_type(const ast::location& loc, uint64_t pos, 
            const ast::type& elem_type, const ast::type& req_type)
{
    (void)pos;
    auto tc = error::type_error_msg::term_type::tuple_element;
    error::type_error_msg(eh).error_type_mismatch(loc, elem_type, req_type, tc);
};

void type_errors::error_type_mismatch_extern(const ast::location& loc, const ast::identifier& func_name,
                        const ast::type& annot, const ast::type& req_type)
{
    (void)func_name;
    auto tc = error::type_error_msg::term_type::extern_func;
    error::type_error_msg(eh).error_type_mismatch(loc, annot, req_type, tc);
};

void type_errors::error_type_mismatch_extern_func_ret(const ast::location& loc, const ast::identifier& name,
        const ast::type& ret_type, const ast::type& func_ret_type)
{
    error::type_error_msg(eh).error_type_mismatch_extern_ret(loc, name, ret_type, func_ret_type);
};
void type_errors::error_type_mismatch_extern_func_size(const ast::location& loc, const ast::identifier& name,
        long req_arg_size, long func_arg_size)
{
    error::type_error_msg(eh).error_type_mismatch_extern_arg_size(loc, name, req_arg_size, func_arg_size);
};
void type_errors::error_type_mismatch_extern_func_arg(const ast::location& loc, const ast::identifier& name,
        long arg_num, const ast::type& ret_type, const ast::type& func_ret_type)
{
    error::type_error_msg(eh).error_type_mismatch_extern_arg(loc, name, arg_num, ret_type, func_ret_type);
};

void type_errors::error_variant_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_variant_type_required(loc, t);
};

void type_errors::error_invalid_variant_elem_type(const ast::location& loc, const ast::type& declared,
            const ast::type& required, const ast::identifier& id)
{
    error::type_error_msg(eh).error_invalid_variant_elem_type(loc, declared, required, id);
};
void type_errors::error_invalid_variant_type(const ast::location& loc, const ast::type& declared,
            const ast::type& required)
{
    error::type_error_msg(eh).error_invalid_variant_type(loc, declared, required);
};
void type_errors::error_lambda_arg_type(const ast::location& loc, const ast::identifier& id,
            const ast::type& declared, const ast::type& required)
{
    error::type_error_msg(eh).error_invalid_lambda_arg(loc, id, declared, required);
};
void type_errors::error_nil_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_nil_type_required(loc, t);
};
void type_errors::error_all_type_required_tlam(const ast::location& loc, const ast::type& t)
{
    auto tc = error::type_error_msg::term_type::lambda_type;
    error::type_error_msg(eh).error_all_type_required(loc, t, tc);
};
void type_errors::error_all_type_required_tapp(const ast::location& loc, const ast::type& t)
{
    auto tc = error::type_error_msg::term_type::type_application;
    error::type_error_msg(eh).error_all_type_required(loc, t, tc);
};
void type_errors::error_invalid_type_lambda_kind(const ast::location& loc, const ast::identifier& id,
            const ast::kind& declared, const ast::kind& required)
{
    error::type_error_msg(eh).error_invalid_kind_annot(loc, id, declared, required);
};
void type_errors::error_invalid_type_application(const ast::location& loc, const ast::type& implied,
            const ast::type& required)
{
    auto tc = error::type_error_msg::term_type::type_application;

    error::type_error_msg(eh).error_type_mismatch(loc, implied, required, tc);
};
void type_errors::error_arrow_rhs_app(const ast::location& loc, const ast::type& implied,
            const ast::type& required)
{
    auto tc = error::type_error_msg::term_type::application;

    error::type_error_msg(eh).error_type_mismatch(loc, implied, required, tc);
};

void type_errors::error_type_mismatch_applic(const ast::location& loc, const ast::type& infered, 
            const ast::type& required)
{
    auto tc = error::type_error_msg::term_type::application;

    error::type_error_msg(eh).error_type_mismatch(loc, infered, required, tc);
}

void type_errors::error_type_mismatch_mu(const ast::location& loc, const ast::type& infered, 
            const ast::type& required)
{
    auto tc = error::type_error_msg::term_type::mu_term;

    error::type_error_msg(eh).error_type_mismatch(loc, infered, required, tc);
}

void type_errors::error_existential_type_required(const ast::location& loc, const ast::type& t)
{
    error::type_error_msg(eh).error_some_type_required(loc, t);
};
void type_errors::error_existential_type_escaped(const ast::location& loc, const ast::identifier& id, 
            const ast::type& ty)
{
    error::type_error_msg(eh).error_existential_type_escaped(loc, id, ty);
};

void type_errors::error_invalid_kind_annotation(const ast::location& loc, const ast::identifier& id,
            const ast::kind& kind_annot, const ast::kind& req_kind)
{
    error::type_error_msg(eh).error_invalid_kind_annot(loc, id, kind_annot, req_kind);
    throw;
};
void type_errors::error_invalid_type_annotation(const ast::location& loc, const ast::identifier& id,
            const ast::type& type_annot, const ast::type& req_type)
{
    error::type_error_msg(eh).error_invalid_type_annotation(loc, id, type_annot, req_type);
};
void type_errors::error_invalid_kind(const ast::location& loc, const ast::identifier& id,
            const ast::kind& kind, const ast::kind& req_kind)
{
    error::type_error_msg(eh).error_invalid_kind_annot(loc, id, kind, req_kind);
};
void type_errors::error_star_kind_required(const ast::location& loc, const ast::identifier& id, 
                                           const ast::kind& kind)
{
    error::type_error_msg(eh).error_invalid_kind_annot_star_req(loc, id, kind);
};

void type_errors::error_invalid_existential_type(const ast::location& loc, const ast::type& given, 
        const ast::type& req)
{
    auto tc = error::type_error_msg::term_type::pack;
    error::type_error_msg(eh).error_type_mismatch(loc, given, req, tc);
};

void type_errors::error_type_mismatch_for_symbol(const  ast::location& loc, const ast::identifier& id,
            const ast::type& id_type, const ast::type& req_type)
{
    error::type_error_msg(eh).error_invalid_symbol_type(loc, id, id_type, req_type);
};

void type_errors::error_type_mismatch_kind_abs(const  ast::location& loc, const ast::type& inf_type, 
                                      const ast::type& req_type)
{
    auto tc = error::type_error_msg::term_type::kind_scope;
    error::type_error_msg(eh).error_type_mismatch(loc, inf_type, req_type, tc);
};

void type_errors::error_missing_type_annotation_all(const ast::location& loc)
{
    auto tc = error::type_error_msg::term_type::lambda_type;
    error::type_error_msg(eh).error_missing_type_annotation(loc, tc);
}
void type_errors::error_missing_type_annotation_some(const ast::location& loc)
{
    auto tc = error::type_error_msg::term_type::pack;
    error::type_error_msg(eh).error_missing_type_annotation(loc, tc);
}

void type_errors::error_recursive_symbol_not_annotated(const ad::var_info& vi, const ast::type& ty, 
        const rec_group& group)
{
    (void)ty;

    std::vector<ad::var_info> rec_sym;
    rec_sym.reserve(group.size());

    for (const auto& pos : group)
    {
        const ad::var_info& v  = std::get<0>(pos);
        rec_sym.push_back(v);
    };

    error::type_error_msg(eh).error_recursive_symbol_not_annotated(vi, rec_sym);
};

void type_errors::error_case_invalid_label(const ast::location& loc, const ast::identifier& id, 
    const ast::variant_type& t_sum)
{
    error::type_error_msg(eh).error_case_invalid_label(loc, id, t_sum);
};
void type_errors::error_case_missing_labels(const ast::location& loc, const std::vector<ast::identifier>& missing,
    const ast::variant_type& t_sum)
{
    error::type_error_msg(eh).error_case_missing_labels(loc, missing, t_sum);
};

}}}
