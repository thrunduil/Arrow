#include "unicode.h"

#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "unicode/uchar.h"

#include <cassert>
#include <limits>
#include <string>

namespace arrow
{

std::string create_utf8_from_code_point(int32_t code_point) 
{
    std::string z;
    append_code_point_to_utf8(z,code_point);
    return z;
}

bool is_unicode_character(int32_t code_point)
{
    return U_IS_UNICODE_CHAR(code_point);
}

bool is_lowercase_identifier_start(int32_t code_point)
{
    return (u_islower(code_point) && !is_superscript_or_subscript_char(code_point));
}

bool is_uppercase_identifier_start(int32_t code_point)
{
    return (u_isupper(code_point) && !is_superscript_or_subscript_char(code_point));
}

bool is_identifier_start(int32_t code_point)
{
    return (u_isIDStart(code_point) && !is_superscript_or_subscript_char(code_point));
}

bool is_identifier_part(int32_t code_point)
{
    return code_point == '_'
        || (u_isIDPart(code_point) && !is_superscript_or_subscript_char(code_point));
}

bool is_identifier(const std::string& utf8)
{
    if (utf8.empty())
        return false;

    int32_t index = 0;
    int32_t code_point = read_code_point_from_utf8(utf8, index);

    if ( ! is_identifier_start(code_point))
        return false;

    while (static_cast< size_t >(index) < utf8.length())
    {
        code_point = read_code_point_from_utf8(utf8, index);
        if ( ! is_identifier_part(code_point))
            return false;
    }

    return true;
}

bool is_white_space(int32_t code_point)
{
    switch (code_point)
    {
        case 0x0020: // SPACE
        case 0x00A0: // NO-BREAK SPACE
        // case 0x1680: // OGHAM SPACE MARK
        // case 0x180E: // MONGOLIAN VOWEL SEPARATOR
        case 0x2000: // EN QUAD
        case 0x2001: // EM QUAD
        case 0x2002: // EN SPACE
        case 0x2003: // EM SPACE
        case 0x2004: // THREE-PER-EM SPACE
        case 0x2005: // FOUR-PER-EM SPACE
        case 0x2006: // SIX-PER-EM SPACE
        case 0x2007: // FIGURE SPACE
        case 0x2008: // PUNCTUATION SPACE
        case 0x2009: // THIN SPACE
        case 0x200A: // HAIR SPACE
        case 0x202F: // NARROW NO-BREAK SPACE
        case 0x205F: // MEDIUM MATHEMATICAL SPACE
        case 0x3000: // IDEOGRAPHIC SPACE
            return true;
        default:
            return false;
    }
}

bool is_line_break(int32_t code_point)
{
    switch (code_point)
    {
        case 0x000A: // LINE FEED (LF)
        case 0x000C: // FORM FEED (FF)
        case 0x000D: // CARRIAGE RETURN (CR)
        case 0x0085: // NEXT LINE (NEL)
        case 0x2028: // LINE SEPARATOR
        case 0x2029: // PARAGRAPH SEPARATOR
            return true;
        default:
            return false;
    }
}

bool is_operator_char(int32_t code_point)
{
    switch (code_point)
    {
    case '!':
//    case '"':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
//    case '(':
//    case ')':
    case '*':
    case '+':
//    case ',':
    case '-':
    case '.':
    case '/':
    case ':':
//    case ';':
    case '<':
    case '=':
    case '>':
    case '?':
    case '@':
//    case '[':
    case '\\':
//    case ']':
    case '^':
    case '_':
    case '`':
//    case '{':
    case '|':
//    case '}':
    case '~':
        return true;
    default:
      return   (code_point >= 0x2190 && code_point <= 0x21ff) // http://unicode-table.com/en/blocks/arrows/
            || (code_point >= 0x2200 && code_point <= 0x22FF) // http://unicode-table.com/en/#mathematical-operators
            || (code_point >= 0x2A00 && code_point <= 0x2AFF) // http://unicode-table.com/en/#supplemental-mathematical-operators
            || is_superscript_or_subscript_char(code_point)
             ;
    }
}

bool is_superscript_or_subscript_char(int32_t code_point)
{
    int decompositionType = u_getIntPropertyValue(code_point, UCHAR_DECOMPOSITION_TYPE);
    return decompositionType == U_DT_SUB || decompositionType == U_DT_SUPER;
}

static bool starts_with(const std::string& str, const std::string& prefix)
{
    if (str.length() < prefix.length())
        return false;

    const char* c_str    = str.c_str();
    const char* c_prefix = prefix.c_str();

    while (*c_str == *c_prefix)
    {
        if (*c_str == 0)
            return true;
        ++c_str;
        ++c_prefix;
    }

    return false;
}

bool is_operator(const std::string& utf8)
{
    if (utf8 == "..")
        return true;

    int32_t index = 0;

    while (static_cast< size_t >(index) < utf8.length())
    {
        if ( ! is_operator_char(read_code_point_from_utf8(utf8, index)))
            return false;
    }

    return index > 0
        && utf8.find("*/") == utf8.npos
        && ! starts_with(utf8, "//")
        && ! starts_with(utf8, "/*")
           ;
}

bool is_valid_utf8(const std::string& utf8)
{
    int32_t index = 0;

    while (static_cast< size_t >(index) < utf8.length())
    {
        if (!is_unicode_character(read_code_point_from_utf8(utf8, index)))
            return false;
    }
    return true;
}

bool append_code_point_to_utf8(std::string& utf8, int32_t code_point)
{
    bool result = is_unicode_character(code_point);
    if (!result)
        code_point = 0xFFFD; // replacement character
    assert(code_point <= 0x10FFFF);

    char buffer[4];
    size_t index = 0;
    U8_APPEND_UNSAFE(buffer, index, code_point);

    assert(index <= sizeof(buffer));
    utf8.append(buffer, index);

    return result;
}

int32_t read_code_point_from_utf8(const std::string& utf8, int32_t& index)
{
    assert(index >= 0);
    assert(static_cast< size_t >(index) < utf8.length());
    assert(utf8.length() <= static_cast< size_t >(std::numeric_limits< int32_t >::max()));

    UChar32 result;
    const uint8_t* utf8Data = reinterpret_cast< const uint8_t* >(utf8.data());
    int32_t utf8Length = static_cast< int32_t >(utf8.length());

    U8_NEXT(utf8Data, index, utf8Length, result);

    return result;
}

int32_t read_previous_code_point_from_utf8(const std::string& utf8, int32_t& index)
{
    assert(index > 0);    
    assert(static_cast< size_t >(index) <= utf8.length());
    assert(utf8.length() < static_cast< size_t >(std::numeric_limits< int32_t >::max()));

    if (utf8.empty())
    {
        index = -1;
        return -1;
    }

    auto cp = utf8.data() + index;

    while(--cp >= utf8.data() ) 
    {
        if ((*cp & 0b10000000) && !(*cp & 0b01000000))
            continue;
        else
            break;
    }

    index           = (int32_t)(cp - utf8.data());
    int32_t index2  = index;

    return read_code_point_from_utf8(utf8, index2);
};

int32_t decode_utf8_code_point(const std::string& utf8)
{
    if (utf8.empty() || utf8.length() > 4)
        return -1;

    int32_t index = 0;
    UChar32 result;
    const uint8_t* utf8Data = reinterpret_cast< const uint8_t* >(utf8.data());
    int32_t utf8Length = static_cast< int32_t >(utf8.length());

    U8_NEXT(utf8Data, index, utf8Length, result);

    return result;
}

int32_t read_code_point_from_utf16(const uint16_t* utf16, int32_t length, int32_t& index)
{
    assert(index >= 0);
    assert(index < length);

    UChar32 result;
    U16_NEXT(utf16, index, length, result);

    return result;
}

/// \see http://www.fileformat.info/info/unicode/category/Ps/list.htm
/// \see http://www.fileformat.info/info/unicode/category/Pe/list.htm

enum 
{
    bracket_pair_count = 59
};

static int32_t brackets[][2] = 
{
    {0x0028, 0x0029}, // LEFT/RIGHT PARENTHESIS
    {0x003C, 0x003E}, // LEFT/RIGHT CURLY BRACKET
    {0x005B, 0x005D}, // LEFT/RIGHT SQUARE BRACKET    
    {0x007B, 0x007D}, // LEFT/RIGHT ANGLE BRACKET
    // {0x0F3A, ?}, // TIBETAN MARK GUG RTAGS GYON
    // {0x0F3C, ?}, // TIBETAN MARK ANG KHANG GYON
    // {0x169B, ?}, // OGHAM FEATHER MARK
    // {0x201A, ?}, // SINGLE LOW-9 QUOTATION MARK
    // {0x201E, ?}, // DOUBLE LOW-9 QUOTATION MARK
    {0x2045, 0x2046}, // LEFT/RIGHT SQUARE BRACKET WITH QUILL
    {0x207D, 0x207E}, // SUPERSCRIPT LEFT/RIGHT PARENTHESIS
    {0x208D, 0x208E}, // SUBSCRIPT LEFT/RIGHT PARENTHESIS
    {0x2308, 0x2309}, // LEFT/RIGHT CEILING
    {0x230A, 0x230B}, // LEFT/RIGHT FLOOR
    {0x2329, 0x232A}, // LEFT/RIGHT-POINTING ANGLE BRACKET
    {0x2768, 0x2769}, // MEDIUM LEFT/RIGHT PARENTHESIS ORNAMENT
    {0x276A, 0x276B}, // MEDIUM FLATTENED LEFT/RIGHT PARENTHESIS ORNAMENT
    {0x276C, 0x276D}, // MEDIUM LEFT/RIGHT-POINTING ANGLE BRACKET ORNAMENT
    {0x276E, 0x276F}, // HEAVY LEFT/RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
    {0x2770, 0x2771}, // HEAVY LEFT/RIGHT-POINTING ANGLE BRACKET ORNAMENT
    {0x2772, 0x2773}, // LIGHT LEFT/RIGHT TORTOISE SHELL BRACKET ORNAMENT
    {0x2774, 0x2775}, // MEDIUM LEFT/RIGHT CURLY BRACKET ORNAMENT
    {0x27C5, 0x27C6}, // LEFT/RIGHT S-SHAPED BAG DELIMITER
    {0x27E6, 0x27E7}, // MATHEMATICAL LEFT/RIGHT WHITE SQUARE BRACKET
    {0x27E8, 0x27E9}, // MATHEMATICAL LEFT/RIGHT ANGLE BRACKET
    {0x27EA, 0x27EB}, // MATHEMATICAL LEFT/RIGHT DOUBLE ANGLE BRACKET
    {0x27EC, 0x27ED}, // MATHEMATICAL LEFT/RIGHT WHITE TORTOISE SHELL BRACKET
    {0x27EE, 0x27EF}, // MATHEMATICAL LEFT/RIGHT FLATTENED PARENTHESIS
    {0x2983, 0x2984}, // LEFT/RIGHT WHITE CURLY BRACKET
    {0x2985, 0x2986}, // LEFT/RIGHT WHITE PARENTHESIS
    {0x2987, 0x2988}, // Z NOTATION LEFT/RIGHT IMAGE BRACKET
    {0x2989, 0x298A}, // Z NOTATION LEFT/RIGHT BINDING BRACKET
    {0x298B, 0x298C}, // LEFT/RIGHT SQUARE BRACKET WITH UNDERBAR
    {0x298D, 0x2990}, // LEFT/RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
    {0x298F, 0x298E}, // LEFT/RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
    {0x2991, 0x2992}, // LEFT/RIGHT ANGLE BRACKET WITH DOT
    {0x2993, 0x2994}, // LEFT/RIGHT ARC LESS/GREATER-THAN BRACKET
    {0x2995, 0x2996}, // DOUBLE LEFT/RIGHT ARC GREATER/LESS-THAN BRACKET
    {0x2997, 0x2998}, // LEFT/RIGHT BLACK TORTOISE SHELL BRACKET
    {0x29D8, 0x29D9}, // LEFT/RIGHT WIGGLY FENCE
    {0x29DA, 0x29DB}, // LEFT/RIGHT DOUBLE WIGGLY FENCE
    {0x29FC, 0x29FD}, // LEFT/RIGHT-POINTING CURVED ANGLE BRACKET
    {0x2E22, 0x2E23}, // TOP LEFT/RIGHT HALF BRACKET
    {0x2E24, 0x2E25}, // BOTTOM LEFT/RIGHT HALF BRACKET
    {0x2E26, 0x2E27}, // LEFT/RIGHT SIDEWAYS U BRACKET
    {0x2E28, 0x2E29}, // LEFT/RIGHT DOUBLE PARENTHESIS
    {0x3008, 0x3009}, // LEFT/RIGHT ANGLE BRACKET
    {0x300A, 0x300B}, // LEFT/RIGHT DOUBLE ANGLE BRACKET
    {0x300C, 0x300D}, // LEFT/RIGHT CORNER BRACKET
    {0x300E, 0x300F}, // LEFT/RIGHT WHITE CORNER BRACKET
    {0x3010, 0x3011}, // LEFT/RIGHT BLACK LENTICULAR BRACKET
    {0x3014, 0x3015}, // LEFT/RIGHT TORTOISE SHELL BRACKET
    {0x3016, 0x3017}, // LEFT/RIGHT WHITE LENTICULAR BRACKET
    {0x3018, 0x3019}, // LEFT/RIGHT WHITE TORTOISE SHELL BRACKET
    {0x301A, 0x301B}, // LEFT/RIGHT WHITE SQUARE BRACKET
    // {0x301D, ?}, // REVERSED DOUBLE PRIME QUOTATION MARK
    {0xFD3E, 0xFD3F}, // ORNATE LEFT/RIGHT PARENTHESIS
    // {0xFE17, ?}, // PRESENTATION FORM FOR VERTICAL LEFT WHITE LENTICULAR BRACKET
    // {0xFE35, 0xFE36}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT PARENTHESIS
    // {0xFE37, 0xFE38}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT CURLY BRACKET
    // {0xFE39, 0xFE3A}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT TORTOISE SHELL BRACKET
    // {0xFE3B, 0xFE3C}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT BLACK LENTICULAR BRACKET
    // {0xFE3D, 0xFE3E}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT DOUBLE ANGLE BRACKET
    // {0xFE3F, 0xFE40}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT ANGLE BRACKET
    // {0xFE41, 0xFE42}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT CORNER BRACKET
    // {0xFE43, 0xFE44}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT WHITE CORNER BRACKET
    // {0xFE47, 0xFE48}, // PRESENTATION FORM FOR VERTICAL LEFT/RIGHT SQUARE BRACKET
    {0xFE59, 0xFE5A}, // SMALL LEFT/RIGHT PARENTHESIS
    {0xFE5B, 0xFE5C}, // SMALL LEFT/RIGHT CURLY BRACKET
    {0xFE5D, 0xFE5E}, // SMALL LEFT/RIGHT TORTOISE SHELL BRACKET
    {0xFF08, 0xFF09}, // FULLWIDTH LEFT/RIGHT PARENTHESIS
    {0xFF3B, 0xFF3D}, // FULLWIDTH LEFT/RIGHT SQUARE BRACKET
    {0xFF5B, 0xFF5D}, // FULLWIDTH LEFT/RIGHT CURLY BRACKET
    {0xFF5F, 0xFF60}, // FULLWIDTH LEFT/RIGHT WHITE PARENTHESIS
    {0xFF62, 0xFF63}, // HALFWIDTH LEFT/RIGHT CORNER BRACKET
};

static_assert(sizeof(brackets) / sizeof(brackets[0]) == bracket_pair_count,
              "bracket_pair_count value must be exactly the same as amount of bracket pairs");

static int32_t find_opening_bracket_index(int32_t code_point)
{
    int32_t l = 0;
    int32_t r = sizeof(brackets) / sizeof(brackets[0]) - 1;

    while (l < r)
    {
        int32_t m = (l + r) / 2;
    
        if (brackets[m][0] < code_point)
            l = m + 1;
        else
            r = m;
    }
    
    if (brackets[l][0] == code_point)
        return l;
    else
        return -1;
}

static int32_t find_closing_bracket_index(int32_t code_point)
{
    int32_t l = 0;
    int32_t r = sizeof(brackets) / sizeof(brackets[0]) - 1;
 
    while (l < r)
    {
        int32_t m = (l + r) / 2;
        if (brackets[m][1] < code_point)
            l = m + 1;
        else
            r = m;
    }
    
    if (brackets[l][1] == code_point)
        return l;
    else
        return -1;
}

bool is_opening_bracket(int32_t code_point)
{
    return find_opening_bracket_index(code_point) >= 0;
}

bool is_closing_bracket(int32_t code_point)
{
    return find_closing_bracket_index(code_point) >= 0;
}

bool do_brackets_match(int32_t opening, int32_t closing)
{
    auto index = find_opening_bracket_index(opening);

    if (index < 0)
        return false;
    
    return brackets[index][1] == closing;
}

}
