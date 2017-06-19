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

#include "arrow/config.h"
#include "arrow/error_handler.h"
#include "arrow/details/fwd_decls.h"

namespace arrow { namespace error
{

namespace ad    = arrow :: details;
namespace atd   = arrow::typing::details;
namespace at    = arrow::typing;

class error_formatter;
class line_mark;

class error_base_msg
{
    protected:
        error_handler_ptr   m_eh;

    public:
        using import_map    = std::map<ast::unique_string, module>;

    public:
        error_base_msg(const error_handler_ptr& eh);

        void    internal_error(const ast::location& loc, const std::string& msg);

    public:
        void    print_type(std::ostream& os, const ast::type& t);
        void    print_kind(std::ostream& os, const ast::kind& k);        
        void    print_vector(std::ostream& os, const std::vector<std::string>& vec);
        void    print_locations(error_formatter& ef, const std::vector<ast::location>& locs);
        void    print_predefined_type(std::ostream& os, ast::predefined_type_code tc);
        void    print_identifiers(std::ostream& os, const std::vector<ast::identifier>& id_vec);
        void    print_imports(std::ostream& os, const import_map& imports);

        std::string get_module_name(ast::unique_string mod_name);
};

class operator_error_msg : public error_base_msg
{
    public:
        operator_error_msg(const error_handler_ptr& eh);

        void    error_invalid_operator_property(const ast::location& loc, const std::string& prop_type, 
                    const std::vector<std::string>& allowed_properties);
        void    error_invalid_operator_property_value(const ast::location& loc, const std::string& prop_type,
                    const std::string& prop_value, const std::vector<std::string>& allowed_properties);
        void    error_invalid_operator_precedence(const ast::location& loc, uint64_t prec, int prec_min, 
                    int prec_max);
        void    error_operator_kind_not_set(const ast::location& loc);
        void    error_precedence_of_infix_operator_not_set(const ast::location& loc);
        void    error_invalid_opening_and_closing_brackets(const ast::location& loc, 
                    const std::string& brack_op_name);    
        void    error_brackets_must_be_indexing_surrounding(const ast::location& loc, ast::unique_string op);
        void    error_only_brackets_can_be_indexing_surrounding(const ast::location& loc, 
                    ast::unique_string op);
        void    error_tuple_operator_cannot_be_surrounding(const ast::location& loc, ast::unique_string op);
        void    error_type_application_not_operator(const ast::location& loc, ast::unique_string op);
        void    error_dot_bracket_cannot_be_surrounding(const ast::location& loc, ast::unique_string op);
        void    error_bracket_operator_cannot_be_prefix(const ast::location& loc, ast::unique_string op);
        void    error_opening_closing_brakets_dont_match(const ast::location& open, const ast::location& close,
                    const std::string& left_br, const std::string& right_br);
        void    error_ambiguous_declarations_of_operator(const ast::location& loc, ast::unique_string name,
                    std::vector<ast::location>& declarations);
        void    error_undefined_operator(const ast::location& loc, ast::unique_string oper, 
                    ast::fixity_type ft);
        void    error_explicit_fixity_for_opening_bracket(const ast::location& loc);
        void    error_unable_parse_operators(const ast::location& loc);
        void    error_no_valid_parse_for_operator(const ast::location& loc, ast::unique_string op);
        void    error_ambiguous_parse_for_operator(const ast::location& loc, ast::unique_string op,
                    bool infix, bool prefix, bool postfix);
        void    error_invalid_sequence_of_nonassociative_operators(const ast::location& loc, 
                    ast::unique_string prev_op, ast::fixity_type prev_ft, ast::unique_string current_op, 
                    ast::fixity_type cur_ft);
        void    error_quoting_operator_requires_identifier(const ast::location& loc, ast::unique_string op);
        void    error_quoting_operator_identifier_with_fixity(const ast::location& loc, ast::unique_string op,
                    ast::unique_string arg_name, ast::fixity_type arg_ft);
        void    error_invalid_operator_definition_no_valid_declaration(const ast::location& open, 
                    ast::unique_string op, ast::fixity_type ft, const oper::operator_declaration& op_decl);
        void    error_fixity_defined(ast::unique_string name, const ast::location& loc,
                    const ast::location& old_loc);        

