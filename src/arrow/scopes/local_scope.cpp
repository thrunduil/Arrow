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

#include "local_scope_impl.h"
#include "semantics/semantics.h"
#include "scopes/overload_set.h"

namespace arrow { namespace details
{

struct loc_overload_fin : overload_finalizator<ad::overload_term_config>
{
    using overload_set  = ad::overload_set<overload_term_config>;

    local_scope_impl*   m_owner;
    module_index        m_mi;

    loc_overload_fin(local_scope_impl* owner, module_index mi)
        : m_owner(owner), m_mi(mi)
    {};

    virtual void make(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft) override
    {
        m_owner->finalize_new_overload_set(os, name, ft, m_mi);
    };
};

//------------------------------------------------------------------
//                   local_scope_impl   
//------------------------------------------------------------------
symbol_iter::symbol_iter(pattern_iter it)
    :m_pattern_iter(it), m_kind(pattern)
{};

symbol_iter::symbol_iter()
    :m_kind(pattern)
{};

symbol_iter::symbol_iter(bool has_definition, fun_sym_iter it)
    :m_func_iter(it)
{
    if (has_definition == true)
        m_kind = function;
    else
        m_kind = overload;
}

bool symbol_iter::is_overloaded() const
{
    return m_kind == overload;
}

const ast::type& symbol_iter::get_signature(bool& has_error) const
{
    switch(m_kind)
    {
        case pattern:
        {
            return m_pattern_iter->second.get_pattern_signature(has_error);
        }
        case function:
        {
            has_error =  m_func_iter->second.has_error();
            return m_func_iter->second.get_signature();
        }
    };

    has_error = true;
    static ast::type ty;
    return ty;
};

const ast::term& symbol_iter::get_definition(const error_handler_ptr& eh, bool& has_error) const
{
    switch(m_kind)
    {
        case pattern:
        {
            error::error_base_msg(eh).internal_error(m_pattern_iter->second.m_location,
                                        "pattern variable does not have definition");
            has_error = true;
            break;
        }
        case function:
        {
            has_error = m_func_iter->second.has_error();
            bool has_body;
            return m_func_iter->second.get_body(has_body);
        }
    };

    has_error = true;
    static ast::term ex;
    return ex;
};

void symbol_iter::update_type(const ast::type& new_ty)
{
    switch(m_kind)
    {
        case pattern:
        {
            return m_pattern_iter->second.update_pattern_type(new_ty);
        }
        case function:
        {            
            return m_func_iter->second.set_signature(new_ty);
        }
    };
};

void symbol_iter::update_let_definition(const ast::term_definition& td)
{
    switch(m_kind)
    {
        case pattern:
        {
            //not possible
            return;
        }
        case function:
        {            
            return m_func_iter->second.update_term_definition(td);
        }
    };
};


term_symbol_kind symbol_iter::get_symbol_kind() const
{
    switch(m_kind)
    {
        case pattern:
            return m_pattern_iter->second.get_symbol_kind();
        case function:
            return term_symbol_kind::local_symbol;            
        case overload:
            return term_symbol_kind::overloaded_symbol;
        default:
            return term_symbol_kind::undefined;            
    };
};

//------------------------------------------------------------------
//                   local_scope_impl   
//------------------------------------------------------------------
local_scope_impl::local_scope_impl(const module& m, scope_index index)
    :m_module(m), m_parent(ptr_type()), m_index(index), m_building_graph(false)
    ,m_has_components(false), m_last_def_index(0)
{};

local_scope_impl::local_scope_impl(const local_scope_impl* parent, scope_index index)
    : m_module(), m_building_graph(false), m_index(index)
    , m_parent(const_cast<local_scope_impl*>(parent)->shared_from_this())    
    , m_has_components(false), m_last_def_index(0)
{};

scope_index local_scope_impl::get_scope_index() const
{
    return m_index;
};

local_scope local_scope_impl::new_scope(const module& owner) const
{    
    return owner.create_child_scope(*this);
};

const local_scope_impl::fun_sym_map& local_scope_impl::get_local_vars() const
{
    return m_function_map;
};

local_scope_impl::fun_sym_map& local_scope_impl::get_local_vars_modify()
{
    return m_function_map;
};

const graph& local_scope_impl::get_graph() const
{
    return m_graph;
}

bool local_scope_impl::is_bound(const ast::identifier& vi) const
{
    local_variables::key_type key(vi.get_name_simple());

    auto pos = m_map.m_vars.find(key);

    if (pos != m_map.m_vars.end())
    {
        const def_index& di = pos->second.m_def_index;
        vi.get_symbol_data()->set_definition(di);
        return true;
    };

    if (m_parent.is_null() == false)
        return m_parent.is_bound(vi);
    else
        return false;
};

bool local_scope_impl::is_referred_function(const ast::identifier& vi) const
{
    local_variables::key_type key(vi.get_name_simple());

    auto pos = m_map.m_vars.find(key);

    if (pos == m_map.m_vars.end())
        return false;
    else
        return pos->second.is_referred();
};

def_index local_scope_impl::get_new_def_index(module_index mi)
{
    long n_defs     = m_last_def_index++;
    def_index di    = def_index(mi, this->get_scope_index(), n_defs);            
    return di;
};
void local_scope_impl::add_to_defmap(int index, symbol_iter it)
{
    if ((int)m_definition_map.size() <= index)
        m_definition_map.resize(2 * index + 1);

    m_definition_map[index]  = it;
};

bool local_scope_impl::check_symbol(const ast::identifier& id, 
                                    const error_handler_ptr& eh) const
{
    if (m_parent.is_null() == false)
        return m_parent.check_symbol(id, eh);

    if (m_module.is_null() == false)
        return m_module.check_symbol(id, eh);

    return true;
};

void local_scope_impl::add_pattern_var(module_index mi, const ast::annotated_identifier& aid,
                                       const error_handler_ptr& eh)
{
    const ast::identifier& id   = aid.get_identifier();

    if (check_symbol(id, eh) == false)
        return;

    const ast::type& id_type    = aid.get_type();
    def_index di                = get_new_def_index(mi);

    local_variables::key_type key(id.get_name_simple());
    
    id.get_symbol_data()->set_local_scope(this->get_scope_index());
    id.get_symbol_data()->set_module(mi);
    id.get_symbol_data()->get_kind().set_sector_term().set_function_var();
    id.get_symbol_data()->set_definition(di);

    using value_type = local_variables::def_map::value_type;

    auto pos = m_map.m_vars.insert(value_type(key, local_var_info(id->get_location(), di, id_type)));
    add_to_defmap(di.get_value(), symbol_iter(pos.first));
};

ast::location local_scope_impl::get_symbol_definition(ast::unique_string str, 
                            bool search_parent, bool& found, loc_vec& ambig) const
{
    found = false;
    local_variables::key_type key(str);

    auto pos = m_map.m_vars.find(key);
    if (pos != m_map.m_vars.end())
    {
        found = true;
        return pos->second.m_location;
    }

    if (search_parent == false)
        return ast::location();

    if (m_parent.is_null() == false)
        return m_parent.get_symbol_definition(str, true, found, ambig);

    if (m_module.is_null() == false)
        return m_module.get_symbol_definition(str, true, found,ambig)
                            .get_location();

    return ast::location();
};

term_symbol_kind local_scope_impl::get_term_symbol_kind(const ast::identifier_term& idt) const
{
    const ast::identifier& id   = idt.get_identifier();
    int index   = id.get_symbol_data()->get_def_index().get_value();

    if (index >= (int)m_definition_map.size() || index < 0)
        return term_symbol_kind::undefined;

    return m_definition_map[index].get_symbol_kind();
};

ast::type local_scope_impl::get_symbol_type(const error_handler_ptr& eh, 
                            const ast::identifier_term& idt, bool& overloaded, 
                            bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    int index   = id.get_symbol_data()->get_def_index().get_value();
    overloaded  = false;
    has_error   = false;

    if (index >= (int)m_definition_map.size() || index < 0)
    {
        std::string msg = std::string() + "symbol " + id.get_name_simple().get_string()
                        + " has not a type assignment";
        error::error_base_msg(eh).internal_error(id->get_location(), msg);

        has_error = true;
        return ast::error_type(id->get_location());
    }
      
    auto info   = m_definition_map[index];

    if (info.is_overloaded() == true)
    {
        overloaded = true;
        return ast::type();
    };
   
    return m_definition_map[index].get_signature(has_error);
};

bool local_scope_impl::is_overloaded_term_def(const ast::identifier_term& idt, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    int index   = id.get_symbol_data()->get_def_index().get_value();
    has_error   = false;

    if (index >= (int)m_definition_map.size() || index < 0)
    {
        has_error = true;
        return false;
    }
   
    return m_definition_map[index].is_overloaded();
};

ast::term local_scope_impl::get_symbol_term(const error_handler_ptr& eh, 
                            const ast::identifier_term& idt, bool& overloaded, 
                            bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    int index   = id.get_symbol_data()->get_def_index().get_value();
    overloaded  = false;
    has_error   = false;

    if (index >= (int)m_definition_map.size() || index < 0)
    {
        std::string msg = std::string() + "symbol " + id.get_name_simple().get_string()
                        + " has not a type assignment";
        error::error_base_msg(eh).internal_error(id->get_location(), msg);

        has_error = true;
        return ast::error_term(id->get_location());
    }
    
    auto info   = m_definition_map[index];

    if (info.is_overloaded() == true)
    {
        overloaded = true;
        return ast::term();
    };
   
    return m_definition_map[index].get_definition(eh, has_error);
};

const local_scope_impl::overload_set* 
local_scope_impl::get_overload_set(const error_handler_ptr& eh, 
                        const ast::identifier_term& idt) const
{
    const ast::identifier& id   = idt.get_identifier();

    ast::unique_string name     = id.get_name_simple();
    ast::fixity_type ft         = id.get_fixity();

    bool has;
    auto pos                    = m_overloads.get(name, ft, has);

    if (has == false)
    {
        std::string msg = std::string() + "symbol " + name.get_string()
                        + " is not defined";
        error::error_base_msg(eh).internal_error(id->get_location(), msg);
        return nullptr;
    }

    return &pos->second;
}

const local_scope_impl::overload_set* 
local_scope_impl::get_overload_set(const ast::identifier& id) const
{
    ast::unique_string name = id.get_name_simple();
    ast::fixity_type ft     = id.get_fixity();

    bool has;
    auto pos                = m_overloads.get(name, ft, has);

    if (has == false)
        return nullptr;

    return &pos->second;
};

void local_scope_impl::update_pattern_var(const ast::identifier& id, const ast::type& new_ty)
{
    int index   = id.get_symbol_data()->get_def_index().get_value();

    if (index >= (int)m_definition_map.size() || index < 0)
    {
        //this should be impossible
        return;
    }
        
    return m_definition_map[index].update_type(new_ty);
};

void local_scope_impl::update_let_definitions(const ast::definitions& defs)
{
    long n      = defs.size();
    for (long i = 0; i < n; ++i)
    {
        const ast::definition& def  = defs.get_definition(i);
        ast::term_definition td     = def.static_cast_to<ast::term_definition>();
        const ast::identifier& id   = td.get_identifier();

        int index   = id.get_symbol_data()->get_def_index().get_value();

        if (index >= (int)m_definition_map.size() || index < 0)
        {
            //this should be impossible
            continue;
        }
        
        m_definition_map[index].update_let_definition(td);
    };
};

bool local_scope_impl::is_local_symbol_defined(const module& mod, const ad::var_info& vi) const
{
    const def_index& di = vi.get_def();

    module_index mi     =  mod.get_module_index();
    if (mi != di.get_module())
        return false;

    if (di.get_scope() == this->get_scope_index())
        return true;

    if (m_parent.is_null() == false)
        return m_parent.is_local_symbol_defined(mod, vi);

    return false;
};

bool local_scope_impl::is_symbol_defined(const ast::identifier& id, bool search_parent,
                                         std::vector<ast::location>& ambig) const
{
    local_variables::key_type key(id.get_name_simple());

    auto pos = m_map.m_vars.find(key);

    if (pos != m_map.m_vars.end())
    {
        def_index di = pos->second.m_def_index;
        id.get_symbol_data()->set_definition(di);
        return true;
    };

    if (search_parent == false)
        return false;

    if (m_parent.is_null() == false)
        return m_parent.is_symbol_defined(id, true, ambig);

    if (m_module.is_null() == false)
        return m_module.is_symbol_defined(id, true, ambig);
    
    return false;
};

void local_scope_impl::add_let_var(module_index mi, const ast::identifier& v, 
                            const ast::term_definition& def, const error_handler_ptr& eh)
{
    if (check_symbol(v, eh) == false)
        return;

    const ast::term& body       = def.get_body();
    const ast::type& ty         = def.get_type();
    bool is_overload            = def.is_overload();

    if (is_overload == true && !body)
    {
        add_overload(mi, v, ty, eh);
        return;
    };

    {
        //perform decomposition t0 := Kind[args ...] t_body
        std::vector<ast::identifier> args;
        ast::term def_body;
        sema::decompose_term(body, args, def_body);

        if (def_body->get_term_code() == ast::details::term_code::term_constant)
        {
            ast::term_constant tc   = def_body.static_cast_to<ast::term_constant>();

            if (tc.is_extern_func() == true)
            {
                error::scope_error_msg(eh).error_extern_function_cannot_be_local
                            (v->get_location(), v);
                return;
            };
        }
    };

    ast::unique_string vs   = v.get_name_simple();
    add_function_definition(mi, v, vs, def, eh);
};

void local_scope_impl::add_overload(module_index mi, const ast::identifier& vi, const ast::type& ty,
                                    const error_handler_ptr& eh)
{
    add_overload(mi, vi, ast::fixity_type::undefined, ty, eh);
    ast::fixity_type ft     = vi.get_fixity();

    ast::unique_string name = vi.get_name_simple();
    bool is_oper            = is_any_operator(name);

    if (is_oper == false)
    {
        if (ft == ast::fixity_type::undefined)
            return;

        error::operator_error_msg(eh).warning_explicit_fixity_not_for_operator
                        (vi->get_location(), name);
        return;
    };

    // add overload for operator with explicit fixity
    bool is_brack   = vi.get_symbol_data()->get_kind().is_bracket_full();

    if (ft == ast::fixity_type::undefined)
    {
        if (is_brack == true)
            ft      = ast::operator_info::default_fixity_bracket;
        else
            ft      = ast::operator_info::default_fixity_operator;
    };

    add_overload(mi, vi, ft, ty, eh);
};

void local_scope_impl::add_overload(module_index mi, const ast::identifier& vi, ast::fixity_type ft, 
                        const ast::type& ty, const error_handler_ptr& eh)
{
    if (check_symbol(vi, eh) == false)
        return;

    bool has_def        = ty != ast::type();
    def_index di        = get_new_def_index(mi);

    vi.get_symbol_data()->get_kind().set_sector_term().set_function_overload();
    vi.get_symbol_data()->set_definition(di);

    ast::unique_string name = vi.get_name_simple();
    key_type_over key_string(name, ft);

    loc_overload_fin fin(this, mi);
    auto pos_over       = m_overloads.add_overload(vi, ft, ty, eh, mi, m_index, &fin);

    if (has_def == true)
    {
        symbol_definition_env def(vi->get_location(), ty, vi, ast::term(), true);

        key_type key(vi);

        long group      = 0;
        auto pos2       = m_function_map.insert(fun_sym_map::value_type(key, def));    
        pos2.first->second.set_def_index(di, group);

        pos_over->second.add(&pos2.first->second);
    };

    local_var_info lvi(vi->get_location(), pos_over);
    auto pos_map = m_map.m_vars.insert(local_variables::def_map::value_type(name, lvi));

    add_to_defmap(di.get_value(), symbol_iter(false, fun_sym_map::iterator()));
};

void local_scope_impl::add_function_definition(module_index mi, const ast::identifier& v, 
                        ast::unique_string vi, const ast::term_definition& term_def, 
                        const error_handler_ptr& eh)
{    
    def_index di    = get_new_def_index(mi);
    bool is_brack   = v.get_symbol_data()->get_kind().is_bracket_full();
    long group      = 0;

    v.get_symbol_data()->set_local_scope(this->get_scope_index());
    v.get_symbol_data()->set_module(mi);
    v.get_symbol_data()->get_kind().set_sector_term().set_function_def();
    v.get_symbol_data()->set_definition(di);

    ast::unique_string name = v.get_name_simple();
    ast::fixity_type ft     = v.get_fixity();
    ast::fixity_type ft_und = ast::fixity_type::undefined;

    key_type        key(v);
    key_type_string key_string(name);
    key_type_over   key_over(name, ft_und);

    bool is_over    = term_def.is_overload() || is_overloaded(key_string);
    bool is_oper    = is_any_operator(key_string);    

    if (ft != ast::fixity_type::undefined && is_oper == false)
    {
        error::operator_error_msg(eh).warning_explicit_fixity_not_for_operator
                        (term_def->get_location(), name);
        ft          = ast::fixity_type::undefined;
    };

    if (is_oper == true)
    {        
        if (ft == ast::fixity_type::undefined)
        {
            if (is_brack == true)
                ft      = ast::operator_info::default_fixity_bracket;
            else
                ft      = ast::operator_info::default_fixity_operator;
        };

        oper::operator_declaration op_decl;
        loc_vec ambig_loc;
        check_operator_type(name, op_decl, true, ambig_loc);

        bool has_declaration = op_decl.has(ft);

        if (has_declaration == false)
        {
            ast::location loc = term_def->get_location();
            error::operator_error_msg(eh).error_invalid_operator_definition_no_valid_declaration
                                    (loc, name, ft, op_decl);
            return;
        };
    };

    ad::symbol_definition_env def(term_def->get_location(), term_def.get_type(), 
                                term_def.get_identifier(), term_def.get_body(), 
                                term_def.is_overload());

    auto pos2       = m_function_map.insert(fun_sym_map::value_type(key, def));    

    pos2.first->second.set_def_index(di, group);

    loc_overload_fin fin(this, mi);
    auto pos_over   = m_overloads.get_or_build(v, ft_und, is_oper || is_over, eh, 
                        mi, m_index, &fin);

    pos_over->second.add(&pos2.first->second);

    auto pos = m_map.m_vars.find(key_string);

    if (pos != m_map.m_vars.end())
    {
        if (is_over == false && is_oper == false)
        {
            ast::location this_loc   = def.get_location();
            ast::location other_loc  = pos->second.m_location;
            error::scope_error_msg(eh).error_symbol_defined(vi, this_loc, other_loc);
            return;
        };
    }
    if (is_over == false && is_oper == false)
    {
        //it is checked that symbols added to this scope are unique;
        //however this symbol can be defined in parent scopes
        std::vector<ast::location> ambig;
        if (this->is_symbol_defined(v, true, ambig) == true)
        {
            bool found;
            std::vector<ast::location> ambig2;
            ast::location old    = this->get_symbol_definition(vi, true, found, ambig2);
            error::scope_error_msg(eh).warning_symbol_hidden(vi, v->get_location(), old);
        }
    };    
    if (is_over == false && !term_def.get_body())
    {    
        error::scope_error_msg(eh).error_function_definition_without_body
                                        (term_def->get_location(), vi);
        return;
    };

    local_var_info lvi(def.get_location(), pos_over);
    auto pos_map = m_map.m_vars.insert(local_variables::def_map::value_type(key_string, lvi));

    add_to_defmap(di.get_value(), symbol_iter(true, pos2.first));

    if (is_oper == false)
        return;

    //build overload set with explicit fixity
    auto pos_over2  = m_overloads.get_or_build(v, ft, true, eh, mi, m_index, &fin);
    pos_over2->second.add(&pos2.first->second);
};

void local_scope_impl::finalize_new_overload_set(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft, module_index mi)
{
    if (os.is_overload() == false)
        return;

    if (m_parent.is_null() == false)
        m_parent.import_overloads(os, name, ft);
    else if (m_module.is_null() == false)
        m_module.import_overloads(os, name, ft);

    // build def index
    if (ft == ast::fixity_type::undefined)
    {
        // overload set must have unique def index the same for
        // overload sets with different fixities; for overload sets
        // with fixities different than undenfined identifiers are the same
        // as for undefined fixity; therefor we must creare new def index
        // for undefined fixity only
        const ast::identifier& id   = os.get_identifier();
        def_index di                = get_new_def_index(mi);

        id.get_symbol_data()->get_kind().set_sector_term().set_function_overload();
        id.get_symbol_data()->set_definition(di);

        add_to_defmap(di.get_value(), symbol_iter(false, fun_sym_map::iterator()));
    };
};

void local_scope_impl::import_overloads(overload_set& os, ast::unique_string id, 
                        ast::fixity_type ft) const
{
    bool has;
    auto pos = m_overloads.get(id, ft, has);

    if (has == true)
    {
        os.import_from(pos->second);
        return;
    };

    if (m_parent.is_null() == false)
        return m_parent.import_overloads(os, id, ft);

    if (m_module.is_null() == false)
        return m_module.import_overloads(os, id, ft);
};

bool local_scope_impl::is_overloaded(const ast::unique_string id) const
{
    if (m_overloads.is_overloaded(id) == true)
        return true;

    if (m_parent.is_null() == false)
        return m_parent.is_overloaded(id);

    if (m_module.is_null() == false)
        return m_module.is_overloaded(id);
    
    return false;
};

bool local_scope_impl::is_any_operator(ast::unique_string vi) const
{
    if (m_operators.is_any_operator(vi) == true)
        return true;

    if (m_parent.is_null() == false)
        return m_parent.is_any_operator(vi);

    if (m_module.is_null() == false)
        return m_module.is_any_operator(vi, true);
    
    return false;
};

void local_scope_impl::check_operator_type(ast::unique_string str, oper::operator_declaration& od,
                            bool search_parent, loc_vec& ambig_loc) const
{
    bool found_all  = m_operators.check_operator_type(str, od);

    if (found_all == true)
        return;

    if (search_parent == false)
        return;

    if (m_parent.is_null() == false)
        return m_parent.check_operator_type(str, od, true, ambig_loc);

    if (m_module.is_null() == false)
        return m_module.check_operator_type(str, od, true, ambig_loc);

    return;
};

void local_scope_impl::building_graph_start()
{
    m_building_graph = true;
};
void local_scope_impl::building_graph_end()
{
    m_building_graph = false;
}
const local_scope_impl::components& local_scope_impl::get_connected_components()
{
    if (m_has_components == true)
        return m_components;

    m_graph.strongly_connected_components(m_components);
    m_has_components = true;

    return m_components;
}

void local_scope_impl::add_referred(const ast::identifier& vi)
{
    local_variables::key_type key(vi.get_name_simple());
    auto pos = m_map.m_vars.find(key);

    if (pos != m_map.m_vars.end())
    {
        vi.get_symbol_data()->set_definition(pos->second.get_definition_index());

        if (m_building_graph == true)
            m_graph.add_edge(vi);

        return pos->second.set_referred();
    }

    if (m_parent.is_null() == false)
        return m_parent.add_referred(vi);
};

void local_scope_impl::add_operator_declaration(module_index mi, const ast::identifier& vi, 
                        const ast::operator_info& fixity, const error_handler_ptr& eh)
{
    if (check_symbol(vi, eh) == false)
        return;

    ast::fixity_type fix_type   = fixity.get_fixity_type();
    ast::unique_string str      = vi.get_name_simple();

	if (is_operator_declared(str, fix_type, false) == true)
    {
        bool found;
        std::vector<ast::location> ambig;
        ast::location old   = get_operator_declaration(str, fix_type, 
                                false, found, ambig).get_location(fix_type); 

        error::operator_error_msg(eh).error_fixity_defined(str, vi->get_location(), old);
        return;
    }
    else if (is_operator_declared(str, fix_type, true) == true)
    {
        bool found;
        std::vector<ast::location> ambig;
        //operator defined in parent environment; warning
        const oper::operator_declaration& old_def
            = get_operator_declaration(str, fix_type, true, found, ambig); 

        error::operator_error_msg(eh).warning_fixity_hidden(str, vi->get_location(),
            old_def.get_location(fix_type));
    }
      
    m_operators.add_operator_declaration(vi, fixity);
    add_overload(mi, vi, fix_type, ast::type(), eh);
    add_overload(mi, vi, ast::fixity_type::undefined, ast::type(), eh);
};

oper::operator_declaration 
local_scope_impl::get_operator_declaration(ast::unique_string str, ast::fixity_type ft, bool search_parent,
                                           bool& found, loc_vec& ambig) const
{
    found       = false;

    // operator defined in given module hides definitions of an operator
    // of the same kind in other modules
    {
        auto res = m_operators.get_operator_declaration(str, ft, found);

        if (found == true)
            return res;
    }

    if (search_parent == false)
    {
        found = false;
        return oper::operator_declaration();
    }

    if (m_parent.is_null() == false)
        return m_parent.get_operator_declaration(str, ft, true, found, ambig);

    if (m_module.is_null() == false)
        return m_module.get_operator_declaration(str, ft, true, found, ambig);

    found = false;
    return oper::operator_declaration();
};

bool local_scope_impl::is_operator_declared(ast::unique_string vi, ast::fixity_type ft, 
                            bool search_parent) const
{
    if (m_operators.is_operator_declared(vi, ft) == true)
        return true;

    if (search_parent == false)
        return false;

    if (m_parent.is_null() == false)
        return m_parent.is_operator_declared(vi, ft, true);

    if (m_module.is_null() == false)
        return m_module.is_operator_declared(vi, ft, true);

    return false;
};

}};

