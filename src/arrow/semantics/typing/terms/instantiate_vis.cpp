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

#include "instantiate_vis.h"
#include "error/error.h"
#include "overload_resolution.h"
#include "semantics/semantics.h"
#include "semantics/typing/types/instantiate_type_kind.h"
#include "semantics/utils.h"
#include "semantics/semantics.h"
#include "scopes/local_def_substitution.h"

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//              instantiate_vis
//------------------------------------------------------------------
instantiate_vis::instantiate_vis(const error_handler_ptr& eh, const module& mod,
                    inst_collector* owner)
    :m_subs(eh, mod), m_eh(eh), m_mod(mod), m_owner(owner)
    ,m_can_inst_local(true)
{};

void instantiate_vis::add_type(const ad::var_info& var, const ast::type& ty, 
                             bool make_fresh_binders)
{
    m_subs.add_type(var, ty, make_fresh_binders);
};

void instantiate_vis::add_kind(const ad::var_info& var, const ast::kind& ty)
{
    m_subs.add_kind(var, ty);
};

ast::term instantiate_vis::make_term(const ast::term& body, bool can_local)
{
    if (m_subs.size_all() == 0)
        return body;

    sema::scoped_var<bool> scope(m_can_inst_local, can_local);
    return visit(body);
}

ast::type instantiate_vis::make_type(const ast::type& body, bool can_local)
{
    if (m_subs.size_all() == 0)
        return body;

    sema::scoped_var<bool> scope(m_can_inst_local, can_local);
    return visit(body);
}

ast::kind instantiate_vis::make_kind(const ast::kind& body, bool can_local)
{
    if (m_subs.size_all() == 0)
        return body;

    sema::scoped_var<bool> scope(m_can_inst_local, can_local);
    return visit(body);
};

void instantiate_vis::clear()
{
    m_subs.clear();
};

long instantiate_vis::size_all() const
{
    return m_subs.size_all();
};

ast::term instantiate_vis::eval(const ast::overloaded_symbol& ast)
{
    //overloaded symbol without application context is not allowed

    error::error_base_msg(m_eh).internal_error(ast->get_location(), "unexpected overloaded symbol");
    return ast::error_term(ast->get_location());
};

ast::term instantiate_vis::eval(const ast::closure_term& ast)
{
    const ast::term& body   = ast.get_body();
    long n                  = ast.size();

    ast::term body2         = visit(body);

    bool modified           = body2 != body;

    ast::closure_term ret(ast->get_location(), body2, ast.get_kind());

    for (long i = 0; i < n; ++i)
    {
        const ast::term& s          = ast.get_substitution(i);
        const ast::identifier& id   = ast.get_internal_ident(i);
        const ast::type& ty         = ast.get_type(i);

        ast::term s2                = visit(s);
        ast::type ty2               = visit(ty);

        modified                    |= s2 != s;
        modified                    |= ty2 != ty;

        ret.add_item(s2, id, ty2);
    };

    if (modified == false)
        return ast;
    else
        return ret;
};

ast::term instantiate_vis::eval(const ast::identifier_term& ast)
{
    auto pos = m_name_subs.find(ast.get_identifier());

    if (pos != m_name_subs.end())
        return ast::identifier_term(ast->get_location(), pos->second);
    else
        return ast;
};

ast::term instantiate_vis::eval_closure(const ast::closure_term& ast, 
            const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos,
            bool& success)
{
    const ast::term& body   = ast.get_body();
    ast::closure_kind kind  = ast.get_kind();
    
    if (kind == ast::closure_kind::function)
    {
        error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                                "unexpected function closure");
        success = false;
        return ast::error_term(ast->get_location());
    };

    if (kind != ast::closure_kind::overload)
    {
        success = false;
        return ast;
    };

    ast::overloaded_symbol os = body.static_cast_to<ast::overloaded_symbol>();
    return eval_overloaded(os, applic_args, last_pos, ast);
};

ast::term instantiate_vis::eval_overloaded(const ast::overloaded_symbol& ast, 
            const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos)
{
    return eval_overloaded(ast, applic_args, last_pos, ast::closure_term());
}

