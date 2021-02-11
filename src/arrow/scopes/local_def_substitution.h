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

namespace arrow { namespace sema
{

//process local definitions
class local_def_subs_caller
{
    public:
        virtual ~local_def_subs_caller(){};

        //process function names
        virtual void        add_name_substitution(const ast::identifier& old_id, 
                                const ast::identifier& new_id) = 0;
        virtual void        remove_name_substitution(const ast::identifier& old_id) = 0;

        //process type and term of local definition
        virtual ast::term   make_term(const ast::identifier& old_id, const ast::term& t,
                                bool overloaded) = 0;
        virtual ast::type   make_type(const ast::identifier& old_id, const ast::type& t,
                                bool overloaded) = 0;

        //process definition after processing its type and body
        virtual void        make_definition(const ast::identifier& old_id,
                                const ast::term_definition& new_def) = 0;

        //query type and definition for given local function name
        virtual ast::type   get_symbol_type(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors) = 0;
        virtual ast::term   get_symbol_term(const ast::identifier_term& idt, bool& overloaded, 
                                bool& has_errors) = 0;
};

class local_def_substitution
{
    private:
                            //old id x new id
        using item_info     = std::pair<ast::identifier, ast::identifier>;
        using items_vec     = std::vector<item_info>;
        using caller_type   = local_def_subs_caller;

    private:
        caller_type*        m_caller;
        items_vec           m_items;
        ast::location       m_loc;

    public:
        local_def_substitution(caller_type* owner, const ast::location& loc);

        ast::definitions    process_definitions(const ast::definitions& defs);
        void                finish();

    public:
        void                add_definition(const ast::identifier& id);

    private:        
        ast::definitions    make();
};

};};