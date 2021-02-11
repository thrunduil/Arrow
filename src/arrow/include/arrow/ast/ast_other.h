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

#include "ast_helpers.h"
#include "ast_atoms.h"
#include "operators.h"

#include <vector>

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      HELPER NODES
//---------------------------------------------------------------
class ARROW_EXPORT module_definition
{
    private:
                            /// import x import alias x location
        using string_loc    = std::tuple<unique_string, unique_string, location>;
        using string_vec    = std::vector<string_loc>;
        using impl_type     = details::module_definition_impl;
        using impl_ptr      = std::shared_ptr<impl_type>;

    private:
        impl_ptr            m_impl;

    public:
        module_definition();
        module_definition(const std::string& module_name, const string_vec& imports,
            const definitions& defs);

        const std::string&  get_module_name() const;
        const string_vec&   get_imports() const;
        const definitions&  get_definitions() const;
};

class ARROW_EXPORT definitions
    : public ast_base<definitions, details::definitions_impl, ast_mark>
{
    private:
        definitions(const impl_ptr& ip);

    public:
        definitions();
        definitions(const location& loc);

        void                add(const definition& def);

        long                size() const;
        const definition&   get_definition(long pos) const;
};

class ARROW_EXPORT identifier_kind
{
    public:
        enum class lexical_type : char
        {
            unknown, ident, oper, bracket_full, bracket_part 
        };

        enum class sector_type : char
        {
            unknown, module, kind, type, term
        };

        enum class function_type : char
        {
            unknown, def, var, overload
        };

    private:
        lexical_type        m_lex;  
        sector_type         m_sector;
        function_type       m_function;

    public:
        /// set proterties
        identifier_kind&    set_lex(lexical_type ty);
        identifier_kind&    set_sector(sector_type ty);
        identifier_kind&    set_function(function_type ty);

        identifier_kind&    set_sector_module();
        identifier_kind&    set_sector_kind();
        identifier_kind&    set_sector_type();
        identifier_kind&    set_sector_term();

        identifier_kind&    set_function_var();
        identifier_kind&    set_function_def();
        identifier_kind&    set_function_overload();

        /// get properties
        lexical_type        get_lexical() const;
        sector_type         get_sector() const;
        function_type       get_function() const;

        /// lexical properties
        bool                is_any_operator() const;
        bool                is_bracket_full() const;
        bool                is_bracket_part() const;

        /// sector properties
        bool                is_sector_kind() const;
        bool                is_sector_type() const;
        bool                is_sector_term() const;
        bool                is_sector_module() const;

        /// function
        bool                is_function_def() const;
        bool                is_function_var() const;
        bool                is_function_overload() const;

        /// sector and function

        /// test def flag
        bool                is_type_def() const;
        bool                is_term_def() const;

        /// test var flag
        bool                is_type_var() const;
        bool                is_kind_var() const;
};

class ARROW_EXPORT identifier 
    : public ast_base<identifier, details::identifier_impl, ast_mark>
{
    public:
        /// mark used to create fresh identifiers
        struct fresh{};

    private:
        identifier(const impl_ptr& ip);

    public:
        identifier();
        identifier(const location& loc, unique_string id, ast::fixity_type fix, 
                   const symbol_data* sd);
        identifier(const location& loc, unique_string id, ast::fixity_type fix, 
                   identifier_kind ki, fresh);

        static identifier       underscore(const location& loc);

        bool                    is_underscore() const;

        unique_string           get_name(ast::fixity_type& ft) const;
        unique_string           get_name_simple() const;
        std::string             get_name_full() const;
        fixity_type             get_fixity() const;
        symbol_data*            get_symbol_data() const;
};

class ARROW_EXPORT annotated_type_identifier
    : public ast_base<annotated_type_identifier, details::annotated_type_identifier_impl, ast_mark>
{
    private:
        annotated_type_identifier(const impl_ptr& ip);

    public:
        annotated_type_identifier();
        annotated_type_identifier(const location& loc, const identifier& id, const kind& ki);

        const identifier&   get_identifier() const;
        const kind&         get_kind() const;
};

class ARROW_EXPORT annotated_identifier
    : public ast_base<annotated_identifier, details::annotated_identifier_impl, ast_mark>
{
    private:
        annotated_identifier(const impl_ptr& ip);

    public:
        annotated_identifier();
        annotated_identifier(const location& loc, const identifier& id, const type& ta);

        const identifier&   get_identifier() const;
        const type&         get_type() const;
};

/// list of prop_type = prop_value assignments; allowed values:
///     kind    := infix | prefix | postfix | surrounding | indexing
///     prec    := integer values in range [prec_min, prec_max]
///     quoting := none | implicit | left | right | both
///     assoc   := left | right | none
/// properties are tested immediately
class ARROW_EXPORT operator_property
{
    private:
        operator_info       m_op_info;

    public:
        operator_property();

        void                add_string(const ast::location& loc, const std::string& prop_type, 
                                const std::string& prop_value, const error_handler_ptr& eh);
        void                add_int(const ast::location& loc, const std::string& prop_type, 
                                int prop_value, const error_handler_ptr& eh);

        const operator_info&    get_operator_info() const;
};

class ARROW_EXPORT symbol_sequence
    : public ast_base<symbol_sequence, details::symbol_sequence_impl, ast_mark>
{
    private:
        symbol_sequence(const impl_ptr& ip);

    public:
        symbol_sequence();
        symbol_sequence(const location& loc);

        void                add(const identifier& );
        long                size() const;
        const identifier&   get(long pos) const;
};

};};

#pragma warning(pop)

