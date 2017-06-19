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

#include "local_def_substitution.h"
#include "semantics/semantics.h"

namespace arrow { namespace sema
{

class visit_definitions : public ast::traversal_visitor<visit_definitions>
{
    private:
        using base_type = ast::traversal_visitor<visit_definitions>;

    private:
        local_def_substitution& m_defs;

    public:
        visit_definitions(local_def_substitution& defs)
            :m_defs(defs)
        {};

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast)
        {
            return base_type::eval(ast);
        }

    public:
        void eval(const ast::term_definition& ast)
        {
            const ast::identifier& id   = ast.get_identifier();
            m_defs.add_definition(id);
        };
        
        void eval(const ast::type_definition& ast)
        {
            (void)ast;
        }
        void eval(const ast::type_definition_var& ast)
        {
            (void)ast;
        }
        void eval(const ast::term_use& ast)
        {
            (void)ast;
        }
};

//------------------------------------------------------------------
//              local_def_substitution
//------------------------------------------------------------------
local_def_substitution::local_def_substitution(caller_type* owner, 
                    const ast::location& loc)
    :m_caller(owner), m_loc(loc)
{};
       
ast::definitions local_def_substitution::process_definitions(const ast::definitions& defs)
{
    visit_definitions vis(*this);

    long n  = defs.size();

    for (long i = 0; i < n; ++i)
    {
        const ast::definition& d    = defs.get_definition(i);    
        vis.visit(d);
    }

    return this->make();
};

void local_def_substitution::add_definition(const ast::identifier& id)
{
    ast::identifier new_id = sema::copy(id);
    m_items.push_back(item_info(id, new_id));
};

ast::definitions local_def_substitution::make()
{
    //process symbol names first
    for (const auto& item : m_items)
    {
        const ast::identifier& old_id   = item.first;
        const ast::identifier& new_id   = item.second;

        m_caller->add_name_substitution(old_id, new_id);
    };

    ast::definitions ret(m_loc);

    for (auto& item : m_items)
    {
        const ast::identifier& old_id   = item.first;
        const ast::identifier& new_id   = item.second;
        ast::identifier_term idt        = ast::identifier_term(old_id->get_location(), old_id);

        bool has_errors1, has_errors2;
        bool overloaded;
        ast::type old_ty                = m_caller->get_symbol_type(idt, overloaded, has_errors1);
        ast::term old_term              = m_caller->get_symbol_term(idt, overloaded, has_errors2);

        ast::type new_ty;
        ast::term new_term;

        if (old_ty)
        {
            if (has_errors1 == false)
                new_ty                  = m_caller->make_type(old_id, old_ty, overloaded);
            else
                new_ty                  = ast::error_type(old_id->get_location());
        }

        if (old_term)
        {
            if (has_errors2 == false)
                new_term                = m_caller->make_term(old_id, old_term, overloaded);
            else
                new_term                = ast::error_term(old_id->get_location());
        };

        ast::term_definition td         = ast::term_definition(old_id->get_location(), 
                                            new_id, new_ty, new_term, overloaded);

        m_caller->make_definition(old_id, td);
        ret.add(td);
    };

    return ret;
};

void local_def_substitution::finish()
{
    for (const auto& item : m_items)
    {
        const ast::identifier& old_id   = std::get<0>(item);
        m_caller->remove_name_substitution(old_id);
    };
};

};};