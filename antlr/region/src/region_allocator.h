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

#include <vector>
#include "utils.h"
#include <stdlib.h>

namespace region
{

namespace details
{

static const size_t max_size = 1024*2;

struct base_block 
{
    virtual ~base_block() {};
    virtual base_block* expand(size_t**) = 0;
    virtual base_block* get_next() = 0;
    virtual size_t* first() = 0;
    virtual void call_deleters(size_t value_size) = 0;
    virtual void call_deleters(size_t value_size, size_t pos) = 0;
};

template<size_t N, bool large>
struct next_block_size_impl          { static const size_t value = 2*N; };
template<size_t N>
struct next_block_size_impl<N,true>  { static const size_t value = N; };

template<size_t N>
struct next_block_size : public next_block_size_impl<N,N>=max_size> {};

inline size_t next_block_size_value(size_t N)
{
    if (N >= max_size)
    {
        return N;
    };
    return 2*N;
};

template<size_t size>
class block : public base_block
{
    private:
        static const size_t next_size = next_block_size<size>::value;

    public:
        size_t              ptrs[size];         //must be first member
        block<next_size>*   next;

    public:

        block()             { next = NULL; };
        ~block()            {};

        virtual base_block* expand(size_t** first)
        {
            next = new block<next_size>();
            *first = next->ptrs;
            return next;
        };
        virtual base_block* get_next()
        {
            return next;
        };
        virtual size_t* first()
        {
            return ptrs;
        };
        virtual void call_deleters(size_t value_size)
        {
            return call_deleters(value_size, size);
        };
        virtual void call_deleters(size_t value_size, size_t pos)
        {
            for(size_t i = 0; i < pos; i += value_size)
            {
                size_t* ptr = &ptrs[i];
                deleter* dis = reinterpret_cast<deleter*>(ptr[0]);
                ptr += 1;
                if (dis)
                {
                    dis->destroy(ptr);
                };
            };
        };
};

template<unsigned size>
class region_blocks
{
    private:        
        static const size_t align_size = sizeof(size_t);
        static const size_t size_header = sizeof(void*);
        static const size_t elem_size = (size + size_header + (align_size-1)) / align_size;

        size_t                  index;
        size_t                  block_size;

        block<elem_size>*       start;
        base_block*             current;
        size_t*                 first;

    public:
        region_blocks()   : index(0),block_size(0) 
        {
            start = NULL;
            current = NULL;
            first = NULL;
        };
        ~region_blocks()
        {
            free_all();
        };

        void* allocate(deleter* dis)
        {
            if(index == block_size)
            {
                expand();
            }
            size_t* retval = first + index;
            index += elem_size;
            retval[0] = reinterpret_cast<size_t>(dis);
            retval += 1;
            return retval;
        }

        void free_all()
        {
            base_block* block = start;
            while(block)
            {
                base_block* ptr = block;
                block = block->get_next();
                if (block)
                {
                    ptr->call_deleters(elem_size);
                }
                else
                {
                    ptr->call_deleters(elem_size,index);
                };
                delete ptr;
            };
            current = NULL;
            start = NULL;
            index = 0;
            first = NULL;
        };

    private:
        void expand()
        {
            if(start == NULL)
            {
                start = new block<elem_size>;
                current = start;
                first = current->first();
                block_size = elem_size;
            }
            else
            {
                current = current->expand(&first);
                index = 0;
                block_size = next_block_size_value(block_size);
            };
        };
};

template<size_t size>
class region_allocator
{
    private:
        region_blocks<size>     blocks;

    public:
        region_allocator() {};

        void* allocate(deleter* dis)
        {
            return blocks.allocate(dis);
        }

        void free_all()
        {
            blocks.free_all();
        };

    private:
        region_allocator(const region_allocator&);
        region_allocator& operator=(const region_allocator&);
};
class region_malloc_allocator
{
    private:
        std::vector<void*> m_ptrs;

