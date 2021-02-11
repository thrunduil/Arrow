/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "semantics.h"
#include "error/error.h"
#include "arrow/ast/ast_visitor.h"
#include "unique_names_provider.h"
#include "print_type.h"
#include "semantics/utils.h"

#include <sstream>

namespace arrow { namespace sema
{

namespace ad = arrow::details;

static void decompose_kind_applic(const ast::type& ex, ast::type& root, 
                                  std::vector<ast::kind>& args)
{
    if (ex->get_type_code() == ast::details::type_code::type_kind_application)
    {
        ast::type_kind_application tta = ex.static_cast_to<ast::type_kind_application>();
        
        decompose_kind_applic(tta.get_lhs(), root, args);
        args.push_back(tta.get_rhs());
        return;
    }
    else if (ex->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv = ex.static_cast_to<ast::type_var>();
        
        if (tv.is_set() == true)
            return decompose_kind_applic(tv.get(), root, args);
    }

    root = ex;
    return;
};

static void decompose_type_applic(const ast::type& ex, ast::type& root, 
                                  std::vector<ast::type>& args)
{
    if (ex->get_type_code() == ast::details::type_code::type_type_application)
    {
        ast::type_type_application tta = ex.static_cast_to<ast::type_type_application>();
        
        decompose_type_applic(tta.get_type(), root, args);
        args.push_back(tta.get_arg());
        return;
    }
    else if (ex->get_type_code() == ast::details::type_code::type_var)
    {
        ast::type_var tv = ex.static_cast_to<ast::type_var>();
        
        if (tv.is_set() == true)
            return decompose_type_applic(tv.get(), root, args);
    }

    root = ex;
    return;
};

//---------------------------------------------------------------
//                  printer_kind_visitor
//---------------------------------------------------------------
printer_kind_visitor::printer_kind_visitor()
    :m_in_star_kind(false)
{};

void printer_kind_visitor::make(std::ostream& os, const ast::kind& k)
{
    m_in_star_kind  = false;

    int prec;
    visit(k, os, prec);
}

void printer_kind_visitor::eval(const ast::star_arrow_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
    {
        os << "*{ ";
    };

    const ast::kind& k1_l      = ast.get_lhs();
    const ast::kind& k1_r      = ast.get_rhs();

    std::ostringstream os2;
    int prec2;

    visit(k1_l, os2, prec2);

    if (prec2 == prec_atom)
        os << os2.str();
    else
        os << "(" << os2.str() << ")";

    os << " -> ";
             
    visit(k1_r, os, prec);    

    if (add_star == true)
    {
        os << "}";
        prec    = prec_atom;
    }
    else
    {
        prec = prec_arrow;
    }
};

void printer_kind_visitor::eval(const ast::star_tuple_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    os << "(";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        int prec_loc;
        visit(ast.get(i), os, prec_loc);
    };

    os << ")";

    if (add_star == true)
        os << "}";        

    prec    = prec_atom;
};

void printer_kind_visitor::eval(const ast::star_all_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    std::vector<ast::kind> args;
    ast::kind body;
    decompose_all_kind(ast, args, body);

    os << "All [";

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        visit(args[i], os, prec);
    };

    os << "] ";

    visit(body, os, prec);

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_all;
    };
};

void printer_kind_visitor::eval(const ast::star_some_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    std::vector<ast::kind> args;
    ast::kind body;
    decompose_some_kind(ast, args, body);

    os << "Some [";

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        visit(args[i], os, prec);
    };

    os << "] ";

    visit(body, os, prec);

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_all;
    };
};

void printer_kind_visitor::eval(const ast::star_predefined_kind& ast, std::ostream& os, int& prec)
{    
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    if (add_star == true)
        os << "*{";

    ast::predefined_type_code code  = ast.get_code();
    print_predefined_type(os, code);

    if (add_star == true)
        os << "}";

    prec = prec_atom;
};

