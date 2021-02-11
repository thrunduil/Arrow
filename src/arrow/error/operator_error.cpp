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
#include "error.h"
#include "error_formatter.h"
#include "semantics/operators/operator_declaration.h"

namespace arrow { namespace error
{

operator_error_msg::operator_error_msg(const error_handler_ptr& eh)
    :error_base_msg(eh)
{};

void operator_error_msg::error_invalid_operator_property(const ast::location& loc, const std::string& prop_type, 
            const std::vector<std::string>& allowed_properties)
{
    error_formatter ef;

    ef.head() << "invalid operator property " << prop_type;
    auto l1 = ef.new_info();
        ef.line() << "allowed values: ";
        ef.new_line(l1);
        print_vector(ef.line(), allowed_properties);

    m_eh->report_parser_error(loc, ef.str());
}

void operator_error_msg::error_invalid_operator_property_value(const ast::location& loc, const std::string& prop_type,
            const std::string& prop_value, const std::vector<std::string>& allowed_properties)
{
    error_formatter ef;
    ef.head() << "invalid value " << prop_value << " for operator property " << prop_type;
    auto l1 = ef.new_info();
        ef.line() << "allowed values: ";
        ef.new_line(l1);
        print_vector(ef.line(), allowed_properties);

    m_eh->report_parser_error(loc, ef.str());
};
void operator_error_msg::error_invalid_operator_precedence(const ast::location& loc, uint64_t prec, int prec_min, 
            int prec_max)
{
    error_formatter ef;
    ef.head() << "invalid value " << prec << " for operator property prec";
    auto l1 = ef.new_info();
    ef.line() << "allowed values: [" << prec_min << "-" << prec_max << "]";

    m_eh->report_parser_error(loc, ef.str());
}

void operator_error_msg::error_invalid_opening_and_closing_brackets(const ast::location& loc, 
            const std::string& brack_op_name)
{
    error_formatter ef;
    ef.head() << "invalid bracket operator; opening and closing bracket does not match: "
        << brack_op_name;
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_operator_kind_not_set(const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "operator kind is not set";
    m_eh->report_parser_error(loc, ef.str());
};
void operator_error_msg::error_precedence_of_infix_operator_not_set(const ast::location& loc)
{
    error_formatter ef;
    ef.head() << "precedence of infix operator is not set";
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_brackets_must_be_indexing_surrounding(const ast::location& loc, ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "invalid explicit kind declaration of " << op.get_string();
    ef.new_info();
       ef.line() << "brackets must be declared as an indexing or surrounding operator";

    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_only_brackets_can_be_indexing_surrounding(const ast::location& loc, 
            ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "invalid explicit kind declaration of " << op.get_string();
    ef.new_info();
       ef.line() <<  "only brackets can be declared as an indexing or surrounding operator";
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_tuple_operator_cannot_be_surrounding(const ast::location& loc, ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "tuple operator " << op.get_string() << "cannot be declared as surrounding";
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_dot_bracket_cannot_be_surrounding(const ast::location& loc, ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "dot bracket " << op.get_string() << "cannot be declared as surrounding";
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_type_application_not_operator(const ast::location& loc, ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "operator " << op.get_string() << "cannot be declared";
    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_fixity_defined(ast::unique_string name, const ast::location& loc,
            const ast::location& old_loc)
{
    error_formatter ef;
    ef.head() << "fixity for the operator " << name.get_string() << " is already defined in "
        << old_loc.to_string(false);

    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::error_bracket_operator_cannot_be_prefix(const ast::location& loc, ast::unique_string op)
{
    error_formatter ef;
    ef.head() << "bracket operator " << op.get_string() << " cannot be marked as prefix";

    m_eh->report_parser_error(loc, ef.str());
};

void operator_error_msg::warning_fixity_hidden(ast::unique_string name, const ast::location& loc,
            const ast::location& old_loc)
{
    error_formatter ef;
    ef.head()  << "fixity for the operator " << name.get_string() << " is already defined in " 
        << old_loc.to_string(true);
    ef.new_info();
        ef.line() << "old definition will be inaccessible";

    m_eh->report_parser_warning(loc, ef.str());
};
void operator_error_msg::warning_explicit_fixity_not_for_operator(const ast::location& loc, ast::unique_string op_name)
{
    error_formatter ef;
    ef.head()   << "explicit fixity for symbol " << op_name.get_string() 
                << " is ignored; this is not an operator definition";
    m_eh->report_parser_warning(loc, ef.str());
};

void operator_error_msg::error_opening_closing_brakets_dont_match(const ast::location& open, 
        const ast::location& close, const std::string& left_br, const std::string& right_br)
{
    error_formatter ef;
    ef.head() << "opening bracket: " << left_br << " does not match with closing bracket: " << right_br
              << " at " << close.to_string(false);

    m_eh->report_parser_error(open, ef.str());
};

void operator_error_msg::error_explicit_fixity_for_opening_bracket(const ast::location& loc)
{
    error_formatter os;
    os.head() << "explicit fixity declaration of opening bracket is not allowed";
    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_ambiguous_declarations_of_operator(const ast::location& loc, 
            ast::unique_string name, std::vector<ast::location>& declarations)
{
    error_formatter os;
    os.head() << "ambiguous declarations of operator " << name.get_string();

    os.new_info();
    os.line() << "existing operator declarations:";
    print_locations(os, declarations);

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_undefined_operator(const ast::location& loc, ast::unique_string oper, 
            ast::fixity_type ft)
{
    error_formatter os;
    os.head() << "undefined operator " << oper.get_string();
    
    if (ft != ast::fixity_type::undefined)
    {
        os.head() << " with kind ";
        ast::operator_info::print_kind_name(os.head(), ft);
    };

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::warning_ambiguous_parsing_operator(const ast::location& loc, ast::unique_string op_name)
{
    error_formatter os;
    os.head() << "operator " << op_name.get_string() << " may be parsed as infix or postfix";
    m_eh->report_parser_warning(loc, os.str());
};

void operator_error_msg::error_ambiguous_parse_for_operator(const ast::location& loc, ast::unique_string op,
                                                   bool infix, bool prefix, bool postfix)
{
    error_formatter os;
    os.head() << "ambiguous parse for operator " << op.get_string() << " ";

    bool add_or = false;

    if (infix)
    {
        ast::operator_info::print_kind_name(os.head(), ast::fixity_type::infix);
        add_or = true;
    }
    if (prefix)
    {
        if (add_or)
            os.head() << " or ";

        ast::operator_info::print_kind_name(os.head(), ast::fixity_type::prefix);
        add_or = true;
    }
    if (postfix)
    {
        if (add_or)
            os.head() << " or ";
        ast::operator_info::print_kind_name(os.head(), ast::fixity_type::postfix);
    }

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_no_valid_parse_for_operator(const ast::location& loc, ast::unique_string op)
{
    error_formatter os;
    os.head() << "no valid parse for operator " << op.get_string();
    os.new_info();
    os.line() << "operator kind is undefined";

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_unable_parse_operators(const ast::location& loc)
{
    error_formatter os;
    os.head() << "unable to parse operator sequence";
    os.new_info();
    os.line() << "unknown reason";

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_invalid_sequence_of_nonassociative_operators(const ast::location& loc, 
            ast::unique_string prev_op, ast::fixity_type prev_ft, ast::unique_string current_op, 
            ast::fixity_type cur_ft)
{
    error_formatter os;
    os.head() << "unable to parse sequence of operators";
    os.new_info();
        os.line()   << "non-associative operator " << current_op.get_string()
                    << " of king ";
                    ast::operator_info::print_kind_name(os.line(), cur_ft);
                    os.line() << " requires paranthesis";
    os.new_info();
        os.line()   << "previous operator: " << prev_op.get_string() << " of kind ";
                    ast::operator_info::print_kind_name(os.line(), prev_ft);

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_quoting_operator_requires_identifier(const ast::location& loc, 
                    ast::unique_string op)
{
    error_formatter os;
    os.head() << "quoting operator " << op.get_string() << " may only quote identifiers";

    m_eh->report_parser_error(loc, os.str());
};

void operator_error_msg::error_quoting_operator_identifier_with_fixity(const ast::location& loc, 
            ast::unique_string op, ast::unique_string arg_name, ast::fixity_type arg_ft)
{
    error_formatter os;
    os.head() << "quoting operator " << op.get_string() << " may only quote identifiers";

    os.new_info();
    os.line() << "applied argument " << arg_name.get_string() << " has explicit kind ";
                ast::operator_info::print_kind_name(os.line(), arg_ft);

    m_eh->report_parser_error(loc, os.str());

};

void operator_error_msg::error_invalid_operator_definition_no_valid_declaration(const ast::location& open, 
            ast::unique_string op, ast::fixity_type ft, const oper::operator_declaration& op_decl)
{
    error_formatter os;
    os.head() << "invalid definition of operator " << op.get_string() << " of king ";
    ast::operator_info::print_kind_name(os.head(), ft);
    
    auto l1 = os.new_info();
    os.line() << "operator is not declared; existing declarations:";

    print_operator_decl(os, l1, op_decl);

    m_eh->report_parser_error(open, os.str());
};

void operator_error_msg::print_operator_decl(error_formatter& os, const line_mark& lm, 
            const oper::operator_declaration& op_decl)
{
    for (auto ft : {ast::fixity_type::infix, ast::fixity_type::prefix, ast::fixity_type::postfix,
                    ast::fixity_type::surrounding, ast::fixity_type::indexing})
    {
        if (op_decl.has(ft) == true)
        {
            const ast::operator_info& info  = op_decl.get_info(ft);
            const ast::location& loc        = op_decl.get_location(ft);

            os.new_line(lm);
            info.print(os.line());
            os.line() << " at " << loc.to_string(true);
        };
    };
};

}}
