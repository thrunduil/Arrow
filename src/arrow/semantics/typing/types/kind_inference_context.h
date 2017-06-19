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
#include "scopes/symbol_data.h"

#include <map>
#include <set>

namespace arrow { namespace typing { namespace details
{

namespace ad = arrow::details;

//polarities are addes as in "Fixed Points of Type Constructors
//and Primitive Recursion", Abel and Matthes in order to allow
//for recursive type constructors

//------------------------------------------------------------------
//              kind_inference_context
//------------------------------------------------------------------
class kind_inference_context;

class kind_pol
{
    private:
        using polarity      = ast::polarity_type;        

    public:
        ast::kind           m_kind;
        polarity            m_pol_orig;
        polarity            m_pol_current;
        long                m_discard_count;

    public:
        kind_pol();
        kind_pol(const ast::kind& k, polarity pol);

        void                negate();
        void                discard_polarized();
        void                restore_polarized();
        polarity            get_original_polarity() const;
        polarity            get_current_polarity(bool& is_visible) const;
        const ast::kind&    get_kind() const;
};

class kind_inference_scope
{
    public:
        enum oper_type
        {
            negate_polarities,
            neutral_polarities,
            remove_var,
            restore_var,
            restore_type_subs
        };

    private:
        kind_inference_context* m_owner;

        ast::polarity_type      m_old_polarity;
        ad::var_info            m_var;
        kind_pol                m_old_elem;
        ast::type               m_old_elem_type;
        oper_type               m_oper;

    private:
        kind_inference_scope(kind_inference_context* owner, ast::polarity_type old,
                             oper_type oper);
        kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi);
        kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi, 
                       const kind_pol& item);
        kind_inference_scope(kind_inference_context* owner, const ad::var_info& vi,
                        const ast::type& ty);
        kind_inference_scope();

        friend kind_inference_context;

    public:
        ~kind_inference_scope();

        kind_inference_scope(const kind_inference_scope&) = delete;
        kind_inference_scope& operator=(const kind_inference_scope&) = delete;

        kind_inference_scope(kind_inference_scope&& other);
};

class kind_inference_context
{
    private:
        using polarity      = ast::polarity_type;        
        using var_map       = std::map<ad::var_info, kind_pol>;
        using subs_map      = std::map<ad::var_info, ast::type>;
        using var_set       = std::set<ad::var_info>;
        using scope_type    = kind_inference_scope;
        using vec_ident     = std::vector<ast::identifier>;

        //Kind args + body + kind to check x symbol name
        using gen_item_k    = std::tuple<vec_ident, ast::kind, ast::kind, ad::var_info>;
        using gen_item_k_vec= std::vector<gen_item_k>;

        friend kind_inference_scope;

    private:
        var_map             m_vars;
        var_set             m_recursive;
        subs_map            m_type_subs;
        gen_item_k_vec      m_gen_items_k;
        polarity            m_polarity_state;

    public:
        kind_inference_context();

        void            clear();

        void            add_generalize_kind(const vec_ident& args, const ast::kind& k_body, 
                            const ast::kind& k_check, const ad::var_info& sym);
        void            add_recursive_symbol(const ad::var_info& sym);
        void            remove_recursive_symbol(const ad::var_info& sym);
        scope_type      add_var(const ast::identifier& id, const ast::kind& k, ast::polarity_type pol);

        /// it is assumed, that ty has fresh binders
        scope_type      add_type_subs(const ast::identifier& id, const ast::type& ty);
        
        scope_type      negate_polarities();
        scope_type      mult_polarities(ast::polarity_type pol);

        polarity        mult_by_state(polarity pol) const;

        ast::polarity_type
                        get_symbol_polarity(const ast::identifier& id, ast::kind& k,
                            ast::polarity_type& orig_pol, bool& is_binder, bool& is_rec,
                            bool& is_visible) const;

        const gen_item_k_vec&  
                        get_generalize_items_kind() const;

        bool            make_substitution(ast::type& t) const;

        static polarity mult_polarities(polarity pol1, polarity pol2);
        static polarity negate(polarity pol);

    private:                
        void            remove_var(const ad::var_info& vi);
        void            restore_var(const ad::var_info& vi, const kind_pol& old);
        void            restore_type_subs(const ad::var_info& vi, const ast::type& old);
        void            restore_negate_polarities();
        void            restore_neutral_polarities(polarity old);
};

};};}