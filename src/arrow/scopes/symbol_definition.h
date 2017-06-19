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

#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"

namespace arrow { namespace details
{

class symbol_definition_env
{
    public:
        enum symbol_type
        {
            function, abstract_overload, closed_overload
        };

    private:
        ast::location       m_location;
        ast::type           m_type;
        ast::identifier     m_symbol;
        ast::term           m_body;
        def_index           m_def_index;
        long                m_group_index;
        mutable bool        m_has_error;
        symbol_type         m_symbol_type;

    public:
        symbol_definition_env();

        symbol_definition_env(const ast::location& loc, const ast::type& sig, 
                const ast::identifier& id, const ast::term& body, bool is_overload);

        const ast::type&        get_signature() const;        
        const ast::identifier&  get_symbol() const;
        const ast::term&        get_body(bool& has_body) const;        
        ast::term_definition    get_term_definition() const;
        void                    update_term_definition(const ast::term_definition& def);

        const ast::location&    get_location() const;        
        const def_index&        get_definition_index() const;
        long                    get_group_index() const;
        symbol_type             get_symbol_type() const;
        bool                    is_overload() const;
        bool                    is_abstract_overload() const;
        bool                    is_closed_overload() const;
        bool                    has_error() const;

        void                    set_signature(const ast::type& ty);
        void                    set_body(const ast::term& ex);
        void                    set_error_flag() const;

        void                    set_def_index(def_index di, long group_index);
};

}};
