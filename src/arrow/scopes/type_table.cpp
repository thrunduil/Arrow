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

#include "type_table.h"
#include "module_impl.h"
#include "semantics/semantics.h"
#include "type_var_scope.h"
#include "semantics/typing/terms/type_checker.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "root/options.h"
#include "semantics/utils.h"

#include <sstream>
#include <iostream>

namespace arrow { namespace details
{

struct type_overload_fin : overload_finalizator<overload_type_config>
{
    using overload_set  = ad::overload_set<overload_type_config>;

    type_table*   m_owner;

    type_overload_fin(type_table* owner)
        : m_owner(owner)
    {};

    virtual void make(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft) override
    {
        m_owner->finalize_new_overload_set(os, name, ft);
    };
};

//-------------------------------------------------------------------
//                   type_alias_info   
//-------------------------------------------------------------------
type_alias_info::type_alias_info()
    :m_def_index(def_index::fresh()), m_has_error(false), m_group_index(-1)
{};
type_alias_info::type_alias_info(const ast::location& loc, const ast::identifier& name, 
                const ast::kind& k, const ast::type& def, bool is_overload)
    :m_loc(loc), m_name(name), m_kind(k), m_def(def), m_def_index(def_index::fresh())
    ,m_has_error(false), m_group_index(-1)
{
    (void)is_overload;
    //TODO: overload
};

const ast::location& type_alias_info::get_location() const
{
    return m_loc;
};
const ast::identifier& type_alias_info::get_name() const
{
    return m_name;
}
const ast::kind& type_alias_info::get_kind() const
{
    return m_kind;
}
const ast::type& type_alias_info::get_def() const
{
    return m_def;
}
bool type_alias_info::has_error() const
{
    return m_has_error;
}
void type_alias_info::set_error() const
{
    m_has_error = true;
}

void type_alias_info::set_def(const ast::type& ty)
{
    if (ty->get_type_code() == ast::details::type_code::error_type)
        m_has_error = true;

    m_def = ty;
}
void type_alias_info::set_kind(const ast::kind& ki)
{
    m_kind = ki;
}

void type_alias_info::set_def_index(const def_index& def, long group)
{
    m_group_index   = group;
    m_def_index     = def;
}
const def_index& type_alias_info::get_definition_index() const
{
    return m_def_index;
}

//-------------------------------------------------------------------
//                   type_table   
//-------------------------------------------------------------------
type_table::type_table(module_index mi, module_impl* owner)
    :m_typevar_scopes(0), m_index(mi), m_owner(owner), m_last_def_index(0)
    ,m_scope_index(scope_index::fresh())
{
    m_global_typevar_scope = new_typevar_scope(ast::location());
};

void type_table::set_local_scope(scope_index si)
{
    m_scope_index = si;
};

void type_table::clear()
{
    m_type_alias.clear();
    m_build_type_iter.clear();
    m_saved_maps.clear();
    m_overloads.clear();
    m_build_symbol_set_string.clear();
    m_instantiations.clear();
};

void type_table::finish_building(bool is_shell)
{
    if (is_shell == true)
    {
        // remove failed defs
        for (auto& item : m_build_type_iter)
        {
            if (item.second.has_error() == false)
                continue;

            remove_definition(item.second.get_name());
        };
    };
    
    if (m_build_type_iter.size() > 0)
        m_saved_maps.push_back(std::move(m_build_type_iter));

    m_build_graph.clear();
};

void type_table::remove_definition(const ast::identifier& id)
{
    ast::unique_string name = id.get_name_simple();

    key_type_string key(name);
    auto pos = m_type_alias.find(key);

    //we can remove regular functions but overloads cannot be removed
    if (pos != m_type_alias.end())
    {
        if (pos->second->second.is_overload() == false)
            m_type_alias.erase(pos);
    };

    m_overloads.remove_failed_definition(id);
};

void type_table::add_type_definition(const ast::type_definition& type_def, const error_handler_ptr& eh)
{
    const ast::identifier& id   = type_def.get_identifier();
    const ast::kind& k          = type_def.get_kind();
    const ast::type& t0         = type_def.get_body();
    bool is_overload            = type_def.is_overload();

    if (is_overload == true && !t0)
    {
        add_overload(id, k, eh);
        return;
    };

    ast::type t                 = make_type_def_body(t0, type_def, eh);

    ast::fixity_type ft;
    ast::unique_string name     = id.get_name(ft);
    ast::fixity_type ft_und     = ast::fixity_type::undefined;

    bool is_global              = check_global_symbol(name);

    if (is_global)
    {
        ast::location this_loc   = type_def->get_location();
        error::scope_error_msg(eh).error_global_symbol(name, this_loc);
        return;
    };

    int n_defs      = get_new_def_index();
    def_index di    = def_index(m_index, get_typealias_scope_index(), n_defs);
    bool is_brack   = id.get_symbol_data()->get_kind().is_bracket_full();
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_type().set_function_def();
    id.get_symbol_data()->set_definition(di);    

    key_type        key(id);
    key_type_string key_string(name);
    key_type_over   key_over(name, ft_und);

    bool is_over    = is_overload || is_overloaded(key_string);
    bool is_oper    = is_any_operator(key_string);

    if (ft != ast::fixity_type::undefined && is_oper == false)
    {
        error::operator_error_msg(eh).warning_explicit_fixity_not_for_operator
                        (type_def->get_location(), name);
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
        check_operator_type(name, op_decl, ambig_loc);

        bool has_declaration = op_decl.has(ft);

        if (has_declaration == false)
        {
            ast::location loc = type_def->get_location();
            error::operator_error_msg(eh).error_invalid_operator_definition_no_valid_declaration
                                    (loc, name, ft, op_decl);
            return;
        };
    };

    type_alias_info def(type_def->get_location(), id, k, t, is_overload);

    auto pos2       = m_build_type_iter.insert(type_alias_map::value_type(key, def));
    pos2.first->second.set_def_index(di, group);

    type_overload_fin fin(this);
    auto pos_over   = m_overloads.get_or_build(id, ft_und, is_oper || is_over, eh, 
                        m_index, m_scope_index, &fin);

    pos_over->second.add(&pos2.first->second);
    m_type_alias[key_string] = pos_over;

    if (is_over == false && is_oper == false)
    {
        auto pos = m_build_symbol_set_string.find(key_string);

        if (pos != m_build_symbol_set_string.end() && is_shell_module() == false)
        {
            ast::location this_loc   = def.get_location();
            ast::location other_loc  = pos->second;
            error::scope_error_msg(eh).error_type_defined(name, this_loc, other_loc);
            return;
        };

        //used only to detect multiply defined symbols
        m_build_symbol_set_string[key_string]   = id->get_location();
    };

    add_to_defmap(di.get_value(), fun_sym_info(true, pos2.first));

    if (is_oper == false)
        return;

    //build overload set with explicit fixity
    auto pos_over2  = m_overloads.get_or_build(id, ft, true, eh, 
                        m_index, m_scope_index, &fin);

    pos_over2->second.add(&pos2.first->second);
};

void type_table::add_type_definition(const ast::type_definition_var& def, const error_handler_ptr& eh)
{
    const ast::identifier& id   = def.get_identifier();
    const ast::type& body       = def.get_body();

    ast::kind def_k             = def.get_return_kind();
    ast::type lambda            = make_type_def_body(body, def, def_k, eh);

    ast::type_definition def2(def->get_location(), id, def_k, lambda, def.is_overload());
    add_type_definition(def2, eh);
};

void type_table::add_overload(const ast::identifier& vi, const ast::kind& ty, 
                               const error_handler_ptr& eh)
{
    add_overload(vi, ast::fixity_type::undefined, ty, eh);
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

    add_overload(vi, ft, ty, eh);
};

void type_table::add_overload(const ast::identifier& vi, ast::fixity_type ft, const ast::kind& ty, 
                               const error_handler_ptr& eh)
{
    bool has_def        = ty != ast::kind();
    int n_defs          = get_new_def_index();
    def_index di        = def_index(m_index, get_typealias_scope_index(), n_defs);
    long group          = get_group_index();

    vi.get_symbol_data()->get_kind().set_sector_type().set_function_overload();
    vi.get_symbol_data()->set_definition(di);

    ast::unique_string name = vi.get_name_simple();
    key_type_over key_string(name, ft);

    type_overload_fin fin(this);
    auto pos_over       = m_overloads.add_overload(vi, ft, ty, eh,
                            m_index, m_scope_index, &fin);

    m_type_alias[name] = pos_over;

    if (has_def == true)
    {
        type_alias_info def(vi->get_location(), vi, ty, ast::type(), true);

        key_type key(vi);

        auto pos2       = m_build_type_iter.insert(type_alias_map::value_type(key, def));    
        pos2.first->second.set_def_index(di, group);

        pos_over->second.add(&pos2.first->second);
    };

    add_to_defmap(di.get_value(), fun_sym_info(false, fun_sym_iter()));
};

scope_index type_table::get_typealias_scope_index() const
{
    return scope_index(0, scope_index::type_alias_scope);
};

bool type_table::check_global_symbol(ast::unique_string name) const
{
    (void)name;
    return false;
}

int type_table::get_new_def_index()
{
    return m_last_def_index++;
}
long type_table::get_group_index()
{
    return (long)m_saved_maps.size();
};

void type_table::build_type_scopes(const module& mod, const error_handler_ptr& eh)
{
    for (auto& pos_alias : m_build_type_iter)
    {
        if (pos_alias.second.has_error() == true)
            continue;

        auto scope  = eh->push_error_scope();

        const ast::identifier& name     = pos_alias.second.get_name();
        const ast::kind k               = pos_alias.second.get_kind();
        const ast::type& body           = pos_alias.second.get_def();

        type_var_scope ts   = this->new_typevar_scope(name->get_location());
        type_var_scope ks   = this->new_kindvar_scope(name->get_location());
        build_scopes(mod, eh, body, k, ts, ks);

        if (eh->has_errors() == true)
            pos_alias.second.set_error();
    };
};

void type_table::type_desugaring(const module& mod, const error_handler_ptr& eh)
{
    for (auto& pos_alias : m_build_type_iter)
    {
        if (pos_alias.second.has_error() == true)
            continue;

        auto scope          = eh->push_error_scope();

        ast::type body      = pos_alias.second.get_def();
        sema::type_desugaring(mod, eh, body);
        pos_alias.second.set_def(body);

        if (eh->has_errors() == true)
            pos_alias.second.set_error();
    };    
}

void type_table::build_type_dependency_graph(const module& mod, const error_handler_ptr& eh)
{
    sema::build_type_dependency_graph(mod, eh, *this);
};

void type_table::build_scopes(const module& mod, const error_handler_ptr& eh, const ast::type& tc, 
                        const ast::kind& k, const type_var_scope& type_scope,
                        const type_var_scope& kind_scope)
{
    sema::build_type_scopes(mod, eh, tc, k, type_scope, kind_scope);
}

type_var_scope type_table::new_typevar_scope(const ast::location& loc)
{
    using impl = std::shared_ptr<details::type_var_scope_impl>;

    scope_index si  = get_new_typevar_scope();

    return type_var_scope(impl(new type_var_scope_impl(loc, m_index, si, false)));
};
type_var_scope type_table::new_kindvar_scope(const ast::location& loc)
{
    using impl = std::shared_ptr<details::type_var_scope_impl>;

    scope_index si  = get_new_typevar_scope();

    return type_var_scope(impl(new type_var_scope_impl(loc, m_index, si, true)));
};

scope_index type_table::get_new_typevar_scope()
{
    long index      = m_typevar_scopes;
    ++m_typevar_scopes;

    scope_index si  = scope_index(index, scope_index::type_scope);
    return si;
};

type_var_scope type_table::make_new_var_scope(const ast::location& loc, const type_var_scope& parent)
{
    using impl = std::shared_ptr<details::type_var_scope_impl>;

    scope_index si  = get_new_typevar_scope();

    return type_var_scope(impl(new type_var_scope_impl(loc, m_index, si, parent)));
};

ast::type type_table::get_type_constructor_definition(const error_handler_ptr& eh, 
                        const ast::identifier& id, bool& overloaded, bool& has_errors) const
{
    has_errors          = false;
    overloaded          = false;
    int index           = id.get_symbol_data()->get_def_index().get_value();

    if (index >= (int)m_def_map.size())
    {
        std::string msg = std::string() + "type symbol " + id.get_name_simple().get_string()
                        + " has not a type assignment";
        error::error_base_msg(eh).internal_error(id->get_location(), msg);

        has_errors = true;
        return ast::error_type(id->get_location());
    }

    auto info   = m_def_map[index];

    if (info.first == false)
    {
        overloaded = true;
        return ast::type();
    };

    if (info.second->second.has_error())
        has_errors = true;

    return info.second->second.get_def();
};

ast::kind type_table::get_type_constructor_kind(const error_handler_ptr& eh, 
                const ast::identifier_type& idt, bool& overloaded, bool& has_errors) const
{
    has_errors                  = false;
    overloaded                  = false;
    const ast::identifier& id   = idt.get_identifier();

    int index           = id.get_symbol_data()->get_def_index().get_value();

    if (index >= (int)m_def_map.size())
    {
        std::string msg = std::string() + "type symbol " + id.get_name_simple().get_string()
                        + " has not a type assignment";
        error::error_base_msg(eh).internal_error(id->get_location(), msg);

        has_errors = true;
        return ast::kind_var(id->get_location(), false);
    }

    auto info   = m_def_map[index];

    if (info.first == false)
    {
        overloaded = true;
        return ast::kind();
    };

    if (info.second->second.has_error())
        has_errors = true;

    return info.second->second.get_kind();
};

void type_table::build_fresh_type_variable(const ast::identifier& id)
{
    m_global_typevar_scope.set_def_index(id);
};


def_index type_table::get_type_definition_index(ast::unique_string ti, 
                                ast::location& loc, bool& found) const
{
    const overload_set& def = get_type_definition(ti, found);

    if (found == false)
        return def_index(def_index::fresh()); 

    def_index di    = def.get_definition_index();
    loc             = def.get_location();
    found           = true;

    return di;
};

const type_table::overload_set&
type_table::get_type_definition(ast::unique_string ti, bool& found) const
{
    {
        auto pos = m_type_alias.find(ti);

        if (pos != m_type_alias.end())
        {
            found   = true;
            return pos->second->second;
        };
    }
    {        
        found = false;
        static overload_set empty;
        return empty;
    };
};

const type_table::overload_set* 
type_table::get_overload_set(const ast::identifier& id) const
{
    ast::unique_string name = id.get_name_simple();
    ast::fixity_type ft     = id.get_fixity();

    bool has;
    auto pos                = m_overloads.get(name, ft, has);

    if (has == false)
        return nullptr;

    return &pos->second;
};

bool type_table::is_overloaded_def(const ast::identifier_type& idt, bool& has_error) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    ast::identifier_kind ik     = sd->get_kind();
    has_error                   = false;