void printer_kind_visitor::eval(const ast::star_list_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    const ast::kind& body   = ast.get_body();

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    os << "list [";    

    visit(body, os, prec);

    os << "]";

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_atom;
    };
};
void printer_kind_visitor::eval(const ast::star_ref_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    const ast::kind& body   = ast.get_body();

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    os << "Ref ";    

   {
        int prec1;
        std::ostringstream os1;
        visit(body, os1, prec1);

        if (prec1 < prec_prefix)
            os << "(" << os1.str() << ")";
        else
            os << os1.str();
    }

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_prefix;
    };
};

void printer_kind_visitor::eval(const ast::star_kind&, std::ostream& os, int& prec)
{
    prec    = prec_atom;
    os << "*";
};

void printer_kind_visitor::eval(const ast::star_record_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    os << "Record {";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        const ast::identifier& ai   = ast.get_identifier(i);
        const ast::kind& ki         = ast.get_kind(i);

        int prec_loc;
        print_identifier(ai, os);

        os << " = ";

        visit(ki, os, prec_loc);
    };

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_atom;
    };
};

void printer_kind_visitor::eval(const ast::star_variant_kind& ast, std::ostream& os, int& prec)
{
    if (ast.is_star() == true)
    {
        os << "*";
        prec = prec_atom;
        return;
    };

    bool add_star   = false;

    if (m_in_star_kind == false)
        add_star    = true;

    sema::scoped_var<bool> scope(m_in_star_kind, true);

    if (add_star == true)
        os << "*{ ";

    os << "Variant {";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        const ast::identifier& ai   = ast.get_identifier(i);
        const ast::kind& ki         = ast.get_kind(i);

        int prec_loc;
        print_identifier(ai, os);

        os << " = ";

        visit(ki, os, prec_loc);
    };

    if (add_star == true)
    {
        os << "}";        
        prec    = prec_atom;
    }
    else
    {
        prec    = prec_atom;
    };
};

void printer_kind_visitor::eval(const ast::kind_var& ast, std::ostream& os, int& prec)
{
    if (ast.is_set() == true)
        return visit(ast.get(), os, prec);

    os << "?";
    prec = prec_atom;
};

void printer_kind_visitor::eval(const ast::product_kind& ast, std::ostream& os, int& prec)
{
    os << "{";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        int prec_loc;
        visit(ast.get(i), os, prec_loc);
    };

    os << "}";
    prec    = prec_atom;
};

void printer_kind_visitor::eval(const ast::arrow_kind& ast, std::ostream& os, int& prec)
{
    const ast::kind& k1_l      = ast.get_lhs();
    const ast::kind& k1_r      = ast.get_rhs();

    ast::polarity_type akt     = ast.get_type();

    std::ostringstream os2;
    int prec2;

    visit(k1_l, os2, prec2);

    if (prec2 == prec_atom)
        os << os2.str();
    else
        os << "(" << os2.str() << ")";

    switch(akt)
    {
        case ast::polarity_type::positive:
            os << " +-> ";
            break;
        case ast::polarity_type::negative:
            os << " --> ";
            break;
        case ast::polarity_type::neutral:
            os << " -> ";
            break;
    }
             
    visit(k1_r, os, prec);
    prec = prec_arrow;
};

void printer_kind_visitor::eval(const ast::value_kind& ast, std::ostream& os, int& prec)
{
    ast::value_kind_code code   = ast.get_code();
    switch(code)
    {
        case ast::value_kind_code::int_kind:
            os << "int";
            break;
        case ast::value_kind_code::bool_kind:
            os << "bool";
            break;
        case ast::value_kind_code::char_kind:
            os << "char";
            break;
        case ast::value_kind_code::string_kind:
            os << "string";
            break;
    };

    prec = prec_atom;
};

void printer_kind_visitor::print_identifier(const ast::identifier& id, std::ostream& os)
{
    //bool has;
    //ast::identifier id_new      = ad::unique_names_provider::get().get_mapping(id, has);
    os << id.get_name_simple().get_string();
};

