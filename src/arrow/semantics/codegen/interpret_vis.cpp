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

#include "interpret_vis.h"
#include "error/error.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "semantics/utils.h"
#include "store.h"
#include "interpret_helpers.h"
#include "substitute_term_vis.h"

namespace arrow { namespace codegen
{

//------------------------------------------------------------------
//              interpreter_vis
//------------------------------------------------------------------
interpreter_vis::interpreter_vis(const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod), m_error(false)
{};

ast::term interpreter_vis::eval(const ast::val_term& ast, bool& progress)
{
    const ast::type& ty = ast.get_type();

    bool is_int         = (ty->get_type_code() == ast::details::type_code::int_type);
    bool is_bool        = (ty->get_type_code() == ast::details::type_code::bool_type);
    bool is_char        = (ty->get_type_code() == ast::details::type_code::char_type);
    bool is_string      = (ty->get_type_code() == ast::details::type_code::string_type);

    if (is_int == true)
    {
        progress            = true;
        ast::int_type it    = ty.static_cast_to<ast::int_type>();

        int64_t val         = it.get_value();
        return ast::int_literal(ast->get_location(), val);
    }
    else if (is_bool == true)
    {
        progress            = true;
        ast::bool_type it   = ty.static_cast_to<ast::bool_type>();

        bool val    = it.get_value();
        return ast::bool_literal(ast->get_location(), val);
    }
    else if (is_char == true)
    {
        progress            = true;
        ast::char_type it   = ty.static_cast_to<ast::char_type>();

        int32_t val         = it.get_value();
        return ast::char_literal(ast->get_location(), val);
    }
    else if (is_string == true)
    {
        progress            = true;
        ast::string_type it = ty.static_cast_to<ast::string_type>();

        const std::string& val  = it.get_value();
        return ast::string_literal(ast->get_location(), val);
    }

    error::error_base_msg(m_eh).internal_error
        (ast->get_location(), "type has not value kind");
    m_error = true;

    return ast::error_term(ast->get_location());
};

ast::term interpreter_vis::eval(const ast::term_kind_scope& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::int_literal& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::bool_literal& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::float_literal& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::char_literal& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::string_literal& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::annotated_term& ast, bool& progress)
{
    return visit(ast.get_term(), progress);
};

ast::term interpreter_vis::eval(const ast::lambda_term& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::error_term& ast, bool& progress)
{
    (void)progress;
    m_error = true;
    return ast;
}

ast::term interpreter_vis::eval(const ast::if_term& ast, bool& progress)
{
    const ast::term& t_cond = ast.get_if();
    const ast::term& t_then = ast.get_then();
    const ast::term& t_else = ast.get_else();

    if (t_cond->get_term_code() == ast::details::term_code::bool_literal)
    {
        ast::bool_literal bl    = t_cond.static_cast_to<ast::bool_literal>();
        progress                = true;

        if (bl.get_value() == true)
            return t_then;
        else
            return t_else;
    }

    ast::term t_cond2   = visit(t_cond, progress);
    return ast::if_term(ast->get_location(), t_cond2, t_then, t_else);
};

ast::term interpreter_vis::eval(const ast::tuple_term& ast, bool& progress)
{
    long n              = ast.size(); 

    for (long i = 0; i < n; ++i)
    {
        const ast::term& ti = ast.get(i);

        if (is_value(ti) == true)
            continue;

        ast::tuple_term ret  = ast::tuple_term(ast->get_location(), ast.get_state());

        for (long j = 0; j < i; ++j)
            ret.add(ast.get(j));

        ast::term ti2   = visit(ast.get(i), progress);
        ret.add(ti2);

        for (long j = i+1; j < n; ++j)
            ret.add(ast.get(j));        

        return ret;
    }

    return ast;
};

ast::term interpreter_vis::eval(const ast::list_term& ast, bool& progress)
{    
    long n              = ast.size(); 

    for (long i = 0; i < n; ++i)
    {
        const ast::term& ti = ast.get(i);

        if (is_value(ti) == true)
            continue;

        ast::list_term ret  = ast::list_term(ast->get_location());

        for (long j = 0; j < i; ++j)
            ret.add(ast.get(j));

        ast::term ti2   = visit(ast.get(i), progress);
        ret.add(ti2);

        for (long j = i+1; j < n; ++j)
            ret.add(ast.get(j));        

        return ret;
    }

    return ast;
};

ast::term interpreter_vis::eval(const ast::term_constant& ast, bool& progress)
{
    long n              = ast.var_size(); 
    bool is_extern      = ast.is_extern_func();

    for (long i = 0; i < n; ++i)
    {
        const ast::term& ti = ast.get_term(i);

        if (is_value(ti) == true)
            continue;

        ast::term_constant ret  = ast::term_constant(ast->get_location(), ast.get_name(), is_extern);
        ret.set_return_type(ast.get_return_type());
        ret.set_function(ast.get_function());

        for (long j = 0; j < i; ++j)
        {
            const auto& li2  = ast.get_label(j);
            const auto& ti2  = ast.get_term(j);
            ret.add_label(li2, ti2);
        }

        ast::term t2   = visit(ast.get_term(i), progress);
        ret.add_label(ast.get_label(i), t2);

        for (long j = i+1; j < n; ++j)
        {
            const auto& li2  = ast.get_label(j);
            const auto& ti2  = ast.get_term(j);
            ret.add_label(li2, ti2);
        }

        return ret;
    }    

    if (ast.get_function())
    {
        progress = true;
        return ast.eval();
    }
    else
    {
        return ast;
    }
};

ast::term interpreter_vis::eval(const ast::record_term& ast, bool& progress)
{
    long n              = ast.size(); 

    for (long i = 0; i < n; ++i)
    {
        const ast::term& ti = ast.get_term(i);

        if (is_value(ti) == true)
            continue;

        ast::record_term ret  = ast::record_term(ast->get_location());

        for (long j = 0; j < i; ++j)
        {
            const auto& li2  = ast.get_label(j);
            const auto& ti2  = ast.get_term(j);
            ret.add(li2, ti2);
        }

        ast::term t2   = visit(ast.get_term(i), progress);
        ret.add(ast.get_label(i), t2);

        for (long j = i+1; j < n; ++j)
        {
            const auto& li2  = ast.get_label(j);
            const auto& ti2  = ast.get_term(j);
            ret.add(li2, ti2);
        }

        return ret;
    }

    return ast;
}

ast::term interpreter_vis::eval(const ast::mu_term& ast, bool& progress)
{
    const ast::term& body       = ast.get_body();
    const ast::identifier& id   = ast.get_var().get_identifier();
    progress                    = true;
    
    return substitute(id, ast, body);
};

ast::term interpreter_vis::eval(const ast::term_application& ast, bool& progress)
{
    const ast::term& lhs    = ast.get_lhs();
    const ast::term& rhs    = ast.get_rhs();

    if (lhs->get_term_code() == ast::details::term_code::lambda_term)
    {
        if (is_value(rhs) == true)
        {
            progress                = true;
            ast::lambda_term lam    = lhs.static_cast_to<ast::lambda_term>();

            const ast::term& body       = lam.get_body();
            const ast::identifier& id   = lam.get_var().get_identifier();
            return substitute(id, rhs, body);
        };
    }

    if (is_value(lhs) == true)
    {
        ast::term rhs2  = visit(rhs, progress);
        return ast::term_application(ast->get_location(), lhs, rhs2);
    };

    ast::term lhs2  = visit(lhs, progress);
    return ast::term_application(ast->get_location(), lhs2, rhs);
}

ast::term interpreter_vis::eval(const ast::term_type_application& ast, bool& progress)
{
    const ast::term& lhs    = ast.get_term();
    const ast::type& rhs    = ast.get_type();

    if (lhs->get_term_code() == ast::details::term_code::all_term)
    {
        progress                    = true;

        using context_type      = typing::details::kind_inference_context;

        context_type ctx;
        typing::bound_map bm;
        typing::inst_collector inst(m_eh, m_mod);

        ast::term ret = inst.make_term_type_application(ast, can_instantiate_local());
        inst.instantiate();
        return ret;
    };

    ast::term lhs2      = visit(lhs, progress);
    return ast::term_type_application(ast->get_location(), lhs2, rhs);
}

ast::term interpreter_vis::eval(const ast::term_kind_application& ast, bool& progress)
{
    const ast::term& lhs    = ast.get_term();
    const ast::kind& rhs    = ast.get_kind();

    if (lhs->get_term_code() == ast::details::term_code::term_kind_scope)
    {
        progress                    = true;

        using context_type      = typing::details::kind_inference_context;

        context_type ctx;
        typing::bound_map bm;
        typing::inst_collector inst(m_eh, m_mod);

        ast::term ret = inst.make_term_kind_application(ast, can_instantiate_local());
        inst.instantiate();
        return ret;
    };

    ast::term lhs2      = visit(lhs, progress);
    return ast::term_kind_application(ast->get_location(), lhs2, rhs);
};

ast::term interpreter_vis::eval(const ast::all_term& ast, bool& progress)
{
    (void)progress;
    return ast;
};

ast::term interpreter_vis::eval(const ast::path_term& ast, bool& progress)
{
    const ast::term& t          = ast.get_term();
    const ast::identifier& lab  = ast.get_identifier();

    if (t->get_term_code() == ast::details::term_code::record_term)
    {
        progress            = true;
        ast::record_term rt = t.static_cast_to<ast::record_term>();
        bool has            = rt.has_field(lab);

        if (has == false)
        {
            error::error_base_msg(m_eh).internal_error
                (ast->get_location(), "invalid interpret: invalid field in record");
            m_error = true;

            return ast::error_term(ast->get_location());
        }

        ast::term ret       = rt.get_field(lab);
        return ret;
    };

    ast::term t2    = visit(t, progress);
    return ast::path_term(ast->get_location(), t2, lab);    
};

ast::term interpreter_vis::eval(const ast::path_int_term& ast, bool& progress)
{
    const ast::term& t          = ast.get_term();
    const ast::int_literal& lab = ast.get_index();

    if (t->get_term_code() == ast::details::term_code::tuple_term)
    {
        progress            = true;
        ast::tuple_term tt  = t.static_cast_to<ast::tuple_term>();

        bool has_err;
        long long_val   = sema::cast_to_long(lab->get_location(), lab.get_value(), 
                            m_eh, has_err);

        if (has_err)
        {
            m_error     = true;
            return ast::error_term(ast->get_location());
        }

        if (long_val < 0 || long_val >= tt.size())
        {
            error::error_base_msg(m_eh).internal_error
                (ast->get_location(), "invalid interpret: invalid tuple projection");
            m_error = true;

            return ast::error_term(ast->get_location());
        }

        return tt.get(long_val);
    };

    ast::term t2    = visit(t, progress);
    return ast::path_int_term(ast->get_location(), t2, lab);    
};

ast::term interpreter_vis::eval(const ast::bracket_term& ast, bool& progress)
{
    error::error_base_msg(m_eh).internal_error
        (ast->get_location(), "invalid interpret: unexpected bracket term");

    (void)progress;
    m_error = true;
    return ast::error_term(ast->get_location());
};

ast::term interpreter_vis::eval(const ast::term_sequence& ast, bool& progress)
{
    error::error_base_msg(m_eh).internal_error
        (ast->get_location(), "invalid interpret: unexpected term sequence");

    (void)progress;
    m_error = true;
    return ast::error_term(ast->get_location());
};

ast::term interpreter_vis::eval(const ast::pack_term& ast, bool& progress)
{
    using annot_id              = ast::annotated_type_identifier;
    const annot_id& var         = ast.get_var();
    const ast::type& ty         = ast.get_var_type();
    const ast::term& ex         = ast.get_body();
    const ast::type& some_ty    = ast.get_some_type();

    ast::term ex2   = visit(ex, progress);
    return ast::pack_term(ast->get_location(), var, ty, ex2, some_ty);
};

ast::term interpreter_vis::eval(const ast::unpack_term& ast, bool& progress)
{
    //pack      = Some[X = T] body
    //open[X] x = pack in term(X,x) ~-> [X := T, x := body] term

    const ast::term& pack       = ast.get_package();
    const ast::term& in         = ast.get_term();
    const auto& ty_id           = ast.get_type_identifier();
    const auto& ex_id           = ast.get_term_identifier();
    const ast::symbol_data* sd  = ast.get_symbol_data();

    if (pack->get_term_code() == ast::details::term_code::pack_term)
    {
        ast::pack_term pt           = pack.static_cast_to<ast::pack_term>();
        const ast::term& pack_ex    = pt.get_body();
        const ast::type& pack_ty    = pt.get_var_type();

        if (is_value(pack_ex) == true)
        {
            progress            = true;
            ast::term t_sub     = substitute(ex_id.get_identifier(), pack_ex, in);
            ast::term ty_sub    = substitute_type(ty_id.get_identifier(), pack_ty, t_sub);
            return ty_sub;
        };
    };

    ast::term pack2 = visit(pack, progress);
    return ast::unpack_term(ast->get_location(), pack2, ty_id, ex_id, in, sd);
};

ast::term interpreter_vis::eval(const ast::let_term& ast, bool& progress)
{
    const ast::term& body   = ast.get_body();
    ast::term ret           = visit(body, progress);   
    return ret;
};

ast::term interpreter_vis::eval(const ast::variant_term& ast, bool& progress)
{
    const ast::type& ty     = ast.get_type();
    const ast::term& body   = ast.get_term();    
    const auto& lab         = ast.get_label();

    ast::term body2         = visit(body, progress);

    return ast::variant_term(ast->get_location(), ty, lab, body2);
};

ast::term interpreter_vis::eval(const ast::variant_case_term& ast, bool& progress)
{
    using annot_id          = ast::annotated_identifier;

    const ast::term& ex     = ast.get_term();
    long n                  = ast.size();

    if (is_value(ex) == false)
    {
        ast::term ex2   = visit(ex, progress);
        ast::variant_case_term ret(ast->get_location(), ast.get_symbol_data());
        ret.set_term(ex2);

        for (long i = 0; i < n; ++i)
            ret.add_pattern(ast.get_label(i), ast.get_bind(i));
        
        return ret;
    };

    if (ex->get_term_code() != ast::details::term_code::variant_term)
    {
        error::error_base_msg(m_eh).internal_error
            (ast->get_location(), "invalid interpret: variant term required");
        m_error = true;

        return ast::error_term(ast->get_location());
    }

    ast::variant_term vt        = ex.static_cast_to<ast::variant_term>();
    const ast::identifier& id   = vt.get_label().get_identifier();

    if (ast.has_label(id) == false)
    {
        error::error_base_msg(m_eh).internal_error
            (ast->get_location(), "invalid interpret: case term without a label");
        m_error = true;

        return ast::error_term(ast->get_location());
    }

    const ast::term& b          = ast.get_label_bind(id);

    const ast::identifier& c_id = ast.get_label(id).get_identifier();
    const ast::term& st_t       = vt.get_term();
    progress                    = true;

    return substitute(c_id, st_t, b);
};

ast::term interpreter_vis::eval(const ast::identifier_term& ast, bool& progress)
{
    ast::term def;
    
    bool overloaded, has_errors;
    ast::term ex = m_mod.get_symbol_term(m_eh, ast, overloaded, has_errors);

    if (has_errors == true)
        return ast::error_term(ast->get_location());

    if (overloaded == false)
    {
        progress    = true;
        return ex;
    };

    error::error_base_msg(m_eh).internal_error
        (ast->get_location(), "overloaded symbol should already be resolved");
    m_error = true;

    return ast::error_term(ast->get_location());
};

ast::term interpreter_vis::eval(const ast::closure_term& ast, bool& progress)
{
    const ast::term& ex     = ast.get_body();
    long n                  = ast.size();

    substitute_term_vis vis(m_mod, m_eh);    

    for (long i = 0; i < n; ++i)
    {
        const ast::term& subs       = ast.get_substitution(i);
        const ast::identifier& id   = ast.get_internal_ident(i);

        ast::term subs2             = codegen::make_fresh_binders(subs, m_eh, m_mod);
        vis.add(id, subs2);
    };

    //expand definition
    if (ex->get_term_code() != ast::details::term_code::identifier_term)
    {
        // closure symbol can be and identifier or overloaded symbol
        // overloaded symbol should already be resolved
        error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                                   "unable to expand closure symbol");

        m_error = true;
        return ast::error_term(ast->get_location());
    };