namespace arrow
{

//------------------------------------------------------------------
//                   local_var_info   
//------------------------------------------------------------------
local_var_info::local_var_info()
    : m_is_referred(false), m_def_index(ad::def_index::fresh())
    , m_symbol_kind(term_symbol_kind::undefined), m_is_def(false)
    , m_has_error(false)
{};

local_var_info::local_var_info(const ast::location& loc, ad::def_index di,
                               const ast::type& sig)
    :m_location(loc), m_is_referred(false), m_def_index(di), m_pattern_type(sig)
    , m_symbol_kind(term_symbol_kind::pattern_var), m_is_def(false)
    , m_has_error(false)
{};

local_var_info::local_var_info(const ast::location& loc, iter_over iter)
    :m_location(loc), m_def_iter(iter), m_is_referred(false)
    , m_def_index(iter->second.get_definition_index())
    , m_symbol_kind(term_symbol_kind::local_symbol), m_is_def(true)
    , m_has_error(false)
{};

void local_var_info::update_pattern_type(const ast::type& ty)
{
    if (ty->get_type_code() == ast::details::type_code::error_type)
        m_has_error = true;

    m_pattern_type = ty;
}

term_symbol_kind local_var_info::get_symbol_kind() const
{
    return m_symbol_kind;
};

const ast::type& local_var_info::get_pattern_signature(bool& has_error) const
{
    has_error = m_has_error;
    return m_pattern_type;
}

const ast::identifier& local_var_info::get_symbol(const error_handler_ptr& eh) const
{    
    if (m_is_def == false)
    {
        static const ast::identifier et;

        error::error_base_msg(eh).internal_error(m_location, "symbol has no definition");
        return et;
    }

    return m_def_iter->second.get_identifier();
}

void local_var_info::set_referred()
{
    m_is_referred = true;
}

bool local_var_info::is_referred() const
{
    return m_is_referred;
};

ad::def_index local_var_info::get_definition_index() const
{
    return m_def_index;
}

//------------------------------------------------------------------
//                   local_scope   
//------------------------------------------------------------------
/// initialize to empty scope
local_scope::local_scope(const module& m, details::scope_index index)
    :m_impl(new details::local_scope_impl(m, index))
{};

local_scope::local_scope(const details::local_scope_impl& parent, details::scope_index index)
:m_impl(new details::local_scope_impl(&parent, index))
{};

local_scope::local_scope(const impl_type& impl)
    :m_impl(impl)
{};

local_scope local_scope::null_scope()
{
    return local_scope(impl_type());
};

const graph& local_scope::get_graph() const
{
    return m_impl->get_graph();
}
void local_scope::building_graph_start()
{
    return m_impl->building_graph_start();
}
void local_scope::building_graph_end()
{
    return m_impl->building_graph_end();
}

const local_scope::components& local_scope::get_connected_components() const
{
    return m_impl->get_connected_components();
};

details::scope_index local_scope::get_scope_index() const
{
    return m_impl->get_scope_index();
}

local_scope local_scope::new_scope(const module& owner)
{
    return m_impl->new_scope(owner);
}

const local_scope::fun_sym_map& local_scope::get_local_vars() const
{
    return m_impl->get_local_vars();
};

local_scope::fun_sym_map& local_scope::get_local_vars_modify() const
{
    return m_impl->get_local_vars_modify();
};

bool local_scope::is_bound(const ast::identifier& vi) const
{
    return m_impl->is_bound(vi);
}
bool local_scope::is_referred_function(const ast::identifier& vi) const
{
    return m_impl->is_referred_function(vi);
}

bool local_scope::is_null() const
{
    return m_impl == nullptr;
}

void local_scope::add_pattern_var(ad::module_index mi, const ast::annotated_identifier& var,
                                  const error_handler_ptr& eh)
{
    m_impl->add_pattern_var(mi, var, eh);
}

ast::location local_scope::get_symbol_definition(ast::unique_string str, bool search_imports, 
                        bool& found, std::vector<ast::location>& ambig) const
{
    return m_impl->get_symbol_definition(str, search_imports, found, ambig);
}

oper::operator_declaration 
local_scope::get_operator_declaration(ast::unique_string str, 
                        ast::fixity_type ft, bool search_imports, 
                        bool& found, std::vector<ast::location>& ambig) const
{
    return m_impl->get_operator_declaration(str, ft, search_imports, 
                          found, ambig);
};

bool local_scope::is_operator_declared(ast::unique_string vi, ast::fixity_type ft, 
                        bool search_parent) const
{
    return m_impl->is_operator_declared(vi, ft, search_parent);
};

void local_scope::add_operator_declaration(ad::module_index mi, const ast::identifier& vi, 
                    const ast::operator_info& decl, const error_handler_ptr& eh)
{
    return m_impl->add_operator_declaration(mi, vi, decl, eh);
};

void local_scope::add_overload(ad::module_index mi, const ast::identifier& id, 
                        const ast::type& ty, const error_handler_ptr& eh)
{
    return m_impl->add_overload(mi, id, ty, eh);
};

term_symbol_kind local_scope::get_term_symbol_kind(const ast::identifier_term& id) const
{
    return m_impl->get_term_symbol_kind(id);
};

ast::type local_scope::get_symbol_type(const error_handler_ptr& eh, 
                        const ast::identifier_term& id, bool& overloaded, bool& has_error) const
{
    return m_impl->get_symbol_type(eh, id, overloaded, has_error);
}
bool local_scope::is_overloaded_term_def(const ast::identifier_term& id, bool& has_error) const
{
    return m_impl->is_overloaded_term_def(id, has_error);
}

ast::term local_scope::get_symbol_term(const error_handler_ptr& eh, 
                        const ast::identifier_term& id, bool& overloaded, bool& has_error) const
{
    return m_impl->get_symbol_term(eh, id, overloaded, has_error);
}
const local_scope::overload_set* 
local_scope::get_overload_set(const error_handler_ptr& eh, 
                        const ast::identifier_term& id) const
{
    return m_impl->get_overload_set(eh, id);
}
const local_scope::overload_set*
local_scope::get_overload_set(const ast::identifier& id) const
{
    return m_impl->get_overload_set(id);
};

bool local_scope::is_symbol_defined(const ast::identifier& id, bool search_parent,
                                    std::vector<ast::location>& ambig) const
{
    return m_impl->is_symbol_defined(id, search_parent, ambig);
}

bool local_scope::is_local_symbol_defined(const module& mod, const ad::var_info& vi) const
{
    return m_impl->is_local_symbol_defined(mod, vi);
};

void local_scope::add_let_var(ad::module_index mi, const ast::identifier& vi, 
                              const ast::term_definition& def, const error_handler_ptr& eh)
{
    return m_impl->add_let_var(mi, vi, def, eh);
}

void local_scope::add_referred(const ast::identifier& vi) const
{
    return m_impl->add_referred(vi);
};

void local_scope::update_pattern_var(const ast::identifier& id, const ast::type& new_ty)
{
    return m_impl->update_pattern_var(id, new_ty);
};
void local_scope::update_let_definitions(const ast::definitions& defs)
{
    return m_impl->update_let_definitions(defs);
};

bool local_scope::is_overloaded(const ast::unique_string id) const
{
    return m_impl->is_overloaded(id);
};
bool local_scope::is_any_operator(ast::unique_string vi) const
{
    return m_impl->is_any_operator(vi);
};

void local_scope::import_overloads(overload_set& os, ast::unique_string id, 
                        ast::fixity_type ft) const
{
    return m_impl->import_overloads(os, id, ft);
};

bool local_scope::check_symbol(const ast::identifier& id, const error_handler_ptr& eh) const
{
    return m_impl->check_symbol(id, eh);
};

void local_scope::check_operator_type(ast::unique_string vi, oper::operator_declaration& oi, 
                        bool search_parent, std::vector<ast::location>& ambig) const
{
    return m_impl->check_operator_type(vi, oi, search_parent, ambig);
}

};