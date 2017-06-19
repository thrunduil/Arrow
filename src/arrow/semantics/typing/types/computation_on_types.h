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
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "scopes/module.h"

namespace arrow { namespace typing
{

namespace details
{
    class type_substitution_impl;
};

/// expand type function given by name and instantiate
/// kind variables
class instantiate_type_function
{
    public:
        using kind_vec      = std::vector<ast::kind>;

    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        inst_collector*     m_collector;

    public:
        instantiate_type_function(const error_handler_ptr& eh, const module& mod,
                                  inst_collector* col);

        ast::type           make(const ast::identifier_type& id, const kind_vec& arg, 
                                bool& expanded, bool& instatiated_kind_vars,
                                bool show_error, kind_subs_map& subs);
};

class type_substitution
{
    private:
        using impl_type     = details::type_substitution_impl;
        using impl_ptr      = std::shared_ptr<impl_type>;

    private:
        impl_ptr            m_impl;

    public:    
        type_substitution(const error_handler_ptr& eh, const module& mod);
        ~type_substitution();

        /// add substitution var -> ty; 
        /// if make_fresh_binders = true, then fresh binders are used in type ty
        /// otherwise ty must have fresh binders
        void        add_type(const ad::var_info& var, const ast::type& ty, 
                        bool make_fresh_binders);

        void        add_kind(const ad::var_info& var, const ast::kind& ki);

        /// add substitution var -> ty; 
        /// if make_fresh_binders = true, then fresh binders are used in type ty,
        /// otherwise ty must have fresh binders;
        /// if var is already substituted then true is returned and old_subs is 
        /// the old substitution
        bool        add_type(const ad::var_info& var, const ast::type& ty, 
                        bool make_fresh_binders, ast::type& old_subs);

        /// remove substitution
        void        remove_type(const ad::var_info& var);

        /// return number of substitutions
        long        size_type() const;
        long        size_kind() const;
        long        size_all() const;

        /// remove all substitutions;
        void        clear() const;

        /// apply substitutions in body
        ast::type   make(const ast::type& body) const;
        ast::kind   make_kind(const ast::kind& body) const;

        ast::type   make(const ast::type& body, bool& substituted) const;
};

class type_substitution_head
{
    public:
        using subs_map  = std::map<ad::var_info, ast::type>;

    public:    
        /// make substitution on head position in body based on
        /// substututions stored in subs map; return true if any substitution
        /// is performed; it is assumed, that types in subs map have fresh binders
        bool        make(const subs_map& subs, ast::type& body) const;
};

/// unfold mu type until head is not a mu type
class unfold_rec_type
{
    private:
        error_handler_ptr   m_eh;
        module              m_mod;

    public:    
        unfold_rec_type(const error_handler_ptr& eh, const module& mod)
            :m_eh(eh), m_mod(mod)
        {};
        
        /// if single_step == true, then only one step of unrolling
        /// is performed; otherwise mu is reduced to the weak head normal 
        /// form
        ast::type   make(ast::type mu, bool single_step);
};

};};