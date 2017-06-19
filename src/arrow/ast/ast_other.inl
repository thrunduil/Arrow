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

#include "ast_common.inl"
#include "error/error.h"

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      identifier
//---------------------------------------------------------------

namespace details
{

class identifier_impl : public ast_atom
{
    public:
        unique_string       m_string;
        fixity_type         m_ft;
        mutable symbol_data m_symbol_data;

    public:
        identifier_impl(const location& loc, unique_string val, fixity_type ft,
                        const symbol_data* sd);
        identifier_impl(const location& loc, unique_string val, fixity_type ft,
                        const symbol_data& sd);
        
        unique_string       get_name(fixity_type& ft) const;
        symbol_data*        get_symbol_data() const;
};

identifier_impl::identifier_impl(const location& loc, unique_string val, fixity_type ft,
                                 const symbol_data* sd)
    :base_type(loc), m_string(val), m_symbol_data(*sd), m_ft(ft)
{
    check_null(val);
};

identifier_impl::identifier_impl(const location& loc, unique_string val, fixity_type ft,
                                 const symbol_data& sd)
    :base_type(loc), m_string(val), m_symbol_data(sd), m_ft(ft)
{
    check_null(val);
};
        
unique_string identifier_impl::get_name(fixity_type& ft) const
{
    ft = m_ft;
    return m_string;
};

symbol_data* identifier_impl::get_symbol_data() const
{
    return &m_symbol_data;
}

};

identifier::identifier()
{};

identifier::identifier(const impl_ptr& ip)
    :base_type(ip)
{};

identifier::identifier(const location& loc, unique_string id, fixity_type ft,
                       const symbol_data* sd)
    :base_type(new impl_type(loc, id, ft, sd))
{}

identifier::identifier(const location& loc, unique_string id, fixity_type ft, 
                       identifier_kind ik, fresh)
    :base_type(new impl_type(loc, id, ft, symbol_data::make_new(ik)))
{}

static unique_string underscore_symbol()
{
    static unique_string id("_");
    return id;
};

identifier identifier::underscore(const location& loc)
{
    identifier_kind ik;
    ik.set_lex(identifier_kind::lexical_type::ident);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::var);

    return identifier(loc, underscore_symbol(), fixity_type::undefined, ik, fresh());
};

bool identifier::is_underscore() const
{
    fixity_type ft;
    return this->get_name(ft) == underscore_symbol();
};

unique_string identifier::get_name(fixity_type& ft) const
{
    return m_value->get_name(ft);
}
unique_string identifier::get_name_simple() const
{
    return m_value->m_string;
}
std::string identifier::get_name_full() const
{
    return operator_properties::get().get_decorated_name
                (get_name_simple().get_string(), get_fixity());
}

fixity_type identifier::get_fixity() const
{
    return m_value->m_ft;
}

symbol_data* identifier::get_symbol_data() const
{
    return m_value->get_symbol_data();
}


//---------------------------------------------------------------
//                      module_definition
//---------------------------------------------------------------
namespace details
{
class module_definition_impl
{
    private:
        using string_loc    = std::tuple<unique_string, unique_string, location>;
        using string_vec    = std::vector<string_loc>;

    private:
        std::string         m_module_name;
        string_vec          m_imports;
        definitions         m_definitions;

    public:
        module_definition_impl(const std::string& module_name, const string_vec& imports,
            const definitions& defs);

        const std::string&  get_module_name() const;
        const string_vec&   get_imports() const;
        const definitions&  get_definitions() const;
};

module_definition_impl::module_definition_impl(const std::string& module_name, 
        const string_vec& imports, const definitions& defs)
    :m_module_name(module_name), m_imports(imports), m_definitions(defs)
{};

const std::string& module_definition_impl::get_module_name() const
{
    return m_module_name;
}
const module_definition_impl::string_vec& 
module_definition_impl::get_imports() const
{
    return m_imports;
}
const definitions& module_definition_impl::get_definitions() const
{
    return m_definitions;
}

};

module_definition::module_definition()
{};

module_definition::module_definition(const std::string& module_name, 
    const string_vec& imports, const definitions& defs)
    :m_impl(new impl_type(module_name, imports, defs))
{};

const std::string& module_definition::get_module_name() const
{
    return m_impl->get_module_name();
}

const module_definition::string_vec& 
module_definition::get_imports() const
{
    return m_impl->get_imports();
}

const definitions& module_definition::get_definitions() const
{
    return m_impl->get_definitions();
}

