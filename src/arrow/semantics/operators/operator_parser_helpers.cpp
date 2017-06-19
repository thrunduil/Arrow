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

#include "operator_parser_helpers.h"
#include "operator_parser.h"

#include <algorithm>

namespace arrow { namespace oper
{

//-----------------------------------------------------------------------
//                          expr_or_oper
//-----------------------------------------------------------------------
template<class Config>
expr_or_oper<Config>::expr_or_oper()
    :m_fixity_flags(0), m_prec_base(0), m_prec_assign(0), m_can_be_assign(false)
{};

template<class Config>
bool expr_or_oper<Config>::is_assign() const
{
    return m_can_be_assign;
};

template<class Config>
const operator_declaration& expr_or_oper<Config>::get_def() const
{
    return m_def;
}

template<class Config>
ast::associativity_type expr_or_oper<Config>::get_assoc() const
{
    return m_assoc;
};

template<class Config>
long expr_or_oper<Config>::get_precedence() const
{
    return m_prec_base;
}

template<class Config>
const typename expr_or_oper<Config>::iterm& 
expr_or_oper<Config>::get_expr() const
{
    return m_expr;
}

template<class Config>
const ast::location& expr_or_oper<Config>::get_location() const
{
    return m_expr->get_location();
};

template<class Config>
void expr_or_oper<Config>::set_kind(item_kind kind)
{
    m_kind = kind;
}

template<class Config>
item_kind expr_or_oper<Config>::get_kind() const
{
    return m_kind;
}        

template<class Config>
void expr_or_oper<Config>::set_precedence(long prec)
{
    m_prec_base = prec;
};

template<class Config>
ast::unique_string expr_or_oper<Config>::get_operator() const
{
    return m_op_name;
};

template<class Config>
expr_or_oper<Config> expr_or_oper<Config>::make_expr(const iterm& ex)
{
    expr_or_oper ret;
    ret.m_expr                  = ex;
    ret.m_fixity_flags          = 0;
    ret.m_assoc                 = ast::associativity_type::none;
    ret.m_prec_base             = (int)ast::named_precedence::call;
    ret.m_prec_assign           = 0;
    ret.m_kind                  = item_kind::expr;
    ret.m_can_be_assign         = false;
    return ret;
}

template<class Config>
expr_or_oper<Config>
expr_or_oper<Config>::make_operator(const iterm& ex, const operator_declaration& od, 
                            ast::unique_string op_name, bool can_be_assign, long assign_prec)
{
    expr_or_oper ret;
    ret.m_op_name               = op_name;
    ret.m_expr                  = ex;
    
    //this is temporaty precedence; will be changed later by unique_fixity function
    ret.m_prec_base             = od.get_info(ast::fixity_type::infix).get_precedence();
    ret.m_prec_assign           = 0;

    ret.m_kind                  = item_kind::oper;
    ret.m_fixity_flags          = od.get_all_fixity_flag();
    ret.m_assoc                 = od.get_info(ast::fixity_type::infix).get_associativity();
    ret.m_def                   = od;
    ret.m_can_be_assign         = false;

    bool has_indexing           
        = (ret.m_fixity_flags & (ast::operator_info::indexing | ast::operator_info::surrounding)) != 0;

    if (has_indexing)
    {
        //change indexing and surrounding to postfix
        ret.m_fixity_flags      |= ast::operator_info::postfix;
        ret.m_fixity_flags      &= ~ast::operator_info::indexing;
        ret.m_fixity_flags      &= ~ast::operator_info::surrounding;
    };

    if (can_be_assign == true)
    {
        // if given operator can be parsed as bracket assign, then
        // parse as bracket assign; this implies, that precedence should
        // not be lower than precedence of the bracket operator
        ret.m_prec_base         = ret.m_prec_base;
        ret.m_prec_assign       = assign_prec;
        ret.m_fixity_flags      |= ast::operator_info::infix;
        ret.m_assoc             = ast::associativity_type::left;
        ret.m_can_be_assign     = true;
    };

    return ret;
};

template<class Config>
expr_or_oper<Config>
expr_or_oper<Config>::make_bracket_assign(const iterm& inner_ex, ast::unique_string op_name,
                                               long precedence)
{
    expr_or_oper ret;
    ret.m_op_name               = op_name;
    ret.m_expr                  = inner_ex;
    ret.m_prec_base             = precedence;
    ret.m_prec_assign           = precedence;
    ret.m_assoc                 = ast::associativity_type::left;
    ret.m_kind                  = item_kind::oper;
    ret.m_fixity_flags          = ast::operator_info::infix;
    ret.m_can_be_assign         = true;

    return ret;
};

template<class Config>
bool expr_or_oper<Config>::is_postfix_expr() const
{
    return m_kind == item_kind::dot_tuple   || m_kind == item_kind::dot_bracket
        || m_kind == item_kind::simple_tuple|| m_kind == item_kind::simple_bracket
        || m_kind == item_kind::projection  || m_kind == item_kind::dot
        || m_kind == item_kind::applic_type || m_kind == item_kind::applic_kind;
};

template<class Config>
ast::operator_info::fixity expr_or_oper<Config>::unique_fixity(expr_or_oper& item)
{
    size_t u = item.m_fixity_flags;

    // an unsigned int is a power of 2 or 0 iff (u & (u-1)) == 0
    ast::operator_info::fixity ft;
    ft = ((u & (u-1)) == 0) ? (ast::operator_info::fixity) u : ast::operator_info::undefined;

    switch (ft)
    {
        case ast::operator_info::infix:
            item.set_infix(); break;
        case ast::operator_info::prefix:
            item.set_prefix(); break;
        case ast::operator_info::postfix:
            item.set_postfix(); break;
    };

    return ft;
};

template<class Config>
ast::fixity_type expr_or_oper<Config>::get_unique_fixity() const
{
    switch (m_fixity_flags)
    {
        case ast::operator_info::infix:
            return ast::fixity_type::infix;
        case ast::operator_info::prefix:
            return ast::fixity_type::prefix;
        case ast::operator_info::postfix:
            return ast::fixity_type::postfix;
        default:
            return ast::fixity_type::undefined;
    };
};

template<class Config>
void expr_or_oper<Config>::set_infix()
{
    m_fixity_flags  = ast::operator_info::infix;
    m_prec_base     = m_def.get_info(ast::fixity_type::infix).get_precedence();

    //if this can be an assign operator, then precedence must be at least
    //as high as precedence of the preceeding bracket (i.e. m_prec_assign)
    m_prec_base     = std::max(m_prec_base, m_prec_assign);

    //associativity is already set to infix associativity, for assign operator
    //this associativity was changes, and stored associativity can be invalid
    //m_assoc         = m_def.get_info(ast::fixity_type::infix).get_associativity();
};

template<class Config>
void expr_or_oper<Config>::set_postfix()
{
    m_fixity_flags  = ast::operator_info::postfix;
    m_prec_base     = m_def.get_info(ast::fixity_type::postfix).get_precedence();
    m_assoc         = m_def.get_info(ast::fixity_type::postfix).get_associativity();
};

template<class Config>
void expr_or_oper<Config>::set_prefix()
{
    m_fixity_flags  = ast::operator_info::prefix;
    m_prec_base     = m_def.get_info(ast::fixity_type::prefix).get_precedence();
    m_assoc         = m_def.get_info(ast::fixity_type::prefix).get_associativity();
};

template<class Config>
void expr_or_oper<Config>::remove_infix()
{
    m_fixity_flags &= ~ast::operator_info::infix;
};

template<class Config>
void expr_or_oper<Config>::remove_prefix()
{
    m_fixity_flags &= ~ast::operator_info::prefix;
};

template<class Config>
void expr_or_oper<Config>::remove_postfix()
{
    m_fixity_flags &= ~ast::operator_info::postfix;
};

template<class Config>
bool expr_or_oper<Config>::is_undefined() const
{
    return (m_fixity_flags == 0);
}

template<class Config>
bool expr_or_oper<Config>::has_prefix() const
{
    return (m_fixity_flags & ast::operator_info::prefix) != 0;
}

template<class Config>
bool expr_or_oper<Config>::has_postfix() const
{
    return (m_fixity_flags & ast::operator_info::postfix) != 0;
}

template<class Config>
bool expr_or_oper<Config>::has_infix() const
{
    return (m_fixity_flags & ast::operator_info::infix) != 0;
}

template<class Config>
bool expr_or_oper<Config>::is_prefix() const
{
    return (m_fixity_flags == ast::operator_info::prefix);
}

template<class Config>
bool expr_or_oper<Config>::is_postfix() const
{
    return (m_fixity_flags == ast::operator_info::postfix);
}

template<class Config>
bool expr_or_oper<Config>::is_infix() const
{
    return (m_fixity_flags == ast::operator_info::infix);
}

template<class Config>
bool expr_or_oper<Config>::is_prefix_or_infix() const
{
    return m_fixity_flags == (ast::operator_info::infix | ast::operator_info::prefix);
}

template<class Config>
bool expr_or_oper<Config>::is_postfix_or_infix() const
{
    return m_fixity_flags == (ast::operator_info::infix | ast::operator_info::postfix);
}

//-----------------------------------------------------------------------
//                          expr_holder
//-----------------------------------------------------------------------
template<class Config>
expr_holder<Config>::expr_holder()
    :m_kind(empty), m_delayed_ex(nullptr)
{};

template<class Config>
expr_holder<Config>::expr_holder(const iterm& ex)
    :m_expr(ex), m_kind(final_expr), m_delayed_ex(nullptr)
{};

template<class Config>
typename expr_holder<Config>::iterm
expr_holder<Config>::make(operator_parser* op) const
{
    switch (m_kind)
    {
        case empty:
            return iterm();
        case final_expr:
            return m_expr;
        case delayed_single_expr:
            return op->make_single_expr(*m_delayed_atom);

        case postfix:
        {
            if (m_delayed_ex->m_kind == delayed_single_expr)
                return op->make_postfix_expr(m_expr, m_delayed_ex->m_delayed_atom);
            else
                return op->make_postfix_expr(m_expr, m_delayed_ex->make(op));
        }

        default:
            return iterm();
    };
};

template<class Config>
expr_holder<Config>
expr_holder<Config>::make_single_expr(operator_parser*, seq_iterator iter)
{
    //we must delay building a single term; for example [1,2,3]
    //cannot be build as surrounding bracket because it may be an indexer

    expr_holder ret;
    ret.m_delayed_atom  = iter;
    ret.m_kind          = delayed_single_expr;

    return ret;
};

template<class Config>
expr_holder<Config>
expr_holder<Config>::make_operator_expr(operator_parser* op, const expr_holder& lhs, 
                            seq_iterator item, const expr_holder& rhs)
{
    seq_iterator it_bracket;
    ast::fixity_type ft;
    bool is_bracket = lhs.is_bracket(it_bracket, ft);
    bool is_assign  = false;

    if (is_bracket == true)
    {
        //this is expression a[x] := b or [x] := b
        long prec;
        is_assign  = op->is_bracket_assign_symbol(item, it_bracket, ft, true, prec);

        //if this is index assign expression but index assign operator is 
        //not defined then we must call regular operator even if surrounding
        //assign is defined, i.e. a[x] := b must be parsed as (a[x]) := b
        // not as (a)([x] := b); later a[x] can be parsed as indexing operator
        // if defined or application to surrounding operator otherwise

        //check indexing assign
        if (is_assign == true)
            return op->make_operator_assign_expr(lhs, item, rhs);
    };
    
    iterm e_rhs = rhs.make(op);
    iterm e_lhs = lhs.make(op);

    iterm ret   = op->make_operator_expr(e_lhs, item, e_rhs);
    return expr_holder(ret);
};

template<class Config>
expr_holder<Config>
expr_holder<Config>::make_postfix_expr(operator_parser* op, const expr_holder& lhs, 
                            const expr_holder* rhs)
{
    //postfix expressions must also be delayed; assignment may arrive
    expr_holder ret;
    ret.m_expr          = lhs.make(op);
    ret.m_delayed_ex    = rhs;
    ret.m_kind          = postfix;
    return ret;
};

template<class Config>
bool expr_holder<Config>::is_bracket(seq_iterator& it_bracket, ast::fixity_type& ft) const
{
    ft  = ast::fixity_type::undefined;

    switch (m_kind)
    {
        case delayed_single_expr:
        {
            item_kind kind  = m_delayed_atom->get_kind();
            bool is_brack   =  kind == item_kind::dot_tuple
                            || kind == item_kind::dot_bracket
                            || kind == item_kind::simple_tuple
                            || kind == item_kind::simple_bracket;

            if (is_brack)
            {
                ft          = ast::fixity_type::surrounding;
                it_bracket  = m_delayed_atom;
                return true;
            }
            else
            {
                return false;
            }
        }

        case postfix:
        {
            bool res = m_delayed_ex->is_bracket(it_bracket, ft);

            if (res == true)
                ft  = ast::fixity_type::indexing;

            return res;
        }
        
        case empty:
        case final_expr:
        default:
            return false;
    };
}

template<class Config>
item_kind expr_holder<Config>::decompose_bracket(iterm& prefix, seq_iterator& brack, 
                                         ast::fixity_type& ft) const
{
    ft          = ast::fixity_type::undefined;

    switch (m_kind)
    {
        case delayed_single_expr:
        {
            item_kind kind = m_delayed_atom->get_kind();
            bool is_brack   =  kind == item_kind::dot_tuple
                            || kind == item_kind::dot_bracket
                            || kind == item_kind::simple_tuple
                            || kind == item_kind::simple_bracket;

            if (is_brack)
            {
                ft          = ast::fixity_type::surrounding;
                brack       = m_delayed_atom;
                return kind;
            }
            else
            {
                return item_kind::expr;
            }
        }

        case postfix:
        {
            item_kind kind = m_delayed_ex->decompose_bracket(prefix, brack, ft);

            if (kind != item_kind::expr)
            {
                ft      = ast::fixity_type::indexing;
                prefix  = m_expr;
            };

            return kind;
        }
        
        case empty:
        case final_expr:
        default:
            return item_kind::expr;
    };
};

template class expr_or_oper<operator_config_type>;
template class expr_or_oper<operator_config_term>;

template class expr_holder<operator_config_type>;
template class expr_holder<operator_config_term>;

};};
