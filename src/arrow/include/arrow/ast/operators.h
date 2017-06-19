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
#include "arrow/details/fwd_decls_ast.h"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <functional>

namespace arrow { namespace ast
{

/// fixity type of an operator
enum class fixity_type : char
{
    undefined = 0,      /// undefined fixity
    infix,              /// infix
    prefix,             /// prefix
    postfix,            /// postfix
    surrounding,        /// surrounding operator, e.g. [a,b,c], or [a,b] -> c
    indexing,           /// indexing operator, e.g. x[a,b,c], or x[a,b] -> c
};

/// associativity of infix operator
enum class associativity_type : char
{
    none = 0,           /// nonassociative
    left,               /// left associative
    right,              /// right associative    
};

/// quoting type
enum class quote_type : char
{
    none = 0,           /// do not quote
    implicit,           /// decide based on fixity type
    left,               /// quote left-hand-side
    right,              /// quote right-hand-side
    both,               /// quote both sides    
};

enum class named_precedence : int
{
    undefined           = 0,        /// no named precedence for this value
    minimum_allowed     = 1,        /// weakest binding strength, no named precedence for this value

    min                 = 10,       /// weakest named precedence
    semi                = 20,       /// as in x ; y
    comma               = 30,       /// as in x , y
    assign              = 40,       /// as in x = y
    elvis               = 50,       /// as in x ? y : z
    op_or               = 60,       /// as in x || y; named precedence is 'or'
    op_and              = 70,       /// as in x && y; named precedence is 'and'
    compare             = 80,       /// as in x == y
    range               = 90,       /// as in x .. y  (range construction)    
    add                 = 100,      /// as in x + y
    mul                 = 110,      /// as in x * y
    pow                 = 120,      /// as in x ^ y
    op_not              = 130,      /// as in !x; named precedence is 'not'
    unary               = 135,      /// default precedence of unary operators
    call                = 140,      /// as in f(x)
    bracket             = 145,      /// default precedence of bracket operators
                                    /// tuple brackets have always precision 'call'
    dot                 = 150,      /// as in x.a    
    max                 = 160,      /// strongest named precedence
    
    maximum_allowed     = 255,      /// strongest binding strength, no named precedence for this value
};

/// get string representing precedence
std::string get_precedence_name(named_precedence prec);
std::string get_precedence_name(int prec);

class operator_info
{
    public:
        enum fixity : size_t
        {
            undefined     = 0x0000, // non-binding expressions such as '1' have this
            infix         = 0x0001,
            prefix        = 0x0002,
            postfix       = 0x0004,
            surrounding   = 0x0008,
            indexing      = 0x0010,
        };

    public:
        size_t              m_fixity;
        int                 m_precedence;
        associativity_type  m_associativity;
        quote_type          m_quoting;

    public:
        static const fixity_type    default_fixity_bracket  = fixity_type::surrounding;
        static const fixity_type    default_fixity_operator = fixity_type::infix;

    public:
        operator_info();

        bool                is_infix() const;
        bool                is_infix_left() const;
        bool                is_infix_right() const;
        bool                is_postfix() const;
        bool                is_prefix() const;
        bool                is_surrounding() const;
        bool                is_indexing() const;
        bool                has_fixity(const fixity_type ft) const;

        int                 get_precedence() const;
        quote_type          get_quoting() const;
        associativity_type  get_associativity() const;
        size_t              get_fixity_flags() const;

        fixity_type         get_fixity_type() const;

        bool                do_quote_left() const;
        bool                do_quote_right() const;

        bool                operator==(const operator_info& other) const;
        bool                operator!=(const operator_info& other) const;

        void                add_fixity(fixity_type);

        void                print(std::ostream& os) const;
        static void         print_kind_name(std::ostream& os, ast::fixity_type ft);
        static void         print_associativity(std::ostream& os, ast::associativity_type assoc);
        static void         print_precedence(std::ostream& os, int prec);
        static void         print_quoting(std::ostream& os, ast::quote_type quote);

};

/// decode property from key->value pairs and insert to operator_info
/// data
class operator_properties
{
    public:
        using function_type = std::function<void (operator_info&)>;

        using value_map     = std::map<std::string, function_type>;
        using string_vec    = std::vector<std::string>;
        
    public:
        struct prop_values
        {
            //solution to VS warning: decorated name length exceeded, name was truncated
            string_vec  first;
            value_map   second;

            prop_values()
            {};

            prop_values(const string_vec& f, const value_map& s)
                :first(f), second(s)
            {};
        };

        using property_map  = std::map<std::string, prop_values>;        

    private:
        property_map        m_properties;
        string_vec          m_allowed_properties;

    public:
        /// get access to operator_properties
        static const operator_properties&
                            get();

        /// decode string property; return false if prop or prop_value is unknown
        bool                decode_property(operator_info& op_info, const std::string& prop, 
                                const std::string& prop_value) const;

        /// decode int property; return false if prop or prop_value  is unknown
        bool                decode_property(operator_info& op_info, const std::string& prop, 
                                int prop_value) const;

        /// return code of given fixity (e.g. infix)
        fixity_type         decode_fixity(const std::string& fixity_name) const;

        std::string         get_decorated_name(const std::string& root, fixity_type ft) const;

        /// minimum allowed precedence
        int                 min_precedence() const;

        /// maximum allowed precedence
        int                 max_precedence() const;

        /// return true if prop_type is a valid operator property
        bool                is_property(const std::string& prop_type) const;

        /// return all allowed properties
        const string_vec&   allowed_properties() const;

        /// return allowed values for property prop_type
        const string_vec&   allowed_property_values(const std::string& prop_type) const;

        /// get allowed values for kind property
        const string_vec&   allowed_kind_values() const;

        /// get allowed values for quoting property
        const string_vec&   allowed_quoting_values() const;

        /// get allowed values for assoc property
        const string_vec&   allowed_assoc_values() const;

        /// get allowed values for prec property
        const string_vec&   allowed_prec_values() const;

    private:
        operator_properties();

        prop_values         create_allowed_kind_values() const;
        prop_values         create_allowed_quoting_values() const;
        prop_values         create_allowed_assoc_values() const;
        prop_values         create_allowed_prec_values() const;
};

/// test properties of special operators i.e. a bracket operator
/// (e.g. {}, {}->, ${}, ${}->) or separator operator ([,], [;])
class special_operators
{
    public:
        /// return true if opening and closing bracket of op match;
        static bool         opening_closing_brackets_match(const std::string& op);

        /// return true if op is a bracked operator
        static bool         is_bracket_operator(const std::string& op, bool& has_assign,
                                bool& is_dot);

        /// return true for tuple operators, i.e. (), ()->, .(), .()->, ...
        static bool         is_tuple_operator(const std::string& op, bool& has_assign,
                                bool& is_dot);

        /// return true for angle operators, i.e. <>, <>->, .<>, .<>->, ...
        static bool         is_angle_operator(const std::string& op, bool& has_assign,
                                bool& is_dot);

        /// split bracket-assign operator (e.g. .[]->) into bracked and assign
        /// return true if assign part is present; op must be a valid bracket operator
        static bool         split_bracket_operator(const std::string& oper, 
                                std::string& open, std::string& close, std::string& assign,
                                bool& dot);

        /// return number of consecutive left angle operators [<] 
        static int          count_langle(const std::string& op);

        /// return number of consecutive right angle operators [>]
        static int          count_rangle(const std::string& op);

        /// names of < and > operators
        static unique_string    operator_langle();
        static unique_string    operator_rangle();
};

};};
