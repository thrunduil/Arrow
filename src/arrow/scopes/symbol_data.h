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

#include "arrow/config.h"
#include "arrow/error_handler.h"
#include "arrow/details/fwd_decls.h"

namespace arrow { namespace details
{

class module_index
{
    public:
        struct fresh{};

    private:
        ast::unique_string  m_value;

    public:
        module_index()      = delete;
        
        module_index(fresh){};
        module_index(ast::unique_string value);

        ast::unique_string  get_value() const;

        bool                operator==(const module_index& other) const;
        bool                operator!=(const module_index& other) const;
        bool                operator<(const module_index& other) const;
        bool                operator>(const module_index& other) const;
};

class scope_index
{
    public:
        enum type
        {
            expr_scope,
            type_alias_scope,
            type_scope,
            undefined_scope,
        };

        struct fresh{};

    private:
        int         m_value;
        type        m_type;

    public:
        scope_index() = delete;
        
        scope_index(fresh);
        scope_index(int value, type t);

        int         get_value() const   { return m_value; };

        bool        operator==(const scope_index& other) const;
        bool        operator!=(const scope_index& other) const;
        bool        operator<(const scope_index& other) const;
        bool        operator>(const scope_index& other) const;
};

struct def_index
{
    public:
        struct fresh{};

    private:
        module_index    m_mod;
        scope_index     m_scope;
        int             m_value;

    public:
        def_index() = delete;

        def_index(fresh);
        def_index(module_index mod, scope_index scope, int value);

        bool            is_set() const      { return m_value != -1; };
        int             get_value() const   { return m_value; };
        module_index    get_module() const  { return m_mod; };
        scope_index     get_scope() const   { return m_scope; };

        bool            operator==(const def_index& other) const;
        bool            operator!=(const def_index& other) const;
        bool            operator<(const def_index& other) const;
        bool            operator>(const def_index& other) const;
};

class var_info
{
    private:      
        def_index               m_def;
        ast::unique_string      m_name;
        ast::fixity_type        m_fix_type;
        ast::identifier_kind    m_kind;
        ast::location           m_loc;

    public:
        var_info();
        var_info(const ast::identifier& vi);

        bool                    operator<(const var_info& other) const;
        bool                    operator==(const var_info& other) const;
        bool                    operator!=(const var_info& other) const;

        const def_index&        get_def() const;
        ast::unique_string      get_name(ast::fixity_type& ft) const;
        ast::unique_string      get_name_simple() const;
        const ast::location&    get_location() const;
        ast::identifier         make_var_identifier() const;

    private:
        void                    check_def_index(int def) const;
};

}}

namespace arrow { namespace ast
{

namespace ad = arrow::details;

class symbol_data
{
    private:
        using module_index  = arrow::details::module_index;
        using scope_index   = arrow::details::scope_index;
        using def_index     = arrow::details::def_index;

    private:
        module_index        m_module;
        scope_index         m_scope;        
        def_index           m_def;
        identifier_kind     m_kind;

    private:
        symbol_data(identifier_kind ik);

    public:        
        void                set_local_scope(scope_index index)  { m_scope = index; }
        void                set_module(module_index index)      { m_module = index;};
        void                set_definition(def_index di)        { m_def = di; };

        scope_index         get_scope_index() const             { return m_scope; };
        module_index        get_module_index() const            { return m_module; };
        def_index           get_def_index() const               { return m_def; };        
        bool                is_any_operator() const             { return m_kind.is_any_operator(); };

        bool                has_definition() const              { return m_def.is_set(); };
        static symbol_data  make_new(identifier_kind ik)        { return symbol_data(ik); };

        /// only for nodes other than identifier
        static symbol_data  make_new()                          { return symbol_data(identifier_kind()); };

        ast::fixity_type    get_fixity() const;
        identifier_kind     get_kind() const                    { return m_kind; };
        identifier_kind&    get_kind()                          { return m_kind; };
};

class overload_index
{
    private:
        using overload_set  = ad::overload_set<ad::overload_term_config>;

    private:
        const overload_set* m_set;

        friend arrow::details::module_impl;

    public:
        overload_index()    :m_set(nullptr){};
};

}};

#include "symbol_data.inl"