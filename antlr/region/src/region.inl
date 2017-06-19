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

#include "utils.h"

namespace region
{

namespace details
{
    template<bool ispod, class T>
    struct alloc_ptr
    {
        static void* eval(region* reg)
        {
            return reg->m_alloc.allocate<sizeof(T)>(get_deleter<T>());
        };
    };
    template<bool ispod, class T>
    struct alloc_array
    {
        static void* eval(region* reg, size_t total_size)
        {
            size_t* ptr = (size_t*)reg->m_alloc.allocate(total_size + sizeof(size_t),get_array_deleter<T>());
            ptr[0] = total_size;
            ++ptr;
            return ptr;
        };
    };
    template<class T>
    struct alloc_ptr<true,T>
    {
        static void* eval(region* reg)
        {
            return reg->m_alloc_pod.allocate<sizeof(T)>();
        };
    };
    template<class T>
    struct alloc_array<true,T>
    {
        static void* eval(region* reg, size_t total_size)
        {
            return reg->m_alloc_pod.allocate(total_size);
        };
    };

    template<bool ispod, class T>
    struct unregister_last_ptr
    {
        static void eval(region* reg, void* ptr)
        {
            (void)reg;
            size_t* ptr_s = static_cast<size_t*>(ptr);
            --ptr_s;
            *ptr_s = 0;
        };
    };
    template<bool ispod, class T>
    struct unregister_last_array
    {
        static void eval(region* reg, void* ptr)
        {
            size_t* ptr_s = static_cast<size_t*>(ptr);
            --ptr_s;
            --ptr_s;
            *ptr_s = 0;
        };
    };
    template<class T>
    struct unregister_last_ptr<true,T>
    {
        static void eval(region* reg, void* ptr) {};
    };
    template<class T>
    struct unregister_last_array<true,T>
    {
        static void eval(region* reg, void* ptr) {};
    };
};

template<class T> 
inline void* region::alloc()
{
    static const bool ispod = details::has_trivial_destructor<T>::value;
    return details::alloc_ptr<ispod,T>::eval(this);
};
template<class T> 
inline void* region::alloc_array(size_t total_size)
{
    static const bool ispod = details::has_trivial_destructor<T>::value;
    return details::alloc_array<ispod,T>::eval(this,total_size);
};
template<class T> 
inline void  region::free(void* ptr)
{
    static const bool ispod = details::has_trivial_destructor<T>::value;
    details::unregister_last_ptr<ispod,T>::eval(this,ptr);
};
template<class T> 
inline void  region::free_array(void* ptr)
{
    static const bool ispod = details::has_trivial_destructor<T>::value;
    details::unregister_last_array<ispod,T>::eval(this,ptr);
};

};