void printer_kind_visitor::eval(const ast::kind_identifier& ast, std::ostream& os, int& prec)
{    
    const ast::identifier& id   = ast.get_identifier();
    print_identifier(id, os);
    prec = prec_atom;
};

void printer_kind_visitor::eval(const ast::kind_scope& ast, std::ostream& os, int& prec)
{
    os << "Kind [";

    ast::kind body;
    print_lambda_args(ast, os, 0, body);
    os << "] ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_kind_visitor::print_lambda_args(const ast::kind_scope& ast, std::ostream& os, 
                        int n_args_printed, ast::kind& ret_body)
{
    const ast::identifier& ai = ast.get_var();
            
    if (n_args_printed > 0)
        os << ", ";

    print_identifier(ai, os);

    ast::kind body  = ast.get_body();

    for (;;)
    {
        if (body->get_kind_code() == ast::details::kind_code::kind_scope)
        {
            ast::kind_scope at    = body.static_cast_to<ast::kind_scope>();
            return print_lambda_args(at, os, n_args_printed+1, ret_body);
        }
        else if (body->get_kind_code() == ast::details::kind_code::kind_var)
        {
            ast::kind_var tv    = body.static_cast_to<ast::kind_var>();

            if (tv.is_set())
                body    = tv.get();
            else
                break;
        }
        else
        {
            break;
        }
    };

    ret_body = body;
}

void printer_kind_visitor::eval(const ast::kind&, std::ostream&, int&)
{};

void printer_kind_visitor::decompose_all_kind(const ast::kind& k, std::vector<ast::kind>& args, 
        ast::kind& tail)
{
    if (k->get_kind_code() == ast::details::kind_code::star_all_kind)
    {
        ast::star_all_kind ak   = k.static_cast_to<ast::star_all_kind>();
        const ast::kind& var    = ak.get_var();
        const ast::kind& body   = ak.get_body();

        decompose_all_kind(body, args, tail);
        args.push_back(var);
        return;
    };

    if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        ast::kind_var ak        = k.static_cast_to<ast::kind_var>();

        if (ak.is_set())
            return decompose_all_kind(ak.get(), args, tail);

        tail = k;
        return;
    }

    tail = k;
    return;
};
void printer_kind_visitor::decompose_some_kind(const ast::kind& k, std::vector<ast::kind>& args, 
        ast::kind& tail)
{
    if (k->get_kind_code() == ast::details::kind_code::star_some_kind)
    {
        ast::star_some_kind ak   = k.static_cast_to<ast::star_some_kind>();
        const ast::kind& var    = ak.get_var();
        const ast::kind& body   = ak.get_body();

        decompose_some_kind(body, args, tail);
        args.push_back(var);
        return;
    };

    if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        ast::kind_var ak        = k.static_cast_to<ast::kind_var>();

        if (ak.is_set())
            return decompose_some_kind(ak.get(), args, tail);

        tail = k;
        return;
    }

    tail = k;
    return;
};

//---------------------------------------------------------------
//                  printer_type_visitor
//---------------------------------------------------------------
printer_type_visitor::printer_type_visitor()
{};

//forward all other functions to the base class
template<class Ast>
void printer_type_visitor::eval(const Ast& ast, std::ostream& os, int& prec)
{
    return base_type::eval(ast, os, prec);
};

template<class Lambda_type>
void printer_type_visitor::print_lambda_args(const Lambda_type& ast, std::ostream& os, 
                        int n_args_printed, ast::type& ret_body)
{
    const auto& ai = ast.get_var();
            
    if (n_args_printed > 0)
        os << ", ";

    int prec;
    visit(ai, os, prec);

    ast::type body          = ast.get_body();

    for (;;)
    {
        if (body->get_type_code() == ast->get_type_code())
        {
            Lambda_type at    = body.static_cast_to<Lambda_type>();
            return print_lambda_args(at, os, n_args_printed+1, ret_body);
        }
        else if (body->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv    = body.static_cast_to<ast::type_var>();

            if (tv.is_set())
                body    = tv.get();
            else
                break;
        }
        else
        {
            break;
        }
    };

    ret_body = body;
    return;
}