ast::term instantiate_vis::eval_overloaded(const ast::overloaded_symbol& ast, 
            const std::vector<details::applic_arg_term>& applic_args, size_t& last_pos,
            const ast::closure_term& ct)
{    
    last_pos                        = 0;
    const ast::identifier_term& idt = ast.get_identifier();
    const ast::type& ty             = ast.get_type();
    const ast::type& ret_ty         = ast.get_return_type();
    long over_size                  = ast.arg_size();
    long applic_size                = (long)applic_args.size();
    bool is_local_func              = ct != ast::closure_term();

    if (over_size > applic_size)
    {
        error::error_base_msg(m_eh).internal_error
                (ast->get_location(), "invalid application for overloaded symbol");
        return ast::error_term(ast->get_location());
    };

    /// first over_size arguments should be the same (after type and kind applications)

    ast::type ty2                   = visit(ty);
    ast::type ret_ty2               = visit(ret_ty);    

    std::vector<details::applic_arg_term> subs_args;
    bool modified;
    instantiate_overload_arguments(subs_args, modified, ast);

    if (modified == false && ty2 == ty && ret_ty2 == ret_ty)
    {
        if (is_local_func == false)
            return ast;
        else
            return ct;
    };

    const ast::overload_index* oi   = ast.get_index();
    bool must_resolve               = check_must_resolve(subs_args, is_local_func);

    if (must_resolve == false)
    {
        // add applications

        ast::overloaded_symbol ret(ast->get_location(), idt, ty2, ret_ty2, oi);

        for (long i = 0; i < over_size; ++i)
        {
            if (subs_args[i].get_item_type() == details::applic_arg_term::function_arg)
                ret.add_term_arg(subs_args[i].get_func_arg_type());
            else if (subs_args[i].get_item_type() == details::applic_arg_term::inst_arg)
                ret.add_type_arg(subs_args[i].get_inst_arg());
            else
                ret.add_kind_arg(subs_args[i].get_kind_arg());
        }

        if (is_local_func == false)
            return ret;
        else
            return ct.change_body(ret);
    };
    
    using subs_ptr      = std::shared_ptr<infered_substitutions>;
    subs_ptr subs;

    bool success;
    ast::term ret       = m_owner->resolve_overload(idt, subs_args, ret_ty2, 
                            subs, oi, success);

    if (m_eh->has_errors())
        return ast::error_term(ast->get_location());

    if (is_local_func == true)
        ret             = ct.change_body(ret, m_eh);

    if (success == false)
        return ret;

    using item_type = details::applic_arg_term::item_type;    

    // process first over_size arguments; remaining args will be
    // processed later

    //add missing type applications
    using vars_subs     = std::shared_ptr<var_substitution>;
    using kind_subs     = std::shared_ptr<kind_subs_map>;

    kind_subs ksubs     = subs->get_kind_substitution();
    long n_subs         = ksubs->number_substitutions();    

    //process explicit kind applications
    for (long i = 0; i < n_subs; ++i)
    {
        if (i >= over_size
            || subs_args[i].get_item_type() != details::applic_arg_term::kind_arg)
        {
            error::error_base_msg(m_eh).internal_error
                    (ast->get_location(), "invalid application for overloaded symbol");
            return ast::error_term(ast->get_location());
        };

        const ast::kind& arg_ty = subs_args[i].get_kind_arg();
        ret             = ast::term_kind_application(ast->get_location(), ret, arg_ty);
    };

    // add infered kind applications
    ret                 = ksubs->make_kind_application(ret);
    ret                 = m_owner->make_term_kind_application(ret, can_instantiate_local());    

    for (long i = n_subs; i < over_size; ++i)
    {
        item_type it1   = subs_args[i].get_item_type();
        item_type it2   = applic_args[i].get_item_type();

        if (it1 != it2)
        {
            error::error_base_msg(m_eh).internal_error
                    (ast->get_location(), "invalid application for overloaded symbol");
            return ast::error_term(ast->get_location());
        }

        if (it1 == details::applic_arg_term::function_arg)
        {            
            vars_subs loc_subs  = subs->get_term_substitution(i);

            if (!loc_subs)
            {
                error::error_base_msg(m_eh).internal_error
                        (ast->get_location(), "missing var substitution");
                return ast::error_term(ast->get_location());
            };

            const ast::term& arg= applic_args[i].get_func_arg();

            ret                 = loc_subs->make_type_application(ret);
            ret                 = m_owner->make_term_type_application(ret, can_instantiate_local());
            ret                 = ast::term_application(applic_args[i].get_location(), 
                                        ret, arg);
            ret                 = loc_subs->generalize(ret);
        }
        else if (it1 == details::applic_arg_term::inst_arg)
        {
            const ast::type& arg_ty = applic_args[i].get_inst_arg();
            ret                 = ast::term_type_application(ast->get_location(), ret, arg_ty);
            ret                 = m_owner->make_term_type_application(ret, can_instantiate_local());
        }
        else if (it1 == details::applic_arg_term::kind_arg)
        {
            const ast::kind& arg_ty = applic_args[i].get_kind_arg();
            ret                 = ast::term_kind_application(ast->get_location(), ret, arg_ty);
            ret                 = m_owner->make_term_kind_application(ret, can_instantiate_local());
        }

        ++last_pos;
    };

    return ret;
};

