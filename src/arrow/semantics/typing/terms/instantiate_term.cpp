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

#include "instantiate_term.h"
#include "arrow/ast/ast_visitor.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "scopes/local_scope.h"
#include "semantics/semantics.h"
#include "semantics/typing/terms/term_check_vis.h"
#include "error/error.h"
#include "overload_resolution.h"
#include "scopes/instantiation_table.h"
#include "instantiate_vis.h"
#include "semantics/utils.h"

#include <iostream>

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              inst_collector
//------------------------------------------------------------------
inst_collector::inst_collector(const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod), m_kind_checker(eh, mod), m_type_eq(&m_kind_checker, mod, eh, this)
    , m_kind_eq(eh), m_instanitiate_local(true)
{};

bool inst_collector::can_instantiate_local() const
{
    return m_instanitiate_local;
};

bool inst_collector::is_function_closure(const ast::term& ex) const
{
    if (ex->get_term_code() != ast::details::term_code::closure_term)
        return false;

    ast::closure_term ct    = ex.static_cast_to<ast::closure_term>();
    return ct.get_kind() == ast::closure_kind::function;
}

ast::term inst_collector::add_closure(const ast::term& def, const ast::term& def_with_closure)
{
    if (is_function_closure(def_with_closure) == false)
        return def;

    ast::closure_term ct    = def_with_closure.static_cast_to<ast::closure_term>();
    return ct.change_body_func(def);
};

ast::term inst_collector::make_term_type_application(const ast::term& ex, bool can_local)
{
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    std::vector<ast::type> args;
    ast::term head;

    collect_type_args(ex, args, head);

    if (args.size() == 0)
        return ex;

    return make_term_type(ex, head, 0, args.size(), args);
};

ast::type inst_collector::make_type_type_application(const ast::type& ex, bool can_local)
{
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    std::vector<ast::type> args;
    ast::type head;

    collect_type_args(ex, args, head);

    if (args.size() == 0)
        return ex;

    return make_type_type(ex, head, 0, args.size(), args);
};

ast::term inst_collector::make_term_kind_application(const ast::term& ex, bool can_local)
{    
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    std::vector<ast::kind> args;
    ast::term head;

    collect_kind_args(ex, args, head);

    if (args.size() == 0)
        return ex;

    return make_term_kind(ex, head, 0, args.size(), args);
};

ast::type inst_collector::make_type_kind_application(const ast::type& ex, bool can_local)
{    
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    std::vector<ast::kind> args;
    ast::type head;

    collect_kind_args(ex, args, head);

    if (args.size() == 0)
        return ex;

    return make_type_kind(ex, head, 0, args.size(), args);
};

ast::term inst_collector::make_term_type_subs(const ast::identifier& id, const ast::type& subs, 
                const ast::term& body, bool can_local)
{
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    instantiate_vis vis(m_eh, m_mod, this);
    vis.add_type(id, subs, true);

    return vis.make_term(body, can_instantiate_local());
};

ast::type inst_collector::instantiate_and_infer_kind(const ast::type& t, 
                const std::vector<details::applic_arg_term>& kind_args, bool show_errors,
                kind_subs_map& subs_map, bool& has_errors, bool can_instantiate_local)
{
    std::vector<ast::kind> args;

    for (size_t i = 0; i < kind_args.size(); ++i)
    {
        if (kind_args[i].get_item_type() == details::applic_arg_term::kind_arg)
        {
            args.push_back(kind_args[i].get_kind_arg());
        }
        else
        {
            break;
        };
    };

    return instantiate_and_infer_kind(t, args, show_errors, subs_map, has_errors,
                                      can_instantiate_local);
};

ast::type inst_collector::instantiate_and_infer_kind(const ast::type& t, 
                const std::vector<ast::kind>& kind_args, bool show_errors,
                kind_subs_map& subs_map, bool& has_errors, bool can_local)
{
    sema::scoped_var<bool> scope(m_instanitiate_local, can_local);

    typing::instantiate_kind vis(m_eh, show_errors);

    ast::type t2    = vis.make(t, kind_args, subs_map, has_errors, this);
    return t2;
};

ast::kind inst_collector::instantiate_and_infer_kind(const ast::kind& k, 
            const std::vector<details::applic_arg_type>& kind_args, bool show_error,
            kind_subs_map& subs_map, bool& error)
{
    std::vector<ast::kind> args;
    error = false;

    for (size_t i = 0; i < kind_args.size(); ++i)
    {
        if (kind_args[i].get_item_type() == details::applic_arg_term::kind_arg)
        {
            args.push_back(kind_args[i].get_kind_arg());
        }
        else
        {
            break;
        };
    };

    typing::instantiate_kind vis(m_eh, show_error);
    return vis.make(k, args, subs_map, error);
};

void inst_collector::collect_type_args(const ast::term& ex, std::vector<ast::type>& args,
                                    ast::term& head)
{
    if (ex->get_term_code() != ast::details::term_code::term_type_application)
    {
        head = ex;
        return;
    }

    ast::term_type_application app;
    app                     = ex.static_cast_to<ast::term_type_application>();

    const ast::term& lhs    = app.get_term();
    const ast::type& rhs    = app.get_type();

    collect_type_args(lhs, args, head);
    args.push_back(rhs);
};
void inst_collector::collect_type_args(const ast::type& ex, std::vector<ast::type>& args,
                                    ast::type& head)
{
    if (ex->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv    = ex.static_cast_to<ast::type_var>();
        if (tv.is_set())
            return collect_type_args(tv.get(), args, head);

        head = ex;
        return;
    };

    if (ex->get_type_code() != ast::details::type_code::type_type_application)
    {
        head = ex;
        return;
    }

    ast::type_type_application app;
    app                     = ex.static_cast_to<ast::type_type_application>();

    const ast::type& lhs    = app.get_type();
    const ast::type& rhs    = app.get_arg();

    collect_type_args(lhs, args, head);
    args.push_back(rhs);
};