    if (ik.is_type_var())        
        return false;

    int index   = sd->get_def_index().get_value();

    if (index >= (int)m_def_map.size())
    {
        has_error = true;
        return false;
    }
        
    auto info   = m_def_map[index];

    if (info.first == false)
        return true;

    if (info.second->second.has_error())
        has_error = true;

    return false;
};

type_symbol_kind type_table::get_symbol_kind(const ast::identifier_type& idt) const
{
    const ast::identifier& id   = idt.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();
    ast::identifier_kind ik     = sd->get_kind();

    if (ik.is_type_var())        
        return type_symbol_kind::pattern_var;

    int index                   = sd->get_def_index().get_value();

    if (index >= (int)m_def_map.size())
        return type_symbol_kind::undefined;
        
    auto info   = m_def_map[index];

    if (info.first == false)
        return type_symbol_kind::overloaded_symbol;
    else
        return type_symbol_kind::global_symbol;
};

void type_table::type_check_types(typing::type_checker& tc, const error_handler_ptr& eh,
                                  const module& mod)
{
    using key_var       = ad::var_info;
    using node_set      = std::set<ad::var_info>;
    using component_vec = std::vector<std::vector<key_var>>;

    component_vec comp;
    m_build_graph.strongly_connected_components(comp);

    for (auto& group : comp)
    {
        auto scope          = eh->push_error_scope();

        typing::inst_collector col(eh, mod);
        
        using group_item    = std::tuple<ad::var_info, ast::type, ast::kind>;
        using rec_group     = std::vector<group_item>;

        rec_group rec;
        long n              = (long)group.size();
        bool is_rec         = n > 1;

        if (n == 1)
        {
            const key_var& key = group[0];
            is_rec          = m_build_graph.is_self_recursive(key);
        };        

        //make group
        for(long i = 0; i < n; ++i)
        {
            const key_var& key = group[i];
            auto pos            = m_build_type_iter.find(key);

            if (pos == m_build_type_iter.end())
            {
                //this is overload root
                continue;
            }

            const auto& def     = pos->second;

            if (def.has_error() == true)
            {
                eh->add_error();
                continue;
            };
            
            const ast::kind& k  = def.get_kind();
            const ast::type& t  = def.get_def();

            rec.push_back(group_item(key, t, k));
        };

        //typecheck
        tc.check_kind(rec, is_rec, &col);

        bool has_errors = eh->has_errors();        

        //update definitions
        for(long i = 0, it = 0; i < n; ++i)
        {
            const key_var& key = group[i];
            auto pos            = m_build_type_iter.find(key);

            if (pos == m_build_type_iter.end())
                continue;

            auto& def           = pos->second;

            if (def.has_error() == true)
                continue;
            
            const ast::type& t2 = std::get<1>(rec[it]);
            const ast::kind& k2 = std::get<2>(rec[it]);
            ++it;

            def.set_kind(k2);
            def.set_def(t2);            

            if (has_errors == true)
                def.set_error();
        };

        //process instantiations
        bool has_errors_inst    = false;

        if (has_errors == false)
        {
            col.instantiate();
            has_errors_inst = eh->has_errors();
        }

        if (has_errors_inst == true)
        {
            for(long i = 0; i < n; ++i)
            {
                const key_type& key = group[i];
                auto pos            = m_build_type_iter.find(key);

                if (pos == m_build_type_iter.end())
                    continue;

                auto& def           = pos->second;
                def.set_error();
            };
        };

        if (eh->has_errors() == true)
            continue;

        //debugging
        for(long i = 0; i < n; ++i)
        {
            const key_type& key = group[i];
            auto pos            = m_build_type_iter.find(key);

            if (pos == m_build_type_iter.end())
                continue;

            auto& def           = pos->second;

            if (def.has_error() == true)
            {
                eh->add_error();
                continue;
            };

            const ast::kind& k2 = def.get_kind();
            const ast::type& t2 = def.get_def();

            if (sema::identity(ad::options::print_in_typecheck) == true)
            {
                std::cout << "\n";
                std::cout << key.get_name_simple().get_string();
                std::cout << " = ";
                sema::print_type_kind(std::cout, t2, k2);
                std::cout << "\n";
            }
        };
    };
};

const type_table::type_alias_map& type_table::get_alias_map() const
{
    return m_build_type_iter;
};
const graph& type_table::get_graph() const
{
    return m_build_graph;
}

void type_table::get_polarities(const ast::kind& k, ast::kind& body, std::vector<kind_polar>& ki_vec)
{
    body = k;

    for (;;)
    {
        if (body->get_kind_code() == ast::details::kind_code::kind_scope)
        {
            ast::kind_scope ak      = body.static_cast_to<ast::kind_scope>();
            body                    = ak.get_body();
            continue;
        };
        if (body->get_kind_code() == ast::details::kind_code::arrow_kind)
        {
            ast::arrow_kind ak      = body.static_cast_to<ast::arrow_kind>();        
            const ast::kind& lhs    = ak.get_lhs();
            body                    = ak.get_rhs();
            ast::polarity_type pol  = ak.get_type();

            ki_vec.push_back(kind_polar(lhs,pol));            
            continue;
        }
        else if (body->get_kind_code() == ast::details::kind_code::kind_var)
        {
            ast::kind_var kv        = body.static_cast_to<ast::kind_var>();

            if (kv.is_set() == true)
            {
                body                = kv.get();
                continue;
            }

            body                    = kv;
            break;
        }
        else
        {
            break;
        }
    };
};

ast::type type_table::make_type_def_body(const ast::type& t0, const ast::type_definition& def,
                                         const error_handler_ptr& eh)
{
    //perform decomposition t0 := Kind[args ...] t_body
    std::vector<ast::identifier> args;
    ast::type t_body;
    sema::decompose_type(t0, args, t_body);

    if (t_body->get_type_code() != ast::details::type_code::type_constant)
        return t0;

    ast::type_constant def_tc   = t_body.static_cast_to<ast::type_constant>();
    const ast::identifier& id   = def.get_identifier();
    const ast::kind& ki         = def.get_kind();
    bool is_extern              = def_tc.is_extern_func();

    ast::kind tail;
    std::vector<kind_polar> ki_vec;
    get_polarities(ki, tail, ki_vec);

    ast::type_constant nt       = ast::type_constant(id->get_location(), id, tail, is_extern);

    if (is_extern == true)
        nt.set_function(m_owner->get_extern_type_function_def(eh, id));

    long n                  = (long)ki_vec.size();

    ast::type body          = nt;
    ast::fixity_type ft     = ast::fixity_type::undefined;

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::ident);
    ik.set_sector(ast::identifier_kind::sector_type::type);
    ik.set_function(ast::identifier_kind::function_type::var);