void instantiate_vis::instantiate_overload_arguments(std::vector<details::applic_arg_term>& subs_args,
                                        bool& modified, const ast::overloaded_symbol& ast)
{
    using arg_class                 = ast::overloaded_symbol::arg_class;

    long over_size                  = ast.arg_size();

    subs_args.reserve(over_size);
    modified                        = false;    

    //instantiate arguments
    for (long i = 0; i < over_size; ++i)
    {
        arg_class arg_cl            = ast.get_arg_class(i);

        if (arg_cl == arg_class::class_kind)
        {
            const ast::kind& arg_ty = ast.get_arg_kind(i);
            ast::kind arg_ty2       = visit(arg_ty);
            modified                |= arg_ty2 != arg_ty;

            subs_args.push_back(details::applic_arg_term::make_kind(arg_ty2));
        }
        else
        {
            const ast::type& arg_ty = ast.get_arg_type(i);
            ast::type arg_ty2       = visit(arg_ty);
            modified                |= arg_ty2 != arg_ty;

            if (arg_cl == arg_class::class_term)
                subs_args.push_back(details::applic_arg_term::make_func_arg_type(arg_ty2));
            else
                subs_args.push_back(details::applic_arg_term::make_inst(arg_ty2));
        };
    };    
};

ast::term instantiate_vis::eval_application(const ast::term& ast, std::vector<details::applic_arg_term>& args,
                ast::term& head)
{
    bool modified       = false;
    size_t i            = 0;

    ast::term head2;

    if (head->get_term_code() == ast::details::term_code::overloaded_symbol)
    {
        ast::overloaded_symbol os   = head.static_cast_to<ast::overloaded_symbol>();
        head2           = eval_overloaded(os, args, i);
        modified        |= head2 != head;
    }
    else if (head->get_term_code() == ast::details::term_code::closure_term)
    {
        ast::closure_term os   = head.static_cast_to<ast::closure_term>();
        bool success;
        head2           = eval_closure(os, args, i, success);

        if (success == false)
            head2       = visit(head);

        modified        |= head2 != head;
    }
    else
    {
        head2           = visit(head);
        modified        |= head2 != head;
    };

    ast::term ret       = head2;
    bool can_instant    = true;

    for (;i < args.size(); ++i)
    {
        details::applic_arg_term& arg   = args[i];

        if (arg.get_item_type() == details::applic_arg_term::function_arg)
        {
            can_instant             = false;
            const ast::term& ex     = arg.get_func_arg();
            ast::term ex2           = visit(ex);
            modified                |= ex2 != ex;

            ret                     = ast::term_application(arg.get_location(), ret, ex2);
        }
        else if (arg.get_item_type() == details::applic_arg_term::inst_arg)
        {
            const ast::type& ty     = arg.get_inst_arg();
            ast::type ty2           = visit(ty);
            modified                |= ty2 != ty;

            ret                     = ast::term_type_application(ast->get_location(), ret, ty2);

            if (can_instant == true && modified == true)
            {
                ast::term ret2      = m_owner->make_term_type_application(ret, can_instantiate_local());
                modified            |= ret2 != ret;
                ret                 = ret2;
            };
        }
        else if (arg.get_item_type() == details::applic_arg_term::kind_arg)
        {
            const ast::kind& ty     = arg.get_kind_arg();
            ast::kind ty2           = visit(ty);
            modified                |= ty2 != ty;

            ret                     = ast::term_kind_application(ast->get_location(), ret, ty2);

            if (can_instant == true && modified == true)
            {
                ast::term ret2      = m_owner->make_term_kind_application(ret, can_instantiate_local());
                modified            |= ret2 != ret;
                ret                 = ret2;
            };
        };
    }

    if (modified == false)
        return ast;
    else
        return ret;
};

ast::term instantiate_vis::eval(const ast::term_type_application& ast)
{
    std::vector<details::applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head);
};

ast::term instantiate_vis::eval(const ast::term_kind_application& ast)
{
    std::vector<details::applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head);
};

ast::term instantiate_vis::eval(const ast::term_application& ast)
{
    std::vector<details::applic_arg_term> args;
    ast::term head;
    sema::uncurry_application(ast, args, head);

    return eval_application(ast, args, head);
};

