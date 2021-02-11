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

#include <sstream>

namespace arrow { namespace sema
{

namespace ad = arrow::details;

//---------------------------------------------------------------
//                  printer_kind_visitor
//---------------------------------------------------------------
class printer_kind_visitor : public ast::traversal_visitor<printer_kind_visitor>
{    
    private:
        using base_type     = ast::traversal_visitor<printer_kind_visitor>;
        using unique_names  = ad::unique_names_provider;

    private:
        bool        m_in_star_kind;

    public:
        static const int prec_atom      = 4;
        static const int prec_arrow     = 3;
        static const int prec_prefix    = 2;
        static const int prec_all       = 1;
        
    public:
        printer_kind_visitor();

        void make(std::ostream& os, const ast::kind& k);

    public:
        void eval(const ast::star_kind&, std::ostream& os, int& prec);
        void eval(const ast::kind_var& ast, std::ostream& os, int& prec);
        void eval(const ast::arrow_kind& ast, std::ostream& os, int& prec);        
        void eval(const ast::product_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::value_kind&, std::ostream& os, int& prec);
        void eval(const ast::kind_identifier& ast, std::ostream& os, int& prec);
        void eval(const ast::kind_scope& ast, std::ostream& os, int& prec);
        void print_lambda_args(const ast::kind_scope& ast, std::ostream& os, 
                               int n_args_printed, ast::kind& ret_body);
        void eval(const ast::kind&, std::ostream&, int&);

        void eval(const ast::star_arrow_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_tuple_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_all_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_some_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_list_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_record_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_variant_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_predefined_kind& ast, std::ostream& os, int& prec);
        void eval(const ast::star_ref_kind& ast, std::ostream& os, int& prec);

    private:
        void print_identifier(const ast::identifier& id, std::ostream& os);
        void decompose_all_kind(const ast::kind& k, std::vector<ast::kind>& args, 
                ast::kind& body);
        void decompose_some_kind(const ast::kind& k, std::vector<ast::kind>& args, 
                ast::kind& body);
};

//---------------------------------------------------------------
//                  printer_type_visitor
//---------------------------------------------------------------
class printer_type_visitor : public ast::traversal_visitor<printer_type_visitor>
{    
    private:
        using base_type = ast::traversal_visitor<printer_type_visitor>;

    public:
        static const int prec_atom          = 100;
        static const int prec_annot         = 70;
        static const int prec_postfix       = 60;   
        static const int prec_prefix        = 50;
        static const int prec_applic        = 40;
        static const int prec_sum           = 30;
        static const int prec_arrow         = 20;
        static const int prec_all           = 10;

    public:
        printer_kind_visitor  m_print_kind;

    public:
        printer_type_visitor();

        void make(std::ostream& os, const ast::type& k);

        //forward all other functions to the base class
        template<class Ast>
        void eval(const Ast& ast, std::ostream& os, int& prec);

        template<class Lambda_type>
        void print_lambda_args(const Lambda_type& ast, std::ostream& os, 
                               int n_args_printed, ast::type& ret_body);
        void print_lambda_args(const ast::lambda_type& ast, std::ostream& os, 
                               int n_args_printed, ast::type& ret_body);
        void print_identifier(const ast::identifier& id, std::ostream& os);

        void eval(const ast::identifier& ti, std::ostream& os, int& prec);

        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        void eval(const ast::identifier_type& t, std::ostream& os, int& prec);
        void eval(const ast::annotated_type_identifier& t, std::ostream& os, int& prec);
        void eval(const ast::predefined_type& t, std::ostream& os, int& prec);
        void eval(const ast::product_of_types& ast, std::ostream& os, int& prec);
        void eval(const ast::type_sequence& ast, std::ostream& os, int& prec);
        void eval(const ast::tuple_type& ast, std::ostream& os, int& prec);
        void eval(const ast::bracket_type& ast, std::ostream& os, int& prec);
        void eval(const ast::annotated_type& ast, std::ostream& os, int& prec);
        void eval(const ast::type_constant& ast, std::ostream& os, int& prec);
        void eval(const ast::record_type& ast, std::ostream& os, int& prec);
        void eval(const ast::type_kind_scope& ast, std::ostream& os, int& prec);
        void eval(const ast::all_type& ast, std::ostream& os, int& prec);
        void eval(const ast::some_type& ast, std::ostream& os, int& prec);
        void eval(const ast::lambda_type& ast, std::ostream& os, int& prec);
        void eval(const ast::mu_type& ast, std::ostream& os, int& prec);
        void eval(const ast::ref_type& ast, std::ostream& os, int& prec);
        void eval(const ast::if_type& ast, std::ostream& os, int& prec);
        void eval(const ast::path_int_type& ast, std::ostream& os, int& prec);
        void eval(const ast::list_type& ast, std::ostream& os, int& prec);
        void eval(const ast::arrow_type& ast, std::ostream& os, int& prec);
        void eval(const ast::type_application& ast, std::ostream& os, int& prec);
        void eval(const ast::type_kind_application& ast, std::ostream& os, int& prec);
        void eval(const ast::type_type_application& ast, std::ostream& os, int& prec);
        void eval(const ast::variant_type& ast, std::ostream& os, int& prec);
        void eval(const ast::type_var& ast, std::ostream& os, int& prec);
        void eval(const ast::error_type& ast, std::ostream& os, int& prec);
        void eval(const ast::int_type& ast, std::ostream& os, int& prec);
        void eval(const ast::bool_type& ast, std::ostream& os, int& prec);
        void eval(const ast::char_type& ast, std::ostream& os, int& prec);
        void eval(const ast::string_type& ast, std::ostream& os, int& prec);

        //---------------------------------------------------------------
        //                  Terms
        //---------------------------------------------------------------
        void eval(const ast::int_literal& ast, std::ostream& os, int& prec);
        void eval(const ast::bool_literal& ast, std::ostream& os, int& prec);

    private:
        bool    check_print_kind(const ast::kind& k) const;
};

}}

