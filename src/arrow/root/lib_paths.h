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

#include <string>
#include <list>

namespace arrow 
{

class lib_paths
{
    private:
        using string_list   = std::list<std::string>;

    private:
        string_list     m_dir_list;

    public:
        lib_paths();

        void            add_lib_path(const std::string& path);

        std::string     get_module_path(const std::string& import, bool& found) const;

    private:
        void            init();
        static std::string
                        default_extension();
};

};