    //build definition
    for (long i = n-1; i >= 0; --i)
    {
        std::ostringstream os;
        os << "a" << i + 1;

        ast::kind k_loc         = ki_vec[i].first;
        ast::identifier lab_id  = ast::identifier(id->get_location(), ast::unique_string(os.str()),
                                    ft, ik, ast::identifier::fresh());
        annot_id lab            = annot_id(id->get_location(), lab_id, k_loc);
        ast::type ty            = ast::identifier_type(lab_id->get_location(), lab_id);
        ast::polarity_type pol  = ki_vec[i].second;

        nt.add_label(lab, ty);
        
        body     = ast::lambda_type(id->get_location(), pol, lab, body);
    };    

    long nargs  = (long)args.size();

    //add kind scope
    for (long i = nargs-1; i >= 0; --i)
        body     = ast::type_kind_scope(id->get_location(), args[i], body);

    body    = sema::copy_identifiers(body);
    return body;
};

ast::type type_table::make_type_def_body(const ast::type& t, const ast::type_definition_var& def,
                                         ast::kind& def_k, const error_handler_ptr& eh)
{
    const ast::identifier& id   = def.get_identifier();
    const auto& kind_vars       = def.get_kind_vars();
    long n                      = def.var_size();

    ast::type body;
    ast::type_constant nt;
    bool is_const;    

    if (t->get_type_code() != ast::details::type_code::type_constant)
    {
        body                = t;
        is_const            = false;        
        if (!def_k)
            def_k           = ast::kind_var(id->get_location(), false);
    }
    else
    {
        ast::type_constant def_tc   = t.static_cast_to<ast::type_constant>();
        bool is_extern      = def_tc.is_extern_func();
        ast::kind ret_ki    = def_tc.get_return_kind();

        if (ret_ki)
            def_k           = ret_ki;
        else
            def_k           = ast::kind_var(id->get_location(), false);

        nt                  = ast::type_constant(id->get_location(), id, def_k, is_extern);
        body                = nt;
        is_const            = true;

        if (is_extern == true)
            nt.set_function(m_owner->get_extern_type_function_def(eh, id));
    };       

    using annot_type            = ast::annotated_type_identifier;

    for (long i = n - 1; i >= 0; --i)
    {
        const annot_type& at    = def.get_arg(i);
        ast::kind k             = at.get_kind();
        ast::polarity_type pol  = def.get_polarity(i);

        if (is_const == true)
        {
            ast::identifier lab_id  = at.get_identifier();
            annot_id lab            = annot_id(id->get_location(), lab_id, k);
            ast::type ty            = ast::identifier_type(lab_id->get_location(), lab_id);
            nt.add_label(lab, ty);
        };

        def_k                   = ast::arrow_kind(k->get_location(), pol, k, def_k);
        body                    = ast::lambda_type(at->get_location(), pol, at, body);
    };

    //add kind scopes
    long n_kind_vars        = (long)kind_vars.size();
    for (long i = n_kind_vars - 1; i >= 0; --i)
    {
        def_k   = ast::kind_scope(def_k->get_location(), kind_vars[i], def_k);
        body    = ast::type_kind_scope(body->get_location(), kind_vars[i], body);
    };

    def_k   = sema::copy_identifiers(def_k);
    body    = sema::copy_identifiers(body);
    return body;
};