//---------------------------------------------------------------
//                      annotated_type_identifier
//---------------------------------------------------------------
namespace details
{

class annotated_type_identifier_impl : public ast_atom
{
    public:
        identifier          m_id;
        kind                m_kind;

    public:
        annotated_type_identifier_impl(const location& loc, const identifier& id, const kind& ki);
};

annotated_type_identifier_impl::annotated_type_identifier_impl(const location& loc, 
        const identifier& id, const kind& ki)
    :base_type(loc), m_id(id), m_kind(ki)
{
    check_null(id);
    check_null(ki);
};

};

annotated_type_identifier::annotated_type_identifier(const impl_ptr& ip)
    :base_type(ip)
{};

annotated_type_identifier::annotated_type_identifier()
{};

annotated_type_identifier::annotated_type_identifier(const location& loc, 
            const identifier& id, const kind& ki)
    :base_type(new impl_type(loc, id, ki))
{};

const identifier& annotated_type_identifier::get_identifier() const
{
    return m_value->m_id;
}
const kind& annotated_type_identifier::get_kind() const
{
    return m_value->m_kind;
}

//---------------------------------------------------------------
//                      definitions
//---------------------------------------------------------------
namespace details
{

class definitions_impl : public ast_atom
{
    private:
        using vec_type      = std::vector<definition>;

    private:
        vec_type            m_data;

    public:
        definitions_impl(const location& loc);

        void                add(const definition& def);

        long                size() const;
        const definition&   get_definition(long pos) const;
};

definitions_impl::definitions_impl(const location& loc)
    :base_type(loc)
{};

void definitions_impl::add(const definition& def)
{
    m_data.push_back(def);
    check_null(def);
}

long definitions_impl::size() const
{
    return (long)m_data.size();
}
const definition& definitions_impl::get_definition(long pos) const
{
    check_pos(m_data,pos);
    return m_data[pos];
}
};

definitions::definitions(const impl_ptr& ip)
    :base_type(ip)
{};

definitions::definitions()
{};

definitions::definitions(const location& loc)
    :base_type(new impl_type(loc))
{};

void definitions::add(const definition& def)
{
    m_value->add(def);
};

long definitions::size() const
{
    return m_value->size();
}
const definition& definitions::get_definition(long pos) const
{
    return m_value->get_definition(pos);
}

//---------------------------------------------------------------
//                      annotated_identifier
//---------------------------------------------------------------

namespace details
{

class annotated_identifier_impl : public ast_atom
{
    public:
        identifier          m_id;
        type                m_type;

    public:
        annotated_identifier_impl(const location& loc, const identifier& id, const type& ta);
};

annotated_identifier_impl::annotated_identifier_impl(const location& loc, 
                const identifier& id, const type& ta)
    :base_type(loc), m_id(id), m_type(ta)
{
    check_null(ta);
    check_null(id);
};

};

annotated_identifier::annotated_identifier(const impl_ptr& ip)
    :base_type(ip)
{};

annotated_identifier::annotated_identifier()
{};

annotated_identifier::annotated_identifier(const location& loc, const identifier& id, const type& ta)
    :base_type(new impl_type(loc, id, ta))
{};

const identifier& annotated_identifier::get_identifier() const
{
    return m_value->m_id;
}
const type& annotated_identifier::get_type() const
{
    return m_value->m_type;
}

//--------------------------------------------------------------------------
//                          symbol_sequence
//--------------------------------------------------------------------------
namespace details
{

class symbol_sequence_impl : public ast_atom
{
    private:
        using base_type     = ast_atom;
        using elem_vec      = std::vector<identifier>;

    private:
        elem_vec            m_defs;

    public:
        symbol_sequence_impl(const location& loc);

        void                add(const identifier& ex);
        long                size() const;
        const identifier&   get(long pos) const;
};

symbol_sequence_impl::symbol_sequence_impl(const location& loc)
    :base_type(loc)
{};

void symbol_sequence_impl::add(const identifier& ex)
{
    m_defs.push_back(ex);
    check_null(ex);
};

long symbol_sequence_impl::size() const
{
    return (long)m_defs.size();
}
const identifier& symbol_sequence_impl::get(long pos) const
{
    check_pos(m_defs, pos);
    return m_defs[pos];
}

};

symbol_sequence::symbol_sequence()
{};

symbol_sequence::symbol_sequence(const location& loc)
    : base_type(new impl_type(loc))
{};

symbol_sequence::symbol_sequence(const impl_ptr& ip)
    :base_type(ip)
{};

void symbol_sequence::add(const identifier& d)
{
    m_value->add(d);
}
long symbol_sequence::size() const
{
    return m_value->size();
}
const identifier& symbol_sequence::get(long pos) const
{
    return m_value->get(pos);
}

