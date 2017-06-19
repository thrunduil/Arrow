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
#include "grammar/output/parser_arrow.hpp"
#include "grammar/output/lexer_arrow.hpp"
#include "unicode.h"
#include "literal_parser.h"
#include "semantics/semantics.h"
#include "semantics/utils.h"
#include "error/error.h"
#include "scopes/symbol_data.h"
#include "antlr/CommonHiddenStreamToken.hpp"


#pragma warning(push)
#pragma warning(disable:4702)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

//---------------------------------------------------------------
//                      term_collector
//---------------------------------------------------------------
term_collector::term_collector(const ast::location& loc)
    :m_term(loc)
{}

void term_collector::add(const ast::term& ex)
{
    m_term.add(ex->get_location(), ex);
};

void term_collector::add_dot_brack(ast::bracket_term& ex)
{
    ex.set_dot(true);
    m_term.add(ex->get_location(), ex);
};
void term_collector::add_dot_tuple(ast::tuple_term& ex)
{
    ex.set_state(ast::tuple_term::dot);
    m_term.add(ex->get_location(), ex);
};

void term_collector::add_proj(const ast::location& loc, const ast::int_literal& pos)
{
    m_term.add_projection(loc, pos);
};
void term_collector::add_dot(const ast::location& loc, const ast::identifier& id)
{
    m_term.add_dot(loc, id);
};
void term_collector::add_type_applic(const ast::location& loc, const std::vector<ast::type>& args)
{
    m_term.add_type_application(loc, args);
};

void term_collector::add_kind_applic(const ast::location& loc, const std::vector<ast::kind>& args)
{
    m_term.add_kind_application(loc, args);
};

ast::term term_collector::make_term() const
{
    return m_term;
}

//---------------------------------------------------------------
//                      type_collector
//---------------------------------------------------------------
type_collector::type_collector(const ast::location& loc)
    :m_type(loc)
{}

void type_collector::add(const ast::type& ex)
{
    m_type.add(ex->get_location(), ex);
};

void type_collector::add_dot_tuple(ast::tuple_type& ex)
{
    ex.set_state(ast::tuple_type::dot);
    m_type.add(ex->get_location(), ex);
};

void type_collector::add_proj(const ast::location& loc, const ast::int_literal& pos)
{
    m_type.add_projection(loc, pos);
};
void type_collector::add_dot(const ast::location& loc, const ast::identifier& id)
{
    m_type.add_dot(loc, id);
};
void type_collector::add_dot_brack(ast::bracket_type& ex)
{
    ex.set_dot(true);
    m_type.add(ex->get_location(), ex);
};
void type_collector::add_kind_applic(const ast::location& loc, const std::vector<ast::kind>& args)
{
    m_type.add_kind_application(loc, args);
};

void type_collector::add_type_applic(const ast::location& loc, const std::vector<ast::type>& args)
{
    m_type.add_type_application(loc, args);
};


ast::type type_collector::make_type() const
{
    return m_type;
}

//---------------------------------------------------------------
//                      parser_arrow
//---------------------------------------------------------------

void parser_arrow::init(const error_handler_ptr& eh, unique_string file,
                        bool shell_mode, lexer_arrow* lexer)
{
    m_eh            = eh;
    m_file          = file;
    m_shell_mode    = shell_mode;
    m_lexer         = lexer;
};

bool parser_arrow::at_eof()
{
    return (LA(1) == antlr::Token::EOF_TYPE);
};

void parser_arrow::reportError(const std::string& str)
{
    if (m_eh)
    {
        if (m_shell_mode == true)
            throw std::runtime_error(str);

        m_eh->report_parser_error(location(), str);
    }
};

void parser_arrow::reportError(const antlr::RecognitionException& ex)
{
    if (m_eh)
    {
        if (m_shell_mode == true)
            throw;

        location loc = location(ex.getLine(), ex.getColumn(), get_file());
        m_eh->report_parser_error(loc, ex.getMessage());
    }
};
void parser_arrow::reportWarning(const std::string& str)
{
    if (m_eh)
        m_eh->report_parser_warning(location(), str);
};

void parser_arrow::consume_until_semi()
{
    int level = 0;

    for(;;)
    {
        int t   = LA(1);

        if (t == antlr::Token::EOF_TYPE)
            throw unwind_all();

        if (t == TOK_lcurly)
            ++level;

        if (t == TOK_rcurly)
            --level;

        if (t == TOK_semi && level == 0)
            break;

        if (is_command_start(t))
            throw unwind_until_command();

        consume();
    };

};

bool parser_arrow::is_command_start(int token)
{
    static antlr::BitSet bs_command = make_bitset_command();
    bool is_command = bs_command.member(token);

    if (is_command == true)
        return true;
    else
        return false;
};

/// these bitsets must contain all stating tokens of command rule that
/// appears only in the command rule;
/// these functions must be modified if grammar is changed
antlr::BitSet parser_arrow::make_bitset_command()
{
    antlr::BitSet bs;
	bs.add(TOK_type);
    bs.add(TOK_overload);

    return bs;
};


ast::unique_string parser_arrow::get_file()
{
    return m_file;
};

