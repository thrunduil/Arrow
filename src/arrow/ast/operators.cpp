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

#include "arrow/ast/operators.h"
#include "grammar/unicode.h"
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"

#include <sstream>

namespace arrow { namespace ast
{

//-------------------------------------------------------------------
//                   helper functions   
//-------------------------------------------------------------------

std::string ast::get_precedence_name(named_precedence prec)
{
    return get_precedence_name((int)prec);
};

std::string ast::get_precedence_name(int prec)
{
    switch (prec)
    {
        case (int)named_precedence::undefined:
            return "undefined";
        case (int)named_precedence::minimum_allowed:
            return "minimum_allowed";
        case (int)named_precedence::min:            
            return "min";
        case (int)named_precedence::semi:
            return "semi";
        case (int)named_precedence::comma:
            return "comma";
        case (int)named_precedence::assign:
            return "assign";
        case (int)named_precedence::elvis:
            return "elvis";
        case (int)named_precedence::op_or:
            return "or";
        case (int)named_precedence::op_and:
            return "and";
        case (int)named_precedence::compare:
            return "compare";
        case (int)named_precedence::range:
            return "range";
        case (int)named_precedence::add:
            return "add";
        case (int)named_precedence::mul:
            return "mul";
        case (int)named_precedence::pow:
            return "pow";
        case (int)named_precedence::op_not:
            return "not";
        case (int)named_precedence::unary:
            return "unary";
        case (int)named_precedence::call:
            return "call";
        case (int)named_precedence::dot:
            return "dot";
        case (int)named_precedence::bracket:
            return "bracket";
        case (int)named_precedence::max:
            return "max";
        case (int)named_precedence::maximum_allowed:
            return "maximum_allowed";
    };

    if (prec >= (int)named_precedence::minimum_allowed 
        && prec <= (int)named_precedence::maximum_allowed)
    {
        std::ostringstream os;
        os << prec;
        return os.str();
    }

    return "invalid";
};

//-------------------------------------------------------------------
//                   operator_info   
//-------------------------------------------------------------------
operator_info::operator_info()
{
    m_fixity        = fixity::undefined;
    m_precedence    = 0;
    m_associativity = associativity_type::none;
    m_quoting       = quote_type::none;
}

void operator_info::add_fixity(fixity_type ft)
{
    switch (ft)
    {
        case fixity_type::undefined:
            return;
        case fixity_type::infix:
            m_fixity |= infix;
            return;
        case fixity_type::prefix:
            m_fixity |= prefix;
            return;
        case fixity_type::postfix:
            m_fixity |= postfix;
            return;
        case fixity_type::surrounding:
            m_fixity |= surrounding;
            return;
        case fixity_type::indexing:
            m_fixity |= indexing;
            return;
    };
}

bool operator_info::is_infix() const
{
    return (m_fixity & infix) != 0;
};
bool operator_info::is_infix_left() const
{
    return is_infix() && m_associativity == associativity_type::left;
};

bool operator_info::is_infix_right() const
{
    return is_infix() && m_associativity == associativity_type::right;
};

bool operator_info::is_postfix() const
{
    return (m_fixity & postfix) != 0;
};
bool operator_info::is_prefix() const
{
    return (m_fixity & prefix) != 0;
};
bool operator_info::is_surrounding() const
{
    return (m_fixity & surrounding) != 0;
}
bool operator_info::is_indexing() const
{
    return (m_fixity & indexing) != 0;
}

bool operator_info::has_fixity(const fixity_type ft) const
{
    switch (ft)
    {
        case fixity_type::undefined:
            return false;
        case fixity_type::infix:
            return is_infix();
        case fixity_type::prefix:
            return is_prefix();
        case fixity_type::postfix:
            return is_postfix();
        case fixity_type::surrounding:
            return is_surrounding();
        case fixity_type::indexing:
            return is_indexing();
    };

    return false;
};

int operator_info::get_precedence() const
{
    return m_precedence;
}

quote_type operator_info::get_quoting() const
{
    return m_quoting;
};

associativity_type operator_info::get_associativity() const
{
    return m_associativity;
};

size_t operator_info::get_fixity_flags() const
{
    return m_fixity;
};

fixity_type operator_info::get_fixity_type() const
{
    if (m_fixity == fixity::infix)
        return fixity_type::infix;
    else if (m_fixity == fixity::prefix)
        return fixity_type::prefix;
    else if (m_fixity == fixity::postfix)
        return fixity_type::postfix;
    else if (m_fixity == fixity::surrounding)
        return fixity_type::surrounding;
    else if (m_fixity == fixity::indexing)
        return fixity_type::indexing;
    else
        return fixity_type::undefined;
}

bool operator_info::do_quote_left() const
{
    return (m_quoting == quote_type::left)
        || (m_quoting ==quote_type::both)
        || (m_quoting == quote_type::implicit)
            && (is_infix() || is_postfix() || is_surrounding() || is_indexing());
}
bool operator_info::do_quote_right() const
{
    return (m_quoting == quote_type::right) 
        || (m_quoting == quote_type::both)
        || (m_quoting == quote_type::implicit)
            && (is_infix() || is_prefix());
}

bool operator_info::operator==(const operator_info& other) const
{
    if (m_fixity != other.m_fixity)
        return false;
    if (m_precedence != other.m_precedence)
        return false;
    if (m_associativity != other.m_associativity)
        return false;
    if (m_quoting != other.m_quoting)
        return false;

    return true;
}
bool operator_info::operator!=(const operator_info& other) const
{
    return !operator==(other);
}

void operator_info::print(std::ostream& os) const
{
    ast::fixity_type ft = this->get_fixity_type();

    os << "operator {";
    os << "kind = ";
    print_kind_name(os, ft);

    if (ft == ast::fixity_type::infix)
    {
        os << ", assoc = ";
        print_associativity(os, this->m_associativity);
    };

    os << ", prec = ";
    print_precedence(os, this->m_precedence);

    os << ", quoting = ";
    print_quoting(os, this->m_quoting);

    os << "}";
};

void operator_info::print_kind_name(std::ostream& os, ast::fixity_type ft)
{
    switch (ft)
    {
        case ast::fixity_type::infix:
        {
            os << "infix";
            return;
        }
        case ast::fixity_type::prefix:
        {
            os << "prefix";
            return;
        }
        case ast::fixity_type::postfix:
        {
            os << "postfix";
            return;
        }
        case ast::fixity_type::surrounding:
        {
            os << "surrounding";
            return;
        }
        case ast::fixity_type::indexing:
        {
            os << "indexing";
            return;
        }
        default:
        {
            os << "unknown";
            return;
        };
    };
};

void operator_info::print_associativity(std::ostream& os, ast::associativity_type assoc)
{
    switch(assoc)
    {
        case ast::associativity_type::none:
            os << "none"; return;
        case ast::associativity_type::left:
            os << "left"; return;
        case ast::associativity_type::right:
            os << "right"; return;
        default:
            os << "unknown"; return;
    };
};

void operator_info::print_precedence(std::ostream& os, int prec)
{
    os << ast::get_precedence_name(prec);
};

void operator_info::print_quoting(std::ostream& os, ast::quote_type quote)
{
    switch(quote)
    {
        case ast::quote_type::none:
            os << "none"; return;
        case ast::quote_type::left:
            os << "left"; return;
        case ast::quote_type::right:
            os << "right"; return;
        case ast::quote_type::both:
            os << "both"; return;
        case ast::quote_type::implicit:
            os << "implicit"; return;
        default:
            os << "unknown"; return;
    };
};

//---------------------------------------------------------------
//                  operator_properties
//---------------------------------------------------------------
operator_properties::operator_properties()
{
    m_properties[""]        = prop_values();
    m_properties["kind"]    = create_allowed_kind_values();
    m_properties["quoting"] = create_allowed_quoting_values();
    m_properties["assoc"]   = create_allowed_assoc_values();
    m_properties["prec"]    = create_allowed_prec_values();

    m_allowed_properties.push_back("kind");
    m_allowed_properties.push_back("quoting");
    m_allowed_properties.push_back("assoc");
    m_allowed_properties.push_back("prec");
}

operator_properties::prop_values
operator_properties::create_allowed_kind_values() const
{
    value_map vals;
    vals["infix"]       = [](operator_info& op)-> void { op.m_fixity = operator_info::infix; };
    vals["prefix"]      = [](operator_info& op)-> void { op.m_fixity = operator_info::prefix; };
    vals["postfix"]     = [](operator_info& op)-> void { op.m_fixity = operator_info::postfix; };
    vals["surrounding"] = [](operator_info& op)-> void { op.m_fixity = operator_info::surrounding; };
    vals["indexing"]    = [](operator_info& op)-> void { op.m_fixity = operator_info::indexing; };

    string_vec string_vals;
    string_vals.push_back("infix");
    string_vals.push_back("prefix");
    string_vals.push_back("postfix");
    string_vals.push_back("surrounding");
    string_vals.push_back("indexing");

    return prop_values(string_vals, vals);
};

operator_properties::prop_values
operator_properties::create_allowed_quoting_values() const
{
    value_map vals;
    vals["none"]        = [](operator_info& op)-> void { op.m_quoting = quote_type::none; };
    vals["implicit"]    = [](operator_info& op)-> void { op.m_quoting = quote_type::implicit; };
    vals["left"]        = [](operator_info& op)-> void { op.m_quoting = quote_type::left; };
    vals["right"]       = [](operator_info& op)-> void { op.m_quoting = quote_type::right; };
    vals["both"]        = [](operator_info& op)-> void { op.m_quoting = quote_type::both; };

    string_vec string_vals;
    string_vals.push_back("none");
    string_vals.push_back("implicit");
    string_vals.push_back("left");
    string_vals.push_back("right");
    string_vals.push_back("both");

    return prop_values(string_vals, vals);
}

operator_properties::prop_values
operator_properties::create_allowed_assoc_values() const
{
    value_map vals;
    vals["none"]    = [](operator_info& op)-> void { op.m_associativity = associativity_type::none; };
    vals["left"]    = [](operator_info& op)-> void { op.m_associativity = associativity_type::left; };
    vals["right"]   = [](operator_info& op)-> void { op.m_associativity = associativity_type::right; };

    string_vec string_vals;
    string_vals.push_back("none");
    string_vals.push_back("left");
    string_vals.push_back("right");

    return prop_values(string_vals, vals);

};

operator_properties::prop_values
operator_properties::create_allowed_prec_values() const
{
    value_map vals;

    vals["min"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::min; };
    vals["semi"]        = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::semi; };
    vals["comma"]       = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::comma; };
    vals["assign"]      = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::assign; };
    vals["elvis"]       = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::elvis; };
    vals["or"]          = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::op_or; };
    vals["and"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::op_and; };
    vals["compare"]     = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::compare; };
    vals["range"]       = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::range; };
    vals["add"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::add; };
    vals["mul"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::mul; };
    vals["pow"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::pow; };
    vals["not"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::op_not; };
    vals["unary"]       = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::unary; };
    vals["call"]        = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::call; };
    vals["dot"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::dot; };
    vals["bracket"]     = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::bracket; };
    vals["max"]         = [](operator_info& op)-> void { op.m_precedence = (int)named_precedence::max; };

    string_vec string_vals;
    string_vals.push_back("min");
    string_vals.push_back("semi");
    string_vals.push_back("comma");
    string_vals.push_back("assign");
    string_vals.push_back("elvis");   
    string_vals.push_back("or");  
    string_vals.push_back("and");     
    string_vals.push_back("compare");
    string_vals.push_back("range");
    string_vals.push_back("add");  
    string_vals.push_back("mul");
    string_vals.push_back("pow");
    string_vals.push_back("not");    
    string_vals.push_back("unary");
    string_vals.push_back("call");  
    string_vals.push_back("dot");   
    string_vals.push_back("bracket");   
    string_vals.push_back("max");    

    return prop_values(string_vals, vals);
};

