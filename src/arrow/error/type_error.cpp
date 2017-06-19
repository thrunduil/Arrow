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

#include "error.h"
#include "semantics/typing/types/kind_checker.h"
#include "error_formatter.h"
#include "semantics/typing/terms/term_check_vis.h"
#include "semantics/typing/terms/overload_resolution.h"
#include "scopes/symbol_definition.h"
#include "scopes/overload_set.h"

#include <sstream>

namespace arrow { namespace error
{

type_error_msg::type_error_msg(const error_handler_ptr& eh)
    :error_base_msg(eh)
{};

void type_error_msg::error_term_constant_not_annotated(const ast::location& loc, const ast::identifier& id,
            const ast::type& ty)
{
    error_formatter ef;

    ef.head() << "term constant " << id.get_name_simple().get_string() << " must have full type annotation";

    auto l1 = ef.new_info();
        ef.line() << "type is:";
        ef.new_line(l1);
            print_type(ef.line(), ty);

    m_eh->report_parser_error(loc, ef.str());
};

void type_error_msg::error_extern_function_cannot_be_polymorphic(const ast::location& loc, const ast::identifier& id,
            const ast::type& ty)
{
    error_formatter ef;

    ef.head() << "extern function " << id.get_name_simple().get_string() << " cannot be polymorphic";

    auto l1 = ef.new_info();
        ef.line() << "type is:";
        ef.new_line(l1);
            print_type(ef.line(), ty);

    m_eh->report_parser_error(loc, ef.str());
};

void type_error_msg::error_type_mismatch_extern_ret(const ast::location& loc, const ast::identifier& name,
            const ast::type& ret_type, const ast::type& func_ret_type)
{
    error_formatter ef;
    ef.head()   << "type mismatch while checking extern function " << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "declared return type:";
        ef.new_line(l1);
            print_type(ef.line(), ret_type);

    auto l2 = ef.new_info();
        ef.line() << "return type of definition:";
        ef.new_line(l2);
            print_type(ef.line(), func_ret_type);

    m_eh->report_parser_error(loc, ef.str());
};
void type_error_msg::error_type_mismatch_extern_arg(const ast::location& loc, const ast::identifier& name,
            long arg_num, const ast::type& ret_type, const ast::type& func_ret_type)
{
    error_formatter ef;
    ef.head()   << "type mismatch while checking " << arg_num + 1 
                << "-th argument of extern function " << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "declared type:";
        ef.new_line(l1);
            print_type(ef.line(), ret_type);

    auto l2 = ef.new_info();
        ef.line() << "definition type:";
        ef.new_line(l2);
            print_type(ef.line(), func_ret_type);

    m_eh->report_parser_error(loc, ef.str());
}

void type_error_msg::error_type_mismatch_extern_arg_size(const ast::location& loc, const ast::identifier& name,
            long req_arg_size, long func_arg_size)
{
    error_formatter ef;
    ef.head()   << "invalid number of arguments of extern function " << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "number of arguments in declaration: " << req_arg_size;

    auto l2 = ef.new_info();
        ef.line() << "number of arguments in definition: " << func_arg_size;

    m_eh->report_parser_error(loc, ef.str());
}

void type_error_msg::error_infinite_type(const ast::location& loc, const ast::type_var& t1, 
                                         const ast::type& t2)
{
    error_formatter ef;

    ef.head() << "infinite type:";

    ef.new_line();
    {
        print_type(ef.line(), t1);
        ef.line() << " :=: ";
        print_type(ef.line(), t2);
    };

    m_eh->report_parser_error(loc, ef.str());
};

void type_error_msg::error_missing_type_annotation(const ast::location& loc, term_type term)
{
    error_formatter ef;

    ef.head() << "type annotation required for ";
              print_term_type(term, ef.head());

    m_eh->report_parser_error(loc, ef.str());
};

void type_error_msg::error_type_mismatch(const ast::location& loc, const ast::type& evaluated, 
                                         const ast::type& expected, term_type term)
{
    error_formatter ef;
    ef.head()   << "type mismatch while checking ";
                print_term_type(term, ef.head());

    auto l1 = ef.new_info();
        ef.line() << "evaluated type:";
        ef.new_line(l1);
            print_type(ef.line(), evaluated);

    auto l2 = ef.new_info();
        ef.line() << "expected type:";
        ef.new_line(l2);
            print_type(ef.line(), expected);

    m_eh->report_parser_error(loc, ef.str());
}

void type_error_msg::error_invalid_tuple_index(const ast::location& loc, uint64_t index, 
            long tuple_size)
{
    error_formatter os;
    os.head() << "invalid tuple index " << index;

    os.new_line();
        os.line() << "tuple size is: " << tuple_size;

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_all_type_required(const ast::location& loc, const ast::type& t, term_type term)
{
    error_formatter os;
    os.head() << "type mismatch while checking ";
              print_term_type(term, os.head());

    os.new_info();
        os.line() << "expecting universal type";

    auto l1 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_arrow_type_required(const ast::location& loc, const ast::type& t, 
            term_type term)
{
    error_formatter os;
    os.head()   << "type mismatch while checking ";
                print_term_type(term, os.head());

    os.new_info();
        os.line() << "expecting arrow type";

    auto l1 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_unit_type_required(const ast::location& loc, const ast::type& t, 
            term_type term)
{
    error_formatter os;
    os.head()   << "type mismatch while checking ";
                print_term_type(term, os.head());

    os.new_info();
        os.line() << "expecting () type";

    auto l1 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_some_tuple_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;
    os.head() << "expecting tuple type";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};
void type_error_msg::error_ref_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting ref type";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_list_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting list type";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_nil_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting polymorphic list type (All[A] [A])";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};


void type_error_msg::error_variant_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting sum type";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};
void type_error_msg::error_some_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting existential type";
    
    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_existential_type_escaped(const ast::location& loc, const ast::identifier& id, 
            const ast::type& ty)
{
    error_formatter os;

    os.head() << "existential type escaped through type variable " << id.get_name_simple().get_string();
    
    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_tuple_type_required(const ast::location& loc, const ast::type& t, long size)
{
    error_formatter os;

    os.head() << "expecting tuple type with " << size << " elements";
    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_record_type_required(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "expecting record type";
    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_type_has_not_value_kind(const ast::location& loc, const ast::type& t)
{
    error_formatter os;

    os.head() << "type has not the value kind";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_predefined_type_required(const ast::location& loc, const ast::type& t, 
            ast::predefined_type_code tc)
{
    error_formatter os;

    os.head() << "expecting ";
    print_predefined_type(os.head(), tc);
    os.head() << " type";

    auto l1 = os.new_info();
        os.line() << "evaluated type is:";
        os.new_line(l1);
            print_type(os.line(), t);

    m_eh->report_parser_error(loc, os.str());
}

void type_error_msg::error_label_not_found_in_type(const ast::location& loc, const ast::identifier& lab, 
            const ast::type& rec_type, record_or_variant rv)
{
    error_formatter os;
    os.head() << "field " << lab.get_name_simple().get_string() << " not found in ";
    
    switch(rv)
    {
        case record_or_variant::record:
            os.head() << "record type";
            break;
        case record_or_variant::variant:
            os.head() << "variant type";
            break;
    };

    auto l1 = os.new_info();
        os.line() << "record type is:";
        os.new_line(l1);
            print_type(os.line(), rec_type);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_label_not_set_in_term(const ast::location& loc, const ast::identifier& lab, 
            const ast::record_type& rec_type)
{
    error_formatter os;
    os.head() << "field " << lab.get_name_simple().get_string() << " not set in record term";

    auto l1 = os.new_info();
        os.line() << "record type is:";
        os.new_line(l1);
            print_type(os.line(), rec_type);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_arrow_type_error_fix(const ast::location& loc, const ast::type& lhs, 
            const ast::type& rhs)
{
    error_formatter os;
    os.head() << "invalid arrow type for fix term: lhs and rhs must be equal";
    
    os.new_info();
        os.line() << "lhs = ";
        print_type(os.line(), lhs);

    os.new_info();
        os.line() << "rhs = ";
        print_type(os.line(), rhs);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_type_annotation(const ast::location& loc, const ast::type& given,
            const ast::type& required)
{
    error_formatter os;
    os.head() << "invalid type annotation:";

    os.new_line();
        print_type(os.line(), given);

    auto l1 = os.new_info();
        os.line() << "expecting type ";
        os.new_line(l1);
            print_type(os.line(), required);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_type_annotation(const ast::location& loc, const ast::identifier& id, 
            const ast::type& given, const ast::type& required)
{
    error_formatter os;
    os.head() << "invalid type annotation for symbol " << id.get_name_simple().get_string() << ": ";

    os.new_line();
        print_type(os.line(), given);

    auto l1 = os.new_info();
        os.line() << "expecting type:";
        os.new_line(l1);
            print_type(os.line(), required);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_field_type(const ast::location& loc, const ast::identifier& label,
            const ast::type& field_type, const ast::type& req_type)
{
    error_formatter os;
    os.head() << "type mismatch for record field " << label.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "type at declaration point:";
        os.new_line(l1);
            print_type(os.line(), field_type);

    auto l2 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l2);
            print_type(os.line(), req_type);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_lambda_arg(const ast::location& loc, const ast::identifier& label,
            const ast::type& field_type, const ast::type& req_type)
{
    error_formatter os;
    os.head() << "type mismatch for function argument " << label.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "type at declaration point:";
        os.new_line(l1);
            print_type(os.line(), field_type);

    auto l2 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l2);
            print_type(os.line(), req_type);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_symbol_type(const ast::location& loc, const ast::identifier& label,
            const ast::type& field_type, const ast::type& req_type)
{
    error_formatter os;

    os.head() << "type mismatch for symbol " << label.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "type at declaration point:";
        os.new_line(l1); 
            print_type(os.line(), field_type);

    auto l2 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l2); 
            print_type(os.line(), req_type);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_variant_elem_type(const ast::location& loc, const ast::type& declared,
            const ast::type& required, const ast::identifier& id)
{
    error_formatter os;

    os.head() << "type mismatch for " << id.get_name_simple().get_string() << " element of a variant";

    auto l1 = os.new_info();
        os.line() << "type at declaration point:";
        os.new_line(l1);
            print_type(os.line(), declared);

    auto l2 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l2);
            print_type(os.line(), required);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_variant_type(const ast::location& loc, const ast::type& declared,
            const ast::type& required)
{
    error_formatter os;

    os.head() << "type mismatch for a variant type";

    auto l1 = os.new_info();
        os.line() << "type at declaration point:";
        os.new_line(l1);
            print_type(os.line(), declared);

    auto l2 = os.new_info();
        os.line() << "evaluated type:";
        os.new_line(l2);
            print_type(os.line(), required);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_kind_annot(const ast::location& loc, const ast::identifier& id, 
            const ast::kind& declared, const ast::kind& required)
{
    error_formatter os;

    os.head() << "kind mismatch for symbol " << id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "declared kind: ";
        print_kind(os.line(), declared);

    auto l2 = os.new_info();
        os.line() << "evaluated kind: ";
        print_kind(os.line(), required);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_kind_annot_star_req(const ast::location& loc, const ast::identifier& id, 
            const ast::kind& declared)
{
    error_formatter os;

    os.head() << "kind mismatch for symbol " << id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "declared kind: ";
        print_kind(os.line(), declared);

    auto l2 = os.new_info();
        os.line() << "star kind is required";

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::print_term_type(term_type term, std::ostream& os)
{
    switch (term)
    {
        case term_type::type_application:
            os << "type application";
            break;
        case term_type::tuple_element:
            os << "tuple element";
        case term_type::lambda_type:
            os << "type abstraction";
            break;
        case term_type::pack:
            os << "Some term";
            break;
        case term_type::lambda_term:
            os << "lambda term";
            break;
        case term_type::mu_term:
            os << "rec term";
            break;
        case term_type::application:
            os << "term application";
            break;
        case term_type::assign_term:
            os << "assign term";
            break;
        case term_type::kind_scope:
            os << "kind abstraction";
            break;
        case term_type::extern_func:
            os << "extern function";
            break;
    };
};

void type_error_msg::error_recursive_symbol_not_annotated(const ad::var_info& vi, 
            const std::vector<ad::var_info>& recursive_group)
{
    error_formatter os;
    ast::fixity_type ft;

    os.head() << "recursive symbol " << vi.get_name(ft).get_string() << " must have type annotation";

    if (recursive_group.size() > 1)
    {
        auto l1 = os.new_info();
            os.line() << "recursive group consists symbols:";
            print_symbol_group(os, l1, recursive_group);
    };

    m_eh->report_parser_error(vi.get_location(), os.str());
};

void type_error_msg::print_symbol_group(error_formatter& os, const line_mark& lm, 
            const std::vector<ad::var_info>& symbol_group)
{
    ast::fixity_type ft;

    for (const auto& elem : symbol_group)
    {
        os.new_line(lm);
        os.line() << elem.get_name(ft).get_string() << ", at: " << elem.get_location().to_string(true);
    }
};

void type_error_msg::error_mu_type_not_contractive(const ast::location& loc, const ast::type& ty,
        const ast::identifier& rec_label, const ast::location& unguarded_loc)
{
    error_formatter os;

    os.head() << "recursive type is not contractive";

    auto l1 = os.new_info();
        os.line() << "type is:";
            os.new_line(l1);
            print_type(os.line(), ty);

    auto l2 = os.new_info();
        os.line() << "location of unguarded use of " << rec_label.get_name_simple().get_string()
                  << ": " << unguarded_loc.to_string(false);

    auto l3 = os.new_info();
        os.line() << "note: any use of this type may lead to nontermination";

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_case_invalid_label(const ast::location& loc, const ast::identifier& id, 
            const ast::variant_type& t_sum)
{
    error_formatter os;

    os.head() << "invalid case label " << id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "variant type is:";
            os.new_line(l1);
            print_type(os.line(), t_sum);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_case_missing_labels(const ast::location& loc, const std::vector<ast::identifier>& missing,
            const ast::variant_type& t_sum)
{
    error_formatter os;

    os.head() << "missing cases in case expression";

    auto l1 = os.new_info();
        os.line() << "required cases for variant labels:";
            os.new_line(l1);
            print_identifiers(os.line(), missing);

    auto l2 = os.new_info();
        os.line() << "variant type is:";
            os.new_line(l2);
            print_type(os.line(), t_sum);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_ambiguous_overload(const ast::location& loc, 
            const ast::identifier& id, const std::vector<atd::applic_arg_term>& args, 
            const std::vector<at::overload_info>& candidates)
{
    error_formatter os;

    os.head() << "ambiguous overloaded symbol " + id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "types of applied arguments:";
            os.new_line(l1);
            print_application_args(os.line(), args);

    auto l2 = os.new_info();
        os.line() << "valid candidates:";
        print_candidates(os, l2, candidates);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_no_valid_overload(const ast::location& loc, const ast::identifier& id, 
            const std::vector<atd::applic_arg_term>& args, const overload_set& overloads)
{
    error_formatter os;

    os.head() << "no valid candidates for overloaded symbol " + id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "types of applied arguments:";
            os.new_line(l1);
            print_application_args(os.line(), args);

    auto l2 = os.new_info();
        os.line() << "available overloads:";
        print_overloads(os, l2, overloads);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_abstract_overload_not_defined(const ast::location& loc, const ast::identifier& id,
            const ast::type& req_type, const overload_set& overloads)
{
    error_formatter os;

    os.head() << "abstract overloaded symbol " << id.get_name_simple().get_string() << " not defined";

    auto l1 = os.new_info();
        os.line() << "with explicit type:";
            os.new_line(l1);
            print_type(os.line(), req_type);

    auto l2 = os.new_info();
        os.line() << "available overloads:";
        print_overloads(os, l2, overloads);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_unexpected_field_access(const ast::location& loc, const ast::type& lhs,
            const ast::identifier& id)
{
    error_formatter os;
    (void)lhs;

    os.head() << "unexpected field label " + id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "field access is not allowed for types";

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_unable_resolve_overload_no_context(const ast::location& loc, const ast::identifier& id)
{
    error_formatter os;

    os.head() << "unable to resolve overloaded symbol " + id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "context is missing";

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::print_application_args(std::ostream& os, 
                 const std::vector<atd::applic_arg_term>& args)
{
    using item_type = atd::applic_arg_term::item_type;

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        item_type it    = args[i].get_item_type();

        if (it == atd::applic_arg_term::function_arg)
        {
            print_type(os, args[i].get_func_arg_type());
        }
        else if (it == atd::applic_arg_term::inst_arg)
        {
            os << ".<";
            print_type(os, args[i].get_inst_arg());
            os << ">";
        }
        else if (it == atd::applic_arg_term::kind_arg)
        {
            os << ".Kind[";
            print_kind(os, args[i].get_kind_arg());
            os << "]";
        }
    };
};

void type_error_msg::print_candidates(error_formatter& os, const line_mark& lm, 
                                      const std::vector<at::overload_info>& candidates)
{
    using symbol_def                = ad::symbol_definition_env;
    using symbol_type               = symbol_def::symbol_type;

    const ad::def_index* di;

    for (size_t i = 0; i < candidates.size(); ++i)
    {
        os.new_line(lm);

        const ast::location& loc    = candidates[i].get_location();
        const ast::type& ty         = candidates[i].get_function_type();
        const symbol_def* def       = candidates[i].get_def(di);
        symbol_type sym_type        = def->get_symbol_type();

        os.line() << loc.to_string(true) << " :: ";
        print_type(os.line(), ty);

        if (sym_type == symbol_def::abstract_overload)
            os.line() << " abstract";
        else if (sym_type == symbol_def::closed_overload)
            os.line() << " closed";
    };
};

void type_error_msg::print_overloads(error_formatter& os, const line_mark& lm, 
                                     const overload_set& overloads)
{    
    long count                      = 0;
    long n                          = overloads.size();
    bool can_cont                   = false;

    using symbol_def                = ad::symbol_definition_env;
    using symbol_type               = symbol_def::symbol_type;

    for (long i = 0; i < n; ++i)
    {
        if (count >= 10)
        {
            can_cont = true;
            break;
        }

        const symbol_def* def;
        ad::def_location def_loc;
        def   = overloads.get_definition(i, def_loc);

        if (!def)
            continue;

        if (def->has_error() == true)
            continue;

        const ast::location& loc    = def->get_location();
        const ast::type& ty         = def->get_signature();
        symbol_type sym_type        = def->get_symbol_type();

        os.new_line(lm);
        os.line() << loc.to_string(true) << " :: ";
        print_type(os.line(), ty);

        if (sym_type == symbol_def::abstract_overload)
            os.line() << " abstract";
        else if (sym_type == symbol_def::closed_overload)
            os.line() << " closed";

        if (def_loc.is_from_import() == true)
            os.line() << " imported";

        ++count;
    };

    if (can_cont == true)
    {
        os.new_line(lm);
        os.line() << "...";
    };
};

void type_error_msg::error_invalid_kind_application_too_many_args(const ast::location& loc, const ast::type& ty,
            long max_allowed, long supplied)
{
    error_formatter os;
    os.head() << "invalid kind application: too many arguments";

    auto l1 = os.new_info();
        os.line() << "type can accept at most " << max_allowed << " kind arguments" << " while "
                    << supplied << " arguments was supplied";

    auto l2 = os.new_info();
        os.line() << "type is:";
        os.new_line(l2);
            print_type(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_invalid_kind_application_too_many_args(const ast::location& loc, const ast::kind& ty,
            long max_allowed, long supplied)
{
    error_formatter os;
    os.head() << "invalid kind application: too many arguments";

    auto l1 = os.new_info();
        os.line() << "type can accept at most " << max_allowed << " kind arguments" << " while "
                    << supplied << " arguments was supplied";

    auto l2 = os.new_info();
        os.line() << "kind is:";
        os.new_line(l2);
            print_kind(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_unable_to_deduce_kind_application(const ast::location& loc, 
            const ast::identifier& kind_id, const ast::kind& infered_kind)
{
    error_formatter os;

    const std::string& name = kind_id.get_name_simple().get_string();

    os.head() << "unable to infer kind application for argument " << name;

    os.new_info();
        os.line() << "location of " << name << ": " << kind_id->get_location().to_string(true);

    if (infered_kind != ast::kind())
    {
        auto l = os.new_info();
            os.line() << "partially infered kind application:";
            os.new_line(l);
                print_kind(os.line(), infered_kind);
    };

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_kind_abstraction_required(const ast::location& loc, const ast::type& ty)
{
    error_formatter os;
    os.head() << "invalid kind application: type with kind abstraction is required";

    auto l2 = os.new_info();
        os.line() << "type is:";
        os.new_line(l2);
            print_type(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_kind_abstraction_required(const ast::location& loc, const ast::kind& ty)
{
    error_formatter os;
    os.head() << "invalid kind application: kind abstraction is required";

    auto l2 = os.new_info();
        os.line() << "kind is:";
        os.new_line(l2);
            print_kind(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

void type_error_msg::error_deduction_failure(const ast::location& loc, const std::vector<ast::identifier>& idents,
                                             const ast::type& ty)
{
    error_formatter os;
    os.head() << "unable to deduce type application for type identifiers:";

    auto l1 = os.new_info();
    print_identifiers(os.line(), idents);

    auto l2 = os.new_info();
        os.line() << "type is:";
        os.new_line(l2);
            print_type(os.line(), ty);

    m_eh->report_parser_error(loc, os.str());
};

}}
