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
#include "arrow/ast/ast.h"
#include "arrow/error_handler.h"

#include <vector>

namespace arrow { namespace oper
{

class operator_declaration
{
    private:
        using location_vec      = std::vector<ast::location>;

    private:
        ast::location           m_location_infix;
        ast::operator_info      m_info_infix;

        ast::location           m_location_prefix;
        ast::operator_info      m_info_prefix;

        ast::location           m_location_postfix;        
        ast::operator_info      m_info_postfix;

        ast::location           m_location_indexing;
        ast::operator_info      m_info_indexing;

        ast::location           m_location_surrounding;
        ast::operator_info      m_info_surrounding;

    public:
        operator_declaration();

        const ast::location&    get_location(ast::fixity_type fix) const;
        const ast::operator_info&
                                get_info(ast::fixity_type fix) const;
        operator_declaration    select_info(ast::fixity_type fix) const;
        bool                    has(ast::fixity_type fix) const;

        void                    add(const ast::location& loc, const ast::operator_info& info, 
                                    ast::fixity_type ft);

        /// info must represent an operator of exactly one fixity type
        void                    add_single(const ast::location& loc, const ast::operator_info& info);

        /// equivalent to add_single called on newly created object
        static operator_declaration
                                make_single(const ast::location& loc, const ast::operator_info& decl);

        /// return true if at least one operator kind is added
        bool                    is_defined() const;

        /// return true if all operator kinds are added
        bool                    all_operators_defined() const;

        /// add definition of operator of given kind if is not set already
        void                    add_definitions(const operator_declaration& defs);

        size_t                  get_all_fixity_flag() const;

        /// return true if all defined operators are equal; not defined
        /// operator in one of sets are ignored; different_locations
        /// vector contains locations of different operators of the same king
        bool                    equal_defined(const operator_declaration& other, 
                                    std::vector<ast::location>& different_locations);

        /// print all declarations
        void                    print(std::ostream& os) const;
};

};};