void type_table::check_operator_type(ast::unique_string str, oper::operator_declaration& od,
                            loc_vec& ambig_locations) const
{
    m_owner->check_operator_type(str, od, true, ambig_locations);
};

bool type_table::is_any_operator(ast::unique_string id)
{
    return m_owner->is_any_operator(id, true);
};

bool type_table::is_shell_module() const
{
    return m_owner->is_shell_module();
}

bool type_table::is_overloaded(const ast::unique_string id) const
{
    return m_overloads.is_overloaded(id);
};

void type_table::finalize_new_overload_set(overload_set& os, ast::unique_string name, 
                        ast::fixity_type ft)
{
    if (os.is_overload() == false)
        return;

    m_owner->import_type_overloads(os, name, ft, true);

    // build def index
    if (ft == ast::fixity_type::undefined)
    {
        // overload set must have unique def index the same for
        // overload sets with different fixities; for overload sets
        // with fixities different than undenfined identifiers are the same
        // as for undefined fixity; therefor we must creare new def index
        // for undefined fixity only
        const ast::identifier& id   = os.get_identifier();
        int n_defs                  = get_new_def_index();
        def_index di    = def_index(m_index, get_typealias_scope_index(), n_defs);

        id.get_symbol_data()->get_kind().set_sector_type().set_function_overload();
        id.get_symbol_data()->set_definition(di);

        add_to_defmap(di.get_value(), fun_sym_info(false, fun_sym_iter()));
    };
};

