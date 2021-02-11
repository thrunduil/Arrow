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

namespace arrow { namespace typing
{

class check_mu_vis;

class check_mu_type
{
    private:        
        using ptr_type  = std::shared_ptr<check_mu_vis>;

    private:
        ptr_type    m_ptr;

    public:
        check_mu_type(const error_handler_ptr& eh);
        ~check_mu_type();

        /// clear visited mu types and internal data
        void        clear();

        /// return true if this type way already checked
        bool        is_checked(const ast::mu_type& mu) const;

        /// returns true if mu_type passed the test
        bool        make(const ast::mu_type& mu, bool show_error);
};

};};