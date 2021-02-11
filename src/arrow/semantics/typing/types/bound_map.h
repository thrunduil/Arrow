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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "semantics/typing/terms/substitution.h"

#include <map>

namespace arrow { namespace typing
{

namespace ad = arrow::details;

class bound_map;

class bound_map_scope
{
    private:
        bound_map_scope(bound_map* bm);
        friend bound_map;

    private:
        bound_map*      m_owner;

        bool            m_has_1;
        ad::var_info    m_var_1;
        long            m_code_1;
        
        bool            m_has_2;        
        ad::var_info    m_var_2;        
        long            m_code_2;

    public:
        ~bound_map_scope();

        bound_map_scope(bound_map_scope&&);

        bound_map_scope(const bound_map_scope&) = delete;
        bound_map_scope& operator=(const bound_map_scope&) = delete;

    private:
        void            add1(const ad::var_info& v, long code);
        void            add2(const ad::var_info& v, long code);
};

class bound_map
{
    private:
        using map_type      = std::map<ad::var_info, long>;
        friend bound_map_scope;

    private:
        map_type            m_map;

    public: 
        void                clear();
        bound_map_scope     add(const ast::identifier& id1, const ast::identifier& id2);
        long                get_code(const ast::identifier& id) const;
        static bool         is_bound(long code);

    private:
        void                restore(const ad::var_info& v, long code);
};

};};