    public:
        ~region_malloc_allocator()
        {
            free_all();
        };
        void free_all()
        {
            for(size_t i = 0; i < m_ptrs.size(); ++i)
            {
                size_t* ptr = static_cast<size_t*>(m_ptrs[i]);
                deleter* dis = reinterpret_cast<deleter*>(ptr[0]);
                ptr += 1;
                if (dis)
                {
                    dis->destroy(ptr);
                };
                ::free(m_ptrs[i]);
            };
        };
        virtual void* allocate(size_t size, deleter* dis)
        {
            size_t * ptr = (size_t*)malloc(size + sizeof(void*));
            ptr[0] = reinterpret_cast<size_t>(dis);
            m_ptrs.push_back(ptr);
            ptr += 1;
            return ptr;
        };
};

template<size_t N> struct type_map {};
template<> struct type_map<1> {static const size_t value = 4; typedef region_allocator<value> type;};
template<> struct type_map<2> {static const size_t value = 8; typedef region_allocator<value> type;};
template<> struct type_map<3> {static const size_t value = 12; typedef region_allocator<value> type;};
template<> struct type_map<4> {static const size_t value = 16; typedef region_allocator<value> type;};
template<> struct type_map<5> {static const size_t value = 20; typedef region_allocator<value> type;};
template<> struct type_map<6> {static const size_t value = 24; typedef region_allocator<value> type;};
template<> struct type_map<7> {static const size_t value = 28; typedef region_allocator<value> type;};
template<> struct type_map<8> {static const size_t value = 32; typedef region_allocator<value> type;};
template<> struct type_map<9> {static const size_t value = 48; typedef region_allocator<value> type;};
template<> struct type_map<10> {static const size_t value = 64; typedef region_allocator<value> type;};
template<> struct type_map<11> {static const size_t value = 96; typedef region_allocator<value> type;};
template<> struct type_map<12> {static const size_t value = 128; typedef region_allocator<value> type;};
template<> struct type_map<13> {static const size_t value = 192; typedef region_allocator<value> type;};
template<> struct type_map<14> {static const size_t value = 256; typedef region_allocator<value> type;};
template<> struct type_map<15> {static const size_t value = 384; typedef region_allocator<value> type;};
template<> struct type_map<16> {static const size_t value = 512; typedef region_allocator<value> type;};
template<> struct type_map<17> {static const size_t value = 768; typedef region_allocator<value> type;};
template<> struct type_map<18> {static const size_t value = 1024; typedef region_allocator<value> type;};
template<> struct type_map<19> {static const size_t value = 1024; typedef region_malloc_allocator type;};

template<size_t N>
struct type_vector_impl : public type_vector_impl<N-1>
{
    typedef typename type_map<N>::type type;
    type    m_member;
};
template<>
struct type_vector_impl<0> {};

template<size_t M, size_t N> struct get
{
    static typename type_map<M>::type& eval(type_vector_impl<N>& alloc)
    {
        return get<M,N-1>::eval(alloc);
    };
};

template<size_t N> struct get<N,N>
{
    static typename type_map<N>::type& eval(type_vector_impl<N>& alloc)
    {
        return alloc.m_member;
    };
};


template<size_t N, size_t M>
struct free_all_impl
{
    static void eval(type_vector_impl<M>& alloc)
    {
        get<N,M>::eval(alloc).free_all();
        free_all_impl<N-1,M>::eval(alloc);
    };
};
template<size_t M>
struct free_all_impl<0,M>
{
    static void eval(type_vector_impl<M>& alloc) {(void)alloc;};
};
template<bool select, size_t pos, size_t size, size_t max_pos>
struct size_to_pos_impl { static const size_t value = pos; };
template<size_t pos, size_t size, size_t max_pos>
struct size_to_pos_impl<false,pos,size,max_pos>
{ 
    static const size_t value = 
            size_to_pos_impl<size <= type_map<pos+1>::value,pos+1,size,max_pos>::value; 
};
template<size_t pos, size_t size>
struct size_to_pos_impl<false,pos,size,pos>
{ 
    static const size_t value = pos;
};

template<size_t pos, size_t size, size_t max_pos>
struct size_to_pos : size_to_pos_impl<size <= type_map<pos>::value,pos,size,max_pos>
{};

template<size_t pos, size_t max_pos>
struct allocate_impl
{
    template<class allocator>
    static void* eval(allocator& alloc, size_t M, deleter* dis)
    {
        (void)M;
        return alloc.allocate(dis);
    };
};
template<size_t pos>
struct allocate_impl<pos,pos>
{
    template<class allocator>
    static void* eval(allocator& alloc, size_t M, deleter* dis)
    {
        return alloc.allocate(M,dis);
    };
};

template<size_t pos_start, size_t pos_end, size_t N>
struct allocate_impl2
{
    static void* eval(type_vector_impl<N>& members,size_t size, deleter* dis)
    {
        static const size_t pos_mean = (pos_start + pos_end)/2;
        static const size_t size_mean = type_map<pos_mean>::value;
        if (size < size_mean)
        {
            return allocate_impl2<pos_start,pos_mean,N>::eval(members,size,dis);
        }
        else if (size > size_mean)
        {
            return allocate_impl2<pos_mean+1,pos_end,N>::eval(members,size,dis);
        }
        else
        {
            return allocate_impl2<pos_mean,pos_mean,N>::eval(members,size,dis);
        };
    };
};

template<size_t pos_start, size_t N>
struct allocate_impl2<pos_start,pos_start,N>
{
    static void* eval(type_vector_impl<N>& members,size_t size, deleter* dis)
    {
        return allocate_impl<pos_start,N>::eval(get<pos_start,N>::eval(members),size,dis);
    };
};

template<size_t N>
class type_vector
{
    private:
        type_vector_impl<N>     m_members;

    public:
        template<size_t M>
        void* allocate(deleter* dis)
        {
            static const size_t pos = size_to_pos<1,M,N>::value;
            return allocate_impl<pos,N>::eval(get<pos,N>::eval(m_members),M,dis);
        };
        void* allocate(size_t size, deleter* dis)
        {
            return allocate_impl2<1,N,N>::eval(m_members,size, dis);
        };

        void free_all()
        {
            free_all_impl<N,N>::eval(m_members);
        };
};

};

class allocator
{
    private:
        details::type_vector<19>         m_allocators;

    public:
        template<size_t N>
        void* allocate(details::deleter* dis)
        {
            return m_allocators.allocate<N>(dis);
        }

        void* allocate(size_t size, details::deleter* dis)
        {
            return m_allocators.allocate(size, dis);
        };

        void free_all()
        {
            m_allocators.free_all();
        };

};

};