const operator_properties& operator_properties::get()
{
    static operator_properties g_instance;
    return g_instance;
}

fixity_type operator_properties::decode_fixity(const std::string& fixity_name) const
{
    operator_info info;
    decode_property(info, "kind", fixity_name);
    return info.get_fixity_type();
};

std::string operator_properties::get_decorated_name(const std::string& root, fixity_type ft) const
{
    switch (ft)
    {
        case fixity_type::undefined:
            return root;
        case fixity_type::infix:
            return "_" + root + "_";
        case fixity_type::prefix:
            return root + "_";
        case fixity_type::postfix:
            return "_" + root;
        case fixity_type::surrounding:
            return root;
        case fixity_type::indexing:
            return "_" + root;
        default:
            return root;
    };
};

bool operator_properties::decode_property(operator_info& op_info, const std::string& prop, 
                                          const std::string& prop_value) const
{
    const property_map& m_map = m_properties;
    using const_iterator    = property_map::const_iterator;

    const_iterator pos      = m_map.find(prop);
    if (pos == m_map.end())
        return false;

    const value_map& vals   = pos->second.second;

    auto pos_val    = vals.find(prop_value);
    if (pos_val == vals.end())
        return false;

    pos_val->second(op_info);
    return true;
};

bool operator_properties::decode_property(operator_info& op_info, const std::string& prop, 
                        int prop_value) const
{
    if (prop != "prec")
        return false;

    if (prop_value < min_precedence() || prop_value > max_precedence())
        return false;

    op_info.m_precedence    = prop_value;
    return true;
};

