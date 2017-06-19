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

#include "arrow/ast/ast.h"
#include <sstream>
#include "boost/functional/hash.hpp"
#include "mmlib_internals/hash_table/object_table.inl"
#include "mmlib_internals/hash_table/hash_equal.inl"

namespace arrow { namespace ast
{

//-----------------------------------------------------------
//                  identifier_impl
//-----------------------------------------------------------

class identifier_impl 
{
    private:
        using string_t  = std::string;

        size_t          m_hash;
        string_t        m_data;

    public:
        explicit identifier_impl(const string_t& val);

        ~identifier_impl();

        const string_t& get_data() const    { return m_data; };
        size_t          get_size() const    { return m_data.size(); };

        static size_t   eval_hash(const std::string& data);
        std::size_t     hash_value() const;  
        bool            equal(const std::string& data) const;
};

inline identifier_impl::identifier_impl(const string_t& val)
{
    m_hash      = eval_hash(val);
    m_data      = val;
};

inline size_t identifier_impl::eval_hash(const std::string& val)
{
    size_t size = val.size();
    size_t seed = size;

    for (size_t i = 0; i < size; ++i)
        boost::hash_combine(seed,val[i]);

    return seed;
};

inline bool identifier_impl::equal(const std::string& val) const
{
    return m_data == val;
};

inline std::size_t identifier_impl::hash_value() const
{
    return m_hash;
};

//---------------------------------------------------------------
//                  identifier_table
//---------------------------------------------------------------

template<class T>
struct ptr_traits
{
    static void check_free(const T*) {};
    static void copy(const T*) {};
};

struct identifier_ptr
{
    using value_type    = identifier_impl*;
    using type_traits   = ptr_traits<identifier_impl>;

    identifier_impl*    m_ptr;

    static identifier_ptr   make(identifier_impl* p) { return identifier_ptr{p}; };
};

class identifier_table
{
    private:        
        using string_hash   = mmlib::details::obj_hasher<identifier_impl>;
        using string_equal  = mmlib::details::obj_equaler<identifier_impl>;
        using string_table  = mmlib::details::object_table<identifier_ptr,string_hash,string_equal,
                                                mmlib::details::default_allocator>;

    private:
        string_table        m_table;

    public:
        identifier_ptr      get(const std::string& val);
};

identifier_ptr identifier_table::get(const std::string& val)
{ 
    return m_table.get(val); 
};

static identifier_table m_id_table;

unique_string::unique_string(const std::string& str)
{
    m_value = &m_id_table.get(str).m_ptr->get_data();
};

const std::string& unique_string::get_string() const
{
    static std::string empty_string;
    return (m_value)? *m_value : empty_string;
}

location::location()
    :m_line(-1), m_column(-1)
{};

location::location(long line, long col, unique_string file)
    :m_line(line), m_column(col), m_file(file)
{};

void location::print(std::ostream& os, bool disp_file) const
{
    if (m_file && disp_file)
    {
        os << "file: " << m_file.get_string() << ", line " << m_line << ":" << m_column;
    }
    else
    {
        os << "line: " << m_line << ":" << m_column;
    };
}
std::string location::to_string(bool disp_file) const
{
    std::ostringstream os;
    print(os, disp_file);
    return os.str();
}

};};
