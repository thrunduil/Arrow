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

#include "lib_paths.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace arrow 
{

lib_paths::lib_paths()
{
    init();
};

void lib_paths::add_lib_path(const std::string& path)
{
    m_dir_list.push_back(path);
};

void lib_paths::init()
{
    add_lib_path("./lib");
};

std::string lib_paths::default_extension()
{
    return ".f";
};

std::string lib_paths::get_module_path(const std::string& import, bool& found) const
{
    namespace fs    = boost::filesystem;    
    found           = false;

    fs::path fp(import);

    if (fp.has_extension() == false)
        fp.replace_extension(default_extension());

    if (exists(fp) == true)
    {
        found = true;
        return fp.string();
    };

    for (const std::string& dir : m_dir_list)
    {
        fs::path fp_dir = fs::path(dir) / fp;

        if (exists(fp_dir) == true)
        {
            found = true;
            return fp_dir.string();
        };
    };

    return "";
};

};
