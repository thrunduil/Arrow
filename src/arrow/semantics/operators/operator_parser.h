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

#include "operator_parser_helpers.h"
#include "scopes/module.h"
#include "operator_parser_config.h"

namespace arrow { namespace oper
{

template<class Config>
class operator_parser
{
    private:
        using iterm         = typename Config::iterm;
        using itype         = typename Config::itype;
        using itype_vec     = std::vector<ast::type>;
        using ikind_vec     = std::vector<ast::kind>;
        using ident_term    = typename Config::ident_term;
        using ituple_term   = typename Config::ituple_term;
        using ibracket_term = typename Config::ibracket_term;
        using iterm_applic  = typename Config::iterm_applic;
        
        using unique_string = ast::unique_string;
        using identifier    = ast::identifier;
        using error_scope   = error_handler_scope;
        using pos_langle    = std::vector<pos_count>;

        using expr_or_oper  = expr_or_oper<Config>;
        using sequence      = std::vector<expr_or_oper>;
        using seq_iterator  = typename sequence::const_iterator;
        using error_scopes  = std::vector<error_scope>;
        using expr_holder   = expr_holder<Config>;
        using expr_holders  = std::list<expr_holder>;

        friend expr_holder;

    private:
        module              m_module;
        local_scope         m_local_scope;
        error_handler_ptr   m_eh;
        ast::location       m_main_location;
        error_scope         m_error_scope;
        pos_langle          m_pos_largle;
        sequence            m_sequence;
        error_scopes        m_error_scopes;
        expr_holders        m_bufor;
        check_state         m_indexing_tuple;

    public:
        operator_parser(const ast::location& loc, const module& mod, 
                        const local_scope& ls, const error_handler_ptr& eh);
        ~operator_parser();

        void            push_basic(const iterm& ex);
        void            push_projection(const ast::int_literal& ex);
        void            push_dot(const ast::identifier& ex);
        void            push_type_application(const ast::location& loc, const itype_vec& args);
        void            push_kind_application(const ast::location& loc, const ikind_vec& args);

        iterm           parse();

    private:        
        void            push_error_scope();
        void            pop_error_scope();

        void            push_nested(const iterm& root_ex);
        void            push_expr(const iterm& ex);
        void            push_type_applic(const iterm& tuple);
        void            push_kind_applic(const iterm& tuple);
        void            push_tuple(const ituple_term& inner_ex);
        void            push_bracket(const ibracket_term& inner_ex);
        void            push_variable(const ident_term& vi);

        void            push_operator(const iterm& inner_ex, const operator_declaration& od, 
                            ast::unique_string op_name, bool can_be_assign, long assign_prec);
        void            push_bracket_assign(const iterm& inner_ex, ast::unique_string op_name,
                            long precedence);

        /// if operator if ambiguous error is printed
        operator_declaration 
                        get_operator_declaration(const ast::location& loc, const local_scope& ls,
                            ast::unique_string oper, ast::fixity_type ft, bool error_if_not_found, 
                            bool error_if_ambig, bool& found, bool& ambig);
        ast::identifier make_new_operator(const ast::identifier& base, ast::unique_string op, 
                            ast::fixity_type ft);
        bool            is_bracket_assign_symbol(const ast::location& loc, const local_scope& ls, 
                            ast::unique_string oper, bool error_if_ambig, long& prec);
        void            try_split_operator(const ast::identifier& vi, ast::fixity_type ft);
        void            make_operator_item(const iterm& ex,const operator_declaration& od, 
                            ast::unique_string op_name, bool can_be_assign, long assign_prec);
        iterm           make_angle_bracket(long pos_start, long pos_end);
        bool            check_exist_operator(const ast::location& loc, const std::string& op_str);
        void            push_split_operator(unique_string op, const identifier& base_op);
        bool            is_last_item_bracket() const;
        bool            is_bracket_assign_symbol(seq_iterator it_oper, seq_iterator it_bracket,
                            ast::fixity_type ft, bool error_if_ambig, long& prec);
        bool            is_bracket_assign_symbol_impl(const ast::location& loc, const local_scope& ls,
                            unique_string oper, seq_iterator it_bracket, ast::fixity_type ft,
                            bool error_if_ambig, long& prec);
        unique_string   get_bracket_operator_name(const ibracket_term& be);

        iterm           parse(long start, long end);
        iterm           make_single_expr(const expr_or_oper& item);
        bool            assign_fixities(long start, long end, bool second_call = false);
        bool            resolve_ambiguities(long start, long end);
        expr_holder     build_parse_tree(long precedence, seq_iterator prev_operator,
                            seq_iterator& start, seq_iterator end, bool& error);
        iterm           make_postfix_expr(const iterm& lhs, seq_iterator item);
        iterm           make_postfix_expr(const iterm& lhs, const iterm& rhs);
        iterm           make_application(const ast::location& loc, const iterm& lhs, const iterm& rhs);
        iterm           expand_tuple(const iterm& ex);
        const expr_holder*  make_delayed_ptr(const expr_holder& eh);
        iterm           final_operator(const iterm& ex, const ast::identifier& vi, 
                            ast::fixity_type kind);
        iterm           convert_bracket_to_tuple(const ibracket_term& be);
        iterm           make_bracket_operator_call(const iterm& args, unique_string op_name, 
                            ast::fixity_type ft, const iterm& prefix);

        bool            check_can_build_indexing_tuple();
        int             get_dot_tuple_precedence();
        int             get_bracket_precendence(const ibracket_term& bt);

        iterm           make_operator_expr(const iterm&, seq_iterator item, const iterm& rhs);
        iterm           make_operator_assign_expr(const expr_holder& lhs, seq_iterator item, 
                            const expr_holder& rhs);
        iterm           get_quoted(const iterm& arg, ast::unique_string op_name);

        iterm           make_bracket_assign_operator_call(const iterm& args, unique_string op_name, 
                            ast::fixity_type ft, const iterm& prefix, const iterm& rhs);        
        ident_term      make_bracket_operator(const ast::location& loc, unique_string op_name, 
                            ast::fixity_type ft);
        ast::identifier make_identifier(const ast::location& loc, const unique_string name,
                            const ast::fixity_type ft, ast::identifier_kind ki);
        
        ast::identifier_kind    make_kind_brack_part();
        ast::identifier_kind    make_kind_brack_full();
        ast::identifier_kind    make_kind_oper();
};

};};
