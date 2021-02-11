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

#include "arrow/ast/ast_visitor.h"
#include "error/error.h"
#include "bound_map.h"

namespace arrow { namespace typing { namespace details
{

class unify_subkinds_vis : public ast::traversal_visitor<unify_subkinds_vis>
{
    private:
        using base_type     = ast::traversal_visitor<unify_subkinds_vis>;

    private:
        ast::location       m_loc;
        bound_map&          m_bm;
        bool                m_show_errors;
        error_handler_ptr   m_eh;

    public:
        unify_subkinds_vis(const ast::location& loc, bound_map& bm, bool show_errors,
                        const error_handler_ptr& eh)
            :m_loc(loc), m_bm(bm), m_show_errors(show_errors), m_eh(eh)
        {};

        bool make(const ast::kind& k_spec, const ast::kind& k_gen);

        template<class Ast>
        void eval(const Ast& k_spec, const ast::kind& k_gen, bool& res)
        {
            base_type::eval(k_spec, k_gen, res);
        };

        void eval(const ast::star_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::product_kind& k_spec, const ast::kind& k_gen, bool& res);        
        void eval(const ast::arrow_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::value_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::kind_identifier& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::kind_scope& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::kind_var& k_spec, const ast::kind& k_gen, bool& res);

        void eval(const ast::star_tuple_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_arrow_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_all_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_some_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_list_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_record_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_variant_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_predefined_kind& k_spec, const ast::kind& k_gen, bool& res);
        void eval(const ast::star_ref_kind& k_spec, const ast::kind& k_gen, bool& res);

    private:
        bool        is_subkind_var(const ast::kind_var& k_spec, const ast::kind& k_gen);
        bool        is_subkind_var(const ast::kind& k_spec, const ast::kind_var& k_gen);
        bool        is_subkind_var(const ast::kind_var& k_spec, const ast::kind_var& k_gen);
        void        is_star(const ast::kind& k_gen, bool& res);        
        static bool is_star_star_kind(const ast::kind& k, bool force_star);

    public:
        static bool is_polarity_less(ast::polarity_type pol_spec, ast::polarity_type pol_gen);
};

};};};