ast::term instantiate_vis::eval(const ast::let_term& ast)
{
    //we dont need scopes anymore
    const ast::term& ex             = ast.get_body();
    const ast::definitions& defs    = ast.get_definitions();

    ast::definitions defs2          = make_fresh_function_names(defs);
    ast::term ex2                   = visit(ex);
            
    if (ex2 == ex && defs2 == defs)
        return ast;
    else
        return ast::let_term(ast->get_location(), defs2, ex2, ast.get_symbol_data());
}

ast::term instantiate_vis::eval(const ast::mu_term& ast)
{
    //we dont need scopes anymore
    return base_type::eval_mu(ast);
};

ast::term instantiate_vis::eval(const ast::lambda_term& ast)
{
    //we dont need scopes anymore
    return base_type::eval_lambda(ast);
};

ast::term instantiate_vis::eval(const ast::unpack_term& ast)
{
    sema::scoped_var<bool> scope_loc(m_can_inst_local, false);

    //we dont need scopes anymore
    return base_type::eval_unpack(ast);
};

ast::term instantiate_vis::eval(const ast::variant_case_term& ast)
{
    //we dont need scopes anymore
    return base_type::eval_case(ast);
};

ast::term instantiate_vis::eval(const ast::term_kind_scope& ast)
{
    sema::scoped_var<bool> scope_loc(m_can_inst_local, false);
    return base_type::eval(ast);
}

ast::term instantiate_vis::eval(const ast::all_term& ast)
{
    sema::scoped_var<bool> scope_loc(m_can_inst_local, false);
    return base_type::eval(ast);
}

ast::term instantiate_vis::eval(const ast::pack_term& ast)
{
    sema::scoped_var<bool> scope_loc(m_can_inst_local, false);
    return base_type::eval(ast);
}

bool instantiate_vis::can_instantiate_local() const
{
    return m_can_inst_local;
};

class inst_term_local_subs : public sema::local_def_subs_caller
{
    private:
        instantiate_vis*      m_owner;

    public:
        inst_term_local_subs(instantiate_vis* owner)
            :m_owner(owner)
        {};

        virtual void add_name_substitution(const ast::identifier& old_id, 
                        const ast::identifier& new_id) override
        {
            m_owner->add_name_substitution(old_id, new_id);
        };
        virtual void remove_name_substitution(const ast::identifier& old_id) override
        {
            m_owner->remove_name_substitution(old_id);
        };
        virtual ast::term make_term(const ast::identifier& old_id, const ast::term& t,
                                    bool overloaded) override
        {
            (void)old_id;
            (void)overloaded;
            return m_owner->visit(t);
        };
        virtual ast::type make_type(const ast::identifier& old_id, const ast::type& t,
                                    bool overloaded) override
        {
            (void)old_id;
            (void)overloaded;
            return m_owner->visit(t);
        };
        virtual ast::type get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors) override
        {
            return m_owner->get_symbol_type(idt, overloaded, has_errors);
        };
        virtual ast::term get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors)
        {
            return m_owner->get_symbol_term(idt, overloaded, has_errors);
        };

        virtual void make_definition(const ast::identifier& old_id,
                                const ast::term_definition& new_def) override
        {
            return m_owner->make_definition(old_id, new_def);
        };
};

ast::definitions instantiate_vis::make_fresh_function_names(const ast::definitions& defs)
{
    inst_term_local_subs caller(this);
    sema::local_def_substitution loc_defs(&caller, defs->get_location());
    ast::definitions ret = loc_defs.process_definitions(defs);

    loc_defs.finish();
    return ret;
};

void instantiate_vis::add_name_substitution(const ast::identifier& old_id, 
                const ast::identifier& new_id)
{
    m_name_subs[old_id] = new_id;
};
void instantiate_vis::remove_name_substitution(const ast::identifier& old_id)
{
    m_name_subs.erase(old_id);
};

void instantiate_vis::make_definition(const ast::identifier& old_id,
                const ast::term_definition& new_def)
{
    (void)old_id;
    m_mod.register_local_function_subst(new_def);
};
ast::type instantiate_vis::get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                bool& has_errors)
{
    return m_mod.get_symbol_type(m_eh, idt, overloaded, has_errors);
};
ast::term instantiate_vis::get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                bool& has_errors)
{
    return m_mod.get_symbol_term(m_eh, idt, overloaded, has_errors);
};

bool instantiate_vis::check_must_resolve(const std::vector<details::applic_arg_term>& subs_args,
                                       bool is_local_func) const
{
    bool val    = inst_collector::is_closed(subs_args);

    if (val == false)
        return false;

    if (is_local_func == true)
        return m_can_inst_local;

    return true;
};

};};