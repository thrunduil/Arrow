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

#include <memory>
#include <string>

#pragma warning(push)
#pragma warning(disable: 4251) // needs to have dll-interface to be used by

namespace arrow
{

/// this class pops error scope at scope exit
class ARROW_EXPORT error_handler_scope
{
    private:
        error_handler_scope(error_handler* bm);
        friend error_handler;

    private:
        error_handler*  m_owner;

    public:
        ~error_handler_scope();

        error_handler_scope(error_handler_scope&&);

        error_handler_scope(const error_handler_scope&) = delete;
        error_handler_scope& operator=(const error_handler_scope&) = delete;
};

/// handle errors and warnings; default implementation prints 
/// errors on given stream
class ARROW_EXPORT error_handler
{
    private:
        using location      = ast::location;
        using error_scope   = std::vector<bool>;

        friend error_handler_scope;

    private:
        std::ostream*   m_os;
        bool            m_has_errors;
        error_scope     m_error_scope;

    public:
        error_handler(std::ostream* os);

        virtual ~error_handler(){};

        /// store old error flag; and set error flag to false
        /// at scope exit old error flags are restored and error flag 
        /// is set to true if there are errors in this new scope
        error_handler_scope push_error_scope();

        virtual void        report_parser_error(const location& loc, const std::string& msg);
        virtual void        report_parser_warning(const location& loc, const std::string& msg);

        virtual bool        has_errors() const;
        virtual void        clear_errors();

        /// set error flag without reporting an error (for example when
        /// error_term is encountered
        void                add_error();

    private:
        void                pop_error_scope();        
};

using error_handler_ptr = std::shared_ptr<error_handler>;

};


#pragma warning(pop)