int operator_properties::min_precedence() const
{
    return (int)named_precedence::minimum_allowed;
};

int operator_properties::max_precedence() const
{
    return (int)named_precedence::maximum_allowed;
}

bool operator_properties::is_property(const std::string& prop_type) const
{
    auto pos = m_properties.find(prop_type);
    if (pos == m_properties.end())
        return false;
    else
        return true;
};

const operator_properties::string_vec& operator_properties::allowed_properties() const
{
    return m_allowed_properties;
};

const operator_properties::string_vec& 
operator_properties::allowed_property_values(const std::string& prop_type) const
{
    auto pos = m_properties.find(prop_type);
    if (pos == m_properties.end())
    {
        pos = m_properties.find("");
        return pos->second.first;
    }
    else
    {
        return pos->second.first;
    }
};

const operator_properties::string_vec& 
operator_properties::allowed_kind_values() const
{
    auto pos = m_properties.find("kind");
    return pos->second.first;
};

const operator_properties::string_vec& 
operator_properties::allowed_quoting_values() const
{
    auto pos = m_properties.find("quoting");
    return pos->second.first;
};

const operator_properties::string_vec& 
operator_properties::allowed_assoc_values() const
{
    auto pos = m_properties.find("assoc");
    return pos->second.first;
};

const operator_properties::string_vec& 
operator_properties::allowed_prec_values() const
{
    auto pos = m_properties.find("prec");
    return pos->second.first;
};

