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

#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "bound_map.h"

namespace arrow { namespace typing { namespace details
{

class occurs_check_vis : public ast::traversal_visitor<occurs_check_vis>
{
    private:
        using base_type = ast::traversal_visitor<occurs_check_vis>;

    private:
        const ast::kind_var&    m_var;

    public:
        bool found;

    public:
        occurs_check_vis(const ast::kind_var& var)
            :m_var(var), found(false)
        {};

        void eval(const ast::kind_var& ast)
        {
            if (ast.is_set() == true)
                return visit(ast.get());

            if (ast == m_var)
                found = true;
        };

        template<class Ast>
        void eval(const Ast& ast)
        {
            base_type::eval(ast);
        };
};

class is_star_star_vis : public ast::traversal_visitor<is_star_star_vis>
{
    private:
        using base_type     = ast::traversal_visitor<is_star_star_vis>;

    private:
        bool                m_force_star;

    public:
        bool make(const ast::kind& k, bool force_star)
        {
            bool res        = false;
            m_force_star    = force_star;

            visit(k, res);
            return res;
        };

        template<class Ast>
        void eval(const Ast& k_inf, bool& res)
        {
            base_type::eval(k_inf, res);
        };        

        void eval(const ast::star_kind&, bool& res)         { res = true;}
        void eval(const ast::product_kind&, bool& res)      { res = false;}       
        void eval(const ast::arrow_kind&, bool& res)        { res = false;}
        void eval(const ast::value_kind&, bool& res)        { res = false;}
        void eval(const ast::kind_identifier&, bool& res)   { res = false;}
        void eval(const ast::kind_scope&, bool& res)        { res = false;}        

        void eval(const ast::kind_var& ast, bool& res)
        {
            if (ast.is_set() == true)
                return visit(ast.get(), res);

            res = false;
        };
        void eval(const ast::star_tuple_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_arrow_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_all_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_some_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_list_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_record_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_variant_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_predefined_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
        void eval(const ast::star_ref_kind& ast, bool& res)
        {
            res = (m_force_star == true)?  ast.make_star() : ast.is_star();
        }
};

class is_star_any_vis : public ast::traversal_visitor<is_star_any_vis>
{
    private:
        using base_type     = ast::traversal_visitor<is_star_any_vis>;

    public:
        bool make(const ast::kind& k)
        {
            bool res = false;
            visit(k, res);
            return res;
        };

        template<class Ast>
        void eval(const Ast& k_inf, bool& res)
        {
            base_type::eval(k_inf, res);
        };        
        
        void eval(const ast::product_kind&, bool& res)      { res = false;}       
        void eval(const ast::arrow_kind&, bool& res)        { res = false;}
        void eval(const ast::value_kind&, bool& res)        { res = false;}
        void eval(const ast::kind_identifier&, bool& res)   { res = false;}
        void eval(const ast::kind_scope&, bool& res)        { res = false;}        

        void eval(const ast::star_kind&, bool& res)         { res = true;}
        void eval(const ast::star_tuple_kind&, bool& res)   { res = true;}
        void eval(const ast::star_arrow_kind&, bool& res)   { res = true;}
        void eval(const ast::star_all_kind&, bool& res)     { res = true;}
        void eval(const ast::star_some_kind&, bool& res)    { res = true;}
        void eval(const ast::star_list_kind&, bool& res)    { res = true;}
        void eval(const ast::star_record_kind&, bool& res)  { res = true;}
        void eval(const ast::star_variant_kind&, bool& res) { res = true;}
        void eval(const ast::star_predefined_kind&, bool& r){ r = true;}
        void eval(const ast::star_ref_kind&, bool& res)     { res = true;}

        void eval(const ast::kind_var& ast, bool& res)
        {
            if (ast.is_set() == true)
                return visit(ast.get(), res);

            ast::kind_var ast2 = ast;
            ast2.set_star_var();
            res = true;
        };
};

class unify_kinds_vis : public ast::traversal_visitor<unify_kinds_vis>
{
    private:
        using base_type     = ast::traversal_visitor<unify_kinds_vis>;

    private:
        ast::location       m_loc;
        bound_map&          m_bm;
        bool                m_show_errors;
        error_handler_ptr   m_eh;

    public:
        unify_kinds_vis(const ast::location& loc, bound_map& bm, bool show_errors,
                        const error_handler_ptr& eh)
            :m_loc(loc), m_bm(bm), m_show_errors(show_errors), m_eh(eh)
        {};

        bool make(const ast::kind& k_inf, const ast::kind& k_req);

        template<class Ast>
        void eval(const Ast& k_inf, const ast::kind& k_req, bool& res)
        {
            base_type::eval(k_inf, k_req, res);
        };

        void eval(const ast::star_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::product_kind& ast, const ast::kind& k_req, bool& res);        
        void eval(const ast::arrow_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::value_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::kind_identifier& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::kind_scope& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::kind_var& ast, const ast::kind& k_req, bool& res);

        void eval(const ast::star_tuple_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_arrow_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_all_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_some_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_list_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_record_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_variant_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_predefined_kind& ast, const ast::kind& k_req, bool& res);
        void eval(const ast::star_ref_kind& ast, const ast::kind& k_req, bool& res);       

    private:
        bool        unify_kinds_var(const ast::kind_var& k_inf, const ast::kind& k_req);
        bool        unify_kinds_var(const ast::kind& k_inf, const ast::kind_var& k_req);
        bool        unify_kinds_var(const ast::kind_var& k_inf, const ast::kind_var& k_req);
        void        unify_stars(const ast::kind& k_inf, const ast::kind& k_req, 
                        bool is_fixed, bool& res);        
        static bool is_star_star_kind(const ast::kind& k, bool force_star);
};

};};};