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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "scopes/module.h"

namespace arrow { namespace typing
{

/// in type Kind[k1, ..., kn] ty substitute some of kind vars by 
/// given kinds or fresh kinds

class kind_subs_map
{
    private:
        using subs_map      = std::map<ad::var_info, ast::kind>;
        using id_kind       = std::pair<ast::identifier, ast::kind>;
        using inst_vec      = std::vector<id_kind>;

    private:
        subs_map            m_subs_map;
        inst_vec            m_inst_vec;

    public:
        kind_subs_map();

        ast::kind           get_substitution(const ast::identifier& id, bool& has) const;
        void                add_substitution(const ast::identifier& id, const ast::kind& k);
        void                add_instantiation(const ast::identifier& id, const ast::kind_var& k);

        long                number_substitutions() const;
        long                number_instantiations() const;

        /// add kind instantiations (it is assumed, that substitutions are 
        /// already processed)
        ast::term           make_kind_application(const ast::term& ex) const;
        ast::type           make_kind_application(const ast::type& ty) const;

        bool                check_all_infered(const ast::location& loc, const error_handler_ptr& eh, 
                                bool show_errors) const;
};

/// make kind-kind application or type-kind application
class instantiate_kind
{
    private:
        error_handler_ptr   m_eh;
        bool                m_show_errors;

    public:    
        instantiate_kind(const error_handler_ptr& eh, bool show_errors)
            :m_eh(eh), m_show_errors(show_errors)
        {};

        ast::type   make(const ast::type& ty, const std::vector<ast::kind>& args,
                        kind_subs_map& subs, bool& has_errors, inst_collector* col);
        ast::kind   make(const ast::kind& k, const std::vector<ast::kind>& args,
                        kind_subs_map& subs, bool& has_errors);
};

};};