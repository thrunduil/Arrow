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

#pragma warning(push)
#pragma warning(disable: 4251) //needs to have dll-interface

namespace arrow { namespace ast
{

//---------------------------------------------------------------
//                      ATOMS
//---------------------------------------------------------------
class ARROW_EXPORT ast_atom
{
    private:
        long                m_refcount;
        location            m_location;

        template<class T>
        friend class details::refcount;

    protected:
        using base_type     = ast_atom;

    public:
        ast_atom(const location& loc);

        virtual ~ast_atom(){};

        const location&     get_location() const    { return m_location; };

    private:
        void                increase_refcount();
        bool                decrease_refcount();
};

class ARROW_EXPORT term_atom : public ast_atom
{
    private:        
        using code_type     = details::term_code;

    protected:
        using base_type     = term_atom;

    private:
        code_type           m_code;

    public:
        term_atom(const location& loc, details::term_code code)
            :ast_atom(loc), m_code(code)
        {};

        details::term_code  get_term_code() const   { return m_code; };
};

class ARROW_EXPORT type_atom : public ast_atom
{
    private:        
        using code_type     = details::type_code;

    protected:
        using base_type     = type_atom;

    private:
        code_type           m_code;

    public:
        type_atom(const location& loc, details::type_code code)
            :ast_atom(loc), m_code(code)
        {};

        details::type_code  get_type_code() const   { return m_code; };
};

class ARROW_EXPORT kind_atom : public ast_atom
{
    private:        
        using code_type     = details::kind_code;

    protected:
        using base_type     = kind_atom;

    private:
        code_type           m_code;

    public:
        kind_atom(const location& loc, details::kind_code code)
            :ast_atom(loc), m_code(code)
        {};

        details::kind_code  get_kind_code() const   { return m_code; };
};

class ARROW_EXPORT definition_atom : public ast_atom
{
    private:        
        using code_type     = details::definition_code;

    protected:
        using base_type     = definition_atom;

    private:
        code_type           m_code;

    public:
        definition_atom(const location& loc, details::definition_code code)
            :ast_atom(loc), m_code(code)
        {};

        details::definition_code
                            get_def_code() const   { return m_code; };
};

};};

#pragma warning(pop)