void inst_collector::collect_kind_args(const ast::term& ex, std::vector<ast::kind>& args,
                                    ast::term& head)
{
    if (ex->get_term_code() != ast::details::term_code::term_kind_application)
    {
        head = ex;
        return;
    }

    ast::term_kind_application app;
    app                     = ex.static_cast_to<ast::term_kind_application>();

    const ast::term& lhs    = app.get_term();
    const ast::kind& rhs    = app.get_kind();

    collect_kind_args(lhs, args, head);
    args.push_back(rhs);
};

void inst_collector::collect_kind_args(const ast::type& ex, std::vector<ast::kind>& args,
                                    ast::type& head)
{
    if (ex->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv    = ex.static_cast_to<ast::type_var>();
        if (tv.is_set())
            return collect_kind_args(tv.get(), args, head);

        head = ex;
        return;
    };

    if (ex->get_type_code() != ast::details::type_code::type_kind_application)
    {
        head = ex;
        return;
    }

    ast::type_kind_application app;
    app                     = ex.static_cast_to<ast::type_kind_application>();

    const ast::type& lhs    = app.get_lhs();
    const ast::kind& rhs    = app.get_rhs();

    collect_kind_args(lhs, args, head);
    args.push_back(rhs);
};

ast::term inst_collector::make_term_type(const ast::term& root, const ast::term& ex_in, 
                size_t start_pos, size_t end_pos, const std::vector<ast::type>& args)
{
    ast::term head  = ex_in;

    if (is_function_closure(head) == true)
    {
        ast::closure_term cl    = head.static_cast_to<ast::closure_term>();
        const ast::term& body   = cl.get_body();
        ast::term ret           = make_term_type(root, body, start_pos, end_pos, args);
        return cl.change_body_func(ret);
    };

    instantiate_vis vis(m_eh, m_mod, this);

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_term_code() == ast::details::term_code::all_term)
        {
            ast::all_term lt            = head.static_cast_to<ast::all_term>();

            const ast::term& body       = lt.get_body();
            const ast::identifier& id   = lt.get_var().get_identifier();
            head                        = body;

            vis.add_type(id, args[i], true);
            ++i;
            continue;
        };

        if (vis.size_all() > 0)
        {
            head                        = vis.make_term(head, can_instantiate_local());
            vis.clear();
            continue;
        };

        bool has_errors     = false;

        if (head->get_term_code() == ast::details::term_code::identifier_term)
        {
            ast::identifier_term idt= head.static_cast_to<ast::identifier_term>();
            bool success; 
            
            ast::term ret           = instantiate_identifier_term_type(idt, i, end_pos, args, 
                                        success, has_errors);

            if (success == true)
                return ret;
        }
        else if (head->get_term_code() == ast::details::term_code::closure_term)
        {
            ast::closure_term cl    = head.static_cast_to<ast::closure_term>();
            bool success;
            ast::term ret           =  instantiate_closure_type(cl, i, end_pos, args, 
                                        success, has_errors);

            if (success == true)
                return ret;
        }

        return instantiate_general_term_type(head, i, end_pos, args, root);
    };

    return vis.make_term(head, can_instantiate_local());
};

ast::type inst_collector::make_type_type(const ast::type& root, const ast::type& ty, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args)
{
    ast::type head  = ty;

    instantiate_vis vis(m_eh, m_mod, this);

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_type_code() == ast::details::type_code::all_type)
        {
            ast::all_type lt            = head.static_cast_to<ast::all_type>();

            const ast::type& body       = lt.get_body();
            const ast::identifier& id   = lt.get_var().get_identifier();
            head                        = body;

            vis.add_type(id, args[i], true);
            ++i;
            continue;
        };

        if (head->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv            = head.static_cast_to<ast::type_var>();
            if (tv.is_set() == true)
            {
                head                    = tv.get();
                continue;
            }
        };

        if (vis.size_all() > 0)
        {
            head                        = vis.make_type(head, can_instantiate_local());
            vis.clear();
            continue;
        };

        bool has_errors     = false;

        if (head->get_type_code() == ast::details::type_code::identifier_type)
        {
            ast::identifier_type idt= head.static_cast_to<ast::identifier_type>();
            bool success; 
            
            ast::type ret           = instantiate_identifier_type_type(idt, i, end_pos, args, 
                                        success, has_errors);

            if (success == true)
                return ret;
        }

        return instantiate_general_type_type(head, i, end_pos, args, root);
    };

    return vis.make_type(head, can_instantiate_local());
};

ast::term inst_collector::make_term_kind(const ast::term& root, const ast::term& ex_in, 
                size_t start_pos, size_t end_pos, const std::vector<ast::kind>& args)
{
    ast::term head  = ex_in;

    if (is_function_closure(head) == true)
    {
        ast::closure_term cl    = head.static_cast_to<ast::closure_term>();
        const ast::term& body   = cl.get_body();
        ast::term ret           = make_term_kind(root, body, start_pos, end_pos, args);
        return cl.change_body_func(ret);
    };

    instantiate_vis vis(m_eh, m_mod, this);

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_term_code() == ast::details::term_code::term_kind_scope)
        {
            ast::term_kind_scope ts     = head.static_cast_to<ast::term_kind_scope>();

            const ast::term& body       = ts.get_body();
            const ast::identifier& id   = ts.get_var();
            head                        = body;

            vis.add_kind(id, args[i]);
            ++i;
            continue;
        };

        if (vis.size_all() > 0)
        {
            head                        = vis.make_term(head, can_instantiate_local());
            vis.clear();
            continue;
        };

        bool has_errors     = false;

        if (head->get_term_code() == ast::details::term_code::identifier_term)
        {
            ast::identifier_term idt    = head.static_cast_to<ast::identifier_term>();
            bool success; 
            ast::term ret               =  instantiate_identifier_term_kind(idt, i, 
                                            end_pos, args, success, has_errors);

            if (success == true)
                return ret;
        }
        else if (head->get_term_code() == ast::details::term_code::closure_term)
        {
            ast::closure_term cl        = head.static_cast_to<ast::closure_term>();
            bool success; 
            ast::term ret               = instantiate_closure_kind(cl, i, end_pos, args, 
                                            success, has_errors);

            if (success == true)
                return ret;
        }

        return instantiate_general_term_kind(head, i, end_pos, args, root);
    };

    return vis.make_term(head, can_instantiate_local());
};

