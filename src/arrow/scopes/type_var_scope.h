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

#include "module.h"
#include "error/error.h"
#include "symbol_data.h"

namespace arrow { namespace details
{

class type_var_scope_impl : public std::enable_shared_from_this<type_var_scope_impl>
{
    private:
        using type_var_scope_ptr    = std::shared_ptr<type_var_scope_impl>;
        using code_kind             = long;
        using var_map               = std::map<ast::unique_string, code_kind>;

    private:
        module_index        m_mod_index;
        scope_index         m_scope_index;
        type_var_scope_ptr  m_parent;
        var_map             m_set;
        ast::location       m_loc;
        long                m_last_def_index;
        bool                m_kind_scope;

    public:
        type_var_scope_impl(const ast::location& loc, module_index mi, scope_index si, 
                const type_var_scope& parent);
        type_var_scope_impl(const ast::location& loc, module_index mi, scope_index si,
                bool is_kind_scope);

        bool                is_kind_scope() const;

        /// set fresh def_index for an identifier; no checks are performed
        def_index           set_def_index(const ast::identifier& vi);
        def_index           set_def_index(const ast::identifier& vi, const def_index& di);

        bool                set_definition_info(const ast::identifier& vi);
        def_index           find_definition(ast::unique_string name, ast::location& loc, bool& has) const;
        def_index           add_var(const error_handler_ptr& eh, const ast::identifier& vi);
};

class type_var_scope
{
    private:
        using impl_type     = details::type_var_scope_impl;
        using impl_ptr      = std::shared_ptr<impl_type>;

    private:
        impl_ptr            m_impl;

        friend impl_type;

    public:
        type_var_scope();
        type_var_scope(const impl_ptr&);
        ~type_var_scope();

        bool                is_kind_scope() const;
        void                add_var(const error_handler_ptr& eh, const ast::identifier& vi);
        bool                set_definition_info(const ast::identifier& vi) const;
        type_var_scope      new_var_scope(const ast::location& loc, const module& mod) const;

        /// set fresh def_index for an identifier; no checks are performed
        def_index           set_def_index(const ast::identifier& vi);
};

}}