//--------------------------------------------------------------------------
//                          operator_property
//--------------------------------------------------------------------------
operator_property::operator_property()
{};

void operator_property::add_string(const ast::location& loc, const std::string& prop_type, 
                                   const std::string& prop_value, const error_handler_ptr& eh)
{
    const auto& table = operator_properties::get();
    bool ok = table.decode_property(m_op_info, prop_type, prop_value);

    if (ok == false)
    {
        if (table.is_property(prop_type) == false)
        {
            error::operator_error_msg(eh).error_invalid_operator_property
                    (loc, prop_type, table.allowed_properties());
        }
        else
        {
            error::operator_error_msg(eh).error_invalid_operator_property_value
                    (loc, prop_type, prop_value, table.allowed_property_values(prop_type));
        }
    };
}
void operator_property::add_int(const ast::location& loc, const std::string& prop_type, int prop_value,
                                const error_handler_ptr& eh)
{
    bool ok = operator_properties::get().decode_property(m_op_info, prop_type, prop_value);

    if (ok == false)
    {
        if (prop_type != "prec")
        {
            error::operator_error_msg(eh).error_invalid_operator_property(loc, prop_type, {"prec"});
        }
        else
        {
            int prec_min    = (int)named_precedence::minimum_allowed;
            int prec_max    = (int)named_precedence::maximum_allowed;
            error::operator_error_msg(eh).error_invalid_operator_precedence(loc, prop_value, prec_min, prec_max);
        };
    };
}

const operator_info& operator_property::get_operator_info() const
{
    return m_op_info;
};

//--------------------------------------------------------------------------
//                          identifier_kind
//--------------------------------------------------------------------------
identifier_kind& identifier_kind::set_lex(lexical_type ty)
{ 
    m_lex = ty; 
    return *this; 
};

identifier_kind& identifier_kind::set_sector(sector_type ty)
{ 
    m_sector = ty; 
    return *this; 
};
identifier_kind& identifier_kind::set_function(function_type ty)
{ 
    m_function = ty; 
    return *this; 
}

identifier_kind& identifier_kind::set_sector_module()
{
    return set_sector(sector_type::module);
}
identifier_kind& identifier_kind::set_sector_kind()
{
    return set_sector(sector_type::kind);
}
identifier_kind& identifier_kind::set_sector_type()
{
    return set_sector(sector_type::type);
}
identifier_kind& identifier_kind::set_sector_term()
{
    return set_sector(sector_type::term);
}

identifier_kind& identifier_kind::set_function_var()
{
    return set_function(function_type::var);
}
identifier_kind& identifier_kind::set_function_def()
{
    return set_function(function_type::def);
}
identifier_kind& identifier_kind::set_function_overload()
{
    return set_function(function_type::overload);
}

bool identifier_kind::is_bracket_full() const
{ 
    return m_lex == lexical_type::bracket_full; 
};
bool identifier_kind::is_bracket_part() const
{ 
    return m_lex == lexical_type::bracket_part; 
};

bool identifier_kind::is_any_operator() const
{
    return m_lex == lexical_type::oper
        || m_lex == lexical_type::bracket_full;
};

identifier_kind::lexical_type identifier_kind::get_lexical() const
{
    return m_lex;
}
identifier_kind::sector_type identifier_kind::get_sector() const
{
    return m_sector;
}
identifier_kind::function_type identifier_kind::get_function() const
{
    return m_function;
}

bool identifier_kind::is_sector_kind() const
{
    return m_sector == sector_type::kind;
}
bool identifier_kind::is_sector_type() const
{
    return m_sector == sector_type::type;
}
bool identifier_kind::is_sector_term() const
{
    return m_sector == sector_type::term;
}
bool identifier_kind::is_sector_module() const
{
    return m_sector == sector_type::module;
}

bool identifier_kind::is_function_def() const
{
    return m_function == function_type::def;
}
bool identifier_kind::is_function_var() const
{
    return m_function == function_type::var;
}
bool identifier_kind::is_function_overload() const
{
    return m_function == function_type::overload;
}

bool identifier_kind::is_type_def() const
{
    return is_sector_type() && is_function_def();
};
bool identifier_kind::is_term_def() const
{
    return is_sector_term() && is_function_def();
};
bool identifier_kind::is_type_var() const
{
    return is_sector_type() && is_function_var();
};
bool identifier_kind::is_kind_var() const
{
    return is_sector_kind() && is_function_var();
};

};};
