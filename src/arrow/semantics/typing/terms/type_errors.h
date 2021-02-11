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

#include "term_checker.h"

namespace arrow { namespace typing { namespace details
{

struct type_errors
{
    using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
    using rec_group     = std::vector<group_item>;

    error_handler_ptr eh;

    type_errors(const error_handler_ptr& eh_)
        :eh(eh_)
    {};

    void error_predefined_type_required(const ast::location& loc, const ast::type& t, 
             ast::predefined_type_code tc);
    void error_type_has_not_value_kind(const ast::location& loc, const ast::type& t);
    void error_tuple_type_required(const ast::location& loc, const ast::type& t, long size);
    void error_tuple_type_required_unknown_size(const ast::location& loc, const ast::type& t);
    void error_ref_type_required(const ast::location& loc, const ast::type& t);
    void error_list_type_required(const ast::location& loc, const ast::type& t);
    void error_invalid_type_annotation(const ast::location& loc, const ast::type& annnot,
             const ast::type& req);
    void error_record_type_required(const ast::location& loc, const ast::type& t);
    void error_arrow_type_required_lambda(const ast::location& loc, const ast::type& t);
    void error_arrow_type_required_app(const ast::location& loc, const ast::type& t);
    void error_arrow_type_not_valid_for_fix(const ast::location& loc, const ast::type& lhs, 
             const ast::type& rhs);
    void error_arrow_rhs_app(const ast::location& loc, const ast::type& infered, 
             const ast::type& required);
    void error_type_mismatch_applic(const ast::location& loc, const ast::type& infered, 
             const ast::type& required);
    void error_type_mismatch_mu(const ast::location& loc, const ast::type& infered, 
             const ast::type& required);
    void error_label_not_found_in_type(const ast::location& loc, const ast::identifier& lab, 
             const ast::type& ty, bool record);
    void error_label_not_set_in_term(const ast::location& loc, const ast::identifier& lab, 
             const ast::record_type& rt);
    void error_invalid_field_type(const ast::location& loc, const ast::identifier& label,
             const ast::type& field_type, const ast::type& req_type);
    void error_invalid_tuple_index(const ast::location& loc, uint64_t pos, long tuple_size);
    void error_invalid_tuple_elem_type(const ast::location& loc, uint64_t pos, 
             const ast::type& elem_type, const ast::type& req_type);
    void error_variant_type_required(const ast::location& loc, const ast::type& t);
    void error_invalid_variant_elem_type(const ast::location& loc, const ast::type& declared,
             const ast::type& required, const ast::identifier& lab);
    void error_invalid_variant_type(const ast::location& loc, const ast::type& declared,
             const ast::type& required);
    void error_lambda_arg_type(const ast::location& loc, const ast::identifier& id,
             const ast::type& declared, const ast::type& required);
    void error_nil_type_required(const ast::location& loc, const ast::type& t);
    void error_all_type_required_tlam(const ast::location& loc, const ast::type& t);
    void error_all_type_required_tapp(const ast::location& loc, const ast::type& t);
    void error_invalid_type_lambda_kind(const ast::location& loc, const ast::identifier& id,
             const ast::kind& declared, const ast::kind& required);
    void error_invalid_type_application(const ast::location& loc, const ast::type& evaled,
             const ast::type& expected);
    void error_existential_type_required(const ast::location& loc, const ast::type& t);
    void error_existential_type_escaped(const ast::location& loc, const ast::identifier& id, 
             const ast::type& ty);
    void error_invalid_kind_annotation(const ast::location& loc, const ast::identifier& id,
             const ast::kind& kind_annot, const ast::kind& req_kind);
    void error_invalid_type_annotation(const ast::location& loc, const ast::identifier& id,
             const ast::type& type_annot, const ast::type& req_type);
    void error_invalid_kind(const ast::location& loc, const ast::identifier& id,
             const ast::kind& kind, const ast::kind& req_kind);
    void error_star_kind_required(const ast::location& loc, const ast::identifier& id, const ast::kind& kind);
    void error_invalid_existential_type(const ast::location& loc, const ast::type& given, const ast::type& req);
    void error_type_mismatch_for_symbol(const  ast::location& loc, const ast::identifier& id,
             const ast::type& id_type, const ast::type& req_type);
    void error_type_mismatch_kind_abs(const  ast::location& loc, const ast::type& inf_type, 
             const ast::type& req_type);
    void error_missing_type_annotation_all(const ast::location& loc);
    void error_missing_type_annotation_some(const ast::location& loc);
    void error_unit_type_required(const ast::location& loc, const ast::type& t);
    void error_recursive_symbol_not_annotated(const ad::var_info& vi, const ast::type& ty, 
            const rec_group& group);
    void error_type_mismatch_extern(const ast::location& loc, const ast::identifier& name, const ast::type& annot, 
            const ast::type& req_type);
    void error_type_mismatch_extern_func_ret(const ast::location& loc, const ast::identifier& name,
            const ast::type& ret_type, const ast::type& func_ret_type);
    void error_type_mismatch_extern_func_size(const ast::location& loc, const ast::identifier& name,
            long req_arg_size, long func_arg_size);
    void error_type_mismatch_extern_func_arg(const ast::location& loc, const ast::identifier& name,
            long arg_num, const ast::type& ret_type, const ast::type& func_ret_type);
    void error_case_invalid_label(const ast::location& loc, const ast::identifier& id, 
            const ast::variant_type& t_sum);
    void error_case_missing_labels(const ast::location& loc, const std::vector<ast::identifier>& missing,
            const ast::variant_type& t_sum);
};

};};};