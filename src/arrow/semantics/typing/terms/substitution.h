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

#include "arrow/config.h"
#include "scopes/symbol_data.h"
#include "scopes/module.h"
#include "semantics/typing/types/bound_map.h"

#include <map>
#include <list>

namespace arrow { namespace typing
{

namespace ad = arrow::details;

class var_subs_info
{
    public:
        struct fresh_binders
        {
            ast::identifier_type    m_binder;

            void set_binder(const ast::identifier_type& bind)
            {
                m_binder = bind;
            };

            const ast::identifier_type&
                            get_binder() const { return m_binder; };
        };

        enum class deduction_state
        {
            initialized, visited, infered
        };

        using annot_id      = ast::annotated_type_identifier;

    private:
        ast::type_var       m_type_var;
        bool                m_is_bound;
        annot_id            m_id;
        fresh_binders       m_binders;
        deduction_state     m_state;

    public:
        var_subs_info(const ast::type_var& tv, const annot_id& id);
        var_subs_info(const annot_id& id, const fresh_binders& fb);

        const ast::type_var&    get_type_var() const;
        const annot_id&         get_identifier() const;
        bool                    is_bound() const;        
        const ast::identifier_type&
                                get_binder() const;
        bool                    is_visited() const;
        bool                    is_infered() const;

        void                    set_binder(const ast::identifier_type& bind);
        void                    mark_visited();
        void                    mark_infered();
};

class var_substitution
{
    public:
        using subs_map      = std::map<ad::var_info, var_subs_info>;

    private:
        using iterator      = subs_map::iterator;        
        using subs_vec      = std::vector<iterator>;
        using context_type  = details::kind_inference_context;
        using delay_info    = std::tuple<ast::type_var, ast::type, bound_map*>;
        using delay_vec     = std::vector<delay_info>;
        using bound_map_list= std::list<bound_map>;

    private:
        subs_map            m_map;
        subs_vec            m_vec;
        module              m_mod;
        delay_vec           m_delays;
        bound_map_list      m_contexts;
        mutable long        m_var_ind;
        long                n_finalized_delays;

    public:
        var_substitution()
            :m_var_ind(0), n_finalized_delays(0)
        {};

        var_substitution(const module& mod)
            :m_mod(mod), m_var_ind(0), n_finalized_delays(0)
        {};

        long                size() const;

        void                add(const ast::annotated_type_identifier& vi, const ast::type_var& t);
        void                add_free(const ast::annotated_type_identifier& vi);

        ast::type           apply(const ast::type& t) const;
        void                mark_free_variables();

        bool                deduce(const ast::location& loc, const ast::type& subs_type, 
                                const ast::type& target_type, type_equaler& eq, bound_map& bm,
                                const context_type& ctx, bool show_errors);
        void                register_type_var(const ast::type_var& tv);
        void                add_type_var(const ast::type_var& var, const ast::type& ty);
        void                finalize_normalization(bound_map& bm);

        ast::term           generalize(const ast::term& t) const;
        ast::type           generalize_type(const ast::type& t) const;
        ast::term           make_type_application(const ast::term& t);

        bool                check_deduction(const ast::location& loc, const ast::type& ty,
                                type_equaler& eq, const context_type& ctx, const error_handler_ptr& eh, 
                                bool show_errors);

    private:
        bool                process_delayed(const ast::location& loc, const ast::type& ty, 
                                type_equaler& eq, const context_type& ctx, const error_handler_ptr& eh, 
                                bool show_errors);
        void                report_deduction_error(const ast::location& loc, const ast::type& ty,
                                const error_handler_ptr& eh) const;
};

};};
