/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "normal_form_type.h"
#include "computation_on_types.h"
#include "semantics/semantics.h"
#include "semantics/utils.h"
#include "error/error.h"
#include "instantiate_type_kind.h"

#include <iostream>

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//                  deduction_context
//------------------------------------------------------------------
deduction_context::deduction_context()
    :m_has_type_vars(false), m_subs(nullptr)
{};

deduction_context::deduction_context(var_substitution* subs)
    :m_has_type_vars(false), m_subs(subs)
{};

bool deduction_context::has_type_vars() const
{
    return m_has_type_vars;
}
bool deduction_context::can_substitute(const ast::type& ty, const ast::kind& var_k) const
{
    if (m_subs == nullptr)
        return true;

    (void)var_k;
    if (sema::has_type_vars(ty) == true)
        return false;
    else
        return true;
};
void deduction_context::add_type_var(const ast::type_var& tv, const ast::type& ty)
{
    if (m_subs == nullptr)
        return;

    m_subs->add_type_var(tv, ty);
    m_has_type_vars = true;
};

void deduction_context::check_type_var(const ast::type_var& tv)
{
    if (m_subs == nullptr)
        return;

    m_subs->register_type_var(tv);
    m_has_type_vars = true;
}

//------------------------------------------------------------------
//                  normal_form_type
//------------------------------------------------------------------
normal_form_vis::normal_form_vis(const error_handler_ptr& eh, const module& mod,
        inst_collector* col, bool expand_rec)
    :m_eh(eh), m_mod(mod), m_expand_rec(expand_rec), m_num_unfolding(0)
    ,m_collector(col)
{};

int normal_form_vis::number_rec_unfolded() const
{
    return m_num_unfolding;
}

ast::type normal_form_vis::make(const ast::type& t)
{
    m_num_unfolding     = 0;
    return visit(t);
};

ast::type normal_form_vis::make_deduce(const ast::type& t, var_substitution* subs)
{
    m_num_unfolding = 0;
    m_deduction     = deduction_context(subs);
    return visit(t);
};

//---------------------------------------------------------------
//                  TYPES
//---------------------------------------------------------------
ast::type normal_form_vis::eval(const ast::type_application& ta)
{
    //sema::print_type(std::cout, ta); std::cout << "\n";

    const ast::type& tl1    = ta.get_lhs();
    const ast::type& tr1    = ta.get_rhs();            

    ast::type tl2           = visit(tl1);
    bool has_tv             = m_deduction.has_type_vars();

    if (has_tv == true)
    {
        ast::type_var tv(ta->get_location());
        ast::type del_ty    = ast::type_application(ta->get_location(), tl2, tr1);

        m_deduction.add_type_var(tv, del_ty);
        return tv;
    };

    bool is_lambda          = (tl2->get_type_code() == ast::details::type_code::lambda_type);

    if (is_lambda == true)
    {
        ast::lambda_type lam_tl     = tl2.static_cast_to<ast::lambda_type>();
        const ast::identifier& var  = lam_tl.get_var().get_identifier();
        const ast::kind& ki         = lam_tl.get_var().get_kind();
        const ast::type& lam_tl_def = lam_tl.get_body();

        (void)ki;

        type_substitution ts(m_eh, m_mod);
        ts.add_type(var, tr1, true);
        ast::type t2    = ts.make(lam_tl_def);
        //m_expand_rec  = false;
        return visit(t2);
    };

    //mu types are not possible; they are expanded to weak head normal form

    if (tl2 == tl1)
        return ta;
    else
        return ast::type_application(ta->get_location(), tl2, tr1);
};

ast::type normal_form_vis::eval(const ast::type_type_application& ta)
{
    //sema::print_type(std::cout, ta); std::cout << "\n";

    const ast::type& tl1    = ta.get_type();
    const ast::type& tr1    = ta.get_arg();            

    ast::type tl2           = visit(tl1);
    bool has_tv             = m_deduction.has_type_vars();

    if (has_tv == true)
    {
        ast::type_var tv(ta->get_location());
        ast::type ty = ast::type_type_application(ta->get_location(), tl2, tr1);
        m_deduction.add_type_var(tv, ty);
        return tv;
    };

    bool is_all             = (tl2->get_type_code() == ast::details::type_code::all_type);

    if (is_all == true)
    {
        ast::all_type all_tl        = tl2.static_cast_to<ast::all_type>();
        const ast::identifier& var  = all_tl.get_var().get_identifier();
        const ast::kind& ki         = all_tl.get_var().get_kind();
        const ast::type& lam_tl_def = all_tl.get_body();

        (void)ki;

        type_substitution ts(m_eh, m_mod);
        ts.add_type(var, tr1, true);
        ast::type t2    = ts.make(lam_tl_def);
        //m_expand_rec  = false;
        return visit(t2);
    };

    //mu types are not possible; they are expanded to weak head normal form

    if (tl2 == tl1)
        return ta;
    else
        return ast::type_type_application(ta->get_location(), tl2, tr1);
};

