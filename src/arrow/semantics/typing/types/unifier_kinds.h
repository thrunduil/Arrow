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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/module.h"
#include "kind_inference_context.h"

namespace arrow { namespace typing
{

class bound_map;

class unifier_kinds
{
    private:
        error_handler_ptr   m_eh;
        mutable bool        m_show_errors;

    public:
        unifier_kinds(const error_handler_ptr& eh);

    public:
        bool        is_subkind(const ast::location& loc, const ast::kind& k_spec, 
                        const ast::kind& k_gen, bool show_errors) const;
        bool        unify_kinds(const ast::location& loc, const ast::kind& k_infered, 
                        const ast::kind& k_required, bool show_errors) const;

        static bool is_star_kind_any(const ast::kind& k);
        static bool is_star_star_kind(const ast::kind& k, bool force_star);

        static bool unify_identifiers(const ast::location& loc, const ast::identifier& id1, 
                        const ast::identifier& id2, const bound_map& bm);
        static bool expand_products(ast::product_kind& pk1, ast::product_kind& pk2);

    public:
        bool        unify_with_star_any(const ast::kind& k) const;
        bool        unify_with_star_arrow(const ast::kind& k, ast::kind& lhs, ast::kind& rhs,
                        bool& must_check_star) const;
        bool        unify_with_star_all(const ast::kind& k, ast::kind& var, ast::kind& body,
                        bool star_allowed, bool& must_check_star) const;
        bool        unify_with_star_some(const ast::kind& k, ast::kind& var, ast::kind& body,
                        bool& must_check_star) const;
        bool        unify_with_star_list(const ast::kind& k, ast::kind& body, bool& must_check_star) const;
        bool        unify_with_star_ref(const ast::kind& k, ast::kind& body, bool& must_check_star) const;
        bool        unify_with_star_predefined(const ast::kind& k, ast::predefined_type_code c,
                        bool& must_check_star) const;
        bool        unify_with_star_tuple(const ast::kind& k, long size, 
                        ast::star_tuple_kind& tup, bool& must_check_star) const;        
        bool        unify_with_star_record(const ast::kind& k, const ast::record_type& rec,
                        ast::star_record_kind& rec_kind, bool& must_check_star) const;
        bool        unify_with_star_variant(const ast::kind& k, const ast::variant_type& rec,
                        ast::star_variant_kind& rec_kind, bool& must_check_star) const;

        bool        unify_with_value(const ast::kind& k, ast::value_kind_code code) const;
        bool        is_value_kind_any(const ast::kind& k) const;
        bool        unify_with_arrow(const ast::kind& k, ast::arrow_kind& ak, 
                        ast::polarity_type pol) const;
        bool        unify_with_product(const ast::kind& k, ast::product_kind& pk, 
                        long size) const;
        bool        unify_with_any_product(const ast::kind& k, ast::product_kind& pk, 
                        long req_index) const;

        static bool is_polarity_less(ast::polarity_type pol_spec, ast::polarity_type pol_gen);
};

};};