ast::location parser_arrow::get_location()
{
    antlr::RefToken l = LT(1);
    return ast::location(l->getLine(), l->getColumn(), get_file());
}

ast::location parser_arrow::get_location(const antlr::RefToken& token)
{
    return ast::location(token->getLine(), token->getColumn(), get_file());
};

std::string parser_arrow::get_text(const antlr::RefToken& tok)
{
    return tok->getText();
};
std::string parser_arrow::get_text_cont(const antlr::RefToken& token, bool& can_continue)
{
    antlr::CommonHiddenStreamToken* tok = dynamic_cast<antlr::CommonHiddenStreamToken*>(token.get());

    //we cannot continue, if next token is hidden
    //(i.e. white space, comments, etc)
    if (tok != nullptr)
    {
        if (tok->getHiddenAfter() == nullptr)
            can_continue = true;
        else
            can_continue = false;
    }
    else
    {
        can_continue = false;
    };

    return token->getText();
};


int64_t parser_arrow::get_int64(const antlr::RefToken& token)
{
    try
    {
        uint64_t val = literal_parser(m_eh, get_location(token)).parse_int(get_text(token));

        bool has_error;
        int64_t val2        = sema::cast_to_int64(get_location(token), val, m_eh, has_error);
        return val2;
    }
    catch (std::exception&)
    {
        return 0;
    }
};

//--------------------------------------------------
//              helpers
//--------------------------------------------------
ast::definitions parser_arrow::make_definitions(const ast::location& loc)
{
    return ast::definitions(loc);
}

ast::identifier parser_arrow::make_basic_identifier(const ast::location& loc, const std::string& str)
{
    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::ident);

    return ast::identifier(loc, unique_string(str), ast::fixity_type::undefined, ik,
                           ast::identifier::fresh());
};

ast::identifier parser_arrow::make_operator_identifier(const ast::location& loc, const std::string& str)
{
    if (str == ".")
    {
        throw antlr::RecognitionException("invalid operator name '.'", loc.get_file().get_string(),
                                          loc.get_line(), loc.get_column());
    };

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::oper);

    return ast::identifier(loc, unique_string(str), ast::fixity_type::undefined, ik,
                           ast::identifier::fresh());
};

ast::annotated_type_identifier parser_arrow::make_annotated_type_id(const ast::identifier& id)
{
    ast::kind ki = ast::kind_var(id->get_location(), false);
    return ast::annotated_type_identifier(id->get_location(), id, ki);
}

ast::annotated_type_identifier parser_arrow::make_annotated_type_id(const ast::identifier& id, 
                const ast::kind& ki)
{
    return ast::annotated_type_identifier(id->get_location(), id, ki);
}
ast::annotated_identifier parser_arrow::make_annotated_id(const ast::location& loc, 
                const ast::identifier& id, const ast::type& ta)
{
    return ast::annotated_identifier(loc, id, ta);
};

ast::annotated_identifier parser_arrow::make_annotated_underscore(const ast::location& loc, 
                const ast::type& ta)
{
    ast::identifier id = make_underscore(loc);
    return ast::annotated_identifier(loc, id, ta);
}

ast::identifier parser_arrow::make_underscore(const ast::location& loc)
{
    return ast::identifier::underscore(loc);
}

ast::module_definition parser_arrow::make_module_definition(const std::string& name, 
        const std::vector<string_loc>& imp, const ast::definitions& defs)
{
    return ast::module_definition(name, imp, defs);
};

parser_arrow::string_loc parser_arrow::make_import(const std::string& import, const std::string& alias, 
                        const ast::location& loc)
{
    if (alias == "")
        return string_loc(unique_string(import), unique_string(import), loc);
    else
        return string_loc(unique_string(import), unique_string(alias), loc);
};

//--------------------------------------------------
//              definitions
//--------------------------------------------------
ast::term_definition parser_arrow::make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const ast::type& t0, const ast::term& e0, bool over)
{
    ast::term e = make_term_kind_scope(e0->get_location(), kind_vars, e0);
    ast::type t = make_type_kind_scope(t0->get_location(), kind_vars, t0);

    return ast::term_definition(id->get_location(), id, t, e, over);
}
ast::term_definition parser_arrow::make_overload_def(const ast::identifier& id, const kind_id_vec& kind_vars)
{
    (void)kind_vars;
    return ast::term_definition(id->get_location(), id, ast::type(), ast::term(), true);
}
ast::term_definition parser_arrow::make_extern_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const ast::type& t0, bool over)
{
    ast::term e0= ast::term_constant(id->get_location(), id, true);
    ast::term e = make_term_kind_scope(e0->get_location(), kind_vars, e0);
    ast::type t = make_type_kind_scope(t0->get_location(), kind_vars, t0);

    return ast::term_definition(id->get_location(), id, t, e, over);
}

ast::term_definition parser_arrow::make_term_constant(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const ast::type& t0, bool over)
{
    ast::term e;
    
    if (over == false)
    {
        e       = ast::term_constant(id->get_location(), id, false);
        e       = make_term_kind_scope(e->get_location(), kind_vars, e);
    }
    
    ast::type t = make_type_kind_scope(t0->get_location(), kind_vars, t0);

    return ast::term_definition(id->get_location(), id, t, e, over);
}