ast::type normal_form_vis::eval(const ast::if_type& ast)
{
    //sema::print_type(std::cout, ta); std::cout << "\n";    

    const ast::type& t_if   = ast.get_if();
    ast::type t_if2         = visit(t_if);
    bool has_tv             = m_deduction.has_type_vars();

    if (has_tv == true)
    {
        ast::type_var tv(ast->get_location());
        ast::type ty = ast::if_type(ast->get_location(), t_if2, ast.get_then(), ast.get_else());
        m_deduction.add_type_var(tv, ty);
        return tv;
    };

    bool is_bool            = (t_if2->get_type_code() == ast::details::type_code::bool_type);

    if (is_bool == true)
    {
        ast::bool_type bt   = t_if2.static_cast_to<ast::bool_type>();
        bool cond           = bt.get_value();
        ast::type t2;

        if (cond == true)
            t2              = ast.get_then();
        else
            t2              = ast.get_else();

        //m_expand_rec      = false;
        return visit(t2);
    };

    if (t_if2 == t_if)
        return ast;
    else
        return ast::if_type(ast->get_location(), t_if2, ast.get_then(), ast.get_else());
};

ast::type normal_form_vis::eval(const ast::path_int_type& ast)
{
    const ast::type& ty         = ast.get_type();
    const ast::int_literal& lab = ast.get_index();

    ast::type ty2           = visit(ty);
    bool has_tv             = m_deduction.has_type_vars();

    if (has_tv == true)
    {
        ast::type_var tv(ast->get_location());
        ast::type del_ty    = ast::path_int_type(ast->get_location(), ty2, lab);

        m_deduction.add_type_var(tv, del_ty);
        return tv;
    };

    if (ty2->get_type_code() == ast::details::type_code::product_of_types)
    {
        ast::product_of_types pt  = ty2.static_cast_to<ast::product_of_types>();

        bool has_err;
        long long_val   = sema::cast_to_long(lab->get_location(), lab.get_value(), 
                            m_eh, has_err);

        if (has_err)
            return ast;

        ast::type elem  =  pt.get(long_val);
        //m_expand_rec  = false;
        return visit(elem);
    };

    if (ty2 == ty)
        return ast;
    else
        return ast::path_int_type(ast->get_location(), ty2, lab);
};

ast::type normal_form_vis::eval(const ast::identifier_type& t_id)
{
    bool has, instantiated;
    instantiate_type_function inst(m_eh, m_mod, m_collector);
    
    bool show_errors    = true;
    kind_subs_map subs;

    ast::type t2        = inst.make(t_id, kind_vec(), has, instantiated, show_errors, subs);

    if (subs.number_instantiations() != 0)
    {
        error::error_base_msg(m_eh).internal_error(t_id->get_location(), 
                "invalid kind application (type normal form computation");
        return ast::error_type(t_id->get_location());
    };

    //m_expand_rec  = false;

    //sema::print_type(std::cout, t2); std::cout << "\n";

    if (has == false)
        return t2;
    else
        return visit(t2);
};

ast::type normal_form_vis::eval(const ast::type_var& tv)
{
    if (tv.is_set() == true)
        return visit(tv.get());

    m_deduction.check_type_var(tv);
    return tv;
};

ast::type normal_form_vis::eval(const ast::annotated_type& tv)
{
    return visit(tv.get_type());
};

ast::type normal_form_vis::eval(const ast::type_kind_application& ast)
{
    kind_vec args;
    ast::identifier_type it;
    bool has_id = visit_kind_application(ast, ast, args, it);

    if (has_id == false)
    {
        //this is impossible, since kinding analysis was performed
        if (m_eh->has_errors() == true)
            return ast;

        error::error_base_msg(m_eh).internal_error(ast->get_location(), 
                "invalid kind application (type normal form computation");

        return ast;
    };

    ast::type ty = make_kind_application(it, args);
    return visit(ty);
};