void printer_type_visitor::print_lambda_args(const ast::lambda_type& ast, std::ostream& os, 
                        int n_args_printed, ast::type& ret_body)
{
    ast::polarity_type pol  = ast.get_polarity();
    const auto& ai = ast.get_var();
            
    if (n_args_printed > 0)
        os << ", ";

    switch(pol)
    {
        case ast::polarity_type::positive:
            os << "+"; break;
        case ast::polarity_type::negative:
            os << "-"; break;
    };

    int prec;
    visit(ai, os, prec);

    ast::type body          = ast.get_body();

    for (;;)
    {
        if (body->get_type_code() == ast->get_type_code())
        {
            ast::lambda_type at = body.static_cast_to<ast::lambda_type>();
            return print_lambda_args(at, os, n_args_printed+1, ret_body);
        }
        else if (body->get_type_code() == ast::details::type_code::type_var)
        {
            ast::type_var tv    = body.static_cast_to<ast::type_var>();

            if (tv.is_set())
                body    = tv.get();
            else
                break;
        }
        else
        {
            break;
        }
    };

    ret_body = body;
    return;
}

void printer_type_visitor::print_identifier(const ast::identifier& id, std::ostream& os)
{
    //bool has;
    //ast::identifier id_new      = ad::unique_names_provider::get().get_mapping(id, has);
    os << id.get_name_simple().get_string();
};

void printer_type_visitor::eval(const ast::identifier& id, std::ostream& os, int& prec)
{
    print_identifier(id, os);
    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::identifier_type& t, std::ostream& os, int& prec)
{
    const ast::identifier& ti   = t.get_identifier();
    print_identifier(ti, os);
            
    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::annotated_type_identifier& t, std::ostream& os, int& prec)
{
    const ast::identifier& ti   = t.get_identifier();
    const ast::kind& k          = t.get_kind();     

    print_identifier(ti, os);
    prec = prec_atom;

    if (check_print_kind(k) == true)
    {
        os << " : ";
        m_print_kind.make(os, k);
        prec = prec_annot;
    };
};
        
void printer_type_visitor::eval(const ast::predefined_type& t, std::ostream& os, int& prec)
{
    ast::predefined_type_code code  = t.get_type_code();
    print_predefined_type(os, code);
    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::product_of_types& ast, std::ostream& os, int& prec)
{
    os << "{";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        int prec_loc;
        visit(ast.get(i), os, prec_loc);
    };

    os << "}";
    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::annotated_type& ast, std::ostream& os, int& prec)
{
    const ast::type& ty = ast.get_type();
    const ast::kind& ki = ast.get_kind();

    os << "(";

    int prec1;
    visit(ty, os, prec1);

    os << " :: ";

    m_print_kind.make(os, ki);

    os << ")";

    prec = prec_atom;
}

