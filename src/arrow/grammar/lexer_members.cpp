#include "grammar/output/lexer_arrow.hpp"
#include <unicode/utf8.h>
#include "lexer_include.h"

void lexer_arrow::consume()
{
    using base_type = CharScanner;
    base_type::consume();
    m_current_offset++;
}
void lexer_arrow::resetText()
{
    using base_type = CharScanner;
    base_type::resetText();
    m_token_start_offset = m_current_offset;
}
bool lexer_arrow::is_begin_line(const antlr::LexerSharedInputState& input_state)
{
    long column = input_state->column;
    long length = m_current_offset - m_token_start_offset;

    return column == length;
};

void lexer_arrow::init(const error_handler_ptr& eh, unique_string file, int tab_size,
                       bool shell_mode)
{
    m_current_offset    = 0;
    m_token_start_offset = 0;
    m_eh                = eh;
    m_file              = file;
    m_shell_mode        = shell_mode;
    this->setTabsize(tab_size);
};

void lexer_arrow::set_location(const ast::location& loc)
{
    if (loc.has_location() == true)
    {
        this->setLine(loc.get_line());
        this->setColumn(loc.get_column());
    };
};

void lexer_arrow::reportError(const antlr::RecognitionException& ex)
{
    if (m_eh)
    {
        if (m_shell_mode == true)
            throw;

        ast::location loc = ast::location(ex.getLine(), ex.getColumn(), get_file());
        m_eh->report_parser_error(loc, ex.getMessage());
    }
}

void lexer_arrow::reportError(const std::string& str)
{
    if (m_eh)
    {
        if (m_shell_mode == true)
            throw std::runtime_error(str);

        m_eh->report_parser_error(ast::location(), str);
    }
}
void lexer_arrow::reportWarning(const std::string& str)
{
    if (m_eh)
        m_eh->report_parser_warning(ast::location(), str);
}

ast::unique_string lexer_arrow::get_file()
{
    return m_file;
};

int32_t lexer_arrow::check_next_code_point()
{
    auto la = this->LA(1);

    if (la == EOF_CHAR || U8_IS_TRAIL(la))
        return la;

    if (U8_IS_SINGLE(la))
        return this->m_next_code_point = static_cast< uint8_t >(la);

    assert(U8_IS_LEAD(la));
    uint8_t utf8[4];
    utf8[0] = static_cast< uint8_t >(la);
    uint8_t bytes = U8_COUNT_TRAIL_BYTES_UNSAFE(utf8[0]);

    uint8_t i = 1;
    while (i <= bytes)
    {
        auto la2 = this->LA(++i);
        if (la2 == EOF_CHAR || ! U8_IS_TRAIL(la2))
            return utf8[0];
        assert(i - 1 < sizeof(utf8));
        utf8[i - 1] = static_cast< uint8_t >(la2);
    }

    U8_GET_UNSAFE(utf8, 0, this->m_next_code_point);

    return this->m_next_code_point;
}

void lexer_arrow::pop_symbol(int num)
{
    long pos    = (long)text.length();

    while(num > 0 && pos > 0)
    {
        text.erase(pos - 1);

        --num;
        --pos;
    };
};
void lexer_arrow::push_symbol(const std::string& sym)
{
    text += sym;
}