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
#pragma once

#include "arrow/parser.h"

#include <fstream>

#include "grammar/output/lexer_arrow.hpp"
#include "grammar/output/parser_arrow.hpp"
#include "antlr/antlr.h"
#include "root/options.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace arrow
{

parser::parser(const error_handler_ptr& eh)
    :m_eh(eh)
{
    m_tab_size = details::options::get_tab_size();
}

class parser_impl
{
    private:
        //must be declared in this order
        region::region          m_reg;
        lexer_arrow             m_lexer;

        antlr::TokenStreamHiddenTokenFilter
                                m_filter;

        parser_arrow            m_parser;

    public:
        parser_impl(std::istringstream& input, const error_handler_ptr& eh, 
                    ast::unique_string fp_short, int tab_size, bool shell_mode)
            : m_lexer(&m_reg, input)
            , m_filter(&m_reg, m_lexer)
            , m_parser(&m_reg, m_filter)
        {
            m_lexer.init(eh, fp_short, tab_size, shell_mode);

            m_filter.hide(lexer_arrow::White_space);
            m_filter.hide(lexer_arrow::Shebang);
            m_filter.hide(lexer_arrow::Comment);

            m_parser.init(eh, fp_short, shell_mode, &m_lexer);
        };

        void set_location(const ast::location& loc)
        {
            m_lexer.set_location(loc);
        };

        long get_current_line() const
        {
            return m_lexer.getLine();
        };

        parser_arrow& get_parser()
        { 
            return m_parser; 
        };

        bool at_eof() 
        { 
            return m_parser.at_eof(); 
        };
};

ast::module_definition parser::parse_module(std::istream& file, const std::string& path_short)
{
    auto eh_scope = m_eh->push_error_scope();

    ast::unique_string file_path_short(path_short);

    file.seekg(0, std::ios::end);
    std::streampos sp         = file.tellg();
    size_t N                  = (size_t)sp;
    char *text                = new char[N+2];

    std::unique_ptr<char []> text_ptr(text);

    file.seekg(0, std::ios::beg);
    file.get(text,(size_t)sp + 1,EOF);
    
    std::istringstream is(text);
    ast::module_definition defs;

    parser_impl m_parser(is, m_eh, file_path_short, m_tab_size, false);

    try
    {
        defs = m_parser.get_parser().toplevel();
    }
    catch (antlr::RecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                file_path_short), e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                file_path_short), e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        m_eh->report_parser_error(ast::location(-1,-1,file_path_short), e.getMessage());
    }
    catch(std::exception& ex)
    {
        m_eh->report_parser_error(ast::location(-1,-1,file_path_short), ex.what());
    }

    return defs;
};

ast::module_definition parser::parse_console_input(const std::string& text, const std::string& path_short,
                            long& line_num, bool& need_more_input)
{
    auto eh_scope = m_eh->push_error_scope();

    ast::unique_string file_path_short(path_short);
    
    std::istringstream is(text);
    ast::module_definition defs;

    ast::location loc   = ast::location(++line_num, 1, file_path_short);

    parser_impl m_parser(is, m_eh, file_path_short, m_tab_size, true);
    m_parser.set_location(loc);

    need_more_input = false;

    try
    {
        defs = m_parser.get_parser().toplevel_shell();
    }
    catch (antlr::RecognitionException& e)
    {
        if (m_parser.at_eof() == true)
        {
            need_more_input = true;
            m_eh->clear_errors();
            return defs;
        }
        else
        {
            m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                file_path_short), e.getMessage());
        }
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        if (m_parser.at_eof() == true)
        {
            need_more_input = true;
            m_eh->clear_errors();
            return defs;
        }
        else
        {
            m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                file_path_short), e.getMessage());
        }
    }
    catch(antlr::ANTLRException& e)
    {
        if (m_parser.at_eof() == true)
        {
            need_more_input = true;
            m_eh->clear_errors();
            return defs;
        }
        else
        {
            m_eh->report_parser_error(ast::location(-1,-1,file_path_short), e.getMessage());
        }
    }
    catch(std::exception& ex)
    {
        if (m_parser.at_eof() == true)
        {
            need_more_input = true;
            m_eh->clear_errors();
            return defs;
        }
        else
        {
            m_eh->report_parser_error(ast::location(-1,-1,file_path_short), ex.what());
        };
    }

    line_num = m_parser.get_current_line();
    return defs;
};

ast::term parser::parse_term(const std::string& syntax, const ast::location& loc)
{
    ast::unique_string fp_short = loc.get_file();
    
    std::istringstream is(syntax);
    ast::term ex;

    parser_impl m_parser(is, m_eh, fp_short, m_tab_size, false);
    m_parser.set_location(loc);

    try
    {
        ex = m_parser.get_parser().term();
    }
    catch (antlr::RecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                fp_short), e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                fp_short), e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), e.getMessage());
    }
    catch(std::exception& ex)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), ex.what());
    }

    return ex;
};

ast::type parser::parse_type(const std::string& syntax, const ast::location& loc)
{
    auto eh_scope = m_eh->push_error_scope();

    ast::unique_string fp_short = loc.get_file();
    
    std::istringstream is(syntax);
    ast::type ty;

    parser_impl m_parser(is, m_eh, fp_short, m_tab_size, false);
    m_parser.set_location(loc);

    try
    {
        ty = m_parser.get_parser().type();
    }
    catch (antlr::RecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                fp_short), e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                fp_short), e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), e.getMessage());
    }
    catch(std::exception& ex)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), ex.what());
    }

    return ty;
};

ast::kind parser::parse_kind(const std::string& syntax, const ast::location& loc)
{
    auto eh_scope = m_eh->push_error_scope();

    ast::unique_string fp_short = loc.get_file();    

    std::istringstream is(syntax);
    ast::kind ki;

    parser_impl m_parser(is, m_eh, fp_short, m_tab_size, false);
    m_parser.set_location(loc);

    try
    {
        ki = m_parser.get_parser().kind();
    }
    catch (antlr::RecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                fp_short), e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                fp_short), e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), e.getMessage());
    }
    catch(std::exception& ex)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), ex.what());
    }

    return ki;
};

ast::identifier parser::parse_identifier(const std::string& syntax, const ast::location& loc)
{
    auto eh_scope = m_eh->push_error_scope();

    ast::unique_string fp_short = loc.get_file();    

    std::istringstream is(syntax);
    ast::identifier id;

    parser_impl m_parser(is, m_eh, fp_short, m_tab_size, false);
    m_parser.set_location(loc);

    try
    {
        id = m_parser.get_parser().root_symbol_name();
    }
    catch (antlr::RecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(),
                fp_short), e.getMessage());
    }
    catch (antlr::TokenStreamRecognitionException& e)
    {
        m_eh->report_parser_error(ast::location(e.getLine(), e.getColumn(), 
                fp_short), e.getMessage());
    }
    catch(antlr::ANTLRException& e)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), e.getMessage());
    }
    catch(std::exception& ex)
    {
        m_eh->report_parser_error(ast::location(-1,-1,fp_short), ex.what());
    }

    return id;
};

};