ast::term_definition parser_arrow::make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const ast::term& e0, bool over)
{
    ast::type t0 = make_type_var(id->get_location());
    ast::term e = make_term_kind_scope(e0->get_location(), kind_vars, e0);
    ast::type t = make_type_kind_scope(t0->get_location(), kind_vars, t0);

    return ast::term_definition(id->get_location(), id, t, e, over);
}
ast::term_definition parser_arrow::make_term_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const term_id_vec& args, const ast::type& ret, const ast::term& e0, bool over)
{
    long n      = (long)args.size();

    ast::type t0;
    if (!ret)
    {
        t0      = make_type_var(id->get_location());
    }
    else
    {
        t0      = ret;

        for (long i = n - 1; i >= 0; --i)
            t0  = ast::arrow_type(args[i]->get_location(), args[i].get_type(), t0);
    }

    ast::term e = e0;
    ast::type t = make_type_kind_scope(t0->get_location(), kind_vars, t0);

    for (long i = n - 1; i >= 0; --i)
        e       = ast::lambda_term(args[i]->get_location(), args[i], e, ast::lambda_term::fresh());

    e           = make_term_kind_scope(e0->get_location(), kind_vars, e);

    return ast::term_definition(id->get_location(), id, t, e, over);
}

ast::type_definition_var parser_arrow::make_type_def_var(const ast::identifier& id, const kind_id_vec& kind_vars,
                                                         bool overload)
{
    return ast::type_definition_var(id->get_location(), id, kind_vars, overload);
}

ast::type_definition parser_arrow::make_type_def(const ast::identifier& id, const kind_id_vec& kind_vars, 
                    const ast::kind& ki_in, const ast::type& t_in, bool overload)
{
    ast::kind ki    = ki_in;
    ast::type t     = t_in;

    // either ki or t is set

    if (!t)
    {
        ast::kind kv= make_kind_var(id->get_location());
        t           = make_type_constant(id, kv, false);
    }

    if (!ki)
        ki          = make_kind_var(id->get_location());

    t               = make_type_kind_scope(t->get_location(), kind_vars, t);
    ki              = make_kind_scope(ki->get_location(), kind_vars, ki);

    return ast::type_definition(id->get_location(), id, ki, t, overload);
}

ast::type_definition parser_arrow::make_type_def_over(const ast::identifier& id, const kind_id_vec& kind_vars)
{
    (void)kind_vars;
    return ast::type_definition(id->get_location(), id, ast::kind(), ast::type(), true);
}

ast::type_definition parser_arrow::make_type_def_extern(const ast::identifier& id, 
                    const kind_id_vec& kind_vars, const ast::kind& ki_in, bool over)
{
    ast::kind ki    = ki_in;    
    ast::kind_var kv= ast::kind_var(id->get_location(), false);
    
    ast::type t     = make_type_constant(id, kv, true);

    t               = make_type_kind_scope(t->get_location(), kind_vars, t);
    ki              = make_kind_scope(ki->get_location(), kind_vars, ki);

    return ast::type_definition(id->get_location(), id, ki, t, over);
}

ast::term_use parser_arrow::make_term_use(const ast::term& e0, const kind_id_vec& kind_vars)
{
    ast::term e = make_term_kind_scope(e0->get_location(), kind_vars, e0);
    return ast::term_use(e->get_location(), e);
}

ast::type parser_arrow::make_type_constant(const ast::identifier& id, ast::kind ki,
                            bool extern_def)
{
    if (!ki)
        ki                  = ast::star_kind(id->get_location());

    ast::type_constant nt   = ast::type_constant(id->get_location(), id, ki, extern_def);
    return nt;
}

void parser_arrow::set_type_def_body(ast::type_definition_var& td, const ast::kind& ki, 
                        const ast::type& t)
{
    td.set_body(t);

    if (ki)
        td.set_return_kind(ki);
};