void type_table::import_type_overloads(overload_set& os, ast::unique_string id, 
                        ast::fixity_type ft) const
{
    bool has;
    auto pos = m_overloads.get(id, ft, has);
    
    if (has == false)
        return;

    os.import_from(pos->second);
};

void type_table::add_to_defmap(int index, fun_sym_info it)
{
    if ((int)m_def_map.size() <= index)
        m_def_map.resize(2 * index + 1);

    m_def_map[index]  = it;
};

void type_table::add_instantiation_type(const ast::identifier_type& id_base, 
                        const ast::type_definition& td, const std::vector<ast::type>& args, 
                        bool true_inst)
{
    const ast::identifier& id   = td.get_identifier();
    const ast::kind& ki         = td.get_kind();
    const ast::type& body       = td.get_body();

    int n_defs      = get_new_def_index();
    def_index di    = def_index(m_index, get_typealias_scope_index(), n_defs);
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_type().set_function_def();
    id.get_symbol_data()->set_definition(di);

    type_alias_info sd(td->get_location(), id, ki, body, td.is_overload());
    sd.set_def_index(di, group);

    auto pos        = m_instantiations.add_type(id_base, id, std::move(sd), args, true_inst);

    add_to_defmap(di.get_value(), fun_sym_info(true, pos));
};

