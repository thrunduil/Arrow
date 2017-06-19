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

#include "check_mu_type.h"
#include "arrow/ast/ast_visitor.h"
#include "scopes/symbol_data.h"
#include "error/error.h"

#include <set>
#include <map>

namespace arrow { namespace typing
{

namespace ad = arrow::details;

// contractiveness check as in [SS05]: Stone, C. A., & Schoonmaker, A. P. (2005)
// Equational theories with recursive types; (with some changes)
class check_mu_vis : public ast::traversal_visitor<check_mu_vis>
{
    private:
        using base_type     = ast::traversal_visitor<check_mu_vis>;
        using var_set       = std::set<ad::var_info>;
        using var_map       = std::map<ad::var_info, ast::location>;

    private:
        var_map             m_unguarded;
        var_set             m_mu_vars;
        var_set             m_mu_visited;
        error_handler_ptr   m_eh;
        bool                m_error;
        bool                m_show_error;

    public:
        check_mu_vis(const error_handler_ptr& eh)
            :m_eh(eh), m_error(false), m_show_error(true)
        {};
        
        bool                make(const ast::mu_type& mu, bool show_error);
        void                clear();
        bool                is_checked(const ast::mu_type& mu);

    public:
        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            return base_type::eval(ast);
        };

        //default traversal
        //void eval(const ast::annotated_type& ast);
        //void eval(const ast::type_var& ast);
        //void eval(const ast::type_kind_scope& ast);
        //void eval(const ast::type_kind_application& ast);
        //void eval(const ast::type_sequence& ast);        

        void eval(const ast::product_of_types& ast)
        { 
            //type level tuples are not contractive; 
            return base_type::eval(ast);
        }
        void eval(const ast::path_int_type& ast)
        {
            // we rule out types like Rec[X] {X->X,X}[1]
            // it is not clear if this can be relaxed since product of types
            // can be unknown at this point
            return base_type::eval(ast);
        };

        void eval(const ast::arrow_type& ast)       { eval_guarded(ast); }
        void eval(const ast::ref_type& ast)         { eval_guarded(ast); }        
        void eval(const ast::list_type& ast)        { eval_guarded(ast); }
        void eval(const ast::variant_type& ast)     { eval_guarded(ast); }
        void eval(const ast::record_type& ast)      { eval_guarded(ast); }
        void eval(const ast::type_constant& ast)    { eval_guarded(ast); }
        void eval(const ast::tuple_type& ast)       { eval_guarded(ast); }

        void eval(const ast::bracket_type& ast)     { (void)ast; return; }
        void eval(const ast::error_type& ast)       { (void)ast; return; }
        void eval(const ast::int_type& ast)         { (void)ast; return; }
        void eval(const ast::bool_type& ast)        { (void)ast; return; }
        void eval(const ast::char_type& ast)        { (void)ast; return; }
        void eval(const ast::string_type& ast)      { (void)ast; return; }
        void eval(const ast::predefined_type& ast)  { (void)ast; return; }       

        void eval(const ast::identifier_type& ast)
        {
            const ast::identifier& id   = ast.get_identifier();

            if (is_mu_var(id) == true)
                add_to_unguarded(id);
        };

        void eval(const ast::mu_type& ast)
        {
            const ast::type& ty         = ast.get_body();
            const ast::identifier& id   = ast.get_var().get_identifier();

            add_mu_var(id);
            visit(ty);

            ast::location unguarded_pos;
            if (is_unguarded(id, unguarded_pos) == true)
            {
                if (m_show_error == true)
                {
                    error::type_error_msg(m_eh).error_mu_type_not_contractive
                            (ast->get_location(), ast, id, unguarded_pos);
                }
                m_error = true;
            };

            remove_mu_var(id);
        };