void parser_arrow::finish_operator_declaration(const ast::location& loc, ast::operator_info& p,
                            const ast::symbol_sequence& syms)
{
    bool errors = false;

    if (p.m_fixity == 0)
    {
        //fixity is undefined
        error::operator_error_msg(m_eh).error_operator_kind_not_set(loc);
        p.m_fixity  = ast::operator_info::prefix;
        errors = true;
    };

    if (p.m_fixity == ast::operator_info::infix)
    {
        if (p.m_precedence == 0)
        {
            //precedence is undefined
            error::operator_error_msg(m_eh).error_precedence_of_infix_operator_not_set(loc);
            p.m_precedence  = (int)ast::named_precedence::call;
            errors          = true;
        };

        // implicit quoting for infix is equivalent to both
        if (p.m_quoting == ast::quote_type::implicit)
            p.m_quoting = ast::quote_type::both;
    }
    else if (p.m_fixity == ast::operator_info::prefix)
    {
        if (p.m_precedence == 0)
        {
            //precedence is undefined set to unary
            p.m_precedence = (int)ast::named_precedence::unary;
        };

        // implicit quoting for infix is equivalent to right
        if (p.m_quoting == ast::quote_type::implicit)
            p.m_quoting = ast::quote_type::right;
    }
    else if (p.m_fixity == ast::operator_info::postfix)
    {
        if (p.m_precedence == 0)
        {
            //precedence is undefined set to unary
            p.m_precedence = (int)ast::named_precedence::unary;
        };

        // implicit quoting for infix is equivalent to both
        if (p.m_quoting == ast::quote_type::implicit)
            p.m_quoting = ast::quote_type::left;
    }
    else if (p.m_fixity == ast::operator_info::indexing)
    {
        int brack_prec      = (int)ast::named_precedence::bracket;

        if (p.m_precedence == 0)
            p.m_precedence  = brack_prec;

        p.m_quoting         = ast::quote_type::none;
        p.m_associativity   = ast::associativity_type::none;
    }
    else if (p.m_fixity == ast::operator_info::surrounding)
    {
        int brack_prec      = (int)ast::named_precedence::bracket;

        if (p.m_precedence == 0)
            p.m_precedence  = brack_prec;

        p.m_quoting         = ast::quote_type::none;
        p.m_associativity   = ast::associativity_type::none;
    }

    if (errors == true)
        return;

    ast::fixity_type ft     = p.get_fixity_type();
    bool is_bracket_kind    = (ft == ast::fixity_type::indexing) 
                            || (ft == ast::fixity_type::surrounding);

    long s  = syms.size();

    for (long i = 0; i < s; ++i)
    {
        const ast::identifier& vi   = syms.get(i);

        ast::fixity_type ft_loc;
        unique_string oper          = vi.get_name(ft_loc);
        const std::string& op_str   = oper.get_string();

        bool is_brack_assign;
        bool is_tuple_assign;
        bool is_dot;

        bool is_bracket     = ast::special_operators::is_bracket_operator(op_str, 
                                is_brack_assign, is_dot);
        bool is_tuple_op    = ast::special_operators::is_tuple_operator(op_str, 
                                is_tuple_assign, is_dot);
        bool is_angle_op    = ast::special_operators::is_angle_operator(op_str, 
                                is_tuple_assign, is_dot);

        if (is_bracket == true)
        {
            bool is_valid_bracket 
                = ast::special_operators::opening_closing_brackets_match(op_str);

            if (is_valid_bracket == false)
                error::operator_error_msg(m_eh).error_invalid_opening_and_closing_brackets(loc, op_str);
            else if (is_bracket_kind == false)
                error::operator_error_msg(m_eh).error_brackets_must_be_indexing_surrounding(loc, oper);
        }
        else if (is_bracket_kind == true)
        {
            error::operator_error_msg(m_eh).error_only_brackets_can_be_indexing_surrounding(loc, oper);
        };

        if (is_bracket_kind == true)
        {
            if (ft == ast::fixity_type::surrounding && is_tuple_op == true 
                && is_tuple_assign == false && is_dot == false)
            {
                //this is '()' operator
                error::operator_error_msg(m_eh).error_tuple_operator_cannot_be_surrounding(loc, oper);
            }

            if (is_dot == true && is_angle_op == true)
            {
                error::operator_error_msg(m_eh).error_type_application_not_operator(loc, oper);
            }

            if (is_dot == true && ft == ast::fixity_type::surrounding)
            {
                error::operator_error_msg(m_eh).error_dot_bracket_cannot_be_surrounding(loc, oper);
            }
        };
    };
};

ast::definition parser_arrow::make_fixity_definition(const ast::location& loc, 
            const ast::operator_property& p, const ast::symbol_sequence& syms)
{
    ast::operator_info op_info = p.get_operator_info();
    finish_operator_declaration(loc, op_info, syms);

    return ast::fixity_definition(loc, op_info, syms);
};

//--------------------------------------------------
//              kinds
//--------------------------------------------------
ast::kind parser_arrow::make_arrow_kind(const ast::kind& k1, ast::polarity_type kat, 
                                        const ast::kind& k2)
{
    return ast::arrow_kind(k1->get_location(), kat, k1, k2);
};
ast::kind parser_arrow::make_arrow_kind_star(const ast::kind& k1, const ast::kind& k2)
{
    return ast::star_arrow_kind(k1->get_location(), k1, k2, ast::star_kind_state::fixed);
};

ast::kind parser_arrow::make_star_kind(const ast::location& loc)
{
    return ast::star_kind(loc);
};
ast::kind parser_arrow::make_kind_var(const ast::location& loc)
{
    return ast::kind_var(loc, false);
};

ast::kind parser_arrow::make_int_kind(const ast::location& loc)
{
    return ast::value_kind(loc, ast::value_kind_code::int_kind);
}
ast::kind parser_arrow::make_bool_kind(const ast::location& loc)
{
    return ast::value_kind(loc, ast::value_kind_code::bool_kind);
}
ast::kind parser_arrow::make_char_kind(const ast::location& loc)
{
    return ast::value_kind(loc, ast::value_kind_code::char_kind);
}
ast::kind parser_arrow::make_string_kind(const ast::location& loc)
{
    return ast::value_kind(loc, ast::value_kind_code::string_kind);
}