        void    warning_ambiguous_parsing_operator(const ast::location& loc, ast::unique_string op_name);
        void    warning_explicit_fixity_not_for_operator(const ast::location& loc, ast::unique_string op_name);        
        void    warning_fixity_hidden(ast::unique_string name, const ast::location& loc,
                    const ast::location& old_loc);        

    private:
        void    print_operator_decl(error_formatter& os, const line_mark& lm, 
                    const oper::operator_declaration& op_decl);
};

class literal_error_msg : public error_base_msg
{
    public:
        literal_error_msg(const error_handler_ptr& eh);

    public:
        void    error_integer_literal_is_too_big(const ast::location& loc, const std::string& literal);
        void    error_integer_literal_is_too_big(const ast::location& loc, uint64_t val);
        void    error_invalid_integer_literal(const ast::location& loc, const std::string& literal);
        void    error_invalid_utf8_byte_sequence(const ast::location& loc);
        void    error_code_point_is_not_unicode_char(const ast::location& loc, const char* buffer);
        void    error_invalid_escape_sequence(const ast::location& loc, const std::string& s);        
        void    error_invalid_unicode_char_in_escape_sequence(const ast::location& loc, 
                    const std::string& escape_sequence);
        void    error_invalid_character_literal(const ast::location& loc, const std::string& literal);
        void    error_invalid_string_literal(const ast::location& loc, const std::string& literal);                
        void    error_invalid_floating_point_literal(const ast::location& loc, const std::string& literal);
        void    error_floating_point_literal_is_not_representable(const ast::location& loc, 
                    const std::string& literal);
};

class scope_error_msg : public error_base_msg
{
    public:
        scope_error_msg(const error_handler_ptr& eh);

    public:
        void    error_underscore_not_allowed(const ast::location& loc);
        void    error_symbol_defined(ast::unique_string name, const ast::location& loc, 
                    const ast::location& old_loc);
        void    error_term_constant_outside_term_definition(const ast::location& loc);
        void    error_cell_term_not_allowed_parse(const ast::location& loc);
        void    error_kind_scope_not_allowed(const ast::location& loc);
        void    error_closure_term_not_allowed(const ast::location& loc);
        void    error_overloaded_symbol_not_allowed(const ast::location& loc);
        void    error_unexpected_filled_type_var(const ast::location& loc, const ast::type_var& ty);
        void    error_type_constant_outside_type_definition(const ast::location& loc);
        void    error_type_var_not_allowed(const ast::location& loc);
        void    error_type_symbol_already_bound(const ast::location& loc, ast::unique_string name,
                    const ast::location& prev_bind);
        void    error_global_symbol(ast::unique_string name, const ast::location& loc);
        void    error_type_defined(ast::unique_string name, const ast::location& loc, 
                    const ast::location& old_loc);
        void    error_extern_function_cannot_be_local(const ast::location& loc, const ast::identifier& id);
        void    error_function_definition_without_body(const ast::location& loc, ast::unique_string name);
        void    error_invalid_imported_module_name(const ast::location& loc, ast::unique_string mod_name,
                    const import_map& imports);
        void    error_module_symbol_used(const ast::location& loc, const ast::identifier& id);
        void    error_extern_function_not_defined(const ast::location& loc, const ast::identifier& id);
        void    error_extern_type_function_not_defined(const ast::location& loc, const ast::identifier& id);
        void    error_ambiguous_symbol(const ast::location& loc, const ast::unique_string name,
                    std::vector<ast::location>& declarations);
        void    error_undefined_variable(ast::unique_string name, const ast::location& loc);                
        void    error_ambiguous_type_symbol(const ast::location& loc, const ast::unique_string name,
                    std::vector<ast::location>& declarations);
        void    error_undefined_type_symbol(const ast::location& loc, ast::unique_string name);
        void    error_undefined_kind_symbol(const ast::location& loc, ast::unique_string name);
        void    error_module_in_expression(const ast::location& loc, const ast::identifier& id);
        void    error_undefined_import(const ast::location& loc, ast::unique_string name);
        void    error_unable_find_import_def(const ast::location& loc, const std::string& mod_name);
        void    error_cyclical_dependencies_module(const std::vector<ast::unique_string>& cycle);

