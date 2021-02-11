/* 
 *  This file is a part of Hasky Lib.
 *
 *  Copyright (c) Paweł Kowal 2017 - 2021
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

#include "region_allocator.h"
#include "region_allocator_pod.h"

namespace region
{

namespace details
{
    template<bool ispod, class T> struct alloc_ptr;
    template<bool ispod, class T> struct alloc_array;
};

class region
{
    private:
        allocator               m_alloc;
        allocator_pod           m_alloc_pod;

    public:
        region();
        ~region();

        template<class T> void* alloc();
        template<class T> void* alloc_array(size_t total_size);
        template<class T> void  free(void* ptr);
        template<class T> void  free_array(void* ptr);

    private:
        region(const region&);
        region& operator=(const region&);

        template<bool ispod, class T> friend struct details::alloc_ptr;
        template<bool ispod, class T> friend struct details::alloc_array;
};

};

#include "region.inl"