ast::kind parser_arrow::make_kind_scope(const ast::location& loc, const kind_id_vec& vars, 
                        const ast::kind& body)
{
    if (vars.size() == 0)
        return body;

    ast::kind_scope ret = ast::kind_scope(loc, sema::copy(vars.back()), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
    {
        ret = ast::kind_scope(loc, sema::copy(vars[i]), ret);
    }

    return ret;
}

ast::kind parser_arrow::make_all_kind_star(const ast::location& loc, const kind_vec& vars, 
                            const ast::kind& body)
{
    if (vars.size() == 0)
        return body;

    ast::star_all_kind ret = ast::star_all_kind(loc, vars.back(), body, ast::star_kind_state::fixed);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::star_all_kind(loc, vars[i], ret, ast::star_kind_state::fixed);

    return ret;
};

ast::kind parser_arrow::make_some_kind_star(const ast::location& loc, const kind_vec& vars, 
                            const ast::kind& body)
{
    if (vars.size() == 0)
        return body;

    ast::star_some_kind ret = ast::star_some_kind(loc, vars.back(), body, ast::star_kind_state::fixed);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::star_some_kind(loc, vars[i], ret, ast::star_kind_state::fixed);

    return ret;
};

ast::kind parser_arrow::make_list_kind_star(const ast::location& loc, const ast::kind& body)
{
    ast::star_list_kind ret = ast::star_list_kind(loc, body, ast::star_kind_state::fixed);
    return ret;
};

ast::kind parser_arrow::make_ref_kind_star(const ast::location& loc, const ast::kind& body)
{
    ast::star_ref_kind ret = ast::star_ref_kind(loc, body, ast::star_kind_state::fixed);
    return ret;
};

ast::kind parser_arrow::make_bool_kind_star(const ast::location& loc)
{
    return ast::star_predefined_kind(loc, ast::predefined_type_code::type_bool,
                                     ast::star_kind_state::fixed);
}
ast::kind parser_arrow::make_int_kind_star(const ast::location& loc)
{
    return ast::star_predefined_kind(loc, ast::predefined_type_code::type_int,
                                     ast::star_kind_state::fixed);
}
ast::kind parser_arrow::make_float_kind_star(const ast::location& loc)
{
    return ast::star_predefined_kind(loc, ast::predefined_type_code::type_float,
                                     ast::star_kind_state::fixed);
}
ast::kind parser_arrow::make_string_kind_star(const ast::location& loc)
{
    return ast::star_predefined_kind(loc, ast::predefined_type_code::type_string,
                                     ast::star_kind_state::fixed);
}
ast::kind parser_arrow::make_char_kind_star(const ast::location& loc)
{
    return ast::star_predefined_kind(loc, ast::predefined_type_code::type_char,
                                     ast::star_kind_state::fixed);
}

void parser_arrow::mark_kind_identifier(const ast::identifier& id)
{
    if (id)
        id.get_symbol_data()->get_kind().set_sector(ast::identifier_kind::sector_type::kind);
};
void parser_arrow::mark_type_identifier(const ast::identifier& id)
{
    if (id)
        id.get_symbol_data()->get_kind().set_sector(ast::identifier_kind::sector_type::type);
};
void parser_arrow::mark_term_identifier(const ast::identifier& id)
{
    if (id)
        id.get_symbol_data()->get_kind().set_sector(ast::identifier_kind::sector_type::term);
};

ast::kind parser_arrow::make_kind_identifier(const ast::location& loc, const ast::identifier& id)
{
    id.get_symbol_data()->get_kind().set_sector(ast::identifier_kind::sector_type::kind);
    return ast::kind_identifier(loc, id);
};

ast::kind parser_arrow::make_tuple_kind_star(const ast::location& loc, const kind_vec& kv, 
                        bool fixed)
{
    if (kv.size() == 1 && fixed == false)
        return kv[0];

    ast::star_tuple_kind ret(loc, fixed, ast::star_kind_state::fixed);
    for (size_t i = 0; i < kv.size(); ++i)
        ret.add(kv[i]);

    return ret;
};

//--------------------------------------------------
//              types
//--------------------------------------------------
ast::type parser_arrow::make_arrow_type(const ast::type& t1, const ast::type& t2)
{
    return ast::arrow_type(t1->get_location(), t1, t2);
}
ast::all_type parser_arrow::make_all_type(const ast::location& loc, const type_id_vec2& vars, 
                                          const ast::type& body)
{
    ast::all_type ret = ast::all_type(loc, vars.back(), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::all_type(loc, vars[i], ret);

    return ret;
};

ast::type parser_arrow::make_int_type(const ast::int_literal& il)
{
    return ast::int_type(il->get_location(), il.get_value());
};
ast::type parser_arrow::make_bool_type(const ast::bool_literal& il)
{
    return ast::bool_type(il->get_location(), il.get_value());
};
ast::type parser_arrow::make_char_type(const ast::char_literal& il)
{
    return ast::char_type(il->get_location(), il.get_value());
};
ast::type parser_arrow::make_string_type(const ast::string_literal& il)
{
    return ast::string_type(il->get_location(), il.get_value());
};

ast::some_type parser_arrow::make_some_type(const ast::location& loc, const type_id_vec2& vars, 
                                const ast::type& body)
{
    ast::some_type ret = ast::some_type(loc, vars.back(), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::some_type(loc, vars[i], ret);

    return ret;
}
ast::lambda_type parser_arrow::make_lambda_type(const ast::location& loc, const type_id_vec& vars, 
                                const ast::kind& ret_ki, const ast::type& body0)
{
    ast::type body = body0;

    if (ret_ki)
        body    = ast::annotated_type(body->get_location(), body, ret_ki);

    ast::lambda_type ret = ast::lambda_type(loc, vars.back().second, vars.back().first, body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::lambda_type(loc, vars[i].second, vars[i].first, ret);

    return ret;
}
ast::mu_type parser_arrow::make_mu_type(const ast::location& loc, const type_id_vec2& vars, 
                                const ast::type& body)
{
    ast::mu_type ret = ast::mu_type(loc, vars.back(), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::mu_type(loc, vars[i], ret);

    return ret;
}

ast::type parser_arrow::make_ref_type(const ast::location& loc, const ast::type& t)
{
    return ast::ref_type(loc, t);
}
ast::type parser_arrow::make_if_type(const ast::location& loc, const ast::type& t_if, 
                            const ast::type& t_then, const ast::type& t_else)
{
    return ast::if_type(loc, t_if, t_then, t_else);
}

ast::type parser_arrow::make_list_type(const ast::location& loc, const ast::type& t)
{
    return ast::list_type(loc, t);
}

ast::type parser_arrow::make_identifier_type(const ast::identifier& id)
{
    mark_type_identifier(id);
    return ast::identifier_type(id->get_location(), id);
}
ast::variant_type parser_arrow::make_variant_type(const ast::location& loc)
{
    ast::variant_type ret(loc);
    return ret;
}
ast::type parser_arrow::make_type_application(const ast::type& t1, const ast::type& t2)
{
    if (!t1)
        return t2;
    else
        return ast::type_application(t2->get_location(), t1, t2);
}
ast::type parser_arrow::make_type_kind_application(const ast::type& t, const ast::kind& ki)
{
    return ast::type_kind_application(ki->get_location(), t, ki);
};
ast::type parser_arrow::make_annotated_type(const ast::type& t, const ast::kind& ki)
{
    return ast::annotated_type(ki->get_location(), t, ki);
};

ast::record_type parser_arrow::make_record_type(const ast::location& loc)
{
    return ast::record_type(loc);
}

ast::type parser_arrow::make_type_var(const ast::location& loc)
{
    return ast::type_var(loc);
};

ast::type parser_arrow::make_type_bool(const ast::location& loc)
{
    return ast::predefined_type(loc, ast::predefined_type_code::type_bool);
}

ast::type parser_arrow::make_type_int(const ast::location& loc)
{
    return ast::predefined_type(loc, ast::predefined_type_code::type_int);
}
ast::type parser_arrow::make_type_float(const ast::location& loc)
{
    return ast::predefined_type(loc, ast::predefined_type_code::type_float);
}
ast::type parser_arrow::make_type_char(const ast::location& loc)
{
    return ast::predefined_type(loc, ast::predefined_type_code::type_char);
}
ast::type parser_arrow::make_type_string(const ast::location& loc)
{
    return ast::predefined_type(loc, ast::predefined_type_code::type_string);
}

ast::type parser_arrow::make_type_kind_scope(const ast::location& loc, const kind_id_vec& vars, 
                        const ast::type& body)
{
    if (vars.size() == 0)
        return body;

    ast::type_kind_scope ret = ast::type_kind_scope(loc, sema::copy(vars.back()), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::type_kind_scope(loc, sema::copy(vars[i]), ret);

    return ret;
}

ast::type parser_arrow::make_type_path(const ast::type& x, const ast::int_literal& pos)
{
    return ast::path_int_type(pos->get_location(), x, pos);
}

template<class Type>
Type parser_arrow::make_bracket(const antlr::RefToken& start, Type& ex, 
                            const antlr::RefToken& end)
{
    ast::location loc_start = get_location(start);
    ast::location loc_end   = get_location(end);

    std::string op_left     = get_text(start);
    std::string op_right    = get_text(end);

    auto opening            = decode_utf8_code_point(op_left);
    auto closing            = decode_utf8_code_point(op_right);
    bool match              = do_brackets_match(opening, closing);

    if (match == false)
    {
        error::operator_error_msg(m_eh).error_opening_closing_brakets_dont_match(loc_start, loc_end, 
                                    op_left, op_right);
        return ex;
    };

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::bracket_part);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::def);

    ast::fixity_type ft     = ast::fixity_type::undefined;
    ast::identifier vo      = ast::identifier(loc_start, unique_string(op_left), ft,
                                ik, ast::identifier::fresh());

    ast::identifier vc      = ast::identifier(loc_end, unique_string(op_right), ft,
                                ik, ast::identifier::fresh());

    ex.set_brackets(false, vo, vc);
    return ex;
};

ast::bracket_type parser_arrow::make_bracket_type(const antlr::RefToken& start, ast::bracket_type& ex, 
                            const antlr::RefToken& end)
{
    return make_bracket(start, ex, end);
};

//--------------------------------------------------
//              terms
//--------------------------------------------------
ast::int_literal parser_arrow::make_int(const ast::location& loc, const std::string& txt)
{
    try
    {
        uint64_t val        = literal_parser(m_eh, loc).parse_int(txt);
        bool has_error;
        int64_t val2        = sema::cast_to_int64(loc, val, m_eh, has_error);
        return ast::int_literal(loc, val2);
    }
    catch (std::exception&)
    {
        return ast::int_literal(loc, 0);
    }

}

ast::char_literal parser_arrow::make_char(const ast::location& loc, const std::string& txt)
{
    try
    {
        return ast::char_literal(loc, literal_parser(m_eh, loc).parse_char(txt));
    }
    catch (std::exception&)
    {
        return ast::char_literal(loc, 0);
    }
};

ast::string_literal parser_arrow::make_string(const ast::location& loc, const std::string& txt)
{
    try
    {
        return ast::string_literal(loc, literal_parser(m_eh, loc).parse_string(txt));
    }
    catch (std::exception&)
    {
        return ast::string_literal(loc, "");
    }
};

ast::term parser_arrow::make_float(const ast::location& loc, const std::string& txt)
{
    try
    {
        double val  = literal_parser(m_eh, loc).parse_float(txt);
        return ast::float_literal(loc, val);
    }
    catch (std::exception&)
    {
        return ast::error_term(loc);
    }
};

ast::term parser_arrow::make_val_term(const ast::location& loc, const ast::type& t)
{
    return ast::val_term(loc, t);
}

ast::bool_literal parser_arrow::make_boolean(const ast::location& loc, bool value)
{
    return ast::bool_literal(loc, value);
};

ast::all_term parser_arrow::make_all_term(const ast::location& loc, 
                const type_id_vec& vars, const ast::term& t)
{
    ast::all_term ret = ast::all_term(loc, vars.back().first, t);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
    {
        ret = ast::all_term(loc, vars[i].first, ret);
    }

    return ret;
}

ast::term parser_arrow::make_lambda_term(const ast::location& loc, 
                const term_id_vec& vars, const ast::type& ret_ty, const ast::term& t0)
{
    ast::term t = t0;

    if (ret_ty)
        t   = ast::annotated_term(t0->get_location(), t0, ret_ty);

    ast::lambda_term ret = ast::lambda_term(loc, vars.back(), t, 
                                ast::lambda_term::fresh());

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::lambda_term(loc, vars[i], ret, ast::lambda_term::fresh());

    return ret;
}

ast::term parser_arrow::make_mu_term(const ast::location& loc, const term_id_vec& vars,
                            const ast::term& body)
{
    ast::mu_term ret = ast::mu_term(loc, vars.back(), body, ast::mu_term::fresh());

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::mu_term(loc, vars[i], ret, ast::mu_term::fresh());

    return ret;
};

ast::term parser_arrow::make_ref_term(const ast::location& loc, const ast::term& t)
{
    return ast::ref_term(loc, t);
}
ast::term parser_arrow::make_assign_term(const ast::term& lhs, const ast::term& rhs)
{
    return ast::assign_term(lhs->get_location(), lhs, rhs);
}
ast::term parser_arrow::make_deref_term(const ast::location& loc, const ast::term& t)
{
    return ast::deref_term(loc, t);
}
ast::term parser_arrow::make_identifier_term(const ast::identifier& id)
{
    mark_term_identifier(id);
    return ast::identifier_term(id->get_location(), id);
}

ast::term parser_arrow::make_annotated_term(const ast::term& x, const ast::type& t)
{
    return ast::annotated_term(x->get_location(), x, t);
}
ast::term parser_arrow::make_if_term(const ast::location& loc, const ast::term& t_if, 
                            const ast::term& t_then, const ast::term& t_else)
{
    return ast::if_term(loc, t_if, t_then, t_else);
}
ast::term parser_arrow::make_term_app(const ast::term& x1, const ast::term& x2)
{
    return ast::term_application(x1->get_location(), x1, x2);
}
ast::term_type_application parser_arrow::make_term_type_app(const ast::term& x, const ast::type& t)
{
    ast::term_type_application ret(t->get_location(), x, t);
    return ret;
};

ast::term parser_arrow::make_let(const ast::location& loc, const ast::definitions& defs, 
                            const ast::term& body)
{
    return ast::let_term(loc, defs, body, ast::let_term::fresh());
}

ast::unpack_term parser_arrow::make_unpack(const ast::location& loc, const ast::term& e, 
                            const ast::annotated_type_identifier& id1, const ast::annotated_identifier& id2,
                            const ast::term& t)
{
    return ast::unpack_term(loc, e, id1, id2, t, ast::unpack_term::fresh());
}
ast::term parser_arrow::make_pack(const ast::location& loc, const id_type_vec& vars, const ast::term& e)
{
    ast::type_var tv    = ast::type_var(loc);
    ast::pack_term ret  = ast::pack_term(loc, vars.back().first, vars.back().second, e, tv);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
    {
        const ast::annotated_type_identifier& id    = vars[i].first;
        const ast::type& ty                         = vars[i].second;
        ast::type_var tvi                           = ast::type_var(loc);
        ret = ast::pack_term(loc, id, ty, ret, tvi);
    }

    return ret;
}

ast::term parser_arrow::make_variant_term(const ast::location& loc, const ast::type& ty, 
                                const ast::annotated_identifier& id, const ast::term& e)
{
    return ast::variant_term(loc, ty, id, e);
};

ast::bracket_term parser_arrow::make_bracket_expr(const antlr::RefToken& start, ast::bracket_term& ex, 
                            const antlr::RefToken& end)
{
    return make_bracket(start, ex, end);
};

ast::term parser_arrow::make_term_kind_scope(const ast::location& loc, const kind_id_vec& vars, 
                        const ast::term& body)
{
    if (vars.size() == 0)
        return body;

    ast::term_kind_scope ret = ast::term_kind_scope(loc, sema::copy(vars.back()), body);

    long n = (long)vars.size();
    for (long i = n - 2; i >= 0; --i)
        ret = ast::term_kind_scope(loc, sema::copy(vars[i]), ret);

    return ret;
}

void parser_arrow::add_operator_property_string(const ast::location& loc, ast::operator_property& prop, 
                        const std::string& prop_type, const std::string& prop_value)
{
    prop.add_string(loc, prop_type, prop_value, m_eh);
};
void parser_arrow::add_operator_property_int(const ast::location& loc, ast::operator_property& prop, 
                        const std::string& prop_type, int_type prop_value)
{
    int min_prec    = (int_type)ast::named_precedence::minimum_allowed;
    int max_prec    = (int_type)ast::named_precedence::maximum_allowed;
    if (prop_value > max_prec || prop_value < min_prec)
    {
        error::operator_error_msg(m_eh).error_invalid_operator_precedence(loc, prop_value, min_prec, max_prec);
        prop.add_int(loc, prop_type, static_cast<int>(ast::named_precedence::call), m_eh);
    }
    else
    {
        prop.add_int(loc, prop_type, static_cast<int>(prop_value), m_eh);
    }
};

ast::identifier parser_arrow::make_brack_assign_op(const ast::location& loc, 
                                    const std::string& brack_op_name, bool is_dot)
{
    (void)is_dot;
    bool is_valid = ast::special_operators::opening_closing_brackets_match(brack_op_name);

    if (is_valid == false)
        error::operator_error_msg(m_eh).error_invalid_opening_and_closing_brackets(loc, brack_op_name);

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::bracket_full);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::def);

    ast::fixity_type ft = ast::fixity_type::undefined;
    ast::identifier ret(loc, unique_string(brack_op_name), ft, ik, ast::identifier::fresh());

    return ret;
}

ast::identifier parser_arrow::make_brack_op(const ast::location& loc, const std::string& brack_op_name,
                                            bool is_dot_brack)
{
    (void)is_dot_brack;

    bool is_valid = ast::special_operators::opening_closing_brackets_match(brack_op_name);

    if (is_valid == false)
        error::operator_error_msg(m_eh).error_invalid_opening_and_closing_brackets(loc, brack_op_name);

    ast::fixity_type ft = ast::fixity_type::undefined;

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::bracket_full);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::def);

    ast::identifier ret(loc, unique_string(brack_op_name), ft, ik, ast::identifier::fresh());
    return ret;
}

