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

#include "error.h"
#include "semantics/typing/types/kind_checker.h"
#include "error_formatter.h"

#include <sstream>

namespace arrow { namespace error
{

kind_error_msg::kind_error_msg(const error_handler_ptr& eh)
    :error_base_msg(eh)
{};

void kind_error_msg::print_stack(error_formatter& os, const std::vector<ast::type>& stack)
{
    for (const auto& ty : stack)
    {
        auto l1 = os.new_info();
            os.line() << "while infering kind of type";
            os.new_line(l1);
                print_type(os.line(), ty);
    };
};

void kind_error_msg::error_star_kind_required_var(const ast::location& loc, const ast::kind& given)
{
    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting kind *";

    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_star_kind_required(const ast::location& loc, const ast::kind& given, 
            arrow::typing::details::star_kind_reason reason, const std::vector<ast::type>& stack)
{
    using reason_type = arrow::typing::details::star_kind_reason;

    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting kind * for ";
        print_reason(reason, os.line());

    print_stack(os, stack);

    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_product_kind_required(const ast::location& loc, const ast::kind& given, 
            long req_size, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting product kind of size " << req_size;

    print_stack(os, stack);

    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_value_kind_required(const ast::location& loc, ast::value_kind_code vc, 
            const ast::kind& given, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting value kind: ";

        switch(vc)
        {
            case ast::value_kind_code::int_kind:
                os.line() << "int"; break;
            case ast::value_kind_code::bool_kind:
                os.line() << "bool"; break;
            case ast::value_kind_code::char_kind:
                os.line() << "char"; break;
            case ast::value_kind_code::string_kind:
                os.line() << "string"; break;
        }

    print_stack(os, stack);

    m_eh->report_parser_error(loc, os.str());
};
void kind_error_msg::error_poly_kind_required(const ast::location& loc, const ast::kind& given, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting polymorphic kind";

    print_stack(os, stack);
    
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_star_kind_field_required(const ast::location& loc, const ast::kind& given, 
            const ast::identifier& field_name, const std::vector<ast::type>& stack)
{
    using reason_type = arrow::typing::details::star_kind_reason;

    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_info();
        os.line() << "expecting kind * for record field " << field_name.get_name_simple().get_string();

    print_stack(os, stack);

    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_arrow_kind_required(const ast::location& loc, const ast::kind& given, 
            arrow::typing::details::arrow_kind_reason reason, const std::vector<ast::type>& stack)
{
    using reason_type = arrow::typing::details::arrow_kind_reason;

    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_line();
        os.line() << "expecting kind *->* for ";

        switch(reason)
        {
            case reason_type::type_application:
                os.line() << "type application"; break;
            case reason_type::type_lambda:
                os.line() << "type function"; break;
        };

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_all_kind_required(const ast::location& loc, const ast::kind& given, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_line();
        os.line() << "expecting star arrow kind (*{All[..] ..}) for type instantiation";

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};


void kind_error_msg::error_kind_mismatch(const ast::location& loc, const ast::kind& required, 
            const ast::kind& given, const std::vector<ast::type>& stack)
{
    using reason_type = arrow::typing::details::arrow_kind_reason;

    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), given);
    
    os.new_line();
        os.line() << "expecting kind: ";
        print_kind(os.line(), required);

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
}

void kind_error_msg::error_kind_mismatch_id(const ast::location& loc, ast::unique_string id, 
            const ast::kind& id_kind, const ast::kind& req, const std::vector<ast::type>& stack)
{
    using reason_type = arrow::typing::details::arrow_kind_reason;

    error_formatter os;
    os.head() << "unexpected kind: ";
        print_kind(os.head(), req);
    
    os.new_line();
        os.line() << "for symbol: " << id.get_string();
    os.new_info();
        os.line() << "this symbol has kind: ";
        print_kind(os.line(), id_kind);

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
}

void kind_error_msg::error_kind_polarity_mismatch_for_symbol(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, ast::polarity_type exp_pol, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "invalid polarity: ";
        print_polarity(os.head(), pol);
    os.head() << " (expecting ";
        print_polarity(os.head(), exp_pol);
        os.head() << ")";
    
    os.head() << " for symbol: " << id.get_name_simple().get_string();

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_kind_polarity_mismatch_for_symbol_rec(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, ast::polarity_type exp_pol, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "invalid polarity: ";
        print_polarity(os.head(), pol);
    os.head() << " (expecting ";
        print_polarity(os.head(), exp_pol);
        os.head() << ")";
    
    os.head() << " for recursive symbol: " << id.get_name_simple().get_string();

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_polarized_symbol_on_neutral_pos(const ast::location& loc, 
            const ast::identifier& id, ast::polarity_type pol, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "symbol: " << id.get_name_simple().get_string() << " with polarity ";
        print_polarity(os.head(), pol);
    os.head() << " cannot appear on netral positions";

    os.new_info();
        os.line() << "symbol is passed to function without polarity annotation";

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_infinite_kind(const ast::location& loc, const ast::kind_var& k1, 
                                         const ast::kind& k2)
{
    error_formatter os;
    os.head() << "infinite kind: ";
    os.new_line();
        print_kind(os.line(), k1);
        os.line()<< " :=: ";
        print_kind(os.line(), k2);

    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_invalid_kind_application(const ast::location& loc, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "invalid kind application";

    os.new_info();
        os.line() << "type has not polymorphic kind";

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_invalid_kind_application_star(const ast::location& loc, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "invalid kind application";

    os.new_info();
        os.line() << "type has not polymorphic kind (has * kind)";

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_invalid_kind_application_sym(const ast::location& loc, const ast::identifier& id,
            const ast::kind& ki, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "invalid kind application to symbol " << id.get_name_simple().get_string();

    auto l1 = os.new_info();
        os.line() << "this symbol has not polymorphic kind but:";
        
        os.new_line(l1);
            print_kind(os.line(), ki);

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_kind_var_not_allowed_polykind(const ast::location& loc, const ast::identifier& sym,
            const ast::kind& ki, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "in polykind annotations kind variable is not allowed";

    auto l1 = os.new_info();
        os.line() << "while checking kind of definition of symbol: " << sym.get_name_simple().get_string();
        os.line() << " at " << sym->get_location().to_string(true);
        
    auto l2 = os.new_info();
        os.line() << "symbol kind is:";
        os.new_line(l2);
            print_kind(os.line(), ki);

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_type_constant_cannot_be_ord(const ast::location& loc, 
            const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "type constant cannot have value kind";

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::error_too_many_kind_args(const ast::location& loc, const ast::identifier& sym,
            const ast::kind& ki, const std::vector<ast::type>& stack)
{
    error_formatter os;
    os.head() << "too many kind variables applied to polykind";

    auto l1 = os.new_info();
        os.line() << "symbol " << sym.get_name_simple().get_string();
        os.line() << " defined at " << sym->get_location().to_string(true);
        os.line() << "has kind:";
        os.new_line(l1);
            print_kind(os.line(), ki);

    print_stack(os, stack);
    m_eh->report_parser_error(loc, os.str());
};

void kind_error_msg::print_reason(arrow::typing::details::star_kind_reason reason, std::ostream& os)
{
    using reason_type = arrow::typing::details::star_kind_reason;

    switch(reason)
    {
        case reason_type::primitive:
            os << "primitive type"; break;
        case reason_type::field_type:
            os << "record field type"; break;
        case reason_type::tuple:
            os << "tuple type"; break;
        case reason_type::arrow:
            os << "arrow type"; break;
        case reason_type::variant:
            os << "variant type"; break;
        case reason_type::record:
            os << "record type"; break;
        case reason_type::ref:
            os << "ref type"; break;
        case reason_type::list:
            os << "list type"; break;
        case reason_type::all:
            os << "All type"; break;
        case reason_type::some:
            os << "Some type"; break;
        case reason_type::type_constant:
            os << "type constant"; break;
        case reason_type::type_var:
            os << "type variable"; break;
    };
};

void kind_error_msg::print_polarity(std::ostream& os, ast::polarity_type pol)
{
    switch(pol)
    {
        case ast::polarity_type::positive:
            os << "positive"; return;
        case ast::polarity_type::negative:
            os << "negative"; return;
        case ast::polarity_type::neutral:
            os << "neutral"; return;
    };
};

void kind_error_msg::error_kind_mismatch_extern_func_ret(const ast::location& loc, const ast::identifier& name,
            const ast::kind& ret_type, const ast::kind& func_ret_type)
{
    error_formatter ef;
    ef.head()   << "kind mismatch while checking extern type function " << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "declared return kind:";
        ef.new_line(l1);
            print_kind(ef.line(), ret_type);

    auto l2 = ef.new_info();
        ef.line() << "return kind of definition:";
        ef.new_line(l2);
            print_kind(ef.line(), func_ret_type);

    m_eh->report_parser_error(loc, ef.str());
};

void kind_error_msg::error_kind_mismatch_extern_func_size(const ast::location& loc, const ast::identifier& name,
            long req_arg_size, long func_arg_size)
{
    error_formatter ef;
    ef.head()   << "invalid number of arguments of extern type function " 
                << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "number of arguments in declaration: " << req_arg_size;

    auto l2 = ef.new_info();
        ef.line() << "number of arguments in definition: " << func_arg_size;

    m_eh->report_parser_error(loc, ef.str());
};

void kind_error_msg::error_kind_mismatch_extern_func_arg(const ast::location& loc, const ast::identifier& name,
            long arg_num, const ast::kind& ret_type, const ast::kind& func_ret_type)
{
    error_formatter ef;
    ef.head()   << "kind mismatch while checking " << arg_num + 1 
                << "-th argument of extern type function " << name.get_name_simple().get_string();

    auto l1 = ef.new_info();
        ef.line() << "declared kind:";
        ef.new_line(l1);
            print_kind(ef.line(), ret_type);

    auto l2 = ef.new_info();
        ef.line() << "definition kind:";
        ef.new_line(l2);
            print_kind(ef.line(), func_ret_type);

    m_eh->report_parser_error(loc, ef.str());
};

}}
