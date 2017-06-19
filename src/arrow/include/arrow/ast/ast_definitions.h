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

#include "ast_helpers.h"
#include "ast_atoms.h"

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      DEFINITIONS
//---------------------------------------------------------------
class ARROW_EXPORT definition 
    : public ast_base<definition, definition_atom, definition_mark>
{
    private:
        definition(const impl_ptr& ip);

    public:
        definition();
        definition(const term_definition& def);
        definition(const type_definition& def);
        definition(const type_definition_var& def);
        definition(const fixity_definition& def);
        
        definition(const term_use& def);

        template<class Ast>
        Ast                 static_cast_to() const;
};

class ARROW_EXPORT term_definition
    : public ast_base<term_definition, details::term_definition_impl, definition_mark>
{
    private:
        term_definition(const impl_ptr& ip);
        friend definition;

    public:
        term_definition();
        term_definition(const location& loc, const identifier& id, const type& t, 
                        const term& e, bool overload);

        const identifier&   get_identifier() const;
        const type&         get_type() const;
        const term&         get_body() const;
        bool                is_overload() const;
};

class ARROW_EXPORT term_use
    : public ast_base<term_use, details::term_use_impl, definition_mark>
{
    private:
        term_use(const impl_ptr& ip);
        friend definition;

    public:
        term_use();
        term_use(const location& loc, const term& e);

        const term&         get_term() const;
};

class ARROW_EXPORT type_definition
    : public ast_base<type_definition, details::type_definition_impl, definition_mark>
{
    private:
        type_definition(const impl_ptr& ip);
        friend definition;

    public:
        type_definition();
        type_definition(const location& loc, const identifier& id, const kind& ki, 
                        const type& t, bool overload);

        const identifier&   get_identifier() const;
        const kind&         get_kind() const;
        const type&         get_body() const;
        bool                is_overload() const;
};

class ARROW_EXPORT type_definition_var
    : public ast_base<type_definition_var, details::type_definition_var_impl, definition_mark>
{
    private:
        type_definition_var(const impl_ptr& ip);
        friend definition;

    public:
        using kind_id_vec   = std::vector<ast::identifier>;

    public:
        type_definition_var();
        type_definition_var(const location& loc, const identifier& id, const kind_id_vec& kind_vars,
                            bool overload);

        void                add_var(const annotated_type_identifier& id, polarity_type pol);
        void                set_body(const type& t);
        void                set_return_kind(const kind& k);

        const identifier&   get_identifier() const;
        const kind_id_vec&  get_kind_vars() const;
        const type&         get_body() const;
        const kind&         get_return_kind() const;
        long                var_size() const;
        bool                is_overload() const;

        const annotated_type_identifier&
                            get_arg(long pos) const;
        polarity_type       get_polarity(long pos) const;
};

class ARROW_EXPORT fixity_definition
    : public ast_base<fixity_definition, details::fixity_definition_impl, definition_mark>
{
    private:
        fixity_definition(const impl_ptr& ip);
        friend class definition;

    public:
        fixity_definition();

        fixity_definition(const location& loc, const ast::operator_info& p,
                          const symbol_sequence& syms);

        const operator_info&    get_operator_type() const;
        const symbol_sequence&  get_symbols() const;
};

};};

#pragma warning(pop)
