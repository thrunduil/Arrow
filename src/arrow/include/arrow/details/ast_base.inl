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

#include "arrow/details/ast_base.h"

#define ARROW_DEBUG_AST 0

namespace arrow { namespace ast { namespace details
{

//----------------------------------------------------------------
//                  refcount
//----------------------------------------------------------------
template<class T>
refcount<T>::refcount()
    :m_ptr(nullptr)
{};

template<class T>
refcount<T>::refcount(T* ptr)
    :m_ptr(ptr)
{
    increase_refcount();
};

template<class T>
refcount<T>::refcount(const refcount& other)
    :m_ptr(other.m_ptr)
{
    increase_refcount();
}

template<class T>
refcount<T>::refcount(refcount&& other)
    :m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

template<class T>
template<class S>
refcount<T>::refcount(const refcount<S>& other)
    :m_ptr(other.m_ptr)
{
    increase_refcount();
}

template<class T>
template<class S>
refcount<T>::refcount(refcount<S>&& other)
    :m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

template<class T>
refcount<T>& refcount<T>::operator=(const refcount& other) &
{
    other.increase_refcount();
    this->decrease_refcount();

    m_ptr   = other.m_ptr;
    return *this;
}

template<class T>
template<class S>
refcount<T>& refcount<T>::operator=(const refcount<S>& other) &
{
    other.increase_refcount();
    this->decrease_refcount();

    m_ptr   = other.m_ptr;
    return *this;
};

template<class T>
refcount<T>& refcount<T>::operator=(refcount&& other) &
{
    std::swap(this->m_ptr, other.m_ptr);
    return *this;
}

template<class T>
template<class S>
refcount<T>& refcount<T>::operator=(refcount<S>&& other) &
{
    std::swap(this->m_ptr, other.m_ptr);
    return *this;
}

template<class T>
refcount<T>::~refcount()
{
    decrease_refcount();
}

template<class T>
void refcount<T>::increase_refcount() const
{
    if (m_ptr)
        m_ptr->increase_refcount();
}

template<class T>
void refcount<T>::decrease_refcount() const
{
    if (m_ptr && m_ptr->decrease_refcount())
        delete m_ptr;
};

template<class T>
T* refcount<T>::operator->()
{
    #if ARROW_DEBUG_AST
        if (m_ptr == nullptr)
            error_null();
    #endif

    return m_ptr;
};

template<class T>
T* refcount<T>::get()
{
    return m_ptr;
};

template<class T>
const T* refcount<T>::get() const
{
    return m_ptr;
};

template<class T>
const T* refcount<T>::operator->() const
{
    #if ARROW_DEBUG_AST
        if (m_ptr == nullptr)
            error_null();
    #endif

    return m_ptr;
};

template<class T>
refcount<T>::operator bool() const
{
    return m_ptr != nullptr;
}

template<class T>
template<class S>
S refcount<T>::static_cast_to() const
{
    using VR  = typename S::value_type;

    return S(static_cast<VR*>(const_cast<value_type*>(this->m_ptr)));
}

}}};

namespace arrow { namespace ast
{

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator==(const ast_base& other) const
{
    return this->m_value.get() == other.m_value.get();
};

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator!=(const ast_base& other) const
{
    return this->m_value.get() != other.m_value.get();
};

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator<=(const ast_base& other) const
{
    return this->m_value.get() <= other.m_value.get();
};

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator>=(const ast_base& other) const
{
    return this->m_value.get() >= other.m_value.get();
};

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator<(const ast_base& other) const
{
    return this->m_value.get() < other.m_value.get();
};

template<class Derived, class Impl, class Mark>
bool ast_base<Derived,Impl,Mark>::operator>(const ast_base& other) const
{
    return this->m_value.get() > other.m_value.get();
};

}}