ast::type inst_collector::make_type_kind(const ast::type& root, const ast::type& ty, size_t start_pos, 
                size_t end_pos, const std::vector<ast::kind>& args)
{
    ast::type head  = ty;

    instantiate_vis vis(m_eh, m_mod, this);

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_type_code() == ast::details::type_code::type_kind_scope)
        {
            ast::type_kind_scope ts     = head.static_cast_to<ast::type_kind_scope>();

            const ast::type& body       = ts.get_body();
            const ast::identifier& id   = ts.get_var();
            head                        = body;

            vis.add_kind(id, args[i]);
            ++i;
            continue;
        };

        if (head->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv            = head.static_cast_to<ast::type_var>();
            if (tv.is_set() == true)
            {
                head                    = tv.get();
                continue;
            }
        };

        if (vis.size_all() > 0)
        {
            head                        = vis.make_type(head, can_instantiate_local());
            vis.clear();
            continue;
        };

        if (head->get_type_code() == ast::details::type_code::identifier_type)
        {
            bool success, has_error;

            ast::identifier_type idt    = head.static_cast_to<ast::identifier_type>();
            ast::type ret               = instantiate_identifier_type_kind(idt, i, 
                                            end_pos, args, success, has_error);

            if (success == true)
                return ret;
        }

        return instantiate_general_type_kind(head, i, end_pos, args, root);
    };

    return vis.make_type(head, can_instantiate_local());
};

ast::kind inst_collector::make_kind_type(const ast::kind& root, const ast::kind& ty, size_t start_pos, 
                    size_t end_pos, const std::vector<ast::type>& args)
{
    if (start_pos == end_pos)
        return ty;

    (void)root;
    (void)args;

    ast::kind head  = ty;

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_kind_code() == ast::details::kind_code::star_all_kind)
        {
            ast::star_all_kind ts       = head.static_cast_to<ast::star_all_kind>();

            const ast::kind& body       = ts.get_body();
            head                        = body;

            ++i;
            continue;
        };

        if (head->get_kind_code() == ast::details::kind_code::kind_var)
        {
            ast::kind_var tv            = head.static_cast_to<ast::kind_var>();
            if (tv.is_set() == true)
            {
                head                    = tv.get();
                continue;
            }
        };

        // kind application must always succeed
        error::error_base_msg(m_eh).internal_error(head->get_location(), 
                                        "unable to make kind-type application");
        return head;
    };

    return head;
};

ast::kind inst_collector::make_kind_kind(const ast::kind& root, const ast::kind& ty, size_t start_pos, 
                    size_t end_pos, const std::vector<ast::kind>& args)
{
    if (start_pos == end_pos)
        return ty;

    (void)root;

    ast::kind head  = ty;

    instantiate_vis vis(m_eh, m_mod, this);

    for (size_t i = start_pos; i < end_pos;)
    {
        if (head->get_kind_code() == ast::details::kind_code::kind_scope)
        {
            ast::kind_scope ts          = head.static_cast_to<ast::kind_scope>();

            const ast::kind& body       = ts.get_body();
            const ast::identifier& id   = ts.get_var();
            head                        = body;

            vis.add_kind(id, args[i]);
            ++i;
            continue;
        };

        if (head->get_kind_code() == ast::details::kind_code::kind_var)
        {
            ast::kind_var tv            = head.static_cast_to<ast::kind_var>();
            if (tv.is_set() == true)
            {
                head                    = tv.get();
                continue;
            }
        };

        if (vis.size_all() > 0)
        {
            head                        = vis.make_kind(head, can_instantiate_local());
            vis.clear();
            continue;
        };

        // kind application must always succeed
        error::error_base_msg(m_eh).internal_error(head->get_location(), 
                                        "unable to make kind application");
        return head;
    };

    return vis.make_kind(head, can_instantiate_local());
};

ast::term inst_collector::instantiate_general_term_type(const ast::term& ex, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args, const ast::term& root)
{
    if (start_pos == 0 && end_pos == args.size() && root)
        return root;

    ast::term ret = ex;
    for (size_t i = start_pos; i < end_pos; ++i)
        ret = ast::term_type_application(args[i]->get_location(), ret, args[i]);

    return ret;
};
ast::type inst_collector::instantiate_general_type_type(const ast::type& ex, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args, const ast::type& root)
{
    if (start_pos == 0 && end_pos == args.size() && root)
        return root;

    ast::type ret = ex;
    for (size_t i = start_pos; i < end_pos; ++i)
        ret = ast::type_type_application(args[i]->get_location(), ret, args[i]);

    return ret;
};

