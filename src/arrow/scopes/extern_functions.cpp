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
#include "extern_functions.h"
#include "grammar/unicode.h"
#include "error/error.h"

#include <map>
#include <functional>
#include <limits>

namespace arrow 
{

namespace aad   = arrow::ast::details;

class extern_func_provider_impl
{
    public:
        using extern_function_ptr   = std::shared_ptr<ast::extern_function_eval>;
        using extern_type_ptr       = std::shared_ptr<ast::extern_type_function_eval>;

        using func_map              = std::map<ast::unique_string, extern_function_ptr>;
        using type_func_map         = std::map<ast::unique_string, extern_type_ptr>;
        using fun_int_int_int       = std::function<int64_t (int64_t,int64_t)>;        
        using fun_int_int           = std::function<int64_t (int64_t)>;
        using fun_int_int_bool      = std::function<bool (int64_t,int64_t)>;
        using fun_bool_bool_bool    = std::function<bool (bool,bool)>;
        using fun_bool_bool         = std::function<bool (bool)>;
        using fun_float_float_float = std::function<double (double,double)>;
        using fun_float_float       = std::function<double (double)>;
        using fun_float             = std::function<double ()>;
        using fun_float_float_bool  = std::function<bool (double,double)>;
        using fun_str_str_str       = std::function<std::string (const std::string&,const std::string&)>;
        using fun_str_char_str      = std::function<std::string (const std::string&, int32_t)>;
        using fun_char_str_str      = std::function<std::string (int32_t,const std::string&)>;
        using fun_str_str_bool      = std::function<bool (const std::string&,const std::string&)>;
        using fun_char_char_bool    = std::function<bool (int32_t,int32_t)>;
        using fun_char_char_str     = std::function<std::string (int32_t,int32_t)>;

    private:
        extern_function_ptr m_error;
        extern_type_ptr     m_error_type;
        func_map            m_func_map;
        type_func_map       m_type_func_map;

    private:
        extern_func_provider_impl();

    public:
        static extern_func_provider_impl*  get();

        extern_function_ptr error_function();
        extern_type_ptr     error_type_function();
        extern_function_ptr get(ast::unique_string id) const;
        extern_type_ptr     get_type(ast::unique_string id) const;

    private:
        extern_function_ptr make_error_function() const;
        extern_type_ptr     make_error_type_function() const;
        void                register_function(const std::string& id, const extern_function_ptr& func);
        void                register_type(const std::string& id, const extern_type_ptr& func);
        void                init_functions();
        void                init_type_functions();

        extern_function_ptr make_func_int(const fun_int_int_int& fun) const;
        extern_function_ptr make_func_int(const fun_int_int& fun) const;
        extern_function_ptr make_func_bool(const fun_int_int_bool& fun) const;
        extern_function_ptr make_func_bool2(const fun_bool_bool_bool& fun) const;
        extern_function_ptr make_func_bool2(const fun_bool_bool& fun) const;
        extern_function_ptr make_func_float(const fun_float_float_float& fun) const;
        extern_function_ptr make_func_float(const fun_float_float& fun) const;
        extern_function_ptr make_func_float(const fun_float& fun) const;
        extern_function_ptr make_func_fl_bool(const fun_float_float_bool& fun) const;
        extern_function_ptr make_func_str(const fun_str_str_str& fun) const;
        extern_function_ptr make_func_str_char(const fun_str_char_str& fun) const;
        extern_function_ptr make_func_char_str(const fun_char_str_str& fun) const;
        extern_function_ptr make_func_str_bool(const fun_str_str_bool& fun) const;
        extern_function_ptr make_func_char(const fun_char_char_str& fun) const;
        extern_function_ptr make_func_char_bool(const fun_char_char_bool& fun) const;