        void    warning_symbol_hidden(ast::unique_string name, const ast::location& new_pos, 
                    const ast::location& old_pos);
        void    warning_unused_let_definition(ast::unique_string name, const ast::location& pos);
};

class kind_error_msg : public error_base_msg
{
    public:
        kind_error_msg(const error_handler_ptr& eh);

    public:
        void    error_infinite_kind(const ast::location& loc, const ast::kind_var& k1, const ast::kind& k2);
        void    error_star_kind_required(const ast::location& loc, const ast::kind& given, 
                    arrow::typing::details::star_kind_reason reason, 
                    const std::vector<ast::type>& stack);
        void    error_star_kind_required_var(const ast::location& loc, const ast::kind& given);
        void    error_product_kind_required(const ast::location& loc, const ast::kind& given, 
                    long req_size, const std::vector<ast::type>& stack);
        void    error_star_kind_field_required(const ast::location& loc, const ast::kind& given, 
                    const ast::identifier& field_name, const std::vector<ast::type>& stack);
        void    error_value_kind_required(const ast::location& loc, ast::value_kind_code vc, 
                    const ast::kind& given, const std::vector<ast::type>& stack);
        void    error_poly_kind_required(const ast::location& loc, const ast::kind& given, 
                    const std::vector<ast::type>& stack);
        void    error_arrow_kind_required(const ast::location& loc, const ast::kind& given, 
                    arrow::typing::details::arrow_kind_reason reason,
                    const std::vector<ast::type>& stack);
        void    error_all_kind_required(const ast::location& loc, const ast::kind& given, 
                    const std::vector<ast::type>& stack);
        void    error_kind_mismatch(const ast::location& loc, const ast::kind& required, 
                    const ast::kind& given, const std::vector<ast::type>& stack);
        void    error_type_constant_cannot_be_ord(const ast::location& loc, 
                    const std::vector<ast::type>& stack);
        void    error_kind_mismatch_id(const ast::location& loc, ast::unique_string id, 
                    const ast::kind& id_kind, const ast::kind& required, 
                    const std::vector<ast::type>& stack);
        void    error_kind_polarity_mismatch_for_symbol(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, ast::polarity_type pol_exp, 
                    const std::vector<ast::type>& stack);
        void    error_kind_polarity_mismatch_for_symbol_rec(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, ast::polarity_type pol_exp, 
                    const std::vector<ast::type>& stack);
        void    error_polarized_symbol_on_neutral_pos(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, 
                    const std::vector<ast::type>& stack);        
        void    error_invalid_kind_application(const ast::location& loc, 
                    const std::vector<ast::type>& stack);
        void    error_invalid_kind_application_star(const ast::location& loc, 
                    const std::vector<ast::type>& stack);
        void    error_invalid_kind_application_sym(const ast::location& loc, const ast::identifier& id,
                    const ast::kind& ki, const std::vector<ast::type>& stack);
        void    error_kind_var_not_allowed_polykind(const ast::location& loc, const ast::identifier& sym,
                    const ast::kind& ki, const std::vector<ast::type>& stack);
        void    error_too_many_kind_args(const ast::location& loc, const ast::identifier& sym,
                    const ast::kind& ki, const std::vector<ast::type>& stack);
        void    error_kind_mismatch_extern_func_ret(const ast::location& loc, const ast::identifier& name,
                    const ast::kind& ret_type, const ast::kind& func_ret_type);
        void    error_kind_mismatch_extern_func_size(const ast::location& loc, const ast::identifier& name,
                    long req_arg_size, long func_arg_size);
        void    error_kind_mismatch_extern_func_arg(const ast::location& loc, const ast::identifier& name,
                    long arg_num, const ast::kind& ret_type, const ast::kind& func_ret_type);

