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
#include "scopes/overload_set.h"

namespace arrow { namespace details
{

namespace ad = arrow::details;

template<class Config>
class overload_finalizator
{
    public:
        virtual ~overload_finalizator(){};

        virtual void    make(overload_set<Config>& os, ast::unique_string name, 
                            ast::fixity_type ft) = 0;
};

template<class Config>
class overload_table
{
    private:
        using key_type_over = std::pair<ast::unique_string, ast::fixity_type>;
        using overload_set  = ad::overload_set<Config>;
        using overload_map  = std::map<key_type_over, overload_set>;        
        using overload_iter = typename overload_map::const_iterator;
        using over_iter2    = typename overload_map::iterator;
        using func_type     = typename Config::function_type;

    public:
        using const_iterator= typename overload_map::const_iterator;
        using iterator      = typename overload_map::iterator;

    private:
        overload_map        m_overload_map;

    public:
        void                clear();

        iterator            get_or_build(const ast::identifier& id, ast::fixity_type ft,
                                bool is_overload, const error_handler_ptr& eh, module_index mi,
                                scope_index si, overload_finalizator<Config>* fin);
        const_iterator      get(ast::unique_string name, ast::fixity_type ft, bool& has) const;
        bool                is_overloaded(const ast::unique_string id) const;

        iterator            add_overload(const ast::identifier& id, ast::fixity_type ft,
                                const func_type& ty, const error_handler_ptr& eh,
                                module_index mi, scope_index si, overload_finalizator<Config>* fin);

        void                remove_failed_definition(const ast::identifier& id);

    private:
        over_iter2          build_new_overload(const ast::identifier& id, ast::fixity_type ft,
                                bool is_overload, const error_handler_ptr& eh,
                                module_index mi, scope_index si, overload_finalizator<Config>* fin);
        overload_set        initialize_overload_set(const ast::identifier& id, ast::fixity_type ft,
                                bool is_overload, module_index mi, scope_index si,
                                overload_finalizator<Config>* fin);
};

};}