        void eval(const ast::some_type& ast)
        {
            // we assume that some_type is not a guard; we may safely assume, that
            // some_type is a guard; but this breaks a symmetry with all_type

            base_type::eval(ast);
        }
        void eval(const ast::all_type& ast)
        {
            // we assume that all_type is not a guard; if we assume, that all_type
            // is a guard, then mu type stil will have the weak head normal form;
            // however type inference of term type application (i.e. (All[..] term)<args>)
            // requires, that All[...] have finitely many arguments which is not the
            // case for Rec[X] All[Z] X.

            base_type::eval(ast);
        }

        void eval(const ast::lambda_type& ast)
        {
            // different rule than in [SS05] since we do not add lambda var
            // to the environment
            base_type::eval(ast);
        };

        void eval(const ast::type_application& ast)
        {
            const ast::type& lhs    = ast.get_lhs();
            const ast::type& rhs    = ast.get_rhs();

            // type application is not a guard
            visit(lhs);
            visit(rhs);
        };

        void eval(const ast::type_type_application& ast)
        {
            const ast::type& lhs    = ast.get_type();
            const ast::type& rhs    = ast.get_arg();

            // type instantiation is not a guard
            visit(lhs);
            visit(rhs);
        };

        void eval(const ast::if_type& ast)
        {
            const ast::type& t_if   = ast.get_if();
            const ast::type& t_then = ast.get_then(); 
            const ast::type& t_else = ast.get_else();

            // if condition must have bool type, which is always a quard
            eval_guarded(t_if);

            visit(t_then);
            visit(t_else);
        };

        void eval_guarded(const ast::type& ty)
        {
            base_type::eval(ty);
            m_unguarded.clear();
        };

    private:        
        void    add_to_unguarded(const ast::identifier& id);
        bool    is_unguarded(const ast::identifier& id, ast::location& loc);
        bool    is_mu_var(const ast::identifier& id);
        void    add_mu_var(const ast::identifier& id);
        void    remove_mu_var(const ast::identifier& id);
};

bool check_mu_vis::is_mu_var(const ast::identifier& id)
{
    auto pos = m_mu_vars.find(id);

    if (pos == m_mu_vars.end())
        return false;
    else
        return true;
};

void check_mu_vis::add_mu_var(const ast::identifier& id)
{
    m_mu_vars.insert(id);
    m_mu_visited.insert(id);
}
void check_mu_vis::remove_mu_var(const ast::identifier& id)
{
    m_mu_vars.erase(id);
}

void check_mu_vis::add_to_unguarded(const ast::identifier& id)
{
    m_unguarded[id] = id->get_location();
};
bool check_mu_vis::is_unguarded(const ast::identifier& id, ast::location& loc)
{
    auto pos = m_unguarded.find(id);
    if (pos != m_unguarded.end())
    {
        loc = pos->second;
        return true;
    }
    else 
    {
        return false;
    };
};
bool check_mu_vis::make(const ast::mu_type& mu, bool show_error)
{
    m_error         = false;
    m_show_error    = show_error;
    visit(mu);
    return m_error;
}

void check_mu_vis::clear()
{
    m_unguarded.clear();
    m_mu_vars.clear();
    m_mu_visited.clear();
    m_error         = false;
    m_show_error    = true;
};

bool check_mu_vis::is_checked(const ast::mu_type& mu)
{
    const ast::identifier& id   = mu.get_var().get_identifier();
    auto pos = m_mu_visited.find(id);
    if (pos == m_mu_visited.end())
        return false;
    else
        return true;
};

check_mu_type::check_mu_type(const error_handler_ptr& eh)
    :m_ptr(new check_mu_vis(eh))
{};

check_mu_type::~check_mu_type()
{};

bool check_mu_type::make(const ast::mu_type& mu, bool show_error)
{
    bool has_error = m_ptr->make(mu, show_error);
    return has_error == false;
};

void check_mu_type::clear()
{
    m_ptr->clear();
}

bool check_mu_type::is_checked(const ast::mu_type& mu) const
{
    return m_ptr->is_checked(mu);
}

};};