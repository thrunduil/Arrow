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
#pragma once

#include "computation_on_types.h"
#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "semantics/semantics.h"
#include "semantics/utils.h"
#include "instantiate_type_kind.h"
#include "semantics/typing/terms/instantiate_term.h"

#include <iostream>

namespace arrow { namespace typing
{

//------------------------------------------------------------------
//                  type_substitution_head
//------------------------------------------------------------------
class type_substitution_head_vis : public ast::rewrite_visitor<type_substitution_head_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<type_substitution_head_vis>;
        using subs_map      = std::map<ad::var_info, ast::type>;

    private:
        const subs_map*     m_subs;
        bool                m_any_subs;

    public:
        /// subs map must contain types with fresh binders
        bool make(const subs_map& subs, ast::type& t)
        {
            m_subs          = &subs;
            m_any_subs      = false;

            t = visit(t);
            return m_any_subs;
        };

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        ast::type eval(const ast::type_application& ta)
        {
            const ast::type& tr1    = ta.get_rhs();
            const ast::type& tl1    = ta.get_lhs();

            ast::type tl2           = visit(tl1);

            if (tl2 == tl1)
                return ta;
            else
                return ast::type_application(ta->get_location(), tl2, tr1);
        };
        ast::type eval(const ast::type_type_application& ta)
        {
            const ast::type& tl1    = ta.get_type();
            const ast::type& tr1    = ta.get_arg();

            ast::type tl2           = visit(tl1);

            if (tl2 == tl1)
                return ta;
            else
                return ast::type_type_application(ta->get_location(), tl2, tr1);
        };

        ast::type eval(const ast::if_type& ast)
        {
            const ast::type& t_if   = ast.get_if();
            ast::type t_if2         = visit(t_if);

            if (t_if2 == t_if)
                return ast;
            else
                return ast::if_type(ast->get_location(), t_if2, ast.get_then(), ast.get_else());
        };

        ast::type eval(const ast::path_int_type& ast)
        {
            const ast::type& ty         = ast.get_type();
            const ast::int_literal& lab = ast.get_index();

            ast::type ty2   = visit(ty);

            if (ty2 == ty)
                return ast;
            else
                return ast::path_int_type(ast->get_location(), ty2, lab);
        };

        ast::type eval(const ast::identifier_type& t_id)
        {
            const ast::identifier& id   = t_id.get_identifier();

            auto pos = m_subs->find(id);
            if (pos == m_subs->end())
                return t_id;
            
            m_any_subs  = true;
            return pos->second;
        };

        ast::type eval(const ast::type_var& tv)
        {
            if (tv.is_set() == true)
                return visit(tv.get());
            else
                return tv;
        };

        ast::type eval(const ast::annotated_type& tv)
        {
            return visit(tv.get_type());
        };

        //do not look inside type

        ast::type eval_constructor(const ast::type& ast)
        {
            return ast;
        }

        ast::type eval(const ast::some_type& ast)
        {
            return eval_constructor(ast);
        }
        ast::type eval(const ast::all_type& ast)
        {
            return eval_constructor(ast);
        }
        ast::type eval(const ast::lambda_type& ast)
        {
            return eval_constructor(ast);
        }

        ast::type eval(const ast::mu_type& ast)
        {
            return eval_constructor(ast);
        };

        ast::type eval(const ast::arrow_type& ast)
        {
            return eval_constructor(ast);
        }        

        auto eval(const ast::ref_type& ast) -> ast::type
        {
            return eval_constructor(ast);
        }        

        auto eval(const ast::list_type& ast) -> ast::type
        {
            return eval_constructor(ast);
        }
        auto eval(const ast::variant_type& ast) -> ast::type
        {
            return eval_constructor(ast);
        }
        auto eval(const ast::record_type& ast) -> ast::type
        {
            return eval_constructor(ast);
        }
        auto eval(const ast::bracket_type& ast) -> ast::type
        {
            //not allowed at this state
            return ast;
        };
        auto eval(const ast::tuple_type& ast) -> ast::type
        {
            return eval_constructor(ast);
        }
        auto eval(const ast::predefined_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::type_sequence& ast) -> ast::type
        {
            //not allowed at this state
            return ast;
        };

        auto eval(const ast::product_of_types& ast) -> ast::type
        {
            return eval_constructor(ast);
        }
        auto eval(const ast::int_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::error_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::bool_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::char_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::string_type& ast) -> ast::type
        {
            return ast;
        }
        auto eval(const ast::type_constant& ast) -> ast::type
        {
            return eval_constructor(ast);
        };