    private:
        void    print_reason(arrow::typing::details::star_kind_reason reason, std::ostream& os);
        void    print_stack(error_formatter& os, const std::vector<ast::type>& stack);
        void    print_polarity(std::ostream& os, ast::polarity_type pol);
};

class type_error_msg : public error_base_msg
{
    public:
        using overload_set  = ad::overload_set<ad::overload_term_config>;

    public:
        type_error_msg(const error_handler_ptr& eh);

        enum class term_type
        {
            type_application,   tuple_element,  lambda_type,    pack,
            lambda_term,        application,    assign_term,    kind_scope,
            extern_func,        mu_term,
        };

        enum class record_or_variant
        {
            record, variant
        };

    public:
        void    error_unexpected_field_access(const ast::location& loc, const ast::type& lhs,
                    const ast::identifier& id);
        void    error_unable_to_deduce_kind_application(const ast::location& loc, 
                    const ast::identifier& kind_id, const ast::kind& infered_kind);
        void    error_kind_abstraction_required(const ast::location& loc, const ast::type& ty);
        void    error_kind_abstraction_required(const ast::location& loc, const ast::kind& ki);
        void    error_invalid_kind_application_too_many_args(const ast::location& loc, const ast::type& ty,
                    long max_allowed, long supplied);
        void    error_invalid_kind_application_too_many_args(const ast::location& loc, const ast::kind& k,
                    long max_allowed, long supplied);
        void    error_mu_type_not_contractive(const ast::location& loc, const ast::type& ty,
                    const ast::identifier& rec_label, const ast::location& unguarded_loc);
        void    error_infinite_type(const ast::location& loc, const ast::type_var& t1, const ast::type& t2);
        void    error_type_has_not_value_kind(const ast::location& loc, const ast::type& t);
        void    error_predefined_type_required(const ast::location& loc, const ast::type& t, 
                    ast::predefined_type_code tc);        
        void    error_tuple_type_required(const ast::location& loc, const ast::type& t, long size);
        void    error_unit_type_required(const ast::location& loc, const ast::type& t, term_type term);
        void    error_some_tuple_type_required(const ast::location& loc, const ast::type& t);
        void    error_ref_type_required(const ast::location& loc, const ast::type& t);
        void    error_list_type_required(const ast::location& loc, const ast::type& t);
        void    error_invalid_type_annotation(const ast::location& loc, const ast::type& given,
                    const ast::type& required);
        void    error_record_type_required(const ast::location& loc, const ast::type& t);
        void    error_arrow_type_required(const ast::location& loc, const ast::type& t, 
                    term_type term);
        void    error_arrow_type_error_fix(const ast::location& loc, const ast::type& lhs, 
                    const ast::type& rhs);
        void    error_label_not_found_in_type(const ast::location& loc, const ast::identifier& lab, 
                    const ast::type& type, record_or_variant rv);
        void    error_label_not_set_in_term(const ast::location& loc, const ast::identifier& lab, 
                    const ast::record_type& rec_type);
        void    error_invalid_field_type(const ast::location& loc, const ast::identifier& label,
                    const ast::type& field_type, const ast::type& req_type);
        void    error_invalid_tuple_index(const ast::location& loc, uint64_t index, 
                    long tuple_size);                        
        void    error_type_mismatch(const ast::location& loc, const ast::type& evaluated, 
                    const ast::type& expected, term_type term);
        void    error_type_mismatch_extern_ret(const ast::location& loc, const ast::identifier& name,
                    const ast::type& ret_type, const ast::type& func_ret_type);
        void    error_type_mismatch_extern_arg_size(const ast::location& loc, const ast::identifier& name,
                    long req_arg_size, long func_arg_size);
        void    error_type_mismatch_extern_arg(const ast::location& loc, const ast::identifier& name,
                    long arg_num, const ast::type& ret_type, const ast::type& func_ret_type);
        void    error_variant_type_required(const ast::location& loc, const ast::type& t);
        void    error_invalid_variant_elem_type(const ast::location& loc, const ast::type& declared,
                    const ast::type& required, const ast::identifier& id);
        void    error_invalid_variant_type(const ast::location& loc, const ast::type& declared,
                    const ast::type& required);
        void    error_invalid_lambda_arg(const ast::location& loc, const ast::identifier& label,
                    const ast::type& declared, const ast::type& req_type);
        void    error_nil_type_required(const ast::location& loc, const ast::type& t);        
        void    error_all_type_required(const ast::location& loc, const ast::type& t, term_type term);        
        void    error_invalid_kind_annot(const ast::location& loc, const ast::identifier& id, 
                    const ast::kind& declared, const ast::kind& required);
        void    error_invalid_kind_annot_star_req(const ast::location& loc, const ast::identifier& id, 
                    const ast::kind& declared);
        void    error_some_type_required(const ast::location& loc, const ast::type& t);
        void    error_existential_type_escaped(const ast::location& loc, const ast::identifier& id, 
                    const ast::type& ty);        
        void    error_invalid_type_annotation(const ast::location& loc, const ast::identifier& id,
                    const ast::type& given, const ast::type& required);
        void    error_invalid_symbol_type(const ast::location& loc, const ast::identifier& label,
                    const ast::type& declared, const ast::type& req_type);
        void    error_missing_type_annotation(const ast::location& loc, term_type term);
        void    error_recursive_symbol_not_annotated(const ad::var_info& vi, 
                    const std::vector<ad::var_info>& recursive_group);
        void    error_case_invalid_label(const ast::location& loc, const ast::identifier& id, 
                    const ast::variant_type& t_sum);
        void    error_case_missing_labels(const ast::location& loc, const std::vector<ast::identifier>& missing,
                    const ast::variant_type& t_sum);
        void    error_unable_resolve_overload_no_context(const ast::location& loc, const ast::identifier& id);
        void    error_abstract_overload_not_defined(const ast::location& loc, const ast::identifier& id,
                    const ast::type& req_type, const overload_set& overloads);
        void    error_deduction_failure(const ast::location& loc, const std::vector<ast::identifier>& idents,
                    const ast::type& ty);
        void    error_ambiguous_overload(const ast::location& loc, const ast::identifier& id, 
                    const std::vector<atd::applic_arg_term>& args, const std::vector<at::overload_info>& candidates);
        void    error_no_valid_overload(const ast::location& loc, const ast::identifier& id, 
                    const std::vector<atd::applic_arg_term>& args, const overload_set& overloads);
        void    error_term_constant_not_annotated(const ast::location& loc, const ast::identifier& id,
                    const ast::type& ty);
        void    error_extern_function_cannot_be_polymorphic(const ast::location& loc, const ast::identifier& id,
                    const ast::type& ty);                                

    private:
        void    print_term_type(term_type term, std::ostream& os);
        void    print_symbol_group(error_formatter& os, const line_mark& lm, 
                    const std::vector<ad::var_info>& recursive_group);
        void    print_application_args(std::ostream& os, const std::vector<atd::applic_arg_term>& args);
        void    print_candidates(error_formatter& os, const line_mark& lm, 
                    const std::vector<at::overload_info>& candidates);
        void    print_overloads(error_formatter& os, const line_mark& lm, const overload_set& overloads);
};

}}