void type_table::add_instantiation_kind(const ast::identifier_type& id_base, 
                        const ast::type_definition& td, const std::vector<ast::kind>& args, 
                        bool true_inst)
{
    const ast::identifier& id   = td.get_identifier();
    const ast::kind& ki         = td.get_kind();
    const ast::type& body       = td.get_body();

    int n_defs      = get_new_def_index();
    def_index di    = def_index(m_index, get_typealias_scope_index(), n_defs);
    long group      = get_group_index();

    id.get_symbol_data()->get_kind().set_sector_type().set_function_def();
    id.get_symbol_data()->set_definition(di);

    type_alias_info sd(td->get_location(), id, ki, body, td.is_overload());
    sd.set_def_index(di, group);

    auto pos        = m_instantiations.add_kind(id_base, id, std::move(sd), args, true_inst);
    add_to_defmap(di.get_value(), fun_sym_info(true, pos));
};

ast::identifier type_table::get_instantiation_type(const ast::identifier_type& base_id, 
                        const std::vector<ast::type>& args, bool& has, type_comparer& comp,
                        long& num_matched)
{
    return m_instantiations.get_existing_type(base_id, args, has, comp, num_matched);
};
ast::identifier type_table::get_instantiation_kind(const ast::identifier_type& base_id, 
                        const std::vector<ast::kind>& args, bool& has, kind_comparer& comp,
                        long& num_matched)
{
    return m_instantiations.get_existing_kind(base_id, args, has, comp, num_matched);
}

};};