ast::identifier parser_arrow::make_operator(const ast::location& loc, const std::string& op_name)
{
    ast::fixity_type ft = ast::fixity_type::undefined;

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::oper);
    ik.set_sector(ast::identifier_kind::sector_type::term);
    ik.set_function(ast::identifier_kind::function_type::def);

    ast::identifier ret(loc, unique_string(op_name), ft, ik, ast::identifier::fresh());

    return ret;
};

ast::identifier parser_arrow::make_fixity_name(const ast::identifier& id, bool pref_under, bool post_under)
{
    ast::fixity_type fix_type;

    if (pref_under == true && post_under == true)
        fix_type    = ast::fixity_type::infix;
    else if (pref_under == true)
        fix_type    = ast::fixity_type::postfix;
    else if (post_under == true)
        fix_type    = ast::fixity_type::prefix;
    else
        return id;

    bool is_brack   = id.get_symbol_data()->get_kind().is_bracket_full();

    if (is_brack == true)
    {
        if (fix_type == ast::fixity_type::prefix)
        {
            ast::fixity_type ft;
            error::operator_error_msg(m_eh).error_bracket_operator_cannot_be_prefix
                        (id->get_location(), id.get_name(ft));
            return id;        
        }

        // _ {} is interpreted as indexing
        if (fix_type == ast::fixity_type::postfix)
            fix_type    = ast::fixity_type::indexing;

        // _ {} _ is interpreted as surrounding
        if (fix_type == ast::fixity_type::infix)
            fix_type    = ast::fixity_type::surrounding;
    }

    ast::fixity_type ft_loc;
    return ast::identifier(id->get_location(), id.get_name(ft_loc), fix_type, id.get_symbol_data());
};