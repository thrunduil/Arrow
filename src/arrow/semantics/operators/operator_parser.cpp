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

#include "operator_parser.h"
#include "scopes/module_impl.h"
#include "semantics/desugaring.h"
#include "grammar/unicode.h"

namespace arrow { namespace oper
{

//-----------------------------------------------------------------------
//                          operator_parser
//-----------------------------------------------------------------------
template<class Config>
operator_parser<Config>::operator_parser(const ast::location& loc, const module& mod, 
            const local_scope& ls, const error_handler_ptr& eh)
    :m_module(mod), m_eh(eh), m_local_scope(ls)
    , m_main_location(loc), m_error_scope(m_eh->push_error_scope())
{};

template<class Config>
operator_parser<Config>::~operator_parser()
{};

template<class Config>
void operator_parser<Config>::push_error_scope()
{
    m_error_scopes.push_back(m_eh->push_error_scope());
};

template<class Config>
void operator_parser<Config>::pop_error_scope()
{
    m_error_scopes.pop_back();
};

template<class Config>
void operator_parser<Config>::push_basic(const iterm& root_ex)
{
    if (Config::is_ident_term(root_ex) == true)
    {
        identifier id  = Config::cast_to_ident_term(root_ex).get_identifier(); 

        // ignore leading module symbols in paths, they are not needed
        // anymore
        if (id.get_symbol_data()->get_kind().is_sector_module() == true)
            return;
    };
    
    return push_nested(root_ex);
};

template<class Config>
void operator_parser<Config>::push_projection(const ast::int_literal& ex)
{
    iterm ex2   = Config::hide_int(ex);

    push_expr(ex2);
    m_sequence.back().set_kind(item_kind::projection);
    m_sequence.back().set_precedence((int)ast::named_precedence::dot);
};

template<class Config>
void operator_parser<Config>::push_dot(const identifier& id)
{
    // ignore leading module symbols in paths, they are not needed
    // anymore
    if (id.get_symbol_data()->get_kind().is_sector_module() == true)
        return;

    bool first      = m_sequence.size() == 0;

    ident_term ex   = ident_term(id->get_location(), id);
    push_expr(ex);

    if (first == false)
    {
        //dot term can be first item if previous items were removed
        m_sequence.back().set_kind(item_kind::dot);
        m_sequence.back().set_precedence((int)ast::named_precedence::dot);
    };
};

template<class Config>
void operator_parser<Config>::push_type_application(const ast::location& loc, const itype_vec& args)
{
    iterm ty        = Config::hide_type_applic(loc, args);
    push_type_applic(ty);
};

template<class Config>
void operator_parser<Config>::push_kind_application(const ast::location& loc, const ikind_vec& args)
{
    iterm ty        = Config::hide_kind_applic(loc, args);
    push_kind_applic(ty);
};

template<class Config>
void operator_parser<Config>::push_type_applic(const iterm& tuple)
{
    m_sequence.push_back(expr_or_oper::make_expr(tuple));
    m_sequence.back().set_kind(item_kind::applic_type);
    m_sequence.back().set_precedence((int)ast::named_precedence::dot);
};

template<class Config>
void operator_parser<Config>::push_kind_applic(const iterm& tuple)
{
    m_sequence.push_back(expr_or_oper::make_expr(tuple));
    m_sequence.back().set_kind(item_kind::applic_kind);
    m_sequence.back().set_precedence((int)ast::named_precedence::dot);
};

template<class Config>
void operator_parser<Config>::push_expr(const iterm& ex)
{
    m_sequence.push_back(expr_or_oper::make_expr(ex));
    return;
};

template<class Config>
void operator_parser<Config>::push_nested(const iterm& ex)
{        
    if (Config::is_tuple(ex) == true)
    {
        ituple_term tt  = Config::cast_to_tuple(ex);
        return push_tuple(tt);
    }
    else if (Config::is_bracket(ex) == true)
    {
        ibracket_term bt    = Config::cast_to_bracket(ex);
        return push_bracket(bt);
    }
    else if (Config::is_ident_term(ex) == true) 
    {
        ident_term id           = Config::cast_to_ident_term(ex);
        return push_variable(id);
    }
    else
    {
        return push_expr(ex);
    }
};

template<class Config>
void operator_parser<Config>::push_tuple(const ituple_term& inner_ex)
{
    bool first      = m_sequence.size() == 0;

    push_expr(inner_ex);

    bool is_dot = inner_ex.get_state() == ast::tuple_term::dot;

    //expression [mod_name].(id) is expanded to .(id), which must be
    //treated as (id)
    if (is_dot == true && first == false)
    {
        m_sequence.back().set_kind(item_kind::dot_tuple);
        int prec = get_dot_tuple_precedence();
        m_sequence.back().set_precedence(prec);
    }
    else
    {
        m_sequence.back().set_kind(item_kind::simple_tuple);

        //tuples must have call precedence, i.e. f g (h) is parsed as (f g) (h)
        m_sequence.back().set_precedence((int)ast::named_precedence::call);
    }
};

template<class Config>
void operator_parser<Config>::push_bracket(const ibracket_term& inner_ex)
{
    push_expr(inner_ex);

    bool is_dot = inner_ex.is_dot();
    int prec    = get_bracket_precendence(inner_ex);

    if (is_dot == true)
    {
        m_sequence.back().set_kind(item_kind::dot_bracket);
        // bracket has higher precedence than call, i.e. f g [h] is parsed as f (g [h])        
        m_sequence.back().set_precedence(prec);
    }
    else
    {
        m_sequence.back().set_kind(item_kind::simple_bracket);
        // bracket has higher precedence than call, i.e. f g [h] is parsed as f (g [h])
        m_sequence.back().set_precedence(prec);
    };
};

template<class Config>
void operator_parser<Config>::push_bracket_assign(const iterm& inner_ex, unique_string op_name,
                                          long prec)
{    
    m_sequence.push_back(expr_or_oper::make_bracket_assign(inner_ex, op_name, prec));
    //precedenced is set in make_bracket_assign function
    return;
};

template<class Config>
void operator_parser<Config>::push_variable(const ident_term& ve)
{
    identifier vi               = ve.get_identifier();
    ast::identifier_kind kind   = vi.get_symbol_data()->get_kind();
    bool is_oper                = vi.get_symbol_data()->is_any_operator();
    ast::fixity_type ft;
    unique_string oper          = vi.get_name(ft);
    const ast::location& loc    = vi->get_location();

    bool is_bracket             = kind.is_bracket_full();

    if (is_bracket == true)
    {
        //this is an explicit function call

        bool found;
        bool ambig;        
        operator_declaration od = get_operator_declaration(loc, m_local_scope, oper, ft, 
                                    true, true, found, ambig);
    
        return push_expr(ve);
    }       

    //if there are unmatched left angle brackets, then leading right angle
    //bracket is not part of this operator

    if (m_pos_largle.size() > 0)
    {
        const std::string& str  = oper.get_string();
        int count_rangle        = ast::special_operators::count_rangle(str);
        bool single_rangle      = (count_rangle == 1) && (str.size() == 1);

        if (count_rangle > 0 && single_rangle == false)
        {
            std::string start   = ">";
            std::string end     = str.substr(1);

            unique_string op_gt  = ast::special_operators::operator_rangle();
            unique_string op_end = unique_string(end);
            identifier vi1      = make_new_operator(vi, op_gt, ast::fixity_type::undefined);
            identifier vi2      = make_new_operator(vi, op_end, ft);

            ident_term ve1      = ident_term(loc, vi1);
            ident_term ve2      = ident_term(loc, vi2);

            push_variable(ve1);
            push_variable(ve2);
            return;
        };
    }

    bool found;
    bool ambig;    
    operator_declaration od = get_operator_declaration(loc, m_local_scope, oper, ft, 
                                false, true, found, ambig);

    if (ambig == true)
        return;

    long prec;
    bool is_bracket_assign  = is_bracket_assign_symbol(loc, m_local_scope, oper, true, prec);

    if (found == true)
        return push_operator(ve, od, oper, is_bracket_assign, prec);

    if (is_bracket_assign)
        return push_bracket_assign(ve, oper, prec);

    if (is_oper ==  false)
        return push_expr(ve);

    // we have an operator without corresponding declaration, try to split
    try_split_operator(vi, ft);
};

template<class Config>
void operator_parser<Config>::push_operator(const iterm& ex, const operator_declaration& od, 
                        unique_string op_name, bool can_be_assign, long assign_prec)
{
    if (can_be_assign)
    {
        operator_declaration od2 = od;

        if (od2.has(ast::fixity_type::infix) == false)
        {
            ast::operator_info oi;
            oi.add_fixity(ast::fixity_type::infix);
            oi.m_precedence = assign_prec;

            od2.add_single(ex->get_location(), oi);
        };

        make_operator_item(ex, od, op_name, true, assign_prec);
    }
    else
    {
        make_operator_item(ex, od, op_name, false, assign_prec);
    }
    return;
};

template<class Config>
void operator_parser<Config>::make_operator_item(const iterm& ex, const operator_declaration& od, 
                    unique_string op_name, bool can_be_assign, long assign_prec)
{
    m_sequence.push_back(expr_or_oper::make_operator
                    (ex, od, op_name, can_be_assign, assign_prec));

    const std::string& str  = op_name.get_string();

    int s               = (int)str.size();
    int count_langle    = ast::special_operators::count_langle(str);
    int count_rangle    = ast::special_operators::count_rangle(str);

    if (count_langle == s)
    {
        m_pos_largle.push_back(pos_count((long)m_sequence.size(), count_langle));
        push_error_scope();
        return;
    }

    if (count_rangle != s)
        return;

    if (m_pos_largle.size() == 0)
        return;

    if (m_pos_largle.size() == 0)
        return;

    //consume > characters
    {
        long pos        = m_pos_largle.back().m_pos;
        long count      = m_pos_largle.back().m_count;

        //post_start must point to '<'
        long pos_start  = pos - 1;
        long pos_end    = (long)m_sequence.size();
        iterm brack     = make_angle_bracket(pos_start, pos_end);

        while(pos_end > pos_start + 1)
        {
            m_sequence.pop_back();
            --pos_end;
        };

        --count;
        --count_rangle;

        iterm angle_ex      = m_sequence[pos_start].get_expr();
        identifier vi       = Config::cast_to_ident_term(angle_ex).get_identifier();

        if (vi.get_fixity() != ast::fixity_type::undefined)
        {
            //fixity annotation is invalid
            error::operator_error_msg(m_eh).error_explicit_fixity_for_opening_bracket
                                        (angle_ex->get_location());
        }

        m_pos_largle.pop_back();
        m_sequence.pop_back();
        pop_error_scope();

        if (count > 0)
        {
            //we must push remaining < again; it is different operator            
            std::string op_str  = std::string(count, '<');
            unique_string op    = unique_string(op_str);

            identifier vi2      = make_new_operator(vi, op, ast::fixity_type::undefined);
            ident_term ve       = ident_term(vi2->get_location(), vi2);
            push_variable(ve);
        }

        push_basic(brack);            
    }
        
    if (count_rangle > 0)
    {
        identifier vi       = Config::cast_to_ident_term(ex).get_identifier();
        std::string op_str  = std::string(count_rangle, '>');
        unique_string op    = unique_string(op_str);

        identifier vi2      = make_new_operator(vi, op, vi.get_fixity());
        ident_term ve       = ident_term(vi2->get_location(), vi2);
        push_variable(ve);
    };

    return;
};

template<class Config>
typename operator_parser<Config>::iterm 
operator_parser<Config>::make_angle_bracket(long pos_start, long pos_end)
{
    //operators and expressions are already on the scope;
    //we must only build parse tree

    //first element is < last is >; we must ignore then    
    iterm ex = parse(pos_start + 1, pos_end - 1);

    identifier vi_open      = Config::cast_to_ident_term(m_sequence[pos_start].get_expr())
                                .get_identifier();
    identifier vi_close     = Config::cast_to_ident_term(m_sequence[pos_end-1].get_expr())
                                .get_identifier();

    unique_string op_langle = ast::special_operators::operator_langle();
    unique_string op_rangle = ast::special_operators::operator_rangle();

    identifier opening      = make_identifier(vi_open->get_location(), op_langle, 
                                ast::fixity_type::undefined, make_kind_brack_part());

    identifier closing      = make_identifier(vi_close->get_location(), op_rangle, 
                                ast::fixity_type::undefined, make_kind_brack_part());

    // build temporary bracket term to store opening and closing
    // brackets
    ibracket_term brack(ex->get_location());
    brack.add(ex);
    brack.set_brackets(false, opening, closing);

    return brack;
};

template<class Config>
void operator_parser<Config>::try_split_operator(const identifier& vi, ast::fixity_type ft)
{
    unique_string oper          = vi.get_name_simple();
    std::string oper_str        = oper.get_string();
    const ast::location& loc    = vi->get_location();

    std::string start, end;

    if (oper_str.size() <= 1)
        goto lab_error;

    {
        if (oper_str.size() == 0)
        {
            //successfull split
            goto label_success;
        };

        //try to find longest match
        int32_t index_last  = (long)oper_str.size();

        while (index_last > 0)
        {
            int32_t code    = read_previous_code_point_from_utf8(oper_str, index_last);

            if (code < 0)
                break;

            start           = oper_str.substr(0, index_last);
            end             = oper_str.substr(index_last);
            bool exist_op   = check_exist_operator(loc, start);

            if (exist_op == true)
                break;
        };

        if (index_last == 0)
            goto lab_error;
        else
            goto label_success;
    };

  lab_error:
    {
        error::operator_error_msg(m_eh).error_undefined_operator(vi->get_location(), oper, 
                                    ast::fixity_type::undefined);
        return;
    };

  label_success:
    {
        unique_string op    = unique_string(start);
        push_split_operator(op, vi);

        unique_string op_end= unique_string(end);
        identifier vi2  = make_new_operator(vi, op_end, ft);
        ident_term ve         = ident_term(vi2->get_location(), vi2);
        push_variable(ve);
    }
};

template<class Config>
void operator_parser<Config>::push_split_operator(unique_string op, const identifier& base_op)
{
    ast::fixity_type ft     = ast::fixity_type::undefined;
    const ast::location& loc= base_op->get_location();
    identifier vi_loc       = make_identifier(loc, op, ft, make_kind_oper());

    ident_term ve_loc       = ident_term(loc, vi_loc);

    bool found;
    bool ambig;
    operator_declaration od = get_operator_declaration(loc, m_local_scope, op, ft, 
                                false, true, found, ambig);

    if (ambig == true)
        return;

    long prec;
    bool is_bracket_assign  = is_bracket_assign_symbol(loc, m_local_scope, op, true, prec);

    if (found == true)
    {
        push_operator(ve_loc, od, op, is_bracket_assign, prec);
        return;
    }

    if (is_bracket_assign)
    {
        push_bracket_assign(ve_loc, op, prec);
        return;
    }

    // this should be impossible
    error::error_base_msg(m_eh).internal_error(loc, "after succesful split operator should be defined");
    return;
}

template<class Config>
bool operator_parser<Config>::check_exist_operator(const ast::location& loc, const std::string& op_str)
{
    bool found;
    bool ambig;
    unique_string oper      = unique_string(op_str);
    ast::fixity_type ft     = ast::fixity_type::undefined;
    operator_declaration od = get_operator_declaration(loc, m_local_scope, oper, ft, 
                                false, false, found, ambig);

    long prec;
    bool is_bracket_assign  = is_bracket_assign_symbol(loc, m_local_scope, oper, false, prec);

    if (found == true || is_bracket_assign == true)
        return true;
    else
        return false;
};

template<class Config>
bool operator_parser<Config>::is_bracket_assign_symbol(seq_iterator it_oper, seq_iterator it_bracket,
                            ast::fixity_type ft, bool error_if_ambig, long& prec)
{
    if (it_oper->is_assign() == false)
        return false;

    unique_string oper          = it_oper->get_operator();
    const ast::location& loc    = it_oper->get_location();
    return is_bracket_assign_symbol_impl(loc, m_local_scope, oper, 
                        it_bracket, ft, error_if_ambig, prec);
};

template<class Config>
bool operator_parser<Config>::is_bracket_assign_symbol(const ast::location& loc, const local_scope& ls, 
                            unique_string oper, bool error_if_ambig, long& prec)
{
    if (is_last_item_bracket() == false)
        return false;

    return is_bracket_assign_symbol_impl(loc, ls, oper, m_sequence.end() - 1, 
                        ast::fixity_type::undefined, error_if_ambig, prec);
};

template<class Config>
bool operator_parser<Config>::is_bracket_assign_symbol_impl(const ast::location& loc, const local_scope& ls, 
                            unique_string oper, seq_iterator it_bracket, ast::fixity_type ft,
                            bool error_if_ambig, long& prec)
{
    prec                = it_bracket->get_precedence();
    item_kind kind      = it_bracket->get_kind();

    if (kind == item_kind::simple_bracket || kind == item_kind::dot_bracket)
    {
        ibracket_term be        = Config::cast_to_bracket(m_sequence.back().get_expr());
        unique_string bo        = get_bracket_operator_name(be);

        unique_string assign    = unique_string(bo.get_string() + oper.get_string());

        bool found;
        bool ambig;

        get_operator_declaration(loc, ls, assign, ft, false, error_if_ambig, found, ambig);

        return found;
    }
    else if (kind == item_kind::simple_tuple)
    {
        unique_string assign    = unique_string("()" + oper.get_string());

        bool found;
        bool ambig;

        get_operator_declaration(loc, ls, assign, ft, false, error_if_ambig, found, ambig);

        return found;
    }
    else if (kind == item_kind::dot_tuple)
    {
        unique_string assign    = unique_string(".()" + oper.get_string());

        bool found;
        bool ambig;

        get_operator_declaration(loc, ls, assign, ft, false, error_if_ambig, found, ambig);

        return found;
    }
    else
    {
        return false;
    }
};

template<class Config>
ast::unique_string operator_parser<Config>::get_bracket_operator_name(const ibracket_term& be)
{
    const identifier& op    = be.get_opening_bracket();
    const identifier& close = be.get_closing_bracket();
    bool is_dot             = be.is_dot();

    std::string op_name     = (is_dot == true) ? "." : "";
    op_name                 += op.get_name_simple().get_string() + close.get_name_simple().get_string();
    unique_string oper      = unique_string(op_name);
    return oper;
};

template<class Config>
bool operator_parser<Config>::is_last_item_bracket() const
{
    if (m_sequence.size() == 0)
        return false;

    item_kind kind  = m_sequence.back().get_kind();

    if (kind == item_kind::simple_bracket || kind == item_kind::simple_tuple
        || kind == item_kind::dot_bracket || kind == item_kind::dot_tuple)
    {
        return true;
    }
    else
    {
        return false;
    }
};

template<class Config>
ast::identifier operator_parser<Config>::make_new_operator(const identifier& base, unique_string op, 
                                                  ast::fixity_type ft)
{
    const ast::location& loc    = base->get_location();
    identifier vi_loc           = make_identifier(loc, op, ft, make_kind_oper());
    return vi_loc;
};

template<class Config>
operator_declaration 
operator_parser<Config>::get_operator_declaration(const ast::location& loc, const local_scope& ls, 
                        unique_string oper, ast::fixity_type ft, bool error_if_not_found, 
                        bool error_if_ambig, bool& found, bool& ambig_flag)
{
    operator_declaration od;
    std::vector<ast::location> ambig;    

    if (ft == ast::fixity_type::undefined)
    {
        ls.check_operator_type(oper, od, true, ambig);
        found  = od.is_defined();
    }
    else
    {
        od = ls.get_operator_declaration(oper, ft, true, found, ambig);
    }

    if (found == true && ambig.size() != 0)
    {
        if (error_if_ambig == true)
        {
            error::operator_error_msg(m_eh).error_ambiguous_declarations_of_operator
                (loc, oper, ambig);
        };

        found       = true;
        ambig_flag  = true;
        return od;
    }
    else if (found == false)
    {
        if (error_if_not_found == true)
            error::operator_error_msg(m_eh).error_undefined_operator(loc, oper, ft);

        found       = false;
        ambig_flag  = false;
        return od;
    }

    found       = true;
    ambig_flag  = false;
    return od;
};

template<class Config>
typename operator_parser<Config>::iterm 
operator_parser<Config>::parse()
{
    long n_angle_scopes = (long)m_pos_largle.size();

    while(n_angle_scopes > 0)
    {
        pop_error_scope();
        --n_angle_scopes;
    };

    long start_pos      = 0;
    long end_pos        = (long)(m_sequence.end() - m_sequence.begin());

    iterm ex = parse(start_pos, end_pos);
    return ex;
}

template<class Config>
typename operator_parser<Config>::iterm 
operator_parser<Config>::parse(long pos_start, long pos_end)
{
    if (m_eh->has_errors() == true)
        return Config::make_error(m_main_location);

    long size = pos_end - pos_start;

    if (size == 1)
        return make_single_expr(m_sequence[pos_start]);

    // For each operator occurrence determine whether the operator
    // is used as an infix, prefix or postfix operator. This is done using
    // the operator's position in the sequence.
    bool ok             = assign_fixities(pos_start, pos_end);
    ok                  &= resolve_ambiguities(pos_start, pos_end);

    if (ok == false)
        return Config::make_error(m_main_location);

    //build parse tree 
    seq_iterator it     = m_sequence.begin() + pos_start;
    seq_iterator end    = m_sequence.begin() + pos_end;

    bool error;
    expr_holder ex_h    = build_parse_tree(0, it, it, end, error);

    if (error == true)
        return Config::make_error(m_main_location);

    iterm ex            = ex_h.make(this);
    return ex;
};

/// For every operator item in a given sequence determine whether it is used
/// as a prefix, infix or postfix, depending on the context.
/// For example, in '- 1' the operator '-' is used as prefix, while in '1 - 2'
/// it is used as infix.
template<class Config>
bool operator_parser<Config>::assign_fixities(long start, long end, bool second_call)
{
    // at this stage fixity flag cannot contain indexing or surrounding

    {
        long item           = end;
        long prev_item      = end;

        // Forward pass
        for (auto it = start; it != end; it++)
        {
            prev_item       = item;
            item            = it;

            if (it == start)
            {
                // sequenece cannot start with a postfix or infix
                m_sequence[it].remove_infix();
                m_sequence[it].remove_postfix();
            }
            else
            {
                if (m_sequence[prev_item].is_undefined() || m_sequence[prev_item].is_postfix())
                {
                    m_sequence[it].remove_prefix();
                }                
                else if (m_sequence[prev_item].is_infix() || m_sequence[prev_item].is_prefix()
                      || m_sequence[prev_item].is_prefix_or_infix())
                {
                    // infix or postfix cannot follow an infix or prefix operator
                    m_sequence[it].remove_infix();
                    m_sequence[it].remove_postfix();
                }
            }
        }
    };

    // Backward pass
    {
        long item           = start - 1;
        long prev_item      = start - 1;

        for (auto it = end - 1; it != start; --it)
        {
            prev_item   = item;
            item        = it;

            if (it == end - 1)
            {
                //last item cannot be prefix or infix
                m_sequence[it].remove_prefix();
                m_sequence[it].remove_infix();
            }
            else
            {
                if (m_sequence[prev_item].is_undefined() || m_sequence[prev_item].is_prefix())
                {
                    // this is the single place at which the backward pass
                    // is not a mirror image of the forward pass:
                    if (m_sequence[prev_item].is_postfix_expr() == false)
                        m_sequence[it].remove_postfix();
                }
                // infix or prefix cannot precede an infix or postfix operator
                else if (m_sequence[prev_item].is_infix() || m_sequence[prev_item].is_postfix()
                      || m_sequence[prev_item].is_postfix_or_infix())
                {
                    m_sequence[it].remove_infix();
                    m_sequence[it].remove_prefix();
                }
            }
        }
    };

    bool fixity_modified    = false;

    // Additional pass to handle ambiguous occurrence of an infix/postfix operator preceding
    // a non-operator, as in
    //   a @ (b + c)
    // where @ is postfix and infix. Disambiguate as an infix use, as postfix use may be
    // enforced by:
    //   (a @) (b + c)
    for (auto it = start; it != end; it++)
    {
        if ( m_sequence[it].is_postfix_or_infix()
            && (it != end - 1) && (m_sequence[it + 1].is_undefined()))
        {
            m_sequence[it].set_infix();
            fixity_modified     = true;

            error::operator_error_msg(m_eh)
                .warning_ambiguous_parsing_operator(m_sequence[it].get_location(),
                     m_sequence[it].get_operator());   
        }
    }

    if (fixity_modified)
    {
        if (second_call == true)
        {
            error::operator_error_msg(m_eh)
                .error_unable_parse_operators(m_sequence[0].get_location());
            return false;
        };

        assign_fixities(start, end, true);
    }

    return true;
};

template<class Config>
bool operator_parser<Config>::resolve_ambiguities(long start, long end)
{
    // Check for ambiguities and resolve operators.
    //
    // If we find out that some operator cannot be used as either infix,
    // prefix or postfix, then the sequence has no valid parse.
    // The simplest example is the sequence [1, +, 1], where '+' is not
    // declared as infix.
    //
    // If we find out that some operator can be used in more that one
    // mode, then we have ambiguous parse, which is an error.
    // For example: [1, +, -, 2], where '+' is may be postfix or infix
    // and '-' may be infix or prefix.

    for (auto it = start; it != end; it++)
    {
        expr_or_oper& item    = m_sequence[it];

        if (item.get_kind() != item_kind::oper)
            continue;

        ast::operator_info::fixity fixity = expr_or_oper::unique_fixity(item);

        if (fixity == ast::operator_info::undefined)
        {
            if (item.is_undefined())
            {
                error::operator_error_msg(m_eh).error_no_valid_parse_for_operator
                        (item.get_location(), item.get_operator());
            }
            else
            {
                bool has_infix      = item.has_infix();
                bool has_prefix     = item.has_prefix();
                bool has_postfix    = item.has_postfix();
                error::operator_error_msg(m_eh).error_ambiguous_parse_for_operator
                        (item.get_location(), item.get_operator(), has_infix, has_prefix, has_postfix);
            }

            return false;
        }
    }

    return true;
};

// Builds an AST for a sequence of operators until the end of sequence
// or an operator with a precedence lower than min_prec is encountered.
//
//  Tricky cases:
//  - 'a (b) c (d)' may be parsed as '(a(b)) c (d)'
//    or 'a ((b) c (d))', if 'a' and 'c' are operators.
//  - 'a! (b)' needs application before '(b)', if '!' is postfix
template<class Config>
expr_holder<Config>
operator_parser<Config>::build_parse_tree(long precedence, seq_iterator prev_operator, 
                                  seq_iterator& start, seq_iterator end, bool& error)
{
    expr_holder lhs;
    expr_holder rhs;
    error = false;

    seq_iterator item   = start;

    if (item->get_kind() == item_kind::oper)
    {
        // expression starts with prefix operator
        ++start;

        rhs             = build_parse_tree(item->get_precedence(), 
                            prev_operator, start, end, error);

        if (error == true)
            return expr_holder();

        lhs             = expr_holder::make_operator_expr(this, expr_holder(), item, rhs);
        prev_operator   = start;
    }
    else
    {
        // expression starts with an expression
        lhs             = expr_holder::make_single_expr(this, item);
        ++start;
    }

    // first expression is parsed, continue parsing operators at the current or higher level
    while (start != end)
    {
        item            = start;

        if (item->get_kind() == item_kind::oper)
        {
            long next_prec  = item->get_precedence();

            if (next_prec < precedence)
            {
                break;
            }
            else if (next_prec == precedence)
            {
                if (item->get_assoc() == ast::associativity_type::right)
                {
                    //continue parsing at this level                    
                }
                else if (item->get_assoc() == ast::associativity_type::left)
                {
                    break;
                }
                else if (prev_operator->get_kind() == item_kind::oper)
                {
                    //non associative operator: error
                    error::operator_error_msg(m_eh).error_invalid_sequence_of_nonassociative_operators
                        (item->get_location(), prev_operator->get_operator(), prev_operator->get_unique_fixity(),
                         item->get_operator(), item->get_unique_fixity());

                    error = true;
                    return expr_holder();
                };
            };

            if (item->is_postfix())
            {
                lhs             = expr_holder::make_operator_expr(this, lhs, item, expr_holder());
                prev_operator   = start;
                ++start;                
            }
            else // infix operator
            {
                prev_operator = start;
                ++start;

                rhs     = build_parse_tree(next_prec, prev_operator, start, end, error);

                if (error == true)
                    return expr_holder();

                lhs     = expr_holder::make_operator_expr(this, lhs, item, rhs);
            }
        }
        else
        {
            // expression lhs item ~-> lhs * item, where * is call
            // other expressions:
            //      lhs .[item];    lhs.<item>;     lhs {item}
            long next_prec  = item->get_precedence();

            if (next_prec <= precedence)
            {
                break;
            }
            else
            {                
                const expr_holder* rhs_ptr;

                rhs     = build_parse_tree(next_prec, prev_operator, start, end, error);
                if (error == true)
                    return expr_holder();

                rhs_ptr = make_delayed_ptr(rhs);
                lhs     = expr_holder::make_postfix_expr(this, lhs, rhs_ptr);
            }
        };
    }

    return lhs;
};

template<class Config>
const expr_holder<Config>* 
operator_parser<Config>::make_delayed_ptr(const expr_holder& eh)
{
    m_bufor.push_back(eh);
    return &m_bufor.back();
}

template<class Config>
typename operator_parser<Config>::iterm 
operator_parser<Config>::make_single_expr(const expr_or_oper& item)
{
    //one-element tuple can be expanded only when we are building
    //application term
    switch(item.get_kind())
    {
        case item_kind::expr:
            return item.get_expr();
       
        case item_kind::oper:
        {
            const ident_term& ve = Config::cast_to_ident_term(item.get_expr());
            return final_operator(item.get_expr(), ve.get_identifier(), ast::fixity_type::undefined);
        }

        case item_kind::simple_tuple:
        {
            return item.get_expr();
        }

        case item_kind::simple_bracket:
        {            
            const ibracket_term& be     = Config::cast_to_bracket(item.get_expr());
            unique_string oper          = get_bracket_operator_name(be);
            ast::fixity_type ft         = ast::fixity_type::surrounding;
            const ast::location& loc    = be->get_location();

            bool found, ambig;
            operator_declaration od = get_operator_declaration(loc, m_local_scope, oper, 
                                            ft, true, true, found, ambig);

            if (found == false || ambig == true)
                return Config::make_error(loc);

            iterm args                  = convert_bracket_to_tuple(be);
            return make_bracket_operator_call(args, oper, ft, iterm());
        }

        case item_kind::projection:
        case item_kind::dot:
        case item_kind::applic_type:
        case item_kind::applic_kind:
        case item_kind::dot_tuple:
        case item_kind::dot_bracket:
        {
            // postfix item cannot be a single expression
            ast::location loc       = item.get_location();
            error::error_base_msg(m_eh).internal_error(loc, "postfix term as single expression");
            return Config::make_error(loc);
        }

        default:
        {
            //invalid case
            ast::location loc       = item.get_location();
            error::error_base_msg(m_eh).internal_error(loc, "unknown item kind in sequence expression");
            return Config::make_error(loc);
        }
    }
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::final_operator(const iterm& ex, const identifier& vi, ast::fixity_type ft_required)
{
    bool found, ambig;
    const ast::location& loc    = vi->get_location();
    
    ast::fixity_type ft_var     = vi.get_fixity();
    ast::fixity_type ft_test    = ft_required;

    if (ft_test == ast::fixity_type::undefined)
        ft_test             = ft_var;

    if (ft_test == ast::fixity_type::undefined)
    {
        ast::identifier_kind k  = vi.get_symbol_data()->get_kind();

        if (k.is_bracket_full() == true)
            ft_test = ast::operator_info::default_fixity_bracket;
        else
            ft_test = ast::operator_info::default_fixity_operator;
    };

    bool is_brack   = vi.get_symbol_data()->get_kind().is_bracket_full();

    if (ft_var == ast::fixity_type::undefined)
    {
        if (is_brack == true)
            ft_var = ast::operator_info::default_fixity_bracket;
        else
            ft_var = ast::operator_info::default_fixity_operator;
    };

    get_operator_declaration(loc, m_local_scope, vi.get_name_simple(), ft_test, true, true, found, ambig);

    if (found == false || ambig == true)
        return Config::make_error(loc);

    if (ft_var == ft_test)
        return ex;

    //we need to correct fixity
    identifier vi_ret;
    
    if (is_brack == true)
        vi_ret  = make_identifier(loc, vi.get_name_simple(), ft_test, make_kind_brack_full());
    else
        vi_ret  = make_identifier(loc, vi.get_name_simple(), ft_test, make_kind_oper());

    ident_term ve_ret(loc, vi_ret);
    return ve_ret;
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_operator_expr(const iterm& lhs, seq_iterator item, const iterm& rhs)
{
    iterm lhs_quoted    = lhs;
    iterm rhs_quoted    = rhs;

    ast::fixity_type ft;

    if (lhs && rhs)
        ft              = ast::fixity_type::infix;
    else if (rhs)
        ft              = ast::fixity_type::prefix;
    else
        ft              = ast::fixity_type::postfix;

    iterm oper          = item->get_expr();
    identifier vi       = Config::cast_to_ident_term(item->get_expr()).get_identifier();
    ast::location loc   = oper->get_location();

    if (lhs)
    {
        if (item->get_def().get_info(ft).do_quote_left())
            lhs_quoted  = get_quoted(lhs, vi.get_name_simple());
    }

    if (rhs)
    {
        if (item->get_def().get_info(ft).do_quote_right())
            rhs_quoted  = get_quoted(rhs, vi.get_name_simple());
    }

    oper                = final_operator(oper, vi,  ft);

    if (lhs_quoted && rhs_quoted)
    {
        //infix:    [a op b] ~-> [op a b] ~-> [op a] b
        iterm ret       = make_application(loc, oper, lhs_quoted);
        ret             = make_application(loc, ret, rhs_quoted);
        return ret;
    }
    else if (rhs_quoted)
    {
        //prefix:   [_ op b]
        iterm ret        = make_application(loc, oper, rhs_quoted);
        return ret;
    }
    else
    {
        //postfix:  [a op _]
        iterm ret       = make_application(loc, oper, lhs_quoted);
        return ret;
    };
}

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::get_quoted(const iterm& arg, ast::unique_string op_name)
{
    if (Config::is_ident_term(arg) == false)
    {
        error::operator_error_msg(m_eh).error_quoting_operator_requires_identifier
                                            (arg->get_location(), op_name);
        return Config::make_error(arg->get_location());
    };

    ast::fixity_type ft;
    unique_string sym = Config::cast_to_ident_term(arg).get_identifier().get_name(ft);

    if (ft != ast::fixity_type::undefined)
    {
        error::operator_error_msg(m_eh).error_quoting_operator_identifier_with_fixity
                                            (arg->get_location(), op_name, sym, ft);
        return Config::make_error(arg->get_location());
    };

    return Config::make_string_literal(arg->get_location(), sym.get_string());
}

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_postfix_expr(const iterm& lhs, const iterm& rhs)
{
    return make_application(rhs->get_location(), lhs, rhs);
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_postfix_expr(const iterm& lhs, seq_iterator item)
{
    iterm suffix        = item->get_expr();
    ast::location loc   = suffix->get_location();

    unique_string oper;
    iterm args;
    bool must_indexing  = false;

    switch(item->get_kind())
    {
        case item_kind::expr:
        {
            //expression like func x
            iterm ret   = make_application(loc, lhs, suffix);
            return ret;
        }

        case item_kind::projection:
        {
            return Config::make_projection(loc, lhs, item->get_expr());
        }

        case item_kind::dot:
        {
            identifier path     = Config::cast_to_ident_term(item->get_expr())
                                    .get_identifier();
            return Config::make_dot(loc, lhs, path, m_eh);
        }

        case item_kind::applic_type:
        {
            return Config::make_applic_type(loc, lhs, item->get_expr());
        };

        case item_kind::applic_kind:
        {
            return Config::make_applic_kind(loc, lhs, item->get_expr());
        };

        case item_kind::simple_tuple:        
        {
            if (check_can_build_indexing_tuple() == true)
            {
                args            = item->get_expr();
                oper            = ast::unique_string("()");
                must_indexing   = true;
                break;
            }
            else
            {
                //expression like func (x)
                iterm suffix2   = item->get_expr();
                iterm ret       = make_application(loc, lhs, suffix2);
                return ret;
            };
        }

        case item_kind::dot_tuple:
        {
            args            = item->get_expr();
            oper            = ast::unique_string(".()");
            must_indexing   = true;
            break;
        }

        case item_kind::simple_bracket:        
        {
            ibracket_term be    = Config::cast_to_bracket(item->get_expr());
            oper                = get_bracket_operator_name(be);
            args                = convert_bracket_to_tuple(be);
            break;
        }

        case item_kind::dot_bracket:
        {
            ibracket_term be    = Config::cast_to_bracket(item->get_expr());
            oper                = get_bracket_operator_name(be);
            args                = convert_bracket_to_tuple(be);
            must_indexing       = true;
            break;
        }

        //oper is impossible here
        case item_kind::oper:
        default:
        {
            error::error_base_msg(m_eh).internal_error
                    (loc, "invalid item kind found during building postfix expression");
            return Config::make_error(loc);
        };
    };

    /// build bracket calls

    //try indexing 
    {
        ast::fixity_type ft = ast::fixity_type::indexing;
        bool found, ambig;
        bool error_not_found= must_indexing == true;

        get_operator_declaration(loc, m_local_scope, oper, ft, error_not_found, 
                                 true, found, ambig);

        if (found == true)
        {
            if (ambig == true)
                return Config::make_error(loc);
            
            // create an application of a postfix bracket operator: 
            // a[e1,...,e_n] --> $[](a,(e1,...,e_n))
            return make_bracket_operator_call(args, oper, ft, lhs);
        }

        if (must_indexing == true)
            return Config::make_error(loc);
    }

    //try surrounding
    {
        ast::fixity_type ft = ast::fixity_type::surrounding;
        bool found, ambig;
        get_operator_declaration(loc, m_local_scope, oper, ft, true, true, found, ambig);

        if (found == false  || ambig == true)
            return Config::make_error(loc);
            
        // create an application of a surrounding bracket operator: 
        // [e1,...,e_n] --> $[]((e1,...,e_n))
        iterm surr = make_bracket_operator_call(args, oper, ft, iterm());

        iterm ret = make_application(loc, lhs, surr);
        return ret;
    }
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_application(const ast::location& loc, const iterm& lhs, const iterm& rhs)
{
    iterm_applic ret(loc, expand_tuple(lhs), expand_tuple(rhs));
    return ret;
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::expand_tuple(const iterm& ex)
{
    if (Config::is_tuple(ex) == false)
        return ex;

    ituple_term tt      = Config::cast_to_tuple(ex);
    bool can_expand     = tt.size() == 1 && tt.get_state() != ast::tuple_term::fixed;

    if (can_expand == false)
        return ex;
    else
        return tt.get(0);
};

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_operator_assign_expr(const expr_holder& lhs, seq_iterator item, 
                    const expr_holder& rhs)
{
    //we know that this is an index assign or surrounding assing call and the
    //operator is declared and assign operator is declared

    iterm oper_ex       = item->get_expr();
    unique_string oper  = item->get_operator();
    ast::location loc   = oper_ex->get_location();

    iterm e_rhs         = rhs.make(this);

    iterm e_prefix;
    seq_iterator it_brack;
    ast::fixity_type ft;
    item_kind kind      = lhs.decompose_bracket(e_prefix, it_brack, ft);

    // create an application of a bracket assign operator: 
    // a[e1,...,e_n] := rhs --> ${oper}(a,(e1,...,e_n), rhs) for indexing
    //  [e1,...,e_n] := rhs --> ${oper}((e1,...,e_n), rhs)   for surrounding
    //
    // where oper is []:=, ():= or other

    switch(kind)
    {
        case item_kind::simple_bracket:
        case item_kind::dot_bracket:
        {
            ibracket_term be        = Config::cast_to_bracket(it_brack->get_expr());
            unique_string bo        = get_bracket_operator_name(be);
            unique_string assign    = unique_string(bo.get_string() + oper.get_string());
            iterm args              = convert_bracket_to_tuple(be);

            return make_bracket_assign_operator_call(args, assign, ft, e_prefix, e_rhs);
        }
        case item_kind::simple_tuple:
        {
            iterm args              = it_brack->get_expr();
            unique_string assign    = unique_string("()" + oper.get_string());
            return make_bracket_assign_operator_call(args, assign, ft, e_prefix, e_rhs);
        }
        case item_kind::dot_tuple:
        {
            iterm args              = it_brack->get_expr();
            unique_string assign    = unique_string(".()" + oper.get_string());
            return make_bracket_assign_operator_call(args, assign, ft, e_prefix, e_rhs);
        }
        default:
        {
            error::error_base_msg(m_eh).internal_error(loc, "unknown bracket kind");
            return Config::make_error(loc);
        }
    }
}

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_bracket_operator_call(const iterm& args, unique_string op_name, 
                    ast::fixity_type ft, const iterm& prefix)
{
    ast::location loc   = args->get_location();
    ident_term oper     = make_bracket_operator(loc, op_name, ft);

    if (ft == ast::fixity_type::indexing)
    {
        //a [e] ~-> op(a, e)
        iterm ret       = make_application(loc, oper, prefix);
        ret             = make_application(loc, ret, args);
        return ret;
    }
    else
    {
        //[e] ~-> op(e)
        iterm ret       = make_application(loc, oper, args);
        return ret;
    }
}

template<class Config>
typename operator_parser<Config>::iterm
operator_parser<Config>::make_bracket_assign_operator_call(const iterm& args, unique_string op_name, 
                    ast::fixity_type ft, const iterm& prefix, const iterm& rhs)
{
    ast::location loc   = prefix->get_location();
    ident_term oper     = make_bracket_operator(loc, op_name, ft);    

    if (ft == ast::fixity_type::indexing)
    {
        //a [e] := x ~-> op(a, e, x)
        iterm ret       = make_application(loc, oper, prefix);
        ret             = make_application(loc, ret, args);
        ret             = make_application(loc, ret, rhs);
        return ret;
    }
    else
    {
        //[e] := x ~-> op(e, x)
        iterm ret       = make_application(loc, oper, args);
        ret             = make_application(loc, ret, rhs);
        return ret;
    }
}

template<class Config>
typename operator_parser<Config>::ident_term
operator_parser<Config>::make_bracket_operator(const ast::location& loc, unique_string op_name, 
                    ast::fixity_type ft)
{
    identifier vi   = make_identifier(loc, op_name, ft, make_kind_brack_full());
    ident_term ve   = ident_term(loc, vi);
    return ve;
};

template<class Config>
typename operator_parser<Config>::iterm 
operator_parser<Config>::convert_bracket_to_tuple(const ibracket_term& be)
{
    ituple_term ex = ituple_term(be->get_location(), ituple_term::can_expand);

    long n      = be.size();

    for (long i = 0; i < n; ++i)
        ex.add(be.get(i));

    return ex;
};

template<class Config>
ast::identifier operator_parser<Config>::make_identifier(const ast::location& loc, const unique_string name,
                    const ast::fixity_type ft, ast::identifier_kind ki)
{
    ast::identifier ret(loc, name, ft, ki, ast::identifier::fresh());
    ast::symbol_data* sd    = ret.get_symbol_data();

    sd->set_module(m_module.get_module_index());
    sd->set_local_scope(m_local_scope.get_scope_index());

    return ret;
};

template<class Config>
ast::identifier_kind operator_parser<Config>::make_kind_brack_part()
{
    ast::identifier_kind ret;
    ret.set_lex(ast::identifier_kind::lexical_type::bracket_part);
    ret.set_sector(ast::identifier_kind::sector_type::term);
    ret.set_function(ast::identifier_kind::function_type::def);
    return ret;
};

template<class Config>
ast::identifier_kind operator_parser<Config>::make_kind_brack_full()
{
    ast::identifier_kind ret;
    ret.set_lex(ast::identifier_kind::lexical_type::bracket_full);
    ret.set_sector(ast::identifier_kind::sector_type::term);
    ret.set_function(ast::identifier_kind::function_type::def);
    return ret;
};

template<class Config>
ast::identifier_kind operator_parser<Config>::make_kind_oper()
{
    ast::identifier_kind ret;
    ret.set_lex(ast::identifier_kind::lexical_type::oper);
    ret.set_sector(ast::identifier_kind::sector_type::term);
    ret.set_function(ast::identifier_kind::function_type::def);
    return ret;
};

template<class Config>
bool operator_parser<Config>::check_can_build_indexing_tuple()
{
    if (m_indexing_tuple.has_value())
        return m_indexing_tuple.get_value();

    unique_string oper          = unique_string("()");
    ast::fixity_type ft         = ast::fixity_type::indexing;
    ast::location loc;

    bool found, ambig;
    operator_declaration od     = get_operator_declaration(loc, m_local_scope, oper, 
                                    ft, false, false, found, ambig);

    if (found == true && ambig == false)
    {
        m_indexing_tuple.set_value(true);
        return true;
    }
    else
    {
        m_indexing_tuple.set_value(false);
        return false;
    };
};

template<class Config>
int operator_parser<Config>::get_dot_tuple_precedence()
{
    unique_string oper          = unique_string(".()");
    ast::fixity_type ft         = ast::fixity_type::indexing;
    ast::location loc;

    bool found, ambig;
    operator_declaration od     = get_operator_declaration(loc, m_local_scope, oper, 
                                    ft, false, false, found, ambig);

    if (found == true && ambig == false)
    {
        const ast::operator_info& i = od.get_info(ft);
        return i.get_precedence();
    }
    else
    {
        return (int)ast::named_precedence::bracket;
    };
};

template<class Config>
int operator_parser<Config>::get_bracket_precendence(const ibracket_term& bt)
{
    unique_string oper          = get_bracket_operator_name(bt);
    ast::fixity_type ft         = ast::fixity_type::indexing;
    const ast::location& loc    = bt->get_location();

    bool found, ambig;
    operator_declaration od     = get_operator_declaration(loc, m_local_scope, oper, 
                                    ft, false, false, found, ambig);

    if (found == true && ambig == false)
    {
        const ast::operator_info& i = od.get_info(ft);
        return i.get_precedence();
    }
    else
    {
        return (int)ast::named_precedence::bracket;
    };
}

template class operator_parser<operator_config_type>;
template class operator_parser<operator_config_term>;

};};
