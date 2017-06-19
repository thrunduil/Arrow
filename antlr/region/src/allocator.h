/* 
 *  This file is a part of Hasky Lib.
 *
 *  Copyright (c) Paweł Kowal 2017
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

#include "region.h"

namespace region
{
    namespace 
    {
        region current_region;
    };
};

template<class T>
inline void* operator new(size_t , region::region& reg, const T* )
{
    return reg.alloc<T>();
};
template<class T>
inline void* operator new [](size_t size, region::region& reg, const T* )
{
    return reg.alloc_array<T>(size);
};
template<class T>
inline void operator delete(void *p, region::region& reg, const T* )
{
    reg.free<T>(p);
};
template<class T>
inline void operator delete[](void *p, region::region& reg, const T* )
{
    reg.free_array<T>(p);
};

 template<class T> const T* get_ptr() {return static_cast<const T*>(NULL); };