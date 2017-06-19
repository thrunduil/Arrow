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

#include <map>
#include <list>

namespace arrow { namespace details
{

class type_comparer
{
    public:
        virtual ~type_comparer(){};
        virtual bool test(const ast::type& first, const ast::type& second) = 0;
};

class kind_comparer
{
    public:
        virtual ~kind_comparer(){};
        virtual bool test(const ast::kind& first, const ast::kind& second) = 0;
};

struct inst_type_conf
{
    using ident_type    = ast::identifier_type;
    using def_type      = type_alias_info;
};

struct inst_term_conf
{
    using ident_type    = ast::identifier_term;
    using def_type      = symbol_definition_env;
};

template<class Conf>
class instantiation_table
{
    private:
        using ident_type    = typename Conf::ident_type;
        using def_type      = typename Conf::def_type;

        using key_type      = var_info;
        using fun_sym_map   = std::map<key_type, def_type>;
        using iterator_type = typename fun_sym_map::iterator;
        using type_vec      = std::vector<ast::type>;
        using kind_vec      = std::vector<ast::kind>;
        using type_inst_info= std::pair<type_vec, ast::identifier>;
        using kind_inst_info= std::pair<kind_vec, ast::identifier>;
        using type_inst_vec = std::list<type_inst_info>;
        using kind_inst_vec = std::list<kind_inst_info>;
        using type_id_map   = std::map<key_type, type_inst_vec>;
        using kind_id_map   = std::map<key_type, kind_inst_vec>;

    private:
        fun_sym_map         m_defs;
        type_id_map         m_type_ident_map;
        kind_id_map         m_kind_ident_map;

    public:
        void                clear();
        iterator_type       add_type(const ident_type& id_base, const ast::identifier& new_id, 
                                def_type&& def, const std::vector<ast::type>& args, bool true_inst);
        iterator_type       add_kind(const ident_type& id_base, const ast::identifier& new_id, 
                                def_type&& def, const std::vector<ast::kind>& args, bool true_inst);
        iterator_type       register_local_function_subst(const ast::identifier& id, 
                                def_type&& def);
        ast::identifier     get_existing_type(const ident_type& base_id, 
                                const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                                long& num_matched);
        ast::identifier     get_existing_kind(const ident_type& base_id, 
                                const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                                long& num_matched);
};

};}