    ast::term def   = visit(ex, progress);

    if (m_error == true)
        return ast::error_term(ast->get_location());

    if (def->get_term_code() != ast::details::term_code::closure_term)
    {
        error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                    "invalid expansion of local symbol, expecing closure");

        m_error = true;
        return ast::error_term(ast->get_location());
    };

    ast::closure_term def_cl    = def.static_cast_to<ast::closure_term>();
    const ast::term& def_body   = def_cl.get_body();
    ast::term def_subs          = vis.visit(def_body);

    return def_subs;
};

ast::term interpreter_vis::eval(const ast::overloaded_symbol& ast, bool& progress)
{
    (void)progress;
    return ast;
};

ast::term interpreter_vis::eval(const ast::cell_term& ast, bool& progress)
{
    (void)progress;
    return ast;
}

ast::term interpreter_vis::eval(const ast::ref_term& ast, bool& progress)
{
    const ast::term& base   = ast.get_term();

    if (is_value(base) == false)
    {
        ast::term base2 = visit(base, progress);
        return ast::ref_term(ast->get_location(), base2);
    };

    progress    = true;
    long index  = store::make_cell(base);
    return ast::cell_term(ast->get_location(), index);
};

ast::term interpreter_vis::eval(const ast::deref_term& ast, bool& progress)
{
    const ast::term& base   = ast.get_term();

    if (is_value(base) == false)
    {
        ast::term base2 = visit(base, progress);
        return ast::deref_term(ast->get_location(), base2);
    };

    if (base->get_term_code() != ast::details::term_code::cell_term)
    {
        error::error_base_msg(m_eh).internal_error
            (ast->get_location(), "invalid interpret: cell term required");
        m_error = true;

        return ast::error_term(ast->get_location());
    }

    progress            = true;
    ast::cell_term ct   = base.static_cast_to<ast::cell_term>();
    long index          = ct.get_index();
    return store::deref_cell(index);
};