        extern_type_ptr     make_type_int(const fun_int_int_int& fun) const;
        extern_type_ptr     make_type_int(const fun_int_int& fun) const;
        extern_type_ptr     make_type_bool(const fun_int_int_bool& fun) const;
        extern_type_ptr     make_type_bool2(const fun_bool_bool_bool& fun) const;
        extern_type_ptr     make_type_bool2(const fun_bool_bool& fun) const;

        extern_type_ptr     make_type_str(const fun_str_str_str& fun) const;
        extern_type_ptr     make_type_str_char(const fun_str_char_str& fun) const;
        extern_type_ptr     make_type_char_str(const fun_char_str_str& fun) const;
        extern_type_ptr     make_type_str_bool(const fun_str_str_bool& fun) const;
        extern_type_ptr     make_type_char(const fun_char_char_str& fun) const;
        extern_type_ptr     make_type_char_bool(const fun_char_char_bool& fun) const;
};

extern_func_provider_impl::extern_func_provider_impl()
{
    init_functions();
    init_type_functions();
};

extern_func_provider_impl* extern_func_provider_impl::get()
{
    static extern_func_provider_impl impl;
    return &impl;
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::get(ast::unique_string id) const
{
    auto pos = m_func_map.find(id);
    if (pos == m_func_map.end())
        return extern_function_ptr();
    else
        return pos->second;
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::get_type(ast::unique_string id) const
{
    auto pos = m_type_func_map.find(id);
    if (pos == m_type_func_map.end())
        return extern_type_ptr();
    else
        return pos->second;
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::error_function()
{
    if (!m_error)
        m_error = make_error_function();

    return m_error;
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::error_type_function()
{
    if (!m_error_type)
        m_error_type = make_error_type_function();

    return m_error_type;
}

class extern_function_error : public ast::extern_function_eval
{
    public:
        virtual ast::term eval(const ast::term_constant& data) const override
        {
            (void)data;
            return ast::error_term(ast::location());
        }

        virtual ast::type get_return_type() const override
        {
            return ast::error_type(ast::location());
        };
        virtual long number_arguments() const override
        {
            return 0;
        }
        virtual ast::type get_arg_type(long i) const override
        {
            (void)i;
            return ast::error_type(ast::location());
        };
};

class extern_type_function_error : public ast::extern_type_function_eval
{
    public:
        virtual ast::type eval(const ast::type_constant& data) const override
        {
            (void)data;
            return ast::error_type(ast::location());
        }

        virtual ast::kind get_return_kind() const override
        {
            return ast::kind_var(ast::location(), false);
        };
        virtual long number_arguments() const override
        {
            return 0;
        }
        virtual ast::kind get_arg_kind(long i) const override
        {
            (void)i;
            return ast::kind_var(ast::location(), false);
        };
};

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_error_function() const
{
    return extern_function_ptr(new extern_function_error());
};

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_error_type_function() const
{
    return extern_type_ptr(new extern_type_function_error());
};

void extern_func_provider_impl::register_function(const std::string& id, const extern_function_ptr& func)
{
    m_func_map[ast::unique_string(id)] = func;
};

void extern_func_provider_impl::register_type(const std::string& id, const extern_type_ptr& func)
{
    m_type_func_map[ast::unique_string(id)] = func;
};

struct string_ty{};
struct char_ty{};

template<class T> struct make_return_type;
template<>        struct make_return_type<int>      { using type = int64_t; };
template<>        struct make_return_type<bool>     { using type = bool; };
template<>        struct make_return_type<double>   { using type = double; };
template<>        struct make_return_type<string_ty>{ using type = std::string; };
template<>        struct make_return_type<char_ty>  { using type = int32_t; };

template<class T> struct make_arg_type;
template<>        struct make_arg_type<int>         { using type = int64_t; };
template<>        struct make_arg_type<bool>        { using type = bool; };
template<>        struct make_arg_type<double>      { using type = double; };
template<>        struct make_arg_type<string_ty>   { using type = const std::string&; };
template<>        struct make_arg_type<char_ty>     { using type = int32_t; };

template<class T> struct make_term_type;
template<>        struct make_term_type<int>        { using type = ast::int_literal; };
template<>        struct make_term_type<bool>       { using type = ast::bool_literal; };
template<>        struct make_term_type<double>     { using type = ast::float_literal; };
template<>        struct make_term_type<string_ty>  { using type = ast::string_literal; };
template<>        struct make_term_type<char_ty>    { using type = ast::char_literal; };

template<class T> struct make_ast_type;
template<>        struct make_ast_type<int>         { using type = ast::int_type; };
template<>        struct make_ast_type<bool>        { using type = ast::bool_type; };
template<>        struct make_ast_type<string_ty>   { using type = ast::string_type; };
template<>        struct make_ast_type<char_ty>     { using type = ast::char_type; };

template<class T> struct get_type_code;
template<> struct get_type_code<ast::int_type>
{ 
    static const aad::type_code value = aad::type_code::int_type; 
};
template<> struct get_type_code<ast::bool_type>
{ 
    static const aad::type_code value = aad::type_code::bool_type; 
};
template<> struct get_type_code<ast::string_type>
{ 
    static const aad::type_code value = aad::type_code::string_type; 
};
template<> struct get_type_code<ast::char_type>
{ 
    static const aad::type_code value = aad::type_code::char_type; 
};

template<class T> struct get_term_code;
template<> struct get_term_code<ast::int_literal>
{ 
    static const aad::term_code value = aad::term_code::int_literal; 
};
template<> struct get_term_code<ast::bool_literal>
{ 
    static const aad::term_code value = aad::term_code::bool_literal; 
};
template<> struct get_term_code<ast::string_literal>
{ 
    static const aad::term_code value = aad::term_code::string_literal; 
};
template<> struct get_term_code<ast::char_literal>
{ 
    static const aad::term_code value = aad::term_code::char_literal; 
};
template<> struct get_term_code<ast::float_literal>
{ 
    static const aad::term_code value = aad::term_code::float_literal; 
};

template<class T> struct make_primitive_type;
template<> struct make_primitive_type<bool>
{ 
    static ast::type eval() 
    { 
        return ast::predefined_type(ast::location(), ast::predefined_type_code::type_bool);
    }; 
};
template<> struct make_primitive_type<int>
{ 
    static ast::type eval() 
    { 
        return ast::predefined_type(ast::location(), ast::predefined_type_code::type_int);
    }; 
};
template<> struct make_primitive_type<double>
{ 
    static ast::type eval() 
    { 
        return ast::predefined_type(ast::location(), ast::predefined_type_code::type_float);
    }; 
};
template<> struct make_primitive_type<string_ty>
{ 
    static ast::type eval() 
    { 
        return ast::predefined_type(ast::location(), ast::predefined_type_code::type_string);
    }; 
};
template<> struct make_primitive_type<char_ty>
{ 
    static ast::type eval() 
    { 
        return ast::predefined_type(ast::location(), ast::predefined_type_code::type_char);
    }; 
};

template<class T> struct make_primitive_kind;
template<> struct make_primitive_kind<bool>
{ 
    static ast::kind eval() 
    { 
        return ast::value_kind(ast::location(), ast::value_kind_code::bool_kind);
    }; 
};
template<> struct make_primitive_kind<int>
{ 
    static ast::kind eval() 
    { 
        return ast::value_kind(ast::location(), ast::value_kind_code::int_kind);
    }; 
};
template<> struct make_primitive_kind<string_ty>
{ 
    static ast::kind eval() 
    { 
        return ast::value_kind(ast::location(), ast::value_kind_code::string_kind);
    }; 
};
template<> struct make_primitive_kind<char_ty>
{ 
    static ast::kind eval() 
    { 
        return ast::value_kind(ast::location(), ast::value_kind_code::char_kind);
    }; 
};

template<class Arg1, class Arg2, class Ret>
class func_2 : public ast::extern_function_eval
{
    private:
        using ret_type              = typename make_return_type<Ret>::type;
        using arg1_type             = typename make_arg_type<Arg1>::type;
        using arg2_type             = typename make_arg_type<Arg2>::type;

        using ret_term              = typename make_term_type<Ret>::type;
        using arg1_term             = typename make_term_type<Arg1>::type;
        using arg2_term             = typename make_term_type<Arg2>::type;
        using func_type             = std::function<ret_type (arg1_type,arg2_type)>;

    private:
        func_type                   m_function;

    public:
        func_2(const func_type& func)
            :m_function(func)
        {};

        virtual ast::term eval(const ast::term_constant& data) const override
        {
            //reverse order
            const ast::term& t1     = data.get_term(1);
            const ast::term& t2     = data.get_term(0);

            if (t1->get_term_code() != get_term_code<arg1_term>::value)
                return data;
            if (t2->get_term_code() != get_term_code<arg2_term>::value)
                return data;

            arg1_term il1           = t1.static_cast_to<arg1_term>();
            arg2_term il2           = t2.static_cast_to<arg2_term>();

            arg1_type v1            = il1.get_value();
            arg2_type v2            = il2.get_value();

            return ret_term(data->get_location(), m_function(v1, v2));
        };

        virtual ast::type get_return_type() const override
        {
            return make_primitive_type<Ret>::eval();
        };

        virtual long number_arguments() const override 
        {
            return 2;
        };
        virtual ast::type get_arg_type(long i) const override
        {
            if (i == 0)
                return make_primitive_type<Arg2>::eval();
            else if (i == 1)
                return make_primitive_type<Arg1>::eval();
            else
                return ast::error_type(ast::location());
        }
};

template<class Arg2, class Ret>
class func_1 : public ast::extern_function_eval
{
    private:
        using ret_type              = typename make_return_type<Ret>::type;
        using arg2_type             = typename make_arg_type<Arg2>::type;

        using ret_term              = typename make_term_type<Ret>::type;
        using arg2_term             = typename make_term_type<Arg2>::type;
        using func_type             = std::function<ret_type (arg2_type)>;

    private:
        func_type                   m_function;

    public:
        func_1(const func_type& func)
            :m_function(func)
        {};

        virtual ast::term eval(const ast::term_constant& data) const override
        {
            const ast::term& t2     = data.get_term(0);

            if (t2->get_term_code() != get_term_code<arg2_term>::value)
                return data;

            arg2_term il2           = t2.static_cast_to<arg2_term>();
            arg2_type v2            = il2.get_value();

            return ret_term(data->get_location(), m_function(v2));
        };

        virtual ast::type get_return_type() const override
        {
            return make_primitive_type<Ret>::eval();
        };

        virtual long number_arguments() const override
        {
            return 1;
        }

        virtual ast::type get_arg_type(long i) const override
        {
            if (i == 0)
                return make_primitive_type<Arg2>::eval();
            else
                return ast::error_type(ast::location());
        }
};

template<class Ret>
class func_0 : public ast::extern_function_eval
{
    private:
        using ret_type              = typename make_return_type<Ret>::type;
        using ret_term              = typename make_term_type<Ret>::type;
        using func_type             = std::function<ret_type ()>;

    private:
        func_type                   m_function;

    public:
        func_0(const func_type& func)
            :m_function(func)
        {};

        virtual ast::term eval(const ast::term_constant& data) const override
        {
            return ret_term(data->get_location(), m_function());
        };

        virtual ast::type get_return_type() const override
        {
            return make_primitive_type<Ret>::eval();
        };

        virtual long number_arguments() const override
        {
            return 0;
        }

        virtual ast::type get_arg_type(long i) const override
        {      
            (void)i;
            return ast::error_type(ast::location());
        }

};

template<class Arg1, class Arg2, class Ret>
class type_func_2 : public ast::extern_type_function_eval
{
    private:
        using ret_type              = typename make_return_type<Ret>::type;
        using arg1_type             = typename make_arg_type<Arg1>::type;
        using arg2_type             = typename make_arg_type<Arg2>::type;

        using ret_ast_type          = typename make_ast_type<Ret>::type;
        using arg1_ast_type         = typename make_ast_type<Arg1>::type;
        using arg2_ast_type         = typename make_ast_type<Arg2>::type;
        using func_type             = std::function<ret_type (arg1_type,arg2_type)>;

    private:
        func_type                   m_function;

    public:
        type_func_2(const func_type& func)
            :m_function(func)
        {};

        virtual ast::type eval(const ast::type_constant& data) const override
        {
            //reverse order
            const ast::type& t1     = data.get_type(1);
            const ast::type& t2     = data.get_type(0);

            if (t1->get_type_code() != get_type_code<arg1_ast_type>::value)
                return data;
            if (t2->get_type_code() != get_type_code<arg2_ast_type>::value)
                return data;

            arg1_ast_type il1       = t1.static_cast_to<arg1_ast_type>();
            arg2_ast_type il2       = t2.static_cast_to<arg2_ast_type>();

            arg1_type v1            = il1.get_value();
            arg2_type v2            = il2.get_value();

            return ret_ast_type(data->get_location(), m_function(v1, v2));
        };

        virtual ast::kind get_return_kind() const override
        {
            return make_primitive_kind<Ret>::eval();
        };

        virtual long number_arguments() const override 
        {
            return 2;
        };
        virtual ast::kind get_arg_kind(long i) const override
        {
            if (i == 0)
                return make_primitive_kind<Arg2>::eval();
            else if (i == 1)
                return make_primitive_kind<Arg1>::eval();
            else
                return ast::kind_var(ast::location(), false);
        }
};

template<class Arg2, class Ret>
class type_func_1 : public ast::extern_type_function_eval
{
    private:
        using ret_type              = typename make_return_type<Ret>::type;
        using arg2_type             = typename make_arg_type<Arg2>::type;

        using ret_ast_type          = typename make_ast_type<Ret>::type;
        using arg2_ast_type         = typename make_ast_type<Arg2>::type;
        using func_type             = std::function<ret_type (arg2_type)>;

    private:
        func_type                   m_function;

    public:
        type_func_1(const func_type& func)
            :m_function(func)
        {};

        virtual ast::type eval(const ast::type_constant& data) const override
        {
            const ast::type& t2     = data.get_type(0);

            if (t2->get_type_code() != get_type_code<arg2_ast_type>::value)
                return data;

            arg2_ast_type il2       = t2.static_cast_to<arg2_ast_type>();
            arg2_type v2            = il2.get_value();

            return ret_ast_type(data->get_location(), m_function(v2));
        };

        virtual ast::kind get_return_kind() const override
        {
            return make_primitive_kind<Ret>::eval();
        };

        virtual long number_arguments() const override
        {
            return 1;
        }

        virtual ast::kind get_arg_kind(long i) const override
        {
            if (i == 0)
                return make_primitive_kind<Arg2>::eval();
            else
                return ast::kind_var(ast::location(), false);
        }
};

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_int(const fun_int_int_int& fun) const
{
    return extern_function_ptr(new func_2<int,int,int>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_int(const fun_int_int& fun) const
{
    return extern_function_ptr(new func_1<int,int>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_bool(const fun_int_int_bool& fun) const
{
    return extern_function_ptr(new func_2<int,int,bool>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_bool2(const fun_bool_bool_bool& fun) const
{
    return extern_function_ptr(new func_2<bool,bool,bool>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_bool2(const fun_bool_bool& fun) const
{
    return extern_function_ptr(new func_1<bool,bool>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_float(const fun_float_float_float& fun) const
{
    return extern_function_ptr(new func_2<double,double,double>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_float(const fun_float_float& fun) const
{
    return extern_function_ptr(new func_1<double,double>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_float(const fun_float& fun) const
{
    return extern_function_ptr(new func_0<double>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_fl_bool(const fun_float_float_bool& fun) const
{
    return extern_function_ptr(new func_2<double,double,bool>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_str(const fun_str_str_str& fun) const
{
    return extern_function_ptr(new func_2<string_ty,string_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_str_char(const fun_str_char_str& fun) const
{
    return extern_function_ptr(new func_2<string_ty,char_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_char_str(const fun_char_str_str& fun) const
{
    return extern_function_ptr(new func_2<char_ty,string_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_str_bool(const fun_str_str_bool& fun) const
{
    return extern_function_ptr(new func_2<string_ty,string_ty,bool>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_char(const fun_char_char_str& fun) const
{
    return extern_function_ptr(new func_2<char_ty,char_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_function_ptr 
extern_func_provider_impl::make_func_char_bool(const fun_char_char_bool& fun) const
{
    return extern_function_ptr(new func_2<char_ty,char_ty,bool>(fun));
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_type_int(const fun_int_int_int& fun) const
{
    return extern_type_ptr(new type_func_2<int,int,int>(fun));
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_type_int(const fun_int_int& fun) const
{
    return extern_type_ptr(new type_func_1<int,int>(fun));
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_type_bool(const fun_int_int_bool& fun) const
{
    return extern_type_ptr(new type_func_2<int,int,bool>(fun));
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_type_bool2(const fun_bool_bool_bool& fun) const
{
    return extern_type_ptr(new type_func_2<bool,bool,bool>(fun));
}

extern_func_provider_impl::extern_type_ptr 
extern_func_provider_impl::make_type_bool2(const fun_bool_bool& fun) const
{
    return extern_type_ptr(new type_func_1<bool,bool>(fun));
}

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_str(const fun_str_str_str& fun) const
{
    return extern_type_ptr(new type_func_2<string_ty,string_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_str_char(const fun_str_char_str& fun) const
{
    return extern_type_ptr(new type_func_2<string_ty,char_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_char_str(const fun_char_str_str& fun) const
{
    return extern_type_ptr(new type_func_2<char_ty,string_ty,string_ty>(fun));
};

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_str_bool(const fun_str_str_bool& fun) const
{
    return extern_type_ptr(new type_func_2<string_ty,string_ty,bool>(fun));
}

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_char(const fun_char_char_str& fun) const
{
    return extern_type_ptr(new type_func_2<char_ty,char_ty,string_ty>(fun));
}

extern_func_provider_impl::extern_type_ptr
extern_func_provider_impl::make_type_char_bool(const fun_char_char_bool& fun) const
{
    return extern_type_ptr(new type_func_2<char_ty,char_ty,bool>(fun));
}

static double double_inf()
{
    return std::numeric_limits<double>::infinity();
};
static double double_nan()
{
    return std::numeric_limits<double>::quiet_NaN();
};

void extern_func_provider_impl::init_functions()
{
    register_function("int_plus",   make_func_int([](int64_t x, int64_t y) { return x+y; }));
    register_function("int_minus",  make_func_int([](int64_t x, int64_t y) { return x-y; }));
    register_function("int_mult",   make_func_int([](int64_t x, int64_t y) { return x*y; }));
    register_function("int_div",    make_func_int([](int64_t x, int64_t y) { return x/y; }));
    register_function("int_uminus", make_func_int([](int64_t x)            { return -x; }));

    register_function("int_cmp_eeq",make_func_bool([](int64_t x, int64_t y) { return x==y; }));
    register_function("int_cmp_neq",make_func_bool([](int64_t x, int64_t y) { return x!=y; }));
    register_function("int_cmp_geq",make_func_bool([](int64_t x, int64_t y) { return x>=y; }));
    register_function("int_cmp_leq",make_func_bool([](int64_t x, int64_t y) { return x<=y; }));
    register_function("int_cmp_gt", make_func_bool([](int64_t x, int64_t y) { return x>y; }));
    register_function("int_cmp_lt", make_func_bool([](int64_t x, int64_t y) { return x<y; }));

    register_function("bool_and",   make_func_bool2([](bool x, bool y)      { return x && y; }));
    register_function("bool_or",    make_func_bool2([](bool x, bool y)      { return x || y; }));
    register_function("bool_xor",   make_func_bool2([](bool x, bool y)      { return (bool)((x && !y) || (!x && y)); }));
    register_function("bool_not",   make_func_bool2([](bool x)              { return !x; }));
    register_function("bool_cmp_eeq",make_func_bool2([](bool x, bool y)     { return x == y; }));
    register_function("bool_cmp_neq",make_func_bool2([](bool x, bool y)     { return x != y; }));

    register_function("float_plus", make_func_float([](double x, double y)  { return x+y; }));
    register_function("float_minus",make_func_float([](double x, double y)  { return x-y; }));
    register_function("float_mult", make_func_float([](double x, double y)  { return x*y; }));
    register_function("float_div",  make_func_float([](double x, double y)  { return x/y; }));
    register_function("float_uminus",make_func_float([](double x)           { return -x; }));
    register_function("float_inf",  make_func_float([]()                    { return double_inf(); }));
    register_function("float_nan",  make_func_float([]()                    { return double_nan(); }));

    register_function("float_cmp_eeq",make_func_fl_bool([](double x, double y) { return x==y; }));
    register_function("float_cmp_neq",make_func_fl_bool([](double x, double y) { return x!=y; }));
    register_function("float_cmp_geq",make_func_fl_bool([](double x, double y) { return x>=y; }));
    register_function("float_cmp_leq",make_func_fl_bool([](double x, double y) { return x<=y; }));
    register_function("float_cmp_gt", make_func_fl_bool([](double x, double y) { return x>y; }));
    register_function("float_cmp_lt", make_func_fl_bool([](double x, double y) { return x<y; }));

    register_function("string_plus", 
        make_func_str([](const std::string& x, const std::string& y)
                    { return x+y; }));

    register_function("string_char_plus", 
        make_func_str_char([](const std::string& x, int32_t y)                   
                    { return x + create_utf8_from_code_point(y); }));

    register_function("char_string_plus", 
        make_func_char_str([](int32_t x, const std::string& y)
                    { return create_utf8_from_code_point(x) + y; }));

    register_function("char_plus", 
        make_func_char([](int32_t x, int32_t y)
                    { return create_utf8_from_code_point(x) + create_utf8_from_code_point(y); }));

    register_function("string_cmp_eeq", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x == y; }));
    register_function("string_cmp_neq", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x != y; }));
    register_function("string_cmp_geq", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x >= y; }));
    register_function("string_cmp_leq", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x <= y; }));
    register_function("string_cmp_gt", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x > y; }));
    register_function("string_cmp_lt", 
        make_func_str_bool([](const std::string& x, const std::string& y)   { return x < y; }));

    register_function("char_cmp_eeq", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x == y; }));
    register_function("char_cmp_neq", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x != y; }));
    register_function("char_cmp_geq", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x >= y; }));
    register_function("char_cmp_leq", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x <= y; }));
    register_function("char_cmp_gt", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x > y; }));
    register_function("char_cmp_lt", 
        make_func_char_bool([](int32_t x, int32_t y)                        { return x < y; }));
};

void extern_func_provider_impl::init_type_functions()
{
    register_type("ty_int_plus",   make_type_int([](int64_t x, int64_t y) { return x+y; }));
    register_type("ty_int_minus",  make_type_int([](int64_t x, int64_t y) { return x-y; }));
    register_type("ty_int_mult",   make_type_int([](int64_t x, int64_t y) { return x*y; }));
    register_type("ty_int_div",    make_type_int([](int64_t x, int64_t y) { return x/y; }));
    register_type("ty_int_uminus", make_type_int([](int64_t x)            { return -x; }));

    register_type("ty_int_cmp_eeq",make_type_bool([](int64_t x, int64_t y) { return x==y; }));
    register_type("ty_int_cmp_neq",make_type_bool([](int64_t x, int64_t y) { return x!=y; }));
    register_type("ty_int_cmp_geq",make_type_bool([](int64_t x, int64_t y) { return x>=y; }));
    register_type("ty_int_cmp_leq",make_type_bool([](int64_t x, int64_t y) { return x<=y; }));
    register_type("ty_int_cmp_gt", make_type_bool([](int64_t x, int64_t y) { return x>y; }));
    register_type("ty_int_cmp_lt", make_type_bool([](int64_t x, int64_t y) { return x<y; }));

    register_type("ty_bool_and",   make_type_bool2([](bool x, bool y)      { return x && y; }));
    register_type("ty_bool_or",    make_type_bool2([](bool x, bool y)      { return x || y; }));
    register_type("ty_bool_xor",   make_type_bool2([](bool x, bool y)      { return (bool)((x && !y) || (!x && y)); }));
    register_type("ty_bool_not",   make_type_bool2([](bool x)              { return !x; }));
    register_type("ty_bool_cmp_eeq",make_type_bool2([](bool x, bool y)     { return x == y; }));
    register_type("ty_bool_cmp_neq",make_type_bool2([](bool x, bool y)     { return x != y; }));

    register_type("ty_string_plus", 
        make_type_str([](const std::string& x, const std::string& y)
                    { return x+y; }));

    register_type("ty_string_char_plus", 
        make_type_str_char([](const std::string& x, int32_t y)                   
                    { return x + create_utf8_from_code_point(y); }));

    register_type("ty_char_string_plus", 
        make_type_char_str([](int32_t x, const std::string& y)
                    { return create_utf8_from_code_point(x) + y; }));

    register_type("ty_char_plus", 
        make_type_char([](int32_t x, int32_t y)
                    { return create_utf8_from_code_point(x) + create_utf8_from_code_point(y); }));

    register_type("ty_string_cmp_eeq", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x == y; }));
    register_type("ty_string_cmp_neq", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x != y; }));
    register_type("ty_string_cmp_geq", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x >= y; }));
    register_type("ty_string_cmp_leq", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x <= y; }));
    register_type("ty_string_cmp_gt", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x > y; }));
    register_type("ty_string_cmp_lt", 
        make_type_str_bool([](const std::string& x, const std::string& y)   { return x < y; }));

    register_type("ty_char_cmp_eeq", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x == y; }));
    register_type("ty_char_cmp_neq", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x != y; }));
    register_type("ty_char_cmp_geq", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x >= y; }));
    register_type("ty_char_cmp_leq", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x <= y; }));
    register_type("ty_char_cmp_gt", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x > y; }));
    register_type("ty_char_cmp_lt", 
        make_type_char_bool([](int32_t x, int32_t y)                        { return x < y; }));
}

extern_func_provider::extern_function_ptr 
extern_func_provider::error_function()
{
    return extern_func_provider_impl::get()->error_function();
};

extern_func_provider::extern_type_ptr 
extern_func_provider::error_type_function()
{
    return extern_func_provider_impl::get()->error_type_function();
};

extern_func_provider::extern_function_ptr 
extern_func_provider::get(ast::unique_string id) const
{
    return extern_func_provider_impl::get()->get(id);
};

extern_func_provider::extern_type_ptr 
extern_func_provider::get_type(ast::unique_string id) const
{
    return extern_func_provider_impl::get()->get_type(id);
};
};