//-------------------------------------------------------------------
//                   special_operators   
//-------------------------------------------------------------------
bool special_operators::opening_closing_brackets_match(const std::string& op)
{
    int32_t index   = 0;
    int32_t size    = (int32_t)op.size();

    if (index >= size)
        return false;

    if (op[0] == '.')
    {
        ++index;

        if (index >= size)
            return false;
    };

    int32_t code_1  = read_code_point_from_utf8(op, index);

    if (index >= size || index <= 0)
        return false;

    int32_t code_2  = read_code_point_from_utf8(op, index);

    if (code_1 < 0 || code_2 < 0)
        return false;

    bool match      = do_brackets_match(code_1, code_2);
    return match;
};

bool special_operators::is_bracket_operator(const std::string& op, 
                            bool& has_assign, bool& is_dot)
{
    int32_t index   = 0;
    int32_t size    = (int32_t)op.size();
    has_assign      = false;
    is_dot          = false;

    if (index >= size)
        return false;

    if (op[0] == '.')
    {
        is_dot      = true;
        ++index;

        if (index >= size)
            return false;
    };

    int32_t code_1  = read_code_point_from_utf8(op, index);

    if (index >= size || index <= 0)
        return false;

    int32_t code_2  = read_code_point_from_utf8(op, index);

    if (code_1 < 0 || code_2 < 0)
        return false;

    bool bracket    = is_opening_bracket(code_1) && is_closing_bracket(code_2);

    if (bracket == false)
        return false;

    if (index < size)
        has_assign = true;

    return true;
};