ast::term interpreter_vis::eval(const ast::assign_term& ast, bool& progress)
{
    const ast::term& lhs    = ast.get_lhs();
    const ast::term& rhs    = ast.get_rhs();

    if (is_value(lhs) == false)
    {
        ast::term lhs2 = visit(lhs, progress);
        return ast::assign_term(ast->get_location(), lhs2, rhs);
    };

    if (is_value(rhs) == false)
    {
        ast::term rhs2 = visit(rhs, progress);
        return ast::assign_term(ast->get_location(), lhs, rhs2);
    };

    if (lhs->get_term_code() != ast::details::term_code::cell_term)
    {
        error::error_base_msg(m_eh).internal_error
            (ast->get_location(), "invalid interpret: cell term required");
        m_error = true;

        return ast::error_term(ast->get_location());
    }

    progress            = true;
    ast::cell_term ct   = lhs.static_cast_to<ast::cell_term>();
    long index          = ct.get_index();
    store::update_cell(index, rhs);

    return ast::tuple_term(ast->get_location(), ast::tuple_term::fixed);
};

bool interpreter_vis::can_instantiate_local() const
{
    // in interpreter instantiations take place on head positions
    // only (therefore in empty type context)
    return true;
};

bool interpreter_vis::is_value(const ast::term& ex)
{
    is_value_vis vis;
    vis.visit(ex);
    return vis.m_value;
}

ast::term interpreter_vis::substitute(const ast::identifier& id, const ast::term& subs, 
                const ast::term& body)
{
    ast::term subs2 = codegen::make_fresh_binders(subs, m_eh, m_mod);
    substitute_term_vis vis(id, subs2, m_mod, m_eh);
    return vis.visit(body);
};

ast::term interpreter_vis::make(const ast::term& ex, bool& progress)
{
    progress        = false;
    m_error         = false; 

    ast::term ret   = visit(ex, progress);

    if (m_error == true)
        progress    = false;

    return ret;
}

ast::term interpreter_vis::substitute_type(const ast::identifier& id, const ast::type& subs, 
                const ast::term& body)
{
    using context_type      = typing::details::kind_inference_context;

    context_type ctx;
    typing::bound_map bm;
    typing::inst_collector inst(m_eh, m_mod);

    ast::term ret = inst.make_term_type_subs(id, subs, body, can_instantiate_local());
    inst.instantiate();
    return ret;
};

};};