        //use defaults
        //auto eval(const ast::type_kind_scope& ast)
        //ast::type eval(const ast::type_kind_application& ast)
};

bool type_substitution_head::make(const subs_map& subs, ast::type& body) const
{
    return type_substitution_head_vis().make(subs, body);
};

//------------------------------------------------------------------
//                  instantiate_type_function
//------------------------------------------------------------------
instantiate_type_function::instantiate_type_function(const error_handler_ptr& eh, 
                            const module& mod, inst_collector* col)
    :m_eh(eh), m_mod(mod), m_collector(col)
{};

ast::type instantiate_type_function::make(const ast::identifier_type& tid, const kind_vec& arg, 
                    bool& expanded, bool& instantiated, bool show_error, kind_subs_map& subs)
{
    const ast::identifier& id   = tid.get_identifier();
    const ast::symbol_data* sd  = id.get_symbol_data();    

    expanded        = false;
    instantiated    = false;

    if (sd->get_kind().is_type_def() == false)
        return tid;

    bool has_errors;
    bool overloaded;
    ast::type def   = m_mod.get_type_constructor_definition(m_eh, tid, 
                        overloaded, has_errors);

    if (has_errors == true)
        return ast::error_type(tid->get_location());

    expanded        = true;

    if (def->get_type_code() != ast::details::type_code::type_kind_scope)
    {
        if (arg.size() != 0)
        {
            //this is impossible
            if (m_eh->has_errors() == true)
                return def;

            error::error_base_msg(m_eh).internal_error(tid->get_location(), 
                    "invalid kind application");
            return ast::error_type(tid->get_location());
        }

        return def;
    };

    instantiated    = true;
    bool can_local  = true;
    def = m_collector->instantiate_and_infer_kind(def, arg, show_error, subs, has_errors, 
                                                    can_local);
    return def;
};

//------------------------------------------------------------------
//                  unfold_rec_type
//------------------------------------------------------------------
ast::type unfold_rec_type::make(ast::type t0, bool single_step)
{
    t0                              = sema::expand_type_var(t0);

    if (t0->get_type_code() == ast::details::type_code::type_kind_scope)
    {
        ast::type_kind_scope ts = t0.static_cast_to<ast::type_kind_scope>();

        const ast::type& body       = ts.get_body();
        const ast::identifier& id   = ts.get_var();

        ast::type body2             = make(body, single_step);
        if (body2 == body)
            return t0;
        else
            return ast::type_kind_scope(t0->get_location(), id, body2);
    };

    using var_set   = std::set<ad::var_info>;
    var_set visited_vars;

    ast::type res   = t0;

    //this loop always terminates since there are finitely many variables
    for (;;)
    {
        if (res->get_type_code()  != ast::details::type_code::mu_type)
            return res;

        ast::mu_type mu             = res.static_cast_to<ast::mu_type>();
        const ast::type& ty         = mu.get_body();
        const ast::identifier& id   = mu.get_var().get_identifier();

        auto pos                    = visited_vars.find(id);
        if (pos != visited_vars.end())
        {
            error::error_base_msg(m_eh).internal_error(mu->get_location(), 
                            "unable to reduce mu type to weak head normal form");
            return ast::error_type(mu->get_location());
        };

        visited_vars.insert(pos, id);

        type_substitution ts(m_eh, m_mod);
        ts.add_type(id, mu, true);

        res             = ts.make(ty);
        res             = sema::expand_type_var(res);

        if (single_step == true)
            return res;
    };
};

//------------------------------------------------------------------
//                  type_substitution
//------------------------------------------------------------------
namespace details
{

class type_subs_vis : public ast::rewrite_visitor<type_subs_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<type_subs_vis>;
        using subs_map_type = std::map<ad::var_info, ast::type>;
        using subs_map_kind = std::map<ad::var_info, ast::kind>;

    private:
        subs_map_type   m_map_type;
        subs_map_kind   m_map_kind;
        bool            m_subs;

    public:
        type_subs_vis() {};

        ast::type make(const ast::type& body, bool& substituted)
        {
            m_subs          = false;
            ast::type res   = visit(body);
            substituted     = m_subs;
            return res;
        };

        /// ty must have fresh binders
        void add_type(const ad::var_info& vi, const ast::type& ty)
        {
            m_map_type[vi] = ty;
        }
        void add_kind(const ad::var_info& vi, const ast::kind& ty)
        {
            m_map_kind[vi] = ty;
        }

