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

class overload_scope
{
    public:
        struct fresh{};

    private:
        module_index    m_mod;
        scope_index     m_scope;

    public:
        overload_scope(fresh);
        overload_scope(module_index mi, scope_index si);

        bool        operator<(const overload_scope& other) const;
        static bool is_same_module(const overload_scope& sc1, const overload_scope& sc2);
};

class def_location
{
    private:
        long                    m_dist_from_scope;

    public:
        def_location();

        static def_location     this_scope();
        static def_location     imported(def_location other_loc, bool the_same_module,
                                    bool local);

        /// return true if first is more local than second
        /// for example if first is in current scope and second
        /// is imported
        static bool             more_local(def_location first, def_location second);

        /// return true if definition is imported from other module
        bool                    is_from_import() const;
};

template<class Config>
class overload_set
{
    private:
        using symbol_def        = typename Config::definition_type;
        using def_info          = std::pair<const symbol_def*, def_location>;

        using def_vec           = std::vector<def_info>;
        using def_vec_ptr       = std::shared_ptr<def_vec>;
        using scope_set         = std::set<overload_scope>;

    private:
        ast::identifier         m_id;
        def_vec_ptr             m_definitions;
        bool                    m_is_overloaded;
        ast::fixity_type        m_fixity;
        overload_scope          m_scope;
        scope_set               m_imported;

    public:
        overload_set();
        overload_set(module_index mi, scope_index si, const ast::identifier& id, 
                     ast::fixity_type ft, bool is_overloaded);

        const ast::identifier&  get_identifier() const;
        const ast::location&    get_location() const;
        def_index               get_definition_index() const;
        const symbol_def*       get_definition(long pos, def_location& loc) const;
        long                    size() const;

        overload_scope          get_scope() const;

        /// fixity type of this overload set can be different than
        /// fixity of its identifier; overload sets for different fixities
        /// must have the same root identifier (this is required during building
        /// dependency graph)
        ast::fixity_type        get_fixity() const;

        void                    add(const symbol_def* def);
        void                    import_from(const overload_set& other);
        bool                    is_overload() const;

        /// import from secondary overload set constructed during
        /// instantiation of overloaded symbol in local scope
        void                    import_from_local(const overload_set& other);

    private:
        void                    import_from(const overload_set& other, bool local);
};

};}