ast::term inst_collector::instantiate_general_term_kind(const ast::term& ex, size_t start_pos, 
                size_t end_pos, const std::vector<ast::kind>& args, const ast::term& root)
{
    if (start_pos == 0 && end_pos == args.size() && root)
        return root;

    ast::term ret = ex;
    for (size_t i = start_pos; i < end_pos; ++i)
        ret = ast::term_kind_application(args[i]->get_location(), ret, args[i]);

    return ret;
};
ast::type inst_collector::instantiate_general_type_kind(const ast::type& ex, size_t start_pos, 
                size_t end_pos, const std::vector<ast::kind>& args, const ast::type& root)
{
    if (start_pos == 0 && end_pos == args.size() && root)
        return root;

    ast::type ret = ex;
    for (size_t i = start_pos; i < end_pos; ++i)
        ret = ast::type_kind_application(args[i]->get_location(), ret, args[i]);

    return ret;
};

ast::term inst_collector::instantiate_identifier_term_type(const ast::identifier_term& id, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args, bool& success, bool& has_errors)
{
    bool is_local;
    bool can_inst   = inst_collector::can_instantiate_term_symbol(id, m_mod, is_local);
    has_errors      = false;
    success         = false;

    if (is_local == true)
    {
        //local symbols must have closures
        error::error_base_msg(m_eh).internal_error(id->get_location(), 
                                        "local symbol not in a closure");
        has_errors  = true;
        return ast::error_term(id->get_location());
    };

    if (can_inst == false)
        return id;

    //only closed types can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool cl_kind, free_vars;
        bool cl = sema::is_closed_type(args[last_pos], cl_kind, free_vars);

        if (cl == false || cl_kind == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return id;

    bool overloaded     = m_mod.is_overloaded_term_def(id, has_errors);

    if (has_errors == true)
        return ast::error_term(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_errors  = true;
        return ast::error_term(id->get_location());
    };

    std::vector<ast::type> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_term id2 = register_term_type_instantiation(id, loc_args);
    return id2;
};

ast::type inst_collector::instantiate_identifier_type_type(const ast::identifier_type& id, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args, bool& success, bool& has_errors)
{
    bool can_inst   = inst_collector::can_instantiate_type_symbol(id, m_mod);
    has_errors      = false;
    success         = false;

    if (can_inst == false)
        return id;

    //only closed types can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool cl_kind, free_vars;
        bool cl = sema::is_closed_type(args[last_pos], cl_kind, free_vars);

        if (cl == false || cl_kind == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return id;

    bool overloaded     = m_mod.is_overloaded_type_def(id, has_errors);

    if (has_errors == true)
        return ast::error_type(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_errors  = true;
        return ast::error_type(id->get_location());
    };

    std::vector<ast::type> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_type id2 = register_type_type_instantiation(id, loc_args);
    return id2;
};

ast::term inst_collector::instantiate_closure_type(const ast::closure_term& ct, size_t start_pos, 
                size_t end_pos, const std::vector<ast::type>& args, bool& success, bool& has_errors)
{
    has_errors      = false;
    success         = false;

    // local function can be instantiated only when body of this
    // function does not contain free type variables; for simplicity
    // we require, that no type variables appears in current context
    bool can_inst   = can_instantiate_local();

    if (can_inst == false)
        return ct;

    const ast::term& body   = ct.get_body();
    ast::closure_kind kind  = ct.get_kind();
    
    if (kind == ast::closure_kind::overload)
        return ct;

    if (kind != ast::closure_kind::identifier)
    {
        error::error_base_msg(m_eh).internal_error
                    (ct->get_location(), "unexpected function closure");
        has_errors = true;
        return ast::error_term(ct->get_location());
    };

    ast::identifier_term id = body.static_cast_to<ast::identifier_term>();

    //only closed types can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool cl_kind, free_vars;
        bool cl = sema::is_closed_type(args[last_pos], cl_kind, free_vars);

        if (cl == false || cl_kind == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return ct;

    bool overloaded     = m_mod.is_overloaded_term_def(id, has_errors);

    if (has_errors == true)
        return ast::error_term(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_errors = true;
        return ast::error_term(id->get_location());
    };

    std::vector<ast::type> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_term id2 = register_term_type_instantiation(id, loc_args);
    return ct.change_body(id2);
};

ast::term inst_collector::instantiate_identifier_term_kind(const ast::identifier_term& id, 
            size_t start_pos, size_t end_pos, const std::vector<ast::kind>& args, bool& success,
            bool& has_error)
{
    bool is_local;
    success         = false;
    has_error       = false;
    bool can_inst   = inst_collector::can_instantiate_term_symbol(id, m_mod, is_local);

    if (is_local == true)
    {
        //local symbols must have closures
        error::error_base_msg(m_eh).internal_error(id->get_location(), 
                                        "local symbol not in a closure");
        has_error   = true;
        return ast::error_term(id->get_location());
    };

    if (can_inst == false)
    {
        //this should alredy be checked
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "cannot apply kinds to local variable");
        has_error   = true;
        return ast::error_term(id->get_location());
    };

    //only closed kinds can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool free_vars;
        bool cl = sema::is_closed_kind(args[last_pos], free_vars);

        if (cl == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return id;

    bool overloaded = m_mod.is_overloaded_term_def(id, has_error);

    if (has_error == true)
        return ast::error_term(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_error   = true;
        return ast::error_term(id->get_location());
    };

    std::vector<ast::kind> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_term id2 = register_term_kind_instantiation(id, loc_args);
    return id2;
};

ast::type inst_collector::instantiate_identifier_type_kind(const ast::identifier_type& id, 
            size_t start_pos, size_t end_pos, const std::vector<ast::kind>& args,
            bool& success, bool& has_error)
{
    bool can_inst   = inst_collector::can_instantiate_type_symbol(id, m_mod);
    success         = false;
    has_error       = false;

    if (can_inst == false)
    {
        //this should alredy be checked
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "cannot apply kinds to local variable");
        has_error   = true;
        return ast::error_type(id->get_location());
    };

    //only closed kinds can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool free_vars;
        bool cl = sema::is_closed_kind(args[last_pos], free_vars);

        if (cl == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return id;

    bool overloaded = m_mod.is_overloaded_type_def(id, has_error);

    if (has_error == true)
        return ast::error_type(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_error   = true;
        return ast::error_type(id->get_location());
    };

    std::vector<ast::kind> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_type id2 = register_type_kind_instantiation(id, loc_args);
    return id2;
};

ast::term inst_collector::instantiate_closure_kind(const ast::closure_term& ct, 
            size_t start_pos, size_t end_pos, const std::vector<ast::kind>& args,
            bool& success, bool& has_error)
{
    const ast::term& body   = ct.get_body();
    ast::closure_kind kind  = ct.get_kind();
    success                 = false;
    has_error               = false;

    if (kind == ast::closure_kind::overload)
        return ct;

    if (kind != ast::closure_kind::identifier)
    {
        error::error_base_msg(m_eh).internal_error
                    (ct->get_location(), "unexpected function closure");
        has_error = true;
        return ast::error_term(ct->get_location());
    };

    //only closed kinds can be applied
    size_t last_pos = start_pos;
    for (; last_pos < end_pos; ++last_pos)
    {
        bool free_vars;
        bool cl = sema::is_closed_kind(args[last_pos], free_vars);

        if (cl == false || free_vars == true)
            break;
    };

    if (last_pos == start_pos || last_pos != end_pos)
        return ct;

    ast::identifier_term id = body.static_cast_to<ast::identifier_term>();

    bool overloaded = m_mod.is_overloaded_term_def(id, has_error);

    if (has_error == true)
        return ast::error_term(id->get_location());

    if (overloaded == true)
    {
        error::error_base_msg(m_eh).internal_error
                    (id->get_location(), "unexpected overloaded symbol");
        has_error = true;
        return ast::error_term(id->get_location());
    };

    std::vector<ast::kind> loc_args;
    for (size_t i = start_pos; i < end_pos; ++i)
        loc_args.push_back(args[i]);

    success = true;
    ast::identifier_term id2 = register_term_kind_instantiation(id, loc_args);
    return ct.change_body(id2);
};

ast::term inst_collector::resolve_overload(const ast::identifier_term& id, 
                const std::vector<details::applic_arg_term>& args, const ast::type& ret_ty,
                subs_ptr& subs, const ast::overload_index* oi, bool& success)
{
    ast::term ret = details::term_check_vis::resolve_overload(id, args, ret_ty, m_mod, 
                                      m_eh, subs, oi, this, success);
    return ret;
};

ast::identifier_term 
inst_collector::register_term_type_instantiation(const ast::identifier_term& base_id,
                    const std::vector<ast::type>& args)
{
    ast::identifier id2     = sema::copy(base_id.get_identifier());

    queue_item item         = queue_item(new delayed_term_inst<ast::type>(id2, base_id, 
                                                args, m_instantiated));
    m_queue.push_back(item);
    return ast::identifier_term(base_id->get_location(), id2);
};

ast::identifier_type 
inst_collector::register_type_type_instantiation(const ast::identifier_type& base_id,
                    const std::vector<ast::type>& args)
{
    ast::identifier id2     = sema::copy(base_id.get_identifier());

    queue_item item         = queue_item(new delayed_type_inst<ast::type>(id2, base_id, 
                                                args, m_instantiated));
    m_queue.push_back(item);
    return ast::identifier_type(base_id->get_location(), id2);
};

ast::identifier_term 
inst_collector::register_term_kind_instantiation(const ast::identifier_term& base_id, 
                    const std::vector<ast::kind>& args)
{
    ast::identifier id2     = sema::copy(base_id.get_identifier());

    queue_item item         = queue_item(new delayed_term_inst<ast::kind>(id2, base_id, 
                                                args, m_instantiated));
    m_queue.push_back(item);
    return ast::identifier_term(base_id->get_location(), id2);
};

ast::identifier_type 
inst_collector::register_type_kind_instantiation(const ast::identifier_type& base_id, 
                    const std::vector<ast::kind>& args)
{
    ast::identifier id2     = sema::copy(base_id.get_identifier());

    //instantiate immediately
    delayed_type_inst<ast::kind> item(id2, base_id, args, m_instantiated);
    process_delayed_type_kind_inst(item, true);

    //queue_item item         = queue_item(new delayed_type_inst<ast::kind>(id2, base_id, 
    //                                            args, m_instantiated));    
    //m_queue.push_back(item);

    return ast::identifier_type(base_id->get_location(), id2);
};

bool inst_collector::is_closed(const std::vector<details::applic_arg_term>& args)
{
    for (const auto& elem : args)
    {
        bool cl_kind;
        bool is_cl;
        bool free_vars;
        
        auto it_type        = elem.get_item_type();
        
        if (it_type == details::applic_arg_term::function_arg)
        {
            is_cl           = sema::is_closed_type(elem.get_func_arg_type(), cl_kind, free_vars);
        }
        else if (it_type == details::applic_arg_term::inst_arg)
        {
            is_cl           = sema::is_closed_type(elem.get_inst_arg(), cl_kind, free_vars);
        }
        else
        {
            //this is kind arg; kind args should already be processed and are
            //impossible now
            is_cl           = false;
            cl_kind         = false;
            free_vars       = true;
        };

        if (is_cl == false || cl_kind == false || free_vars == true)
            return false;
    };

    return true;
};

bool inst_collector::can_instantiate_term_symbol(const ast::identifier_term& id, const module& mod,
                                            bool& is_local)
{
    term_symbol_kind sym_kind   = mod.get_term_symbol_kind(id);
    is_local                    = false;

    switch(sym_kind)
    {
        case term_symbol_kind::undefined:
            return false;

        case term_symbol_kind::overloaded_symbol:
            return true;
        case term_symbol_kind::global_symbol:
            return true;

        case term_symbol_kind::local_symbol:
        {
            is_local = true;
            return true;
        }

        case term_symbol_kind::pattern_var:
            // pattern symbols (i.e. lambda vars, case vars, etc.) do not have
            // assigned terms, therefore cannot be instantiated
            return false;

        default:
            return false;
    };
};

bool inst_collector::can_instantiate_type_symbol(const ast::identifier_type& id, const module& mod)
{
    type_symbol_kind sym_kind   = mod.get_type_symbol_kind(id);

    switch(sym_kind)
    {
        case type_symbol_kind::undefined:
            return false;

        case type_symbol_kind::overloaded_symbol:
            return true;
        case type_symbol_kind::global_symbol:
            return true;

        case type_symbol_kind::pattern_var:
            // pattern symbols (i.e. lambda vars, case vars, etc.) do not have
            // assigned terms, therefore cannot be instantiated
            return false;

        default:
            return false;
    };
};

void inst_collector::clear_instantiations()
{
    m_queue.clear();
};

void inst_collector::instantiate()
{
    for (auto& item : m_queue)
        item->set_process(true);

    using term_kind_item = delayed_term_inst<ast::kind>;
    using term_type_item = delayed_term_inst<ast::type>;
    using type_kind_item = delayed_type_inst<ast::kind>;
    using type_type_item = delayed_type_inst<ast::type>;

    // process items added during instantiation of root terms
    while(m_queue.empty() == false)
    {
        const queue_item& item  = m_queue.front();

        if (item->process() == false)
            break;

        bool is_term;
        bool is_kind    = item->is_kind(is_term);
        if (is_kind == true)
        {
            if (is_term == true)
            {
                term_kind_item* it   = static_cast<term_kind_item*>(item.get());
                process_delayed_term_kind_inst(*it, true);
            }
            else
            {
                type_kind_item* it   = static_cast<type_kind_item*>(item.get());
                process_delayed_type_kind_inst(*it, true);
            };
        }
        else
        {
            if (is_term == true)
            {
                term_type_item* it   = static_cast<term_type_item*>(item.get());
                process_delayed_term_type_inst(*it, true);
            }
            else
            {
                type_type_item* it   = static_cast<type_type_item*>(item.get());
                process_delayed_type_type_inst(*it, true);
            };
        };       
        
        m_queue.pop_front();
    };

    // process items directly referenced in instantiated functions
    // we cannot instantiate previously instantiated symbols with
    // different arguments (this may happen only for recursive symbols)
    while(m_queue.empty() == false)
    {
        const queue_item& item  = m_queue.front();
        bool is_term;
        bool is_kind    = item->is_kind(is_term);
        if (is_kind == true)
        {
            if (is_term == true)
            {
                term_kind_item* it   = static_cast<term_kind_item*>(item.get());
                process_delayed_term_kind_inst(*it, false);
            }
            else
            {
                type_kind_item* it   = static_cast<type_kind_item*>(item.get());
                process_delayed_type_kind_inst(*it, false);
            };
        }
        else
        {
            if (is_term == true)
            {
                term_type_item* it   = static_cast<term_type_item*>(item.get());
                process_delayed_term_type_inst(*it, false);
            }
            else
            {
                type_type_item* it   = static_cast<type_type_item*>(item.get());
                process_delayed_type_type_inst(*it, false);
            };
        };       
        
        m_queue.pop_front();
    };
};

struct type_comparer_impl : ad::type_comparer
{
    using context_type      = details::kind_inference_context;

    type_equaler&           m_type_eq;
    context_type&           m_ctx;
    bound_map&              m_bound_vars;

    type_comparer_impl(type_equaler& ty_eq, context_type& ctx, bound_map& bm)
        : m_type_eq(ty_eq), m_ctx(ctx), m_bound_vars(bm)
    {};

    virtual bool test(const ast::type& t1, const ast::type& t2) override
    {
        bool show_error = false;
        ast::location loc;

        //sema::print_type(std::cout, t1); std::cout << "\n";
        //sema::print_type(std::cout, t2); std::cout << "\n";

        return m_type_eq.unify_types(loc, t1, t2, m_bound_vars, m_ctx, show_error);
    }
};

struct kind_comparer_impl : ad::kind_comparer
{
    unifier_kinds&          m_kind_eq;

    kind_comparer_impl(unifier_kinds& ki_eq)
        : m_kind_eq(ki_eq)
    {};

    virtual bool test(const ast::kind& t1, const ast::kind& t2) override
    {
        bool show_error = false;
        ast::location loc;

        //sema::print_type(std::cout, t1); std::cout << "\n";
        //sema::print_type(std::cout, t2); std::cout << "\n";

        return m_kind_eq.unify_kinds(loc, t1, t2, show_error);
    }
};

void inst_collector::close_arg_types(const close_type_fun& fun)
{
    using term_type_item = delayed_term_inst<ast::type>;
    using type_type_item = delayed_type_inst<ast::type>;

    for (auto& elem : m_queue)
    {
        bool is_term;
        if (elem->is_kind(is_term) == true)
            continue;

        if (is_term == true)
        {
            term_type_item* it   = static_cast<term_type_item*>(elem.get());

            std::vector<ast::type>& args    = it->m_args;

            for (auto& ty : args)
                ty = fun(ty);
        }
        else
        {
            type_type_item* it   = static_cast<type_type_item*>(elem.get());

            std::vector<ast::type>& args    = it->m_args;

            for (auto& ty : args)
                ty = fun(ty);
        };
    };
};

void inst_collector::close_arg_kinds(const close_kind_fun& fun)
{
    using term_kind_item = delayed_term_inst<ast::kind>;
    using type_kind_item = delayed_type_inst<ast::kind>;

    for (auto& elem : m_queue)
    {
        bool is_term;
        if (elem->is_kind(is_term) == false)
            continue;

        if (is_term == true)
        {
            term_kind_item* it   = static_cast<term_kind_item*>(elem.get());

            std::vector<ast::kind>& args    = it->m_args;

            for (auto& ty : args)
                ty = fun(ty);
        }
        else
        {
            type_kind_item* it   = static_cast<type_kind_item*>(elem.get());

            std::vector<ast::kind>& args    = it->m_args;

            for (auto& ty : args)
                ty = fun(ty);
        };
    };
};

void inst_collector::process_delayed_term_type_inst(delayed_term_inst<ast::type>& item, bool must_inst)
{
    const ast::identifier&  new_id      = item.m_new_id;
    ast::identifier_term base_id        = item.m_base_id;
    m_instantiated                      = item.get_instantiated();

    bool overloaded;
    
    bool has_errors1, has_errors2;
    ast::type id_ty                     = m_mod.get_symbol_type(m_eh, base_id, overloaded,
                                            has_errors1);
    ast::term id_term                   = m_mod.get_symbol_term(m_eh, base_id, overloaded,
                                            has_errors2);

    if (has_errors1 || has_errors2)
        return;

    const std::vector<ast::type>& args  = item.m_args;

    bool has_inst   = item.has_instantiation(base_id.get_identifier());
    m_instantiated.insert(base_id.get_identifier());

    //all types are closed; we can use empty context;
    context_type ctx;
    bound_map bm;
    type_comparer_impl comparer(m_type_eq, ctx, bm);

    bool has;
    long num_matched;
    ast::identifier inst    = m_mod.get_instantiation_term_type(base_id, args, 
                                has, comparer, num_matched);

    if (has == true)
    {
        new_id.get_symbol_data()->set_definition(inst.get_symbol_data()->get_def_index());
        return;
    };

    if (num_matched > 0)
    {
        base_id             = ast::identifier_term(inst->get_location(), inst);
        id_ty               = m_mod.get_symbol_type(m_eh, base_id, overloaded, has_errors1);
        id_term             = m_mod.get_symbol_term(m_eh, base_id, overloaded, has_errors2);
    };

    if (has_errors1 || has_errors2)
        return;

    ast::type subs_ty       = id_ty;
    subs_ty                 = make_type_type(subs_ty, subs_ty, num_matched, args.size(), args);

    if (must_inst == false && has_inst == true)
    {
        //do not instantiate recursive symbols

        //make type application
        ast::term applic    = add_closure(base_id, id_term);
        applic              = instantiate_general_term_type(applic, num_matched, args.size(),
                                args, ast::term());
        applic              = add_closure(applic, id_term);

        //sema::print_term(std::cout, applic, m_eh, m_mod); std::cout << "\n";

        ast::term_definition td(base_id->get_location(), new_id,  subs_ty, applic, false);
        m_mod.add_instantiation_term_type(base_id, td, args, false);
        return;
    };

    ast::term subs_term = make_term_type(ast::term(), id_term, num_matched, args.size(), args);    

    //sema::print_term(std::cout, subs_term, m_eh, m_mod); std::cout << "\n";

    ast::term_definition td(base_id->get_location(), new_id,  subs_ty, subs_term, false);
    m_mod.add_instantiation_term_type(base_id, td, args, true);
    return;
};

void inst_collector::process_delayed_type_type_inst(delayed_type_inst<ast::type>& item, bool must_inst)
{
    const ast::identifier&  new_id      = item.m_new_id;
    ast::identifier_type base_id        = item.m_base_id;
    m_instantiated                      = item.get_instantiated();

    bool overloaded;
    
    bool has_errors1, has_errors2;
    ast::kind id_ki                     = m_mod.get_type_constructor_kind(m_eh, base_id, overloaded,
                                            has_errors1);
    ast::type id_def                    = m_mod.get_type_constructor_definition(m_eh, base_id, overloaded,
                                            has_errors2);

    if (has_errors1 || has_errors2)
        return;

    const std::vector<ast::type>& args  = item.m_args;

    bool has_inst   = item.has_instantiation(base_id.get_identifier());
    m_instantiated.insert(base_id.get_identifier());

    //all types are closed; we can use empty context;
    context_type ctx;
    bound_map bm;
    type_comparer_impl comparer(m_type_eq, ctx, bm);

    bool has;
    long num_matched;
    ast::identifier inst    = m_mod.get_instantiation_type_type(base_id, args, 
                                has, comparer, num_matched);

    if (has == true)
    {
        new_id.get_symbol_data()->set_definition(inst.get_symbol_data()->get_def_index());
        return;
    };

    if (num_matched > 0)
    {
        base_id             = ast::identifier_type(inst->get_location(), inst);
        id_ki               = m_mod.get_type_constructor_kind(m_eh, base_id, overloaded, has_errors1);
        id_def              = m_mod.get_type_constructor_definition(m_eh, base_id, overloaded, has_errors2);
    };

    if (has_errors1 || has_errors2)
        return;

    ast::kind subs_ki       = id_ki;
    subs_ki                 = make_kind_type(subs_ki, subs_ki, num_matched, args.size(), args);

    if (must_inst == false && has_inst == true)
    {
        //do not instantiate recursive symbols

        //make type application
        ast::type applic    = base_id;
        applic              = instantiate_general_type_type(applic, num_matched, args.size(),
                                args, ast::type());

        //sema::print_term(std::cout, applic, m_eh, m_mod); std::cout << "\n";

        ast::type_definition td(base_id->get_location(), new_id,  subs_ki, applic, false);
        m_mod.add_instantiation_type_type(base_id, td, args, false);
        return;
    };

    ast::type subs_def = make_type_type(ast::type(), id_def, num_matched, args.size(), args);    

    //sema::print_term(std::cout, subs_term, m_eh, m_mod); std::cout << "\n";

    ast::type_definition td(base_id->get_location(), new_id,  subs_ki, subs_def, false);
    m_mod.add_instantiation_type_type(base_id, td, args, true);
    return;
};

void inst_collector::process_delayed_term_kind_inst(delayed_term_inst<ast::kind>& item, bool must_inst)
{
    const ast::identifier&  new_id      = item.m_new_id;
    ast::identifier_term base_id        = item.m_base_id;
    m_instantiated                      = item.get_instantiated();
    bool overloaded;

    bool has_err1, has_err2;

    ast::type id_ty                     = m_mod.get_symbol_type(m_eh, base_id, overloaded, has_err1);
    ast::term id_term                   = m_mod.get_symbol_term(m_eh, base_id, overloaded, has_err2);

    if (has_err1 || has_err2)
        return; 

    const std::vector<ast::kind>& args  = item.m_args;

    bool has_inst   = item.has_instantiation(base_id.get_identifier());
    m_instantiated.insert(base_id.get_identifier());

    //all types are closed; we can use empty context;
    kind_comparer_impl comparer(m_kind_eq);

    bool has;
    long num_matched;
    ast::identifier inst    = m_mod.get_instantiation_term_kind(base_id, args, has, 
                                comparer, num_matched);

    if (has == true)
    {
        new_id.get_symbol_data()->set_definition(inst.get_symbol_data()->get_def_index());
        return;
    };

    if (num_matched > 0)
    {
        base_id             = ast::identifier_term(inst->get_location(), inst);
        id_ty               = m_mod.get_symbol_type(m_eh, base_id, overloaded, has_err1);
        id_term             = m_mod.get_symbol_term(m_eh, base_id, overloaded, has_err2);
    };

    if (has_err1 || has_err2)
        return;

    ast::type subs_ty       = id_ty;
    subs_ty                 = make_type_kind(subs_ty, subs_ty, num_matched, args.size(), args);

    if (must_inst == false && has_inst == true)
    {
        //do not instantiate recursive symbols

        //make kind application
        ast::term applic    = add_closure(base_id, id_term);
        applic              = instantiate_general_term_kind(applic, num_matched, args.size(), 
                                args, ast::term());
        applic              = add_closure(applic, id_term);

        ast::term_definition td(base_id->get_location(), new_id,  subs_ty, applic, false);
        m_mod.add_instantiation_term_kind(base_id, td, args, false);
        return;
    };

    ast::term subs_term = make_term_kind(ast::term(), id_term, num_matched, args.size(), args);    

    ast::term_definition td(base_id->get_location(), new_id,  subs_ty, subs_term, false);
    m_mod.add_instantiation_term_kind(base_id, td, args, true);

    return;
};

void inst_collector::process_delayed_type_kind_inst(delayed_type_inst<ast::kind>& item, bool must_inst)
{
    const ast::identifier&  new_id      = item.m_new_id;
    ast::identifier_type base_id        = item.m_base_id;
    m_instantiated                      = item.get_instantiated();
    bool overloaded;

    bool has_err1, has_err2;

    ast::kind id_ki                     = m_mod.get_type_constructor_kind(m_eh, base_id, overloaded,
                                            has_err1);
    ast::type id_def                    = m_mod.get_type_constructor_definition(m_eh, base_id, overloaded,
                                            has_err2);

    if (has_err1 || has_err2)
        return; 

    const std::vector<ast::kind>& args  = item.m_args;

    bool has_inst   = item.has_instantiation(base_id.get_identifier());
    m_instantiated.insert(base_id.get_identifier());

    //all types are closed; we can use empty context;
    kind_comparer_impl comparer(m_kind_eq);

    bool has;
    long num_matched;
    ast::identifier inst    = m_mod.get_instantiation_type_kind(base_id, args, has, 
                                comparer, num_matched);

    if (has == true)
    {
        new_id.get_symbol_data()->set_definition(inst.get_symbol_data()->get_def_index());
        return;
    };

    if (num_matched > 0)
    {
        base_id             = ast::identifier_type(inst->get_location(), inst);
        id_ki               = m_mod.get_type_constructor_kind(m_eh, base_id, overloaded, has_err1);
        id_def              = m_mod.get_type_constructor_definition(m_eh, base_id, overloaded, has_err2);
    };

    if (has_err1 || has_err2)
        return;

    ast::kind subs_ki       = id_ki;
    subs_ki                 = make_kind_kind(subs_ki, subs_ki, num_matched, args.size(), args);

    if (must_inst == false && has_inst == true)
    {
        //do not instantiate recursive symbols

        //make kind application
        ast::type applic    = base_id;
        applic              = instantiate_general_type_kind(applic, num_matched, args.size(), 
                                args, ast::type());

        ast::type_definition td(base_id->get_location(), new_id,  subs_ki, applic, false);
        m_mod.add_instantiation_type_kind(base_id, td, args, false);
        return;
    };

    ast::type subs_def = make_type_kind(ast::type(), id_def, num_matched, args.size(), args);    

    ast::type_definition td(base_id->get_location(), new_id,  subs_ki, subs_def, false);
    m_mod.add_instantiation_type_kind(base_id, td, args, true);

    return;
};

};};