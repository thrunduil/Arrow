#ifndef INC_lexer_arrow_hpp_
#define INC_lexer_arrow_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "arrow.g" -> "lexer_arrow.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "arrow_vocabularyTokenTypes.hpp"
#include <antlr/CharScanner.hpp>

    //parser.hpp includes
    #include "grammar/parser_include.h"

class CUSTOM_API lexer_arrow : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public arrow_vocabularyTokenTypes
{

    #include "grammar/lexer_members.h"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	lexer_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(std)istream& in);
	lexer_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	lexer_arrow(antlr::pool_parser pool, const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mTOK_lparen(bool _createToken);
	public: void mTOK_rparen(bool _createToken);
	public: void mTOK_lbracket(bool _createToken);
	public: void mTOK_rbracket(bool _createToken);
	public: void mTOK_lcurly(bool _createToken);
	public: void mTOK_rcurly(bool _createToken);
	public: void mTOK_comma(bool _createToken);
	public: void mTOK_semi(bool _createToken);
	protected: void mTOK_arrow(bool _createToken);
	protected: void mTOK_double_arrow(bool _createToken);
	protected: void mTOK_equal(bool _createToken);
	protected: void mTOK_underscore(bool _createToken);
	protected: void mTOK_dot_langle(bool _createToken);
	protected: void mTOK_star(bool _createToken);
	protected: void mTOK_dot(bool _createToken);
	protected: void mTOK_plus(bool _createToken);
	protected: void mTOK_minus(bool _createToken);
	protected: void mTOK_colon(bool _createToken);
	protected: void mTOK_dcolon(bool _createToken);
	public: void mAny(bool _createToken);
	protected: void mUTF8_bom(bool _createToken);
	protected: void mShebang(bool _createToken);
	protected: void mWhite_space(bool _createToken);
	protected: void mIdentifier_start(bool _createToken);
	protected: void mIdentifier(bool _createToken);
	protected: void mComment(bool _createToken);
	protected: void mString_literal(bool _createToken);
	protected: void mBin_prefix(bool _createToken);
	protected: void mBin_digit(bool _createToken);
	protected: void mBin_integer_literal(bool _createToken);
	protected: void mOct_prefix(bool _createToken);
	protected: void mOct_digit(bool _createToken);
	protected: void mOct_integer_literal(bool _createToken);
	protected: void mHex_floating_literal(bool _createToken);
	protected: void mDec_floating_literal(bool _createToken);
	protected: void mHex_prefix(bool _createToken);
	protected: void mHex_digit(bool _createToken);
	protected: void mHex_integer_literal(bool _createToken);
	protected: void mDec_integer_literal(bool _createToken);
	protected: void mCharacter_literal(bool _createToken);
	protected: void mOperator(bool _createToken);
	protected: void mUTF8_lead_byte(bool _createToken);
	protected: void mUTF8_multibyte_char(bool _createToken);
	protected: void mOperator_ascii_char(bool _createToken);
	protected: void mPredefined_operator(bool _createToken);
	protected: void mUTF8_next_byte(bool _createToken);
	protected: void mLine_break(bool _createToken);
	protected: void mUTF8_not_line_break(bool _createToken);
	protected: void mLine_comment(bool _createToken);
	protected: void mBlock_comment(bool _createToken);
	protected: void mBlock_comment_content(bool _createToken);
	protected: void mWysiwyg_string(bool _createToken);
	protected: void mDouble_quoted_string(bool _createToken);
	protected: void mEscape_sequence(bool _createToken);
	protected: void mEscaped_char(bool _createToken);
	protected: void mDec_integer(bool _createToken);
	protected: void mDec_digit(bool _createToken);
	protected: void mBin_integer(bool _createToken);
	protected: void mOct_integer(bool _createToken);
	protected: void mHex_integer(bool _createToken);
	protected: void mDec_exponent(bool _createToken);
	protected: void mHex_exponent(bool _createToken);
	protected: void mIdentifier_ascii(bool _createToken);
	protected: void mIdentifier_ascii_start(bool _createToken);
	protected: void mOperator_char(bool _createToken);
	protected: void mEOF(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_17;
	static const unsigned long _tokenSet_18_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_18;
	static const unsigned long _tokenSet_19_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_19;
	static const unsigned long _tokenSet_20_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_20;
	static const unsigned long _tokenSet_21_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_21;
	static const unsigned long _tokenSet_22_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_22;
	static const unsigned long _tokenSet_23_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_23;
	static const unsigned long _tokenSet_24_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_24;
	static const unsigned long _tokenSet_25_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_25;
	static const unsigned long _tokenSet_26_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_26;
	static const unsigned long _tokenSet_27_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_27;
	static const unsigned long _tokenSet_28_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_28;
	static const unsigned long _tokenSet_29_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_29;
	static const unsigned long _tokenSet_30_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_30;
	static const unsigned long _tokenSet_31_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_31;
	static const unsigned long _tokenSet_32_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_32;
	static const unsigned long _tokenSet_33_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_33;
	static const unsigned long _tokenSet_34_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_34;
	static const unsigned long _tokenSet_35_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_35;
	static const unsigned long _tokenSet_36_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_36;
	static const unsigned long _tokenSet_37_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_37;
	static const unsigned long _tokenSet_38_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_38;
	static const unsigned long _tokenSet_39_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_39;
	static const unsigned long _tokenSet_40_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_40;
	static const unsigned long _tokenSet_41_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_41;
	static const unsigned long _tokenSet_42_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_42;
	static const unsigned long _tokenSet_43_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_43;
	static const unsigned long _tokenSet_44_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_44;
	static const unsigned long _tokenSet_45_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_45;
};

#endif /*INC_lexer_arrow_hpp_*/