        /// ty must have fresh binders
        bool add_type(const ad::var_info& vi, const ast::type& ty, ast::type& old)
        {
            auto pos = m_map_type.find(vi);
            
            if (pos == m_map_type.end())
            {
                m_map_type[vi] = ty;
                return false;
            }
            else
            {
                old = pos->second;
                pos->second = ty;
                return true;
            };
        }

        void remove_type(const ad::var_info& var)
        {
            m_map_type.erase(var);
        }

        long size_type() const
        {
            return (long)m_map_type.size();
        }
        long size_kind() const
        {
            return (long)m_map_kind.size();
        }

        void clear()
        {
            m_map_kind.clear();
            m_map_type.clear();
        }

        template<class Ast>
        auto eval(const Ast& t) -> typename decltype(base_type::eval(t))
        {
            return base_type::eval(t);
        }

        auto eval(const ast::identifier_type& ast)
            -> ast::type
        {
            const ast::identifier& id = ast.get_identifier();
            auto pos    = m_map_type.find(id);

            if (pos == m_map_type.end())
                return ast;
            
            m_subs = true;
            return pos->second;
        };

        auto eval(const ast::kind_identifier& ast)
            -> ast::kind
        {
            const ast::identifier& id = ast.get_identifier();
            auto pos    = m_map_kind.find(id);

            if (pos == m_map_kind.end())
                return ast;
            
            m_subs = true;
            return pos->second;
        };
};

class type_substitution_impl
{
    private:
        type_subs_vis       m_vis;
        error_handler_ptr   m_eh;
        module              m_mod;

    public:
        type_substitution_impl(const error_handler_ptr& eh, const module& mod)
            :m_eh(eh), m_mod(mod)
        {};

        void add_type(const ad::var_info& var, const ast::type& ty, bool make_fresh_binders)
        {
            if (make_fresh_binders == true)
            {
                ast::type t2    = sema::make_fresh_binders(ty, m_eh, m_mod);
                m_vis.add_type(var, t2);
            }            
            else
            {
                m_vis.add_type(var, ty);
            };
        }
        void add_kind(const ad::var_info& var, const ast::kind& ty)
        {
            m_vis.add_kind(var, ty);
        }

        bool add_type(const ad::var_info& var, const ast::type& ty, 
                        bool make_fresh_binders, ast::type& old_subs)
        {
            ast::type t2;
            if (make_fresh_binders == true)
                t2    = sema::make_fresh_binders(ty, m_eh, m_mod);
            else
                t2 = ty;

            return m_vis.add_type(var, t2, old_subs);
        };

        void remove_type(const ad::var_info& var)
        {
            return m_vis.remove_type(var);
        }

        long size_type() const
        {
            return m_vis.size_type();
        }
        long size_kind() const
        {
            return m_vis.size_kind();
        }

        void clear()
        {
            return m_vis.clear();
        }

        ast::type make(const ast::type& body, bool& substituted)
        {
            return m_vis.make(body, substituted);
        };

        ast::type make(const ast::type& t)
        {
            return m_vis.visit(t);
        }
        ast::kind make_kind(const ast::kind& t)
        {
            return m_vis.visit(t);
        }
};

}

type_substitution::type_substitution(const error_handler_ptr& eh, const module& mod)
    :m_impl(new impl_type(eh, mod))
{};


type_substitution::~type_substitution()
{};

void type_substitution::add_type(const ad::var_info& var, const ast::type& ty, 
                                 bool make_fresh_binders)
{
    m_impl->add_type(var, ty, make_fresh_binders);
}
void type_substitution::add_kind(const ad::var_info& var, const ast::kind& ty)
{
    m_impl->add_kind(var, ty);
}

bool type_substitution::add_type(const ad::var_info& var, const ast::type& ty, 
                bool make_fresh_binders, ast::type& old_subs)
{
    return m_impl->add_type(var, ty, make_fresh_binders, old_subs);
}

void type_substitution::remove_type(const ad::var_info& var)
{
    return m_impl->remove_type(var);
}

long type_substitution::size_type() const
{
    return m_impl->size_type();
}
long type_substitution::size_kind() const
{
    return m_impl->size_kind();
}
long type_substitution::size_all() const
{
    return size_type() + size_kind();
}

void type_substitution::clear() const
{
    return m_impl->clear();
}

ast::type type_substitution::make(const ast::type& body, bool& substituted) const
{
    return m_impl->make(body, substituted);
}

ast::type type_substitution::make(const ast::type& body) const
{
    return m_impl->make(body);
}

ast::kind type_substitution::make_kind(const ast::kind& body) const
{
    return m_impl->make_kind(body);
}

};};