void printer_type_visitor::eval(const ast::type_sequence& ast, std::ostream& os, int& prec)
{
    os << "(";

    using item_type     = ast::type_sequence_item::item_type;
    using kind_vec      = std::vector<ast::kind>;
    using type_vec      = std::vector<ast::type>;
    long s              = ast.size();

    //making operator sequence
    for (long i = 0; i < s; ++i)
    {
        const auto& e   = ast.get(i);
        item_type it    = e.get_item_type();

        switch(it)
        {
            case item_type::basic_item:
            {
                const ast::type& ex     = e.get_basic();
                visit(ex, os, prec);
                break;
            };
            case item_type::proj_item:
            {
                const ast::int_literal il= e.get_projection();
                os << ".[";
                visit(il, os, prec);
                os << "]";
                break;
            };
            case item_type::dot_item:
            {
                const ast::identifier& id   = e.get_dot();
                os << ".";
                visit(id, os, prec);
                break;
            };

            case item_type::type_applic_item:
            {
                const type_vec& args    = e.get_type_applic_args();
                os << ".<";
                for (size_t j = 0; j < args.size(); ++j)
                {
                    if (j > 0)
                        os << ", ";

                    visit(args[j], os, prec);
                };
                os << ">";
            }

            case item_type::kind_applic_item:
            {
                const kind_vec& args    = e.get_kind_applic_args();
                os << ".Kind[";
                for (size_t j = 0; j < args.size(); ++j)
                {
                    if (j > 0)
                        os << ", ";

                    m_print_kind.make(os, args[j]);
                };
                os << "]";
            }
        };
    };

    os << ")";
    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::tuple_type& ast, std::ostream& os, int& prec)
{
    os << "(";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        int prec_loc;
        visit(ast.get(i), os, prec_loc);
    };

    os << ")";
    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::bracket_type& ast, std::ostream& os, int& prec)
{
    const ast::identifier& vo   = ast.get_opening_bracket();
    const ast::identifier& vc   = ast.get_closing_bracket();

    visit(vo, os, prec);

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        int prec_loc;
        visit(ast.get(i), os, prec_loc);
    };

    visit(vc, os, prec);
    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::type_constant& ast, std::ostream& os, int& prec)
{
    const ast::identifier& id   = ast.get_name();
    long k                      = ast.var_size();
    const ast::kind& k_ast      = ast.get_return_kind();

    os << "$";
    print_identifier(id, os);

    if (k > 0)
        os << "<";

    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        const ast::identifier& lab  = ast.get_label(i).get_identifier();
        const ast::type& t          = ast.get_type(i);

        print_identifier(lab, os);

        os << " = ";

        int prec_loc;

        if (t)
            visit(t, os, prec_loc);
        else
            os << "?";
    };

    if (k > 0)
        os << ">";

    if (check_print_kind(k_ast) == true)
    {
        os << "(";
        m_print_kind.make(os, k_ast);
        os << ")";
    }

    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::record_type& ast, std::ostream& os, int& prec)
{
    os << "Record {";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        const ast::annotated_type_identifier& ai = ast.get_identifier(i);
        const ast::type& t  = ast.get_type(i);

        int prec_loc;
        visit(ai, os, prec_loc);

        os << " = ";

        visit(t, os, prec_loc);
    };

    os << "}";
    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::variant_type& ast, std::ostream& os, int& prec)
{
    os << "Variant {";

    long k                      = ast.size();
    for (long i = 0; i < k; ++i)
    {
        if (i > 0)
            os << ", ";

        const ast::annotated_type_identifier& ai = ast.get_identifier(i);
        const ast::type& t  = ast.get_type(i);

        int prec_loc;
        visit(ai, os, prec_loc);

        os << " = ";

        visit(t, os, prec_loc);
    };

    os << "}";
    prec    = prec_atom;
};

