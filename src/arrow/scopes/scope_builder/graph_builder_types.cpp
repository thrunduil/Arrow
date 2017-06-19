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

#include "graph_builder_types.h"

namespace arrow { namespace sema { namespace details
{

//------------------------------------------------------------------
//                   build_type_graph_visitor   
//------------------------------------------------------------------
build_type_graph_visitor::build_type_graph_visitor(const error_handler_ptr& eh, 
                            const module& mod, const graph& g, bool build)
    :m_mod(mod), m_eh(eh), m_graph(g), m_build(build)
{};

//---------------------------------------------------------------
//               TYPES
//---------------------------------------------------------------
void build_type_graph_visitor::build_identifier(const ast::identifier& v, bool& errors)
{
    errors = false;

    ast::fixity_type ft;
    ast::unique_string name = v.get_name(ft);
    ast::location loc;
    bool found;
    std::vector<ast::location> ambig;

    ad::def_index di    = m_mod.get_type_definition_index
                            (name, true, loc, found, ambig);

    if (found == true && ambig.size() == 0)
    {
        v.get_symbol_data()->get_kind().set_sector_type().set_function_def();
        v.get_symbol_data()->set_definition(di);
        return;
    };

    if (found == true)
    {
        error::scope_error_msg(m_eh).error_ambiguous_type_symbol
                                (v->get_location(), name, ambig);
        errors = true;
        return;
    }
    else
    {
        error::scope_error_msg(m_eh).error_undefined_type_symbol
                                (v->get_location(), name);
        errors = true;
        return;
    }

    return;
};

void build_type_graph_visitor::eval(const ast::identifier_type& ast)
{
    const ast::identifier& v    = ast.get_identifier();
    ast::symbol_data* sd        = v.get_symbol_data();

    if (sd->has_definition() == false)
    {
        bool errors;
        build_identifier(v, errors);

        if (errors == true)
            return;
    }
            
    ad::module_index mi         = sd->get_def_index().get_module();
    ast::identifier_kind kind   = sd->get_kind();

    if (kind.is_type_def() == false)
        return;

    if (m_build)
    {
        if (mi != m_mod.get_module_index())
            m_graph.add_extern_edge(v);
        else
            m_graph.add_edge(v);
    };
};

};};};
