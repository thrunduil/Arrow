/* 
 *  This file is a part of Hasky Lib.
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

#include "semantics.h"
#include "error/error.h"
#include "arrow/ast/ast_visitor.h"
#include "unique_names_provider.h"
#include "print_type.h"
#include "scopes/local_scope.h"
#include "scopes/symbol_definition.h"

#include <sstream>

namespace arrow { namespace sema
{

namespace ad = arrow::details;

static void decompose_type_applic(const ast::term& ex, ast::term& root, 
                                  std::vector<ast::type>& args)
{
    if (ex->get_term_code() == ast::details::term_code::term_type_application)
    {
        ast::term_type_application tta = ex.static_cast_to<ast::term_type_application>();
        
        decompose_type_applic(tta.get_term(), root, args);
        args.push_back(tta.get_type());
        return;
    }

    root = ex;
    return;
};

static void decompose_kind_applic(const ast::term& ex, ast::term& root, 
                                  std::vector<ast::kind>& args)
{
    if (ex->get_term_code() == ast::details::term_code::term_kind_application)
    {
        ast::term_kind_application tta = ex.static_cast_to<ast::term_kind_application>();
        
        decompose_kind_applic(tta.get_term(), root, args);
        args.push_back(tta.get_kind());
        return;
    }

    root = ex;
    return;
};

//---------------------------------------------------------------
//                  printer_term_visitor
//---------------------------------------------------------------
class printer_term_visitor : public ast::traversal_visitor<printer_term_visitor>
{    
    private:
        using base_type = ast::traversal_visitor<printer_term_visitor>;

        static const int prec_atom          = 100;
        static const int prec_annot         = 60;
        static const int prec_postfix       = 50;
        static const int prec_prefix        = 40;        
        static const int prec_applic        = 20;                
        static const int prec_all           = 10;

    private:
        printer_type_visitor    m_print_type;
        module                  m_mod;
        error_handler_ptr       m_eh;

    public:
        printer_term_visitor(const module& mod, const error_handler_ptr& eh)
            :m_mod(mod), m_eh(eh)
        {};

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast, std::ostream& os, int& prec)
        {
            return base_type::eval(ast, os, prec);
        };

    public:
        //---------------------------------------------------------------
        //                  TERMS
        //---------------------------------------------------------------
        void eval(const ast::int_literal& ast, std::ostream& os, int& prec)
        {
            os << ast.get_value();
            prec = prec_atom;
        };
        void eval(const ast::bool_literal& ast, std::ostream& os, int& prec)
        {
            os << ((ast.get_value() == true) ? "true" : "false");
            prec = prec_atom;
        };
        void eval(const ast::float_literal& ast, std::ostream& os, int& prec)
        {
            os << ast.get_value();
            prec = prec_atom;
        };
        void eval(const ast::char_literal& ast, std::ostream& os, int& prec)
        {
            os << "\'" << ast.get_value() << "\'";
            prec = prec_atom;
        };
        void eval(const ast::string_literal& ast, std::ostream& os, int& prec)
        {
            os << "\"" << ast.get_value() << "\"";
            prec = prec_atom;
        };
        void eval(const ast::error_term& ast, std::ostream& os, int& prec)
        {
            (void)ast;
            os << ":error";
            prec = prec_atom;
        };

        void eval(const ast::term_application& ast, std::ostream& os, int& prec)
        {
            const ast::term& lhs    = ast.get_lhs();
            const ast::term& rhs    = ast.get_rhs();

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

        void print_identifier(const ast::identifier& id, std::ostream& os)
        {
            os << id.get_name_simple().get_string();
        }

        void eval(const ast::identifier& ti, std::ostream& os, int& prec)
        {
            print_identifier(ti, os);            
            prec = prec_atom;
        };

        void eval(const ast::identifier_term& t, std::ostream& os, int& prec)
        {
            const ast::identifier& ti   = t.get_identifier();
            print_identifier(ti, os);            
            prec = prec_atom;
        };

        void eval(const ast::closure_term& t, std::ostream& os, int& prec)
        {
            const ast::term& body   = t.get_body();
            long n                  = t.size();

            os << "closure [";

            for (long i = 0; i < n; ++i)
            {
                if (i > 0)
                    os << ", ";

                const ast::term& s          = t.get_substitution(i);
                const ast::identifier& id   = t.get_internal_ident(i);

                print_identifier(id, os);

                os << " = ";
                visit(s, os, prec);
            };

            os << "] (";
            visit(body, os, prec);
            os << ")";

            prec = prec_atom;
        };

        void eval(const ast::overloaded_symbol& t, std::ostream& os, int& prec)
        {
            const ast::identifier_term& id = t.get_identifier();
            return visit(id, os, prec);
        };

        void eval(const ast::identifier_type& it, std::ostream& os, int& prec)
        {
            const ast::identifier& ti   = it.get_identifier();
            print_identifier(ti, os);            
            prec = prec_atom;
        };

        void eval(const ast::term_sequence& ast, std::ostream& os, int& prec)
        {
            os << "(";

            using item_type     = ast::term_sequence_item::item_type;
            using type_vec      = std::vector<ast::type>;
            using kind_vec      = std::vector<ast::kind>;
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
                        const ast::term& ex     = e.get_basic();
                        eval(ex, os, prec);
                        break;
                    };
                    case item_type::proj_item:
                    {
                        const ast::int_literal il= e.get_projection();
                        os << ".[";
                        eval(il, os, prec);
                        os << "]";
                        break;
                    };
                    case item_type::dot_item:
                    {
                        const ast::identifier& id   = e.get_dot();
                        os << ".";
                        eval(id, os, prec);
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

                            m_print_type.make(os, args[j]);
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

                            m_print_type.m_print_kind.make(os, args[j]);
                        };
                        os << "]";
                    }

                };
            };

            os << ")";
            prec = prec_atom;
        };

        void eval(const ast::tuple_term& ast, std::ostream& os, int& prec)
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

        void eval(const ast::bracket_term& ast, std::ostream& os, int& prec)
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

        void eval(const ast::list_term& ast, std::ostream& os, int& prec)
        {
            os << "list [";

            long k                      = ast.size();
            for (long i = 0; i < k; ++i)
            {
                if (i > 0)
                    os << ", ";

                int prec_loc;
                visit(ast.get(i), os, prec_loc);
            };

            os << "]";
            prec    = prec_atom;
        };

        template<class Lambda_type>
        void print_lambda_args(const Lambda_type& ast, std::ostream& os, 
                               int n_args_printed, ast::term& ret_body)
        {
            const auto& ai = ast.get_var();
            
            if (n_args_printed > 0)
                os << ", ";

            int prec;
            visit(ai, os, prec);

            ast::term body          = ast.get_body();

            for (;;)
            {
                if (body->get_term_code() == ast->get_term_code())
                {
                    Lambda_type at    = body.static_cast_to<Lambda_type>();
                    return print_lambda_args(at, os, n_args_printed+1, ret_body);
                }
                else
                {
                    break;
                }
            };

            ret_body = body;
            return;
        }

        void print_lambda_args(const ast::pack_term& ast, std::ostream& os, 
                               int n_args_printed, ast::term& ret_body)
        {
            const auto& ai      = ast.get_var();
            const ast::type& ty = ast.get_var_type();
            
            if (n_args_printed > 0)
                os << ", ";

            int prec;
            visit(ai, os, prec);
            os << " = ";
            m_print_type.make(os, ty);

            ast::term body          = ast.get_body();

            for (;;)
            {
                if (body->get_term_code() == ast->get_term_code())
                {
                    ast::pack_term at    = body.static_cast_to<ast::pack_term>();
                    return print_lambda_args(at, os, n_args_printed+1, ret_body);
                }
                else
                {
                    break;
                }
            };

            ret_body = body;
            return;
        }

        void eval(const ast::term_kind_scope& ast, std::ostream& os, int& prec)
        {
            os << "Kind [";

            ast::term body;
            print_lambda_args(ast, os, 0, body);
            os << "] ";

            int prec_loc;
            visit(body, os, prec_loc);

            prec = prec_all;
        };

        void eval(const ast::all_term& ast, std::ostream& os, int& prec)
        {
            os << "All [";

            ast::term body;
            print_lambda_args(ast, os, 0, body);
            os << "] ";

            int prec_loc;
            visit(body, os, prec_loc);

            prec = prec_all;
        };

        void eval(const ast::lambda_term& ast, std::ostream& os, int& prec)
        {
            os << "fun [";

            ast::term body;
            print_lambda_args(ast, os, 0, body);
            os << "] => ";

            int prec_loc;
            visit(body, os, prec_loc);

            prec = prec_all;
        };

        void eval(const ast::mu_term& ast, std::ostream& os, int& prec)
        {
            os << "rec [";

            ast::term body;
            print_lambda_args(ast, os, 0, body);
            os << "] ";

            int prec_loc;
            visit(body, os, prec_loc);

            prec = prec_all;
        };

        void eval(const ast::term_kind_application& ast, std::ostream& os, int& prec)
        {
            ast::term root;
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

                m_print_type.m_print_kind.make(os, args[i]);
            };

            os << "]";

            prec = prec_postfix;
        };

        void eval(const ast::term_type_application& ast, std::ostream& os, int& prec)
        {
            ast::term root;
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

                m_print_type.make(os, args[i]);
            };

            os << ">";

            prec = prec_postfix;
        };

        void eval(const ast::annotated_type_identifier& t, std::ostream& os, int& prec)
        {
            const ast::identifier& ti   = t.get_identifier();
            const ast::kind& k          = t.get_kind();     

            print_identifier(ti, os);
            os << " : ";

            m_print_type.m_print_kind.make(os, k);

            prec = prec_annot;
        };

        void eval(const ast::annotated_identifier& t, std::ostream& os, int& prec)
        {
            const ast::identifier& ti   = t.get_identifier();
            const ast::type& k          = t.get_type();     

            print_identifier(ti, os);
            os << " : ";

            m_print_type.make(os, k);

            prec = prec_annot;
        };

        void eval(const ast::ref_term& ast, std::ostream& os, int& prec)
        {
            os << "ref ";

            int prec_loc;
            visit(ast.get_term(), os, prec_loc);

            prec = prec_prefix;
        };

        void eval(const ast::deref_term& ast, std::ostream& os, int& prec)
        {
            os << "! ";

            int prec_loc;
            visit(ast.get_term(), os, prec_loc);

            prec = prec_prefix;
        };

        void eval(const ast::assign_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& ex1 = ast.get_lhs();
            const ast::term& ex2 = ast.get_rhs();

            int prec1;
            std::ostringstream os1;
            visit(ex1, os1, prec1);

            if (prec1 < prec_applic)
                os << "(" << os1.str() << ")";
            else
                os << os1.str();

            os << " := ";

            int prec2;
            std::ostringstream os2;
            visit(ex2, os2, prec2);

            if (prec2 < prec_applic)
                os << "(" << os2.str() << ")";
            else
                os << os2.str();

            prec = prec_applic;
        };

        void eval(const ast::if_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& ex_if  = ast.get_if();
            const ast::term& ex_t   = ast.get_then();
            const ast::term& ex_e   = ast.get_else();

            os << "if ";

            {
                int prec1;
                std::ostringstream os1;
                visit(ex_if, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            }

            os << " then ";

            {
                int prec1;
                std::ostringstream os1;
                visit(ex_t, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            }

            os << " else ";

            {
                int prec1;
                std::ostringstream os1;
                visit(ex_e, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            }

            prec = prec_all;
        };

        void eval(const ast::annotated_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& ex = ast.get_term();
            const ast::type& ty = ast.get_type();

            os << "(";

            int prec1;
            visit(ex, os, prec1);

            os << " :: ";

            int prec2;
            m_print_type.visit(ty, os, prec2);

            os << ")";
            prec = prec_atom;
        }

        void eval(const ast::val_term& ast, std::ostream& os, int& prec)
        {
            os << "val [";
            m_print_type.make(os, ast.get_type());
            os << "]";

            prec = prec_atom;
        }
        
        void eval(const ast::path_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& ex         = ast.get_term();
            const ast::identifier& id   = ast.get_identifier();

            int prec1;
            std::ostringstream os1;
            visit(ex, os1, prec1);

            if (prec1 < prec_postfix)
                os << "(" << os1.str() << ")";
            else
                os << os1.str();

            os << ".";

            int prec_loc;
            visit(id, os, prec_loc);

            prec = prec_postfix;
        };

        void eval(const ast::path_int_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& ex         = ast.get_term();
            const ast::int_literal& ind = ast.get_index();

            int prec1;
            std::ostringstream os1;
            visit(ex, os1, prec1);

            if (prec1 < prec_postfix)
                os << "(" << os1.str() << ")";
            else
                os << os1.str();

            os << ".[";

            int prec_loc;
            visit(ind, os, prec_loc);

            os << "]";

            prec = prec_postfix;
        };        

        void eval(const ast::cell_term& ast, std::ostream& os, int& prec)
        {
            os << "cell [";
            os << ast.get_index();
            os << "]";

            prec = prec_atom;
        };

        void eval(const ast::term_constant& ast, std::ostream& os, int& prec)
        {
            const ast::identifier& id   = ast.get_name();
            long k                      = ast.var_size();

            os << "$";
            print_identifier(id, os);

            if (k > 0)
                os << "<";

            for (long i = 0; i < k; ++i)
            {
                if (i > 0)
                    os << ", ";

                const ast::identifier& lab  = ast.get_label(i).get_identifier();
                const ast::term& t          = ast.get_term(i);

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

            os << "(";
            const ast::type& ret_type   = ast.get_return_type();
            m_print_type.make(os, ret_type);
            os << ")";

            prec    = prec_atom;
        };

        void eval(const ast::record_term& ast, std::ostream& os, int& prec)
        {
            os << "record {";

            long k                      = ast.size();
            for (long i = 0; i < k; ++i)
            {
                if (i > 0)
                    os << ", ";

                const ast::annotated_identifier& ai = ast.get_label(i);
                const ast::term& t  = ast.get_term(i);

                int prec_loc;
                visit(ai, os, prec_loc);

                os << " = ";

                visit(t, os, prec_loc);
            };

            os << "}";
            prec    = prec_atom;
        };

        void eval(const ast::variant_term& ast, std::ostream& os, int& prec)
        {
            const ast::type& ty = ast.get_type();

            os << "variant [";
            m_print_type.make(os, ty);
            os << "]";
            
            os << " {";

            const ast::term& ex                     = ast.get_term();
            const ast::annotated_identifier& aid    = ast.get_label();

            visit(aid, os, prec);

            os << " = ";
            
            visit(ex, os, prec);

            os << "}";

            prec = prec_atom;
        };

        void eval(const ast::pack_term& ast, std::ostream& os, int& prec)
        {
            ast::term body;

            os << "Some [";            
            print_lambda_args(ast, os, 0, body);
            os << "] ";

            int prec1;
            std::ostringstream os1;
            visit(body, os1, prec1);

            if (prec1 < prec_all)
                os << "(" << os1.str() << ")";
            else
                os << os1.str();

            prec = prec_all;
        };

        void eval(const ast::unpack_term& ast, std::ostream& os, int& prec)
        {
            const ast::term& pack   = ast.get_package();
            const ast::term& ex     = ast.get_term();
            const ast::annotated_type_identifier& ty_id = ast.get_type_identifier();
            const ast::annotated_identifier& ex_id      = ast.get_term_identifier();

            int prec_loc;

            os << "open [";
            visit(ty_id, os, prec_loc);
            os << "] ";

            visit(ex_id, os, prec_loc);
            os << " = ";

            {
                int prec1;
                std::ostringstream os1;
                visit(pack, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            };            

            os << " in ";

            {
                int prec1;
                std::ostringstream os1;
                visit(ex, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            };

            prec = prec_all;
        };

        void eval(const ast::variant_case_term& ast, std::ostream& os, int& prec)
        {
            using annot_id      = ast::annotated_identifier;

            const ast::term& ex = ast.get_term();
            long n              = ast.size();

            os << "case ";

            {
                int prec1;
                std::ostringstream os1;
                visit(ex, os1, prec1);

                if (prec1 < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            };

            os << " in ";

            int prec_loc;

            if (n == 1)
            {
                const ast::term& b1 = ast.get_bind(0);
                const annot_id& id1 = ast.get_label(0);

                visit (id1, os, prec_loc);

                os << " => ";

                std::ostringstream os1;
                visit(b1, os1, prec_loc);

                if (prec_loc < prec_all)
                    os << "(" << os1.str() << ")";
                else
                    os << os1.str();
            }
            else
            {
                for (long i = 0; i < n; ++i)
                {
                    if (i > 0)
                        os << "; ";

                    const ast::term& b1 = ast.get_bind(i);
                    const annot_id& id1 = ast.get_label(i);

                    visit (id1, os, prec_loc);

                    os << " => ";

                    visit(b1, os, prec_loc);
                };
            };

            prec = prec_all;
        };

        void eval(const ast::let_term& ast, std::ostream& os, int& prec)
        {
            const local_scope& ls   = m_mod.get_scope_for_let(ast);

            using fun_sym_map       = local_scope::fun_sym_map;
            const fun_sym_map& loc  = ls.get_local_vars();

            bool single             = loc.size() == 1;
            const ast::term& body   = ast.get_body();

            os << "let ";

            if (single == false)
                os << "{ ";

            int prec_loc;

            for (const auto& elem : loc)
            {   
                const ast::type& ty = elem.second.get_signature();
                bool has_body;
                const ast::term& ex = elem.second.get_body(has_body);
                ast::identifier id  = elem.second.get_symbol();

                visit(id, os, prec_loc);
                os << " : ";
                m_print_type.make(os, ty);

                if (has_body)
                {
                    os << " = ";
                    visit(ex, os, prec_loc);
                };

                if (single == false)
                    os << "; ";
            };

            if (single == false)
                os << "}";

            os << " in ";
            visit(body, os, prec_loc);
            prec = prec_all;
        };
};

void sema::print_term(std::ostream& os, const ast::term& k, const error_handler_ptr& eh,
            const module& mod)
{
    printer_term_visitor pkv(mod, eh);
    int prec;
    pkv.visit(k, os, prec);
};

}}

