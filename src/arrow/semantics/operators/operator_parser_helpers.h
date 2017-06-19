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

#include "arrow/config.h"
#include "arrow/ast/ast_visitor.h"
#include "arrow/error_handler.h"
#include "scopes/local_scope.h"
#include "operator_parser_config.h"

namespace arrow { namespace oper
{

struct pos_count
{
    long    m_pos;
    long    m_count;

    pos_count()                     :m_pos(0), m_count(0){};
    pos_count(long pos, long count) :m_pos(pos), m_count(count){};
};

enum class item_kind
{
    expr,
    oper,
    dot_tuple,
    simple_tuple,
    dot_bracket,
    simple_bracket,
    projection,
    dot,
    applic_type,
    applic_kind,
};

template<class Config>
class expr_or_oper
{
    private:
        using iterm         = typename Config::iterm;
        using itype         = typename Config::itype;

    private:
        iterm               m_expr;
        item_kind           m_kind;
        ast::unique_string  m_op_name;

    private:
        size_t              m_fixity_flags;
        ast::associativity_type  
                            m_assoc;
        long                m_prec_base;
        long                m_prec_assign;
        bool                m_can_be_assign;
                
        operator_declaration m_def;

    public:
        expr_or_oper();

        /// typed_expr is the most inner expression in nested expressions
        /// (like type_annotation_expr)
        static expr_or_oper make_expr(const iterm& ex);

        static expr_or_oper make_bracket_assign(const iterm& ex, ast::unique_string op_name,
                                long precedence);

        static expr_or_oper make_operator(const iterm& ex,const operator_declaration& od, 
                                ast::unique_string op_name, bool can_be_assign, long assign_prec);

        void                    set_kind(item_kind kind);
        item_kind               get_kind() const;
        void                    set_precedence(long prec);

        const ast::location&    get_location() const;
        ast::unique_string      get_operator() const;
        const iterm&            get_expr() const;
        bool                    is_assign() const;

        const operator_declaration&
                                get_def() const;
        ast::associativity_type get_assoc() const;
        long                    get_precedence() const;

        void                    remove_prefix(); 
        void                    remove_infix();
        void                    remove_postfix();

        bool                    is_undefined() const;
        bool                    is_prefix() const;
        bool                    is_postfix() const;
        bool                    is_infix() const;
        bool                    is_prefix_or_infix() const;
        bool                    is_postfix_or_infix() const;

        //flag is not indexing or surrounding
        void                    set_infix();
        void                    set_postfix();
        void                    set_prefix();
        bool                    has_prefix() const;
        bool                    has_postfix() const;
        bool                    has_infix() const;

        /// return true if expression can be used as postfix operator
        bool                    is_postfix_expr() const;

        ast::fixity_type        get_unique_fixity() const;

        static ast::operator_info::fixity
                                unique_fixity(expr_or_oper& item);
};

template<class Config>
class operator_parser;

template<class Config>
class expr_holder
{
    private:
        enum kind
        {
            empty,
            final_expr,
            delayed_single_expr,
            postfix,
        };

    private:
        using expr_or_oper  = expr_or_oper<Config>;
        using sequence      = std::vector<expr_or_oper>;
        using seq_iterator  = typename sequence::const_iterator;
        using iterm         = typename Config::iterm;
        using operator_parser= operator_parser<Config>;

    private:
        seq_iterator        m_delayed_atom;
        const expr_holder*  m_delayed_ex;
        iterm               m_expr;
        kind                m_kind;

    public:
        expr_holder();
        expr_holder(const iterm& ex);

        iterm               make(operator_parser* op) const;

        static expr_holder  make_operator_expr(operator_parser* op, const expr_holder&, 
                                seq_iterator item, const expr_holder& rhs);
        static expr_holder  make_single_expr(operator_parser* op, seq_iterator item);
        static expr_holder  make_postfix_expr(operator_parser* op, const expr_holder& lhs,
                                const expr_holder* rhs);

        bool                is_bracket(seq_iterator& it_bracket, ast::fixity_type& ft) const;

        item_kind           decompose_bracket(iterm& prefix, seq_iterator& brack, 
                                ast::fixity_type& ft) const;
};

class check_state
{
    private:
        enum state { unknown, yes, no};

    private:
        state               m_state;

    public:
        check_state() : m_state(unknown){};

        bool                has_value() const   { return m_state != unknown; };
        bool                get_value() const   { return m_state == yes ? true : false; };
        void                set_value(bool v)   { m_state = (v == true) ? yes : no; };
};

};};