bool special_operators::is_tuple_operator(const std::string& op, 
                            bool& has_assign, bool& is_dot)
{    
    if (op.size() < 2)
    {
        has_assign  = false;
        is_dot      = false;
        return false;
    }

    if (op[0] == '.')
    {
        is_dot      = true;

        if (op.size() < 3)
        {
            has_assign  = false;
            return false;
        };

        if (op[1] == '(' && op[2] == ')')
        {
            has_assign = (op.size() > 3);
            return true;
        }
        else
        {
            has_assign = false;
            return false;
        }
    }
    else
    {
        is_dot      = false;

        if (op[0] == '(' && op[1] == ')')
        {
            has_assign = (op.size() > 2);
            return true;
        }
        else
        {
            has_assign = false;
            return false;
        }
    };
};

bool special_operators::is_angle_operator(const std::string& op, 
                            bool& has_assign, bool& is_dot)
{    
    if (op.size() < 2)
    {
        has_assign  = false;
        is_dot      = false;
        return false;
    }

    if (op[0] == '.')
    {
        is_dot      = true;

        if (op.size() < 3)
        {
            has_assign  = false;
            return false;
        };

        if (op[1] == '<' && op[2] == '>')
        {
            has_assign = (op.size() > 3);
            return true;
        }
        else
        {
            has_assign = false;
            return false;
        }
    }
    else
    {
        is_dot      = false;

        if (op[0] == '<' && op[1] == '>')
        {
            has_assign = (op.size() > 2);
            return true;
        }
        else
        {
            has_assign = false;
            return false;
        }
    };
};

bool special_operators::split_bracket_operator(const std::string& op, 
                            std::string& open, std::string& close, std::string& assign,
                            bool& is_dot)
{
    int32_t index   = 0;
    int32_t size    = (int32_t)op.size(); 
    is_dot          = false;

    if (index >= size)
    {
        open    = "";
        close   = "";
        assign  = "";
        return false;
    };

    if (op[0] == '.')
    {
        is_dot      = true;
        ++index;

        if (index >= size)
        {
            open    = "";
            close   = "";
            assign  = "";
            return false;
        };
    };

    int32_t code_1  = read_code_point_from_utf8(op, index);

    if (index >= (int32_t)op.size() || index <= 0)
        return false;

    int32_t index1  = index;
    open            = op.substr(0, index);

    int32_t code_2  = read_code_point_from_utf8(op, index);
    close           = op.substr(index1, index - index1);

    if (index >= (int32_t)op.size() || index <= 0)
    {
        assign = "";
        return false;
    };

    (void)code_1;
    (void)code_2;
    assign  = op.substr(index); 
    return true;
};

int special_operators::count_langle(const std::string& op)
{
    size_t count    = 0;
    size_t s        = op.size();

    for(; count < s;)
    {
        if (op[count] != '<')
            break;

        ++count;
    };

    return (int)count;
};

int special_operators::count_rangle(const std::string& op)
{
    size_t count    = 0;
    size_t s        = op.size();

    for(; count < s;)
    {
        if (op[count] != '>')
            break;

        ++count;
    };

    return (int)count;
};

unique_string special_operators::operator_langle()
{
    static unique_string str("<");
    return str;
};
unique_string special_operators::operator_rangle()
{
    static unique_string str(">");
    return str;
};

}};
