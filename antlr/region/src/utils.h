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

#include <boost/type_traits.hpp>

namespace region { namespace details
{

static const size_t array_header_size = sizeof(size_t);

template<class T>
struct has_trivial_destructor
{
    static const bool value = boost::has_trivial_destructor<T>::value;
};
struct deleter
{
    virtual void destroy(void* ptr) = 0;
};
template<class T>
struct type_deleter : public deleter
{
    virtual void destroy(void* ptr)
    {
        static_cast<T*>(ptr)->~T();
    };
};
template<class T>
struct type_array_deleter : public deleter
{
    virtual void destroy(void* ptr)
    {
        size_t* ptr2 = static_cast<size_t*>(ptr);
        size_t n_elem = (ptr2[0] - array_header_size) / sizeof(T);
        ++ptr2;
        T* real_ptr = static_cast<T*>((void*)ptr2);
        for(size_t i = 0; i < n_elem; ++i)
        {
            real_ptr->~T();
            ++real_ptr;
        };
    };
};
template<class T> inline deleter* get_deleter()
{
    static type_deleter<T> del;
    return &del;
};
template<class T> inline deleter* get_array_deleter()
{
    static type_array_deleter<T> del;
    return &del;
};
};};
