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
#include "overload_set.h"
#include "module_impl.h"
#include "type_table.h"
#include "scopes/symbol_data.h"
#include "scopes/symbol_definition.h"

namespace arrow { namespace details
{

//-------------------------------------------------------------------
//                   overload_scope   
//-------------------------------------------------------------------
overload_scope::overload_scope(fresh)
    :m_mod(module_index::fresh()), m_scope(scope_index::fresh())
{};

overload_scope::overload_scope(module_index mi, scope_index si)
    :m_mod(mi), m_scope(si)
{};

bool overload_scope::operator<(const overload_scope& other) const
{
    if (m_scope < other.m_scope)
        return true;
    if (m_scope > other.m_scope)
        return false;

    return m_mod < other.m_mod;
};

bool overload_scope::is_same_module(const overload_scope& sc1, const overload_scope& sc2)
{
    return sc1.m_mod == sc2.m_mod;
}

//-------------------------------------------------------------------
//                   def_location   
//-------------------------------------------------------------------
def_location::def_location()
    :m_dist_from_scope(-1)
{};

def_location def_location::this_scope()
{
    def_location ret;
    ret.m_dist_from_scope = 0;

    return ret;
};

def_location def_location::imported(def_location other_loc, bool the_same_module,
                                    bool local)
{
    def_location ret;

    if (local == true)
        ret.m_dist_from_scope = -2;
    else if (the_same_module == false)
        ret.m_dist_from_scope = -1;
    else
        ret.m_dist_from_scope = other_loc.m_dist_from_scope + 1;

    return ret;
};

bool def_location::is_from_import() const
{
    return m_dist_from_scope == -1;
};

bool def_location::more_local(def_location first, def_location second)
{
    if (first.m_dist_from_scope < 0 || second.m_dist_from_scope < 0)
    {
        if (first.m_dist_from_scope >= 0)
            return true;

        if (second.m_dist_from_scope >= 0)
            return false;

        // both are imported or locally defined in instantiation
        // point

        // prefer imported (-1) over local (-2)
        return first.m_dist_from_scope > second.m_dist_from_scope;
    };

    return first.m_dist_from_scope < second.m_dist_from_scope;
};

//-------------------------------------------------------------------
//                   overload_set   
//-------------------------------------------------------------------
template<class Config>
overload_set<Config>::overload_set()
    :m_is_overloaded(false), m_scope(overload_scope::fresh())
{};

template<class Config>
overload_set<Config>::overload_set(module_index mi, scope_index si, const ast::identifier& id,
                           ast::fixity_type ft, bool is_overloaded)
    :m_id(id), m_is_overloaded(is_overloaded), m_fixity(ft), m_scope(mi, si)
{
    m_imported.insert(m_scope);
};

template<class Config>
const ast::identifier& overload_set<Config>::get_identifier() const
{
    return m_id;
};

template<class Config>
const ast::location& overload_set<Config>::get_location() const
{
    return m_id->get_location();
};

template<class Config>
bool overload_set<Config>::is_overload() const
{
    return m_is_overloaded;
}

template<class Config>
ast::fixity_type overload_set<Config>::get_fixity() const
{
    return m_fixity;
}

template<class Config>
void overload_set<Config>::add(const symbol_def* def)
{
    if (!m_definitions)
        m_definitions = def_vec_ptr(new def_vec());

    if (m_is_overloaded == false)
    {
        m_definitions->clear();
        m_definitions->push_back(def_info(def,def_location::this_scope()));
        return;
    }
    else
    {
        m_definitions->push_back(def_info(def,def_location::this_scope()));
    }
};

template<class Config>
void overload_set<Config>::import_from(const overload_set& other)
{
    return import_from(other, false);
};

template<class Config>
void overload_set<Config>::import_from_local(const overload_set& other)
{
    return import_from(other, true);
};

template<class Config>
void overload_set<Config>::import_from(const overload_set& other, bool local)
{
    if (!m_definitions)
        m_definitions = def_vec_ptr(new def_vec());

    /// this should never happened
    if (m_is_overloaded == false)
        return;

    if (other.m_is_overloaded == false)
        return;

    overload_scope other_sc = other.get_scope();

    //check if imported
    {
        auto pos    = m_imported.find(other_sc);
        if (pos != m_imported.end())
            return;

        m_imported.insert(pos, other_sc);
        m_imported.insert(other.m_imported.begin(), other.m_imported.end());
    };

    bool same_mod           = overload_scope::is_same_module(m_scope, other.m_scope);

    if (!other.m_definitions)
        return;

    long n  = (long)other.m_definitions->size();
    for (long i = 0; i < n; ++i)
    {
        def_location other_loc  = (*other.m_definitions)[i].second;
        const symbol_def* def   = (*other.m_definitions)[i].first;        
        m_definitions->push_back(def_info(def, def_location::imported(other_loc, same_mod, local)));
    }
};

template<class Config>
def_index overload_set<Config>::get_definition_index() const
{
    if (m_is_overloaded == true)
        return m_id.get_symbol_data()->get_def_index();

    if (this->size() != 1)
    {
        //this should be impossible
        return m_id.get_symbol_data()->get_def_index();
    }

    return (*m_definitions)[0].first->get_definition_index();
};

template<class Config>
const typename overload_set<Config>::symbol_def* 
overload_set<Config>::get_definition(long pos, def_location& loc) const
{
    if (!m_definitions)
        return nullptr;

    if ((long)m_definitions->size() <= pos)
        return nullptr;

    loc = (*m_definitions)[pos].second;
    return (*m_definitions)[pos].first;
};

template<class Config>
long overload_set<Config>::size() const
{
    if (!m_definitions)
        return 0;
    else
        return (long)m_definitions->size();
}

template<class Config>
overload_scope overload_set<Config>::get_scope() const
{
    return m_scope;
};

template class overload_set<ad::overload_term_config>;
template class overload_set<ad::overload_type_config>;

}};