ast::type normal_form_vis::eval_extern(const ast::type_constant& ast)
{
    m_expand_rec    = false;
    long n          = ast.var_size();
    bool modif      = false;
    bool is_closed  = true;

    std::vector<ast::type> args;

    for (long i = 0; i < n; ++i)
    {
        const ast::type& t  = ast.get_type(i);
        ast::type t2        = visit(t);
        modif               |= t2 != t;

        bool kind_closed;
        bool free_vars;
        is_closed           &= sema::is_closed_type(t2,kind_closed, free_vars);
        is_closed           &= kind_closed;
        is_closed           &= (free_vars == false);

        args.push_back(t2);
    };

    ast::type_constant ret;

    if (modif == false)
    {
        ret                 = ast;
    }
    else
    {
        ret                 = ast::type_constant(ast->get_location(), 
                                ast.get_name(), ast.get_return_kind(), true);

        ret.set_function(ast.get_function());

        for (long i = 0; i < n; ++i)
            ret.add_label(ast.get_label(i), args[i]);
    };

    if (is_closed == true)
        return ret.eval();
    else
        return ret;
};

//do not look inside type

ast::type normal_form_vis::eval_constructor(const ast::type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::some_type& ast)
{
    return eval_constructor(ast);
}
ast::type normal_form_vis::eval(const ast::all_type& ast)
{
    return eval_constructor(ast);
}
ast::type normal_form_vis::eval(const ast::lambda_type& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::mu_type& ast)
{
    if (m_expand_rec == true)
    {
        ast::type res   = unfold_rec_type(m_eh, m_mod).make(ast, false);
        ++m_num_unfolding;
        return visit(res);
    }
    else
    {
        return eval_constructor(ast);
    }
};

ast::type normal_form_vis::eval(const ast::arrow_type& ast)
{
    return eval_constructor(ast);
}        

ast::type normal_form_vis::eval(const ast::ref_type& ast)
{
    return eval_constructor(ast);
}        

ast::type normal_form_vis::eval(const ast::list_type& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::variant_type& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::record_type& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::bracket_type& ast)
{
    //not allowed at this stage
    return ast;
};

ast::type normal_form_vis::eval(const ast::tuple_type& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::predefined_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::type_sequence& ast)
{
    //not allowed at this stage
    return ast;
};

ast::type normal_form_vis::eval(const ast::product_of_types& ast)
{
    return eval_constructor(ast);
}

ast::type normal_form_vis::eval(const ast::int_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::error_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::bool_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::char_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::string_type& ast)
{
    return ast;
}

ast::type normal_form_vis::eval(const ast::type_constant& ast)
{
    if (ast.is_extern_func() == false)
        return eval_constructor(ast);
    else
        return eval_extern(ast);
};

//use defaults
//auto eval(const ast::type_kind_scope& ast)                                

//---------------------------------------------------------------
//                  KINDS
//---------------------------------------------------------------
ast::kind normal_form_vis::eval(const ast::kind_var& kv)
{
    if (kv.is_set() == true)
        return visit(kv.get());

    return kv;
}

bool normal_form_vis::visit_kind_application(const ast::type& applic_ast, const ast::type& ty, 
                        kind_vec& args, ast::identifier_type& id)
{
    //kind argument can be applied to identifiers only
    if (ty->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv    = ty.static_cast_to<ast::type_var>();

        if (tv.is_set())
            return visit_kind_application(applic_ast, tv.get(), args, id);

        return false;
    }
    else if (ty->get_type_code() == ast::details::type_code::type_kind_application)
    {
        ast::type_kind_application ta   = ty.static_cast_to<ast::type_kind_application>();

        const ast::type& base   = ta.get_lhs();
        const ast::kind& k_new  = ta.get_rhs();

        args.push_back(k_new);
        return visit_kind_application(applic_ast, base, args, id);
    }
    else if (ty->get_type_code() == ast::details::type_code::identifier_type)
    {
        ast::identifier_type it = ty.static_cast_to<ast::identifier_type>();
        id = it;
        return true;
    }

    return false;
};