void printer_type_visitor::eval(const ast::type_kind_scope& ast, std::ostream& os, int& prec)
{
    os << "Kind [";

    ast::type body;
    print_lambda_args(ast, os, 0, body);
    os << "] ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::all_type& ast, std::ostream& os, int& prec)
{
    os << "All [";

    ast::type body;
    print_lambda_args(ast, os, 0, body);
    os << "] ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::some_type& ast, std::ostream& os, int& prec)
{
    os << "Some [";

    ast::type body;
    print_lambda_args(ast, os, 0, body);
    os << "] ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::lambda_type& ast, std::ostream& os, int& prec)
{
    os << "Fun [";

    ast::type body;
    print_lambda_args(ast, os, 0, body);
    os << "] => ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::mu_type& ast, std::ostream& os, int& prec)
{
    os << "Rec [";

    ast::type body;
    print_lambda_args(ast, os, 0, body);
    os << "] ";

    int prec_loc;
    visit(body, os, prec_loc);

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::path_int_type& ast, std::ostream& os, int& prec)
{
    const ast::type& ty         = ast.get_type();
    const ast::int_literal& ind = ast.get_index();

    int prec1;
    std::ostringstream os1;
    visit(ty, os1, prec1);

    if (prec1 < prec_postfix)
        os << "(" << os1.str() << ")";
    else
        os << os1.str();

    os << "[";

    int prec_loc;
    visit(ind, os, prec_loc);

    os << "]";

    prec = prec_postfix;
};

void printer_type_visitor::eval(const ast::ref_type& ast, std::ostream& os, int& prec)
{
    os << "Ref ";

   {
        int prec1;
        std::ostringstream os1;
        visit(ast.get_type(), os1, prec1);

        if (prec1 < prec_prefix)
            os << "(" << os1.str() << ")";
        else
            os << os1.str();
    }

    prec = prec_prefix;
};

void printer_type_visitor::eval(const ast::if_type& ast, std::ostream& os, int& prec)
{
    const ast::type& t_if   = ast.get_if();
    const ast::type& t_t    = ast.get_then();
    const ast::type& t_e    = ast.get_else();

    os << "If ";

    {
        int prec1;
        std::ostringstream os1;
        visit(t_if, os1, prec1);

        if (prec1 < prec_all)
            os << "(" << os1.str() << ")";
        else
            os << os1.str();
    }

    os << " Then ";

    {
        int prec1;
        std::ostringstream os1;
        visit(t_t, os1, prec1);

        if (prec1 < prec_all)
            os << "(" << os1.str() << ")";
        else
            os << os1.str();
    }

    os << " Else ";

    {
        int prec1;
        std::ostringstream os1;
        visit(t_e, os1, prec1);

        if (prec1 < prec_all)
            os << "(" << os1.str() << ")";
        else
            os << os1.str();
    }

    prec = prec_all;
};

void printer_type_visitor::eval(const ast::list_type& ast, std::ostream& os, int& prec)
{
    os << "list [";

    int prec_loc;
    visit(ast.get_type(), os, prec_loc);

    os << "]";

    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::arrow_type& ast, std::ostream& os, int& prec)
{
    std::ostringstream os1;
    std::ostringstream os2;
    int prec1, prec2;

    visit(ast.get_lhs(), os1, prec1);
    visit(ast.get_rhs(), os2, prec2);

    if (prec1 < prec_arrow)
        os << "(" << os1.str() << ")";
    else
        os << os1.str();
            
    os << " -> ";
    os << os2.str();

    prec = prec_arrow;
};

void printer_type_visitor::eval(const ast::type_application& ast, std::ostream& os, int& prec)
{
    const ast::type& lhs    = ast.get_lhs();
    const ast::type& rhs    = ast.get_rhs();

    int prec1;
    std::ostringstream os1;
    visit(lhs, os1, prec1);

    if (prec1 < prec_applic)
        os << "(" << os1.str() << ")";
    else
        os << os1.str();

    os << " ";

    int prec2;
    std::ostringstream os2;
    visit(rhs, os2, prec2);

    if (prec2 <= prec_applic)
        os << "(" << os2.str() << ")";
    else
        os << os2.str();

    prec = prec_applic;
};

void printer_type_visitor::eval(const ast::type_kind_application& ast, std::ostream& os, int& prec)
{
    ast::type root;
    std::vector<ast::kind> args;
    decompose_kind_applic(ast, root, args);

    int prec1;
    std::ostringstream os1;
    visit(root, os1, prec1);

    if (prec1 < prec_postfix)
        os << "(" << os1.str() << ")";
    else
        os << os1.str();

    os << ".Kind[";
    for (long i = 0; i < (long)args.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        m_print_kind.make(os, args[i]);
    };

    os << "]";
    prec = prec_postfix;
};

void printer_type_visitor::eval(const ast::type_type_application& ast, std::ostream& os, int& prec)
{
    ast::type root;
    std::vector<ast::type> args;
    decompose_type_applic(ast, root, args);

    int prec1;
    std::ostringstream os1;
    visit(root, os1, prec1);

    if (prec1 < prec_postfix)
        os << "(" << os1.str() << ")";
    else
        os << os1.str();

    os << ".<";
    for (long i = 0; i < (long)args.size(); ++i)
    {
        if (i > 0)
            os << ", ";

        visit(args[i], os, prec);
    };

    os << ">";
    prec = prec_postfix;
};

void printer_type_visitor::eval(const ast::type_var& ast, std::ostream& os, int& prec)
{
    bool has_subs   = ast.is_set();

    if (has_subs == false)
    {
        os << "?";
        prec = prec_atom;
        return;
    }
    else
    {
        visit(ast.get(), os, prec);
        return;
    }
};

void printer_type_visitor::eval(const ast::int_type& ast, std::ostream& os, int& prec)
{
    os << ast.get_value();
    prec = prec_atom;
};
void printer_type_visitor::eval(const ast::bool_type& ast, std::ostream& os, int& prec)
{
    os << ((ast.get_value() == true) ? "true" : "false");
    prec = prec_atom;
};
void printer_type_visitor::eval(const ast::char_type& ast, std::ostream& os, int& prec)
{
    os << "\'" << ast.get_value() << "\'";
    prec = prec_atom;
};
void printer_type_visitor::eval(const ast::string_type& ast, std::ostream& os, int& prec)
{
    os << "\"" << ast.get_value() << "\"";
    prec = prec_atom;
};

void printer_type_visitor::eval(const ast::error_type& ast, std::ostream& os, int& prec)
{
    (void)ast;

    os << ":error";
    prec = prec_atom;
}
void printer_type_visitor::eval(const ast::int_literal& ast, std::ostream& os, int& prec)
{
    os << ast.get_value();
    prec = prec_atom;
};
void printer_type_visitor::eval(const ast::bool_literal& ast, std::ostream& os, int& prec)
{
    os << (ast.get_value() == true) ? "True" : "False";
    prec = prec_atom;
};

bool printer_type_visitor::check_print_kind(const ast::kind& k) const
{
    if (k->get_kind_code() == ast::details::kind_code::star_kind)
        return false;
    if (k->get_kind_code() == ast::details::kind_code::kind_var)
    {
        ast::kind_var kv = k.static_cast_to<ast::kind_var>();

        if (kv.is_set())
            return check_print_kind(kv.get());
        else
            return true;
    }

    return true;
}

void printer_type_visitor::make(std::ostream& os, const ast::type& k)
{
    int prec;
    visit(k, os, prec);
}
void sema::print_type(std::ostream& os, const ast::type& k)
{
    printer_type_visitor pkv;
    int prec;
    pkv.visit(k, os, prec);
};

void sema::print_predefined_type(std::ostream& os, ast::predefined_type_code code)
{
    switch (code)
    {
        case ast::predefined_type_code::type_bool:
            os << "bool";
            break;
        case ast::predefined_type_code::type_int:
            os << "int";
            break;
        case ast::predefined_type_code::type_float:
            os << "float";
            break;
        case ast::predefined_type_code::type_string:
            os << "string";
            break;
        case ast::predefined_type_code::type_char:
            os << "char";
            break;
    };

};

void sema::print_type_kind(std::ostream& os, const ast::type& ty, const ast::kind& ki)
{
    sema::print_type(os, ty);
    os << " : ";
    sema::print_kind(os, ki);
}

void sema::print_kind(std::ostream& os, const ast::kind& k)
{
    printer_kind_visitor pkv;
    int prec;
    pkv.visit(k, os, prec);
};

}}

