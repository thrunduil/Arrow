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

#include <memory>

namespace arrow { namespace ast
{
    class unique_string;
    class location;
    class symbol_data;
    class overload_index;

    class identifier;
    class annotated_type_identifier;
    class annotated_identifier;
    class module_definition;

    class definitions;
    class term_definition;
    class term_use;
    class type_definition;
    class type_definition_var;
    class fixity_definition;

    class term;
    class closure_term;
    class unpack_term;
    class record_term;
    class term_constant;
    class lambda_term;
    class mu_term;
    class all_term;
    class int_literal;
    class char_literal;
    class bool_literal;
    class string_literal;
    class float_literal;
    class term_type_application;
    class term_kind_application;
    class term_application;
    class tuple_term;
    class term_sequence;
    class list_term;
    class bracket_term;
    class assign_term;
    class record_term;
    class error_term;
    class ref_term;
    class deref_term;
    class identifier_term;
    class overloaded_symbol;
    class annotated_term;
    class if_term;
    class let_term;
    class path_term;
    class path_int_term;
    class pack_term;
    class variant_term;
    class variant_case_term;
    class cell_term;
    class val_term;
    class term_kind_scope;

    class type;
    class arrow_type;
    class some_type;
    class variant_type;
    class all_type;
    class lambda_type;
    class mu_type;
    class record_type;
    class annotated_type;
    class tuple_type;
    class bracket_type;
    class ref_type;
    class if_type;
    class path_int_type;
    class identifier_type;
    class type_application;
    class predefined_type;
    class type_var;
    class list_type;
    class type_constant;
    class product_of_types;
    class type_sequence;
    class error_type;
    class int_type;
    class bool_type;
    class char_type;
    class string_type;
    class type_kind_scope;
    class type_kind_application;
    class type_type_application;

    class kind;
    class arrow_kind;
    class star_arrow_kind;
    class product_kind;
    class star_tuple_kind;
    class star_kind;
    class value_kind;
    class kind_var;
    class kind_identifier;
    class kind_scope;
    class star_all_kind;
    class star_some_kind;
    class star_list_kind;
    class star_record_kind;
    class star_variant_kind;
    class star_predefined_kind;
    class star_ref_kind;

    class definition;

    class term_atom;
    class type_atom;
    class kind_atom;
    class ast_atom;
    class definition_atom;

}};

namespace arrow { namespace ast { namespace details
{
    class identifier_impl;
    class annotated_type_identifier_impl;
    class annotated_identifier_impl;
    class field_type_impl;
    class module_definition_impl;
    class symbol_sequence_impl;

    class definitions_impl;
    class term_definition_impl;
    class term_use_impl;
    class type_definition_impl;
    class type_definition_var_impl;
    class fixity_definition_impl;

    class term_impl;
    class unpack_term_impl;
    class closure_term_impl;
    class record_term_impl;
    class term_constant_impl;
    class lambda_term_impl;
    class mu_term_impl;
    class all_term_impl;
    class int_literal_impl;
    class char_literal_impl;
    class bool_literal_impl;
    class string_literal_impl;
    class float_literal_impl;
    class term_type_application_impl;
    class term_kind_application_impl;
    class term_application_impl;
    class tuple_term_impl;
    class term_sequence_impl;
    class error_term_impl;
    class ref_term_impl;
    class deref_term_impl;
    class identifier_term_impl;
    class overloaded_symbol_impl;
    class annotated_term_impl;
    class if_term_impl;
    class let_term_impl;
    class path_term_impl;
    class path_int_term_impl;
    class pack_term_impl;
    class variant_term_impl;
    class variant_case_term_impl;
    class list_term_impl;
    class bracket_term_impl;
    class assign_term_impl;
    class cell_term_impl;
    class val_term_impl;
    class term_kind_scope_impl;

    class type_impl;
    class arrow_type_impl;
    class some_type_impl;
    class variant_type_impl;
    class all_type_impl;
    class lambda_type_impl;
    class mu_type_impl;
    class record_type_impl;
    class path_int_type_impl;
    class annotated_type_impl;
    class tuple_type_impl;
    class bracket_type_impl;
    class list_type_impl;
    class ref_type_impl;
    class if_type_impl;
    class identifier_type_impl;
    class type_application_impl;
    class predefined_type_impl;
    class type_var_impl;
    class type_constant_impl;
    class product_of_types_impl;
    class type_sequence_impl;
    class unpack_list_type_impl;
    class error_type_impl;
    class int_type_impl;
    class bool_type_impl;
    class char_type_impl;
    class string_type_impl;
    class type_kind_scope_impl;
    class type_kind_application_impl;
    class type_type_application_impl;

    class kind_impl;
    class arrow_kind_impl;
    class star_arrow_kind_impl;
    class product_kind_impl;
    class star_tuple_kind_impl;
    class star_kind_impl;
    class value_kind_impl;
    class kind_var_impl;
    class kind_identifier_impl;
    class kind_scope_impl;
    class star_all_kind_impl;
    class star_some_kind_impl;
    class star_list_kind_impl;
    class star_record_kind_impl;
    class star_variant_kind_impl;
    class star_predefined_kind_impl;
    class star_ref_kind_impl;

    class definition_impl;

}}}