ast::type normal_form_vis::make_kind_application(const ast::identifier_type& id, kind_vec& args)
{
    instantiate_type_function itf(m_eh, m_mod, m_collector);
    bool expanded, instantiated;

    bool show_error = true;
    kind_subs_map subs;
    ast::type ty    = itf.make(id, args, expanded, instantiated, show_error, subs);

    if (subs.number_instantiations() != 0)
    {
        error::error_base_msg(m_eh).internal_error(id->get_location(), 
                "invalid kind application (type normal form computation");
        return ast::error_type(id->get_location());
    };

    if (instantiated == false)
    {
        //this should be checked during kinding analysis

        if (m_eh->has_errors() == true)
            return id;

        error::error_base_msg(m_eh).internal_error(id->get_location(), 
                "invalid kind application (type normal form computation");

        return id;
    };

    return ty;
};

}}}

namespace arrow { namespace typing 
{

normal_form_type::normal_form_type(const error_handler_ptr& eh, const module& mod, 
            inst_collector* col, bool expand_rec_types)
    :m_eh(eh), m_mod(mod), m_expand_rec_types(expand_rec_types)
    ,m_collector(col)
{};

ast::type normal_form_type::make(const ast::type& t, int& num_unfolding)
{
    details::normal_form_vis vis(m_eh, m_mod, m_collector, m_expand_rec_types);
    ast::type res   = vis.make(t);
    num_unfolding   = vis.number_rec_unfolded();
    return res;
};

ast::type normal_form_type::make_deduce(const ast::type& t, var_substitution* subs, 
                        int& num_unfolding)
{
    details::normal_form_vis vis(m_eh, m_mod, m_collector, m_expand_rec_types);
    ast::type res   = vis.make_deduce(t, subs);
    num_unfolding   = vis.number_rec_unfolded();
    return res;
};

//------------------------------------------------------------------
//                  head_type_vis
//------------------------------------------------------------------
class head_type_vis : public ast::traversal_visitor<head_type_vis>
{
    private:
        using base_type     = ast::traversal_visitor<head_type_vis>;
        using type_code     = ast::details::type_code;

    public:
        head_type_vis()
        {};

        type_code make(const ast::type& t)
        {
            type_code ret;
            visit(t, ret);
            return ret;
        };

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast, type_code& tc)
            ->decltype(base_type::eval(ast, tc))
        {
            return base_type::eval(ast, tc);
        };

    public:
        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        void eval(const ast::type_application& ta, type_code& tc)
        {
            const ast::type& tl1        = ta.get_lhs();
            visit(tl1, tc);
            return;
        };

        void eval(const ast::type_type_application& ta, type_code& tc)
        {
            const ast::type& tl1        = ta.get_type();
            visit(tl1, tc);
            return;
        };

        void eval(const ast::if_type& ast, type_code& tc)
        {
            const ast::type& t_if       = ast.get_if();
            visit(t_if, tc);
            return;
        };
        void eval(const ast::path_int_type& ast, type_code& tc)
        {
            const ast::type& ty         = ast.get_type();
            visit(ty, tc);
        };

        void eval(const ast::identifier_type& t_id, type_code& tc)
        {
            tc = t_id->get_type_code();
        };

        void eval(const ast::type_var& tv, type_code& tc)
        {
            if (tv.is_set() == true)
                return visit(tv.get(), tc);

            tc = tv->get_type_code();
        };

        void eval(const ast::annotated_type& tv, type_code& tc)
        {
            return visit(tv.get_type(), tc);
        };        

        void eval(const ast::type_kind_application& ast, type_code& tc)
        {
            const ast::type& ty = ast.get_lhs();
            return visit(ty, tc);
        };

        void eval(const ast::some_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::all_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::lambda_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }

        void eval(const ast::mu_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        };

        void eval(const ast::arrow_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }        

        void eval(const ast::ref_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        
        void eval(const ast::list_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::variant_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::record_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::bracket_type& ast, type_code& tc)
        {
            //not allowed at this state
            (void)ast;
            tc = ast::details::type_code::error_type;
        };
        void eval(const ast::tuple_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::predefined_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::type_sequence& ast, type_code& tc)
        {
            //not allowed at this state
            (void)ast;
            tc = ast::details::type_code::error_type;
        };

        void eval(const ast::product_of_types& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::int_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::error_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::bool_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::char_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::string_type& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        }
        void eval(const ast::type_constant& ast, type_code& tc)
        {
            tc = ast->get_type_code();
        };

        void eval(const ast::type_kind_scope& ast, type_code& tc)
        {
            const ast::type& ty = ast.get_body();
            return visit(ty, tc);
        };
};

ast::details::type_code normal_form_type::get_head_type(const ast::type& ty)
{
    arrow::typing::head_type_vis vis;
    return vis.make(ty);
};

};};