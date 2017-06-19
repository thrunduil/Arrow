/* $ANTLR 2.7.7 (2006-11-01): "arrow.g" -> "lexer_arrow.cpp"$ */
#include "lexer_arrow.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>


#include "grammar/lexer_include.h"

lexer_arrow::lexer_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(std)istream& in)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(pool,new(*pool,get_ptr<antlr::CharBuffer>()) ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
{
	initLiterals();
}

lexer_arrow::lexer_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(pool,ib,true)
{
	initLiterals();
}

lexer_arrow::lexer_arrow(antlr::pool_parser pool,const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state)
	: ANTLR_USE_NAMESPACE(antlr)CharScanner(pool,state,true)
{
	initLiterals();
}

void lexer_arrow::initLiterals()
{
	literals["identifier ascii"] = 117;
	literals["binary integer"] = 94;
	literals["shebang char"] = 83;
	literals["type"] = 17;
	literals["Fun"] = 46;
	literals["dot bracket assign definition"] = 70;
	literals["octal integer value"] = 103;
	literals["extern"] = 11;
	literals["list"] = 40;
	literals["an identifier"] = 65;
	literals["left paren"] = 26;
	literals["case"] = 55;
	literals["octal integer"] = 95;
	literals["Variant"] = 39;
	literals["any token"] = 77;
	literals["opening bracket"] = 71;
	literals["line comment"] = 87;
	literals["bracket assign definition"] = 68;
	literals["operator symbol"] = 42;
	literals["operator character"] = 121;
	literals["bin prefix"] = 99;
	literals["left curly bracket"] = 19;
	literals["then"] = 53;
	literals["Kind"] = 28;
	literals["decimal integer"] = 93;
	literals["Then"] = 49;
	literals["decimal integer value"] = 97;
	literals["hexal integer"] = 106;
	literals["string literal rule"] = 112;
	literals["module"] = 5;
	literals["float"] = 37;
	literals["operator ascii character"] = 119;
	literals["block comment"] = 88;
	literals["hexal digit"] = 107;
	literals["decimal exponent"] = 110;
	literals["hexal prefix"] = 105;
	literals["operator"] = 18;
	literals["char literal"] = 113;
	literals["decimal digit"] = 98;
	literals["right angle bracket"] = 45;
	literals["over"] = 9;
	literals["utf8 bom"] = 82;
	literals["octal prefix"] = 102;
	literals["open"] = 57;
	literals["utf8 next byte"] = 79;
	literals["ref"] = 61;
	literals["Record"] = 38;
	literals["block comment content"] = 116;
	literals["id start"] = 115;
	literals["escape char"] = 92;
	literals["Ref"] = 36;
	literals["bin digit"] = 101;
	literals["closing bracket"] = 72;
	literals["escape sequence"] = 91;
	literals["deref"] = 62;
	literals["octal digit"] = 104;
	literals["an operator"] = 120;
	literals["number literal"] = 76;
	literals["an id"] = 114;
	literals["comment section"] = 86;
	literals["hexal number"] = 109;
	literals["right bracket"] = 14;
	literals["if"] = 52;
	literals["record"] = 63;
	literals["utf8 lead byte"] = 78;
	literals["line break"] = 85;
	literals["wysiwig string"] = 90;
	literals["rec"] = 60;
	literals["whitespace char"] = 84;
	literals["right curly bracket"] = 20;
	literals["string literal"] = 66;
	literals["bracket definition"] = 67;
	literals["Rec"] = 47;
	literals["left bracket"] = 12;
	literals["Some"] = 35;
	literals["All"] = 34;
	literals["int"] = 29;
	literals["decimal number"] = 108;
	literals["hexal iteger"] = 96;
	literals["char"] = 31;
	literals["utf8 multibyte"] = 80;
	literals["variant"] = 64;
	literals["string"] = 32;
	literals["identifier ascii start"] = 118;
	literals["false"] = 75;
	literals["val"] = 8;
	literals["right paren"] = 27;
	literals["bin integer"] = 100;
	literals["left angle bracket"] = 43;
	literals["dot bracket definition"] = 69;
	literals["If"] = 48;
	literals["bool"] = 30;
	literals["integer literal"] = 21;
	literals["else"] = 54;
	literals["double quoted string"] = 89;
	literals["import"] = 6;
	literals["in"] = 56;
	literals["let"] = 58;
	literals["character literal"] = 73;
	literals["Else"] = 50;
	literals["utf8 not line break"] = 81;
	literals["true"] = 74;
	literals["fun"] = 59;
	literals["hexal exponent"] = 111;
}

ANTLR_USE_NAMESPACE(antlr)RefToken lexer_arrow::nextToken()
{
	ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
	for (;;) {
		ANTLR_USE_NAMESPACE(antlr)RefToken theRetToken;
		int _ttype = ANTLR_USE_NAMESPACE(antlr)Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0x28 /* '(' */ :
			{
				mTOK_lparen(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x29 /* ')' */ :
			{
				mTOK_rparen(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5b /* '[' */ :
			{
				mTOK_lbracket(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5d /* ']' */ :
			{
				mTOK_rbracket(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7b /* '{' */ :
			{
				mTOK_lcurly(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7d /* '}' */ :
			{
				mTOK_rcurly(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2c /* ',' */ :
			{
				mTOK_comma(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3b /* ';' */ :
			{
				mTOK_semi(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((_tokenSet_0.member(LA(1)))) {
					mAny(true);
					theRetToken=_returnToken;
				}
			else {
				if (LA(1)==EOF_CHAR)
				{
					uponEOF();
					_returnToken = makeToken(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
				}
				else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			}
			if ( !_returnToken )
				goto tryAgain; // found SKIP token

			_ttype = _returnToken->getType();
			_ttype = testLiteralsTable(_ttype);
			_returnToken->setType(_ttype);
			return _returnToken;
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& e) {
			{
				reportError(e);
				consume();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamIOException& csie) {
			throw ANTLR_USE_NAMESPACE(antlr)TokenStreamIOException(csie.io);
		}
		catch (ANTLR_USE_NAMESPACE(antlr)CharStreamException& cse) {
			throw ANTLR_USE_NAMESPACE(antlr)TokenStreamException(cse.getMessage());
		}
tryAgain:;
	}
}

void lexer_arrow::mTOK_lparen(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_lparen;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("(");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_rparen(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_rparen;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match(")");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_lbracket(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_lbracket;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("[");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_rbracket(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_rbracket;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("]");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_lcurly(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_lcurly;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("{");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_rcurly(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_rcurly;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("}");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_comma(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_comma;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match(",");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_semi(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_semi;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match(";");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_arrow(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_arrow;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_double_arrow(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_double_arrow;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_equal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_equal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_underscore(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_underscore;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_dot_langle(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_dot_langle;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_star(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_star;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_dot(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_dot;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_plus(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_plus;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_minus(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_minus;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_colon(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_colon;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mTOK_dcolon(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = TOK_dcolon;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mAny(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Any;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		bool synPredMatched380 = false;
		if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x58 /* 'X' */  || LA(2) == 0x78 /* 'x' */ ) && (_tokenSet_2.member(LA(3))) && (_tokenSet_3.member(LA(4))))) {
			int _m380 = mark();
			synPredMatched380 = true;
			inputState->guessing++;
			try {
				{
				mHex_floating_literal(false);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched380 = false;
			}
			rewind(_m380);
			inputState->guessing--;
		}
		if ( synPredMatched380 ) {
			mHex_floating_literal(false);
			if ( inputState->guessing==0 ) {
				_ttype = TOK_number;
			}
		}
		else if (((LA(1) == 0xef) && (LA(2) == 0xbb) && (LA(3) == 0xbf) && (true))&&( getLine() <= 1 && getColumn() <= 1 )) {
			mUTF8_bom(false);
			if ( inputState->guessing==0 ) {
				_ttype = White_space;
			}
		}
		else {
			bool synPredMatched382 = false;
			if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_4.member(LA(2))) && (_tokenSet_5.member(LA(3))) && (true))) {
				int _m382 = mark();
				synPredMatched382 = true;
				inputState->guessing++;
				try {
					{
					mDec_floating_literal(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched382 = false;
				}
				rewind(_m382);
				inputState->guessing--;
			}
			if ( synPredMatched382 ) {
				mDec_floating_literal(false);
				if ( inputState->guessing==0 ) {
					_ttype = TOK_number;
				}
			}
			else {
				bool synPredMatched384 = false;
				if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x58 /* 'X' */  || LA(2) == 0x78 /* 'x' */ ) && (_tokenSet_2.member(LA(3))) && (true))) {
					int _m384 = mark();
					synPredMatched384 = true;
					inputState->guessing++;
					try {
						{
						mHex_prefix(false);
						mHex_digit(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched384 = false;
					}
					rewind(_m384);
					inputState->guessing--;
				}
				if ( synPredMatched384 ) {
					mHex_integer_literal(false);
					if ( inputState->guessing==0 ) {
						_ttype = TOK_integer;
					}
				}
				else {
					bool synPredMatched387 = false;
					if (((LA(1) == 0x27 /* '\'' */ ) && (_tokenSet_6.member(LA(2))) && (_tokenSet_7.member(LA(3))) && (true))) {
						int _m387 = mark();
						synPredMatched387 = true;
						inputState->guessing++;
						try {
							{
							mCharacter_literal(false);
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							(void) pe; /* suppress warnings about unreferenced var 'pe' */
							synPredMatched387 = false;
						}
						rewind(_m387);
						inputState->guessing--;
					}
					if ( synPredMatched387 ) {
						mCharacter_literal(false);
						if ( inputState->guessing==0 ) {
							_ttype = TOK_char;
						}
					}
					else if (((LA(1) == 0x23 /* '#' */ ) && (LA(2) == 0x21 /* '!' */ ) && (true) && (true))&&( getLine() <= 1 && getColumn() <= 1 )) {
						mShebang(false);
						if ( inputState->guessing==0 ) {
							_ttype = Comment;
						}
					}
					else {
						bool synPredMatched372 = false;
						if (((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2a /* '*' */  || LA(2) == 0x2f /* '/' */ ) && (true) && (true))) {
							int _m372 = mark();
							synPredMatched372 = true;
							inputState->guessing++;
							try {
								{
								if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2f /* '/' */ )) {
									match("//");
								}
								else if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2a /* '*' */ )) {
									match("/*");
								}
								else {
									throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
								}
								
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								(void) pe; /* suppress warnings about unreferenced var 'pe' */
								synPredMatched372 = false;
							}
							rewind(_m372);
							inputState->guessing--;
						}
						if ( synPredMatched372 ) {
							mComment(false);
							if ( inputState->guessing==0 ) {
								_ttype = Comment;
							}
						}
						else {
							bool synPredMatched376 = false;
							if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x42 /* 'B' */  || LA(2) == 0x62 /* 'b' */ ))) {
								int _m376 = mark();
								synPredMatched376 = true;
								inputState->guessing++;
								try {
									{
									mBin_prefix(false);
									mBin_digit(false);
									}
								}
								catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
									(void) pe; /* suppress warnings about unreferenced var 'pe' */
									synPredMatched376 = false;
								}
								rewind(_m376);
								inputState->guessing--;
							}
							if ( synPredMatched376 ) {
								mBin_integer_literal(false);
								if ( inputState->guessing==0 ) {
									_ttype = TOK_integer;
								}
							}
							else {
								bool synPredMatched378 = false;
								if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x4f /* 'O' */  || LA(2) == 0x6f /* 'o' */ ))) {
									int _m378 = mark();
									synPredMatched378 = true;
									inputState->guessing++;
									try {
										{
										mOct_prefix(false);
										mOct_digit(false);
										}
									}
									catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
										(void) pe; /* suppress warnings about unreferenced var 'pe' */
										synPredMatched378 = false;
									}
									rewind(_m378);
									inputState->guessing--;
								}
								if ( synPredMatched378 ) {
									mOct_integer_literal(false);
									if ( inputState->guessing==0 ) {
										_ttype = TOK_integer;
									}
								}
								else {
									bool synPredMatched389 = false;
									if (((LA(1) == 0x24 /* '$' */ ) && (LA(2) == 0x2e /* '.' */ ) && (true) && (true))) {
										int _m389 = mark();
										synPredMatched389 = true;
										inputState->guessing++;
										try {
											{
											match("$.");
											}
										}
										catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
											(void) pe; /* suppress warnings about unreferenced var 'pe' */
											synPredMatched389 = false;
										}
										rewind(_m389);
										inputState->guessing--;
									}
									if ( synPredMatched389 ) {
										{
										match("$.");
										}
										{
										bool synPredMatched397 = false;
										if (((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (_tokenSet_8.member(LA(3))) && (_tokenSet_8.member(LA(4))))&&( is_opening_bracket(check_next_code_point()) ))) {
											int _m397 = mark();
											synPredMatched397 = true;
											inputState->guessing++;
											try {
												{
												mUTF8_lead_byte(false);
												}
											}
											catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
												(void) pe; /* suppress warnings about unreferenced var 'pe' */
												synPredMatched397 = false;
											}
											rewind(_m397);
											inputState->guessing--;
										}
										if ( synPredMatched397 ) {
											if ( inputState->guessing==0 ) {
												pop_symbol(2); push_symbol(".");
											}
											mUTF8_multibyte_char(false);
											if (!( is_closing_bracket(check_next_code_point()) ))
												throw ANTLR_USE_NAMESPACE(antlr)SemanticException(" is_closing_bracket(check_next_code_point()) ");
											mUTF8_multibyte_char(false);
											{
											if ((_tokenSet_9.member(LA(1)))) {
												mOperator(false);
												if ( inputState->guessing==0 ) {
													_ttype = TOK_dot_bracket_assign_dolar;
												}
											}
											else {
												if ( inputState->guessing==0 ) {
													_ttype = TOK_dot_bracket_dolar;
												}
											}
											
											}
										}
										else {
											bool synPredMatched393 = false;
											if (((LA(1) == 0x28 /* '(' */  || LA(1) == 0x3c /* '<' */  || LA(1) == 0x5b /* '[' */  || LA(1) == 0x7b /* '{' */ ) && (LA(2) == 0x29 /* ')' */  || LA(2) == 0x3e /* '>' */  || LA(2) == 0x5d /* ']' */  || LA(2) == 0x7d /* '}' */ ) && (true) && (true))) {
												int _m393 = mark();
												synPredMatched393 = true;
												inputState->guessing++;
												try {
													{
													if ((LA(1) == 0x28 /* '(' */ )) {
														match("()");
													}
													else if ((LA(1) == 0x7b /* '{' */ )) {
														match("{}");
													}
													else if ((LA(1) == 0x5b /* '[' */ )) {
														match("[]");
													}
													else if ((LA(1) == 0x3c /* '<' */ )) {
														match("<>");
													}
													else {
														throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
													}
													
													}
												}
												catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
													(void) pe; /* suppress warnings about unreferenced var 'pe' */
													synPredMatched393 = false;
												}
												rewind(_m393);
												inputState->guessing--;
											}
											if ( synPredMatched393 ) {
												if ( inputState->guessing==0 ) {
													pop_symbol(2); push_symbol(".");
												}
												{
												if ((LA(1) == 0x28 /* '(' */ )) {
													match("()");
												}
												else if ((LA(1) == 0x7b /* '{' */ )) {
													match("{}");
												}
												else if ((LA(1) == 0x5b /* '[' */ )) {
													match("[]");
												}
												else if ((LA(1) == 0x3c /* '<' */ )) {
													match("<>");
												}
												else {
													throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
												}
												
												}
												{
												if ((_tokenSet_9.member(LA(1)))) {
													mOperator(false);
													if ( inputState->guessing==0 ) {
														_ttype = TOK_dot_bracket_assign_dolar;
													}
												}
												else {
													if ( inputState->guessing==0 ) {
														_ttype = TOK_dot_bracket_dolar;
													}
												}
												
												}
											}
											else {
												{
												if ((_tokenSet_9.member(LA(1)))) {
													mOperator(false);
												}
												else {
												}
												
												}
												if ( inputState->guessing==0 ) {
													_ttype = TOK_operator;
												}
											}
										}
										}
									}
									else {
										bool synPredMatched368 = false;
										if (((_tokenSet_10.member(LA(1))) && (true) && (true) && (true))) {
											int _m368 = mark();
											synPredMatched368 = true;
											inputState->guessing++;
											try {
												{
												switch ( LA(1)) {
												case 0x20 /* ' ' */ :
												{
													match(' ' /* charlit */ );
													break;
												}
												case 0x9 /* '\t' */ :
												{
													match('\t' /* charlit */ );
													break;
												}
												case 0xa /* '\n' */ :
												{
													match('\n' /* charlit */ );
													break;
												}
												case 0xc /* '\14' */ :
												{
													match('\14' /* charlit */ );
													break;
												}
												case 0xd /* '\r' */ :
												{
													match('\r' /* charlit */ );
													break;
												}
												default:
												{
													throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
												}
												}
												}
											}
											catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
												(void) pe; /* suppress warnings about unreferenced var 'pe' */
												synPredMatched368 = false;
											}
											rewind(_m368);
											inputState->guessing--;
										}
										if ( synPredMatched368 ) {
											mWhite_space(false);
											if ( inputState->guessing==0 ) {
												_ttype = White_space;
											}
										}
										else {
											bool synPredMatched370 = false;
											if (((_tokenSet_11.member(LA(1))) && (true) && (true) && (true))) {
												int _m370 = mark();
												synPredMatched370 = true;
												inputState->guessing++;
												try {
													{
													mIdentifier_start(false);
													}
												}
												catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
													(void) pe; /* suppress warnings about unreferenced var 'pe' */
													synPredMatched370 = false;
												}
												rewind(_m370);
												inputState->guessing--;
											}
											if ( synPredMatched370 ) {
												mIdentifier(false);
												if ( inputState->guessing==0 ) {
													_ttype = TOK_identifier;
												}
											}
											else {
												bool synPredMatched374 = false;
												if (((LA(1) == 0x22 /* '\"' */ ))) {
													int _m374 = mark();
													synPredMatched374 = true;
													inputState->guessing++;
													try {
														{
														match('\"' /* charlit */ );
														}
													}
													catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
														(void) pe; /* suppress warnings about unreferenced var 'pe' */
														synPredMatched374 = false;
													}
													rewind(_m374);
													inputState->guessing--;
												}
												if ( synPredMatched374 ) {
													mString_literal(false);
													if ( inputState->guessing==0 ) {
														_ttype = TOK_string;
													}
												}
												else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true) && (true)) {
													mDec_integer_literal(false);
													if ( inputState->guessing==0 ) {
														_ttype = TOK_integer;
													}
												}
												else if ((LA(1) == 0x0 /* '\0' */  || LA(1) == 0x1a /* '\32' */ )) {
													{
													if ((LA(1) == 0x0 /* '\0' */ )) {
														match('\0' /* charlit */ );
													}
													else if ((LA(1) == 0x1a /* '\32' */ )) {
														match('\32' /* charlit */ );
													}
													else {
														throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
													}
													
													}
													if ( inputState->guessing==0 ) {
														_ttype = antlr::Token::EOF_TYPE;
													}
												}
												else if ((LA(1) == 0x5f /* '_' */ )) {
													match("_");
													if ( inputState->guessing==0 ) {
														_ttype = TOK_underscore;
													}
												}
												else {
													bool synPredMatched401 = false;
													if (((LA(1) == 0x24 /* '$' */ ) && (true) && (true) && (true))) {
														int _m401 = mark();
														synPredMatched401 = true;
														inputState->guessing++;
														try {
															{
															match('$' /* charlit */ );
															}
														}
														catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
															(void) pe; /* suppress warnings about unreferenced var 'pe' */
															synPredMatched401 = false;
														}
														rewind(_m401);
														inputState->guessing--;
													}
													if ( synPredMatched401 ) {
														{
														match('$' /* charlit */ );
														}
														{
														bool synPredMatched409 = false;
														if (((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (_tokenSet_8.member(LA(3))) && (_tokenSet_8.member(LA(4))))&&( is_opening_bracket(check_next_code_point()) ))) {
															int _m409 = mark();
															synPredMatched409 = true;
															inputState->guessing++;
															try {
																{
																mUTF8_lead_byte(false);
																}
															}
															catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
																(void) pe; /* suppress warnings about unreferenced var 'pe' */
																synPredMatched409 = false;
															}
															rewind(_m409);
															inputState->guessing--;
														}
														if ( synPredMatched409 ) {
															if ( inputState->guessing==0 ) {
																pop_symbol(1);
															}
															mUTF8_multibyte_char(false);
															if (!( is_closing_bracket(check_next_code_point()) ))
																throw ANTLR_USE_NAMESPACE(antlr)SemanticException(" is_closing_bracket(check_next_code_point()) ");
															mUTF8_multibyte_char(false);
															{
															if ((_tokenSet_9.member(LA(1)))) {
																mOperator(false);
																if ( inputState->guessing==0 ) {
																	_ttype = TOK_bracket_assign_dolar;
																}
															}
															else {
																if ( inputState->guessing==0 ) {
																	_ttype = TOK_bracket_dolar;
																}
															}
															
															}
														}
														else {
															bool synPredMatched405 = false;
															if (((LA(1) == 0x28 /* '(' */  || LA(1) == 0x3c /* '<' */  || LA(1) == 0x5b /* '[' */  || LA(1) == 0x7b /* '{' */ ) && (LA(2) == 0x29 /* ')' */  || LA(2) == 0x3e /* '>' */  || LA(2) == 0x5d /* ']' */  || LA(2) == 0x7d /* '}' */ ) && (true) && (true))) {
																int _m405 = mark();
																synPredMatched405 = true;
																inputState->guessing++;
																try {
																	{
																	if ((LA(1) == 0x28 /* '(' */ )) {
																		match("()");
																	}
																	else if ((LA(1) == 0x7b /* '{' */ )) {
																		match("{}");
																	}
																	else if ((LA(1) == 0x5b /* '[' */ )) {
																		match("[]");
																	}
																	else if ((LA(1) == 0x3c /* '<' */ )) {
																		match("<>");
																	}
																	else {
																		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
																	}
																	
																	}
																}
																catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
																	(void) pe; /* suppress warnings about unreferenced var 'pe' */
																	synPredMatched405 = false;
																}
																rewind(_m405);
																inputState->guessing--;
															}
															if ( synPredMatched405 ) {
																if ( inputState->guessing==0 ) {
																	pop_symbol(1);
																}
																{
																if ((LA(1) == 0x28 /* '(' */ )) {
																	match("()");
																}
																else if ((LA(1) == 0x7b /* '{' */ )) {
																	match("{}");
																}
																else if ((LA(1) == 0x5b /* '[' */ )) {
																	match("[]");
																}
																else if ((LA(1) == 0x3c /* '<' */ )) {
																	match("<>");
																}
																else {
																	throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
																}
																
																}
																{
																if ((_tokenSet_9.member(LA(1)))) {
																	mOperator(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_bracket_assign_dolar;
																	}
																}
																else {
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_bracket_dolar;
																	}
																}
																
																}
															}
															else {
																{
																if ((_tokenSet_9.member(LA(1)))) {
																	mOperator(false);
																}
																else {
																}
																
																}
																if ( inputState->guessing==0 ) {
																	_ttype = TOK_operator;
																}
															}
														}
														}
													}
													else {
														bool synPredMatched413 = false;
														if (((_tokenSet_9.member(LA(1))) && (true) && (true) && (true))) {
															int _m413 = mark();
															synPredMatched413 = true;
															inputState->guessing++;
															try {
																{
																mOperator_ascii_char(false);
																}
															}
															catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
																(void) pe; /* suppress warnings about unreferenced var 'pe' */
																synPredMatched413 = false;
															}
															rewind(_m413);
															inputState->guessing--;
														}
														if ( synPredMatched413 ) {
															{
															bool synPredMatched416 = false;
															if (((_tokenSet_12.member(LA(1))) && (true) && (true) && (true))) {
																int _m416 = mark();
																synPredMatched416 = true;
																inputState->guessing++;
																try {
																	{
																	mPredefined_operator(false);
																	}
																}
																catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
																	(void) pe; /* suppress warnings about unreferenced var 'pe' */
																	synPredMatched416 = false;
																}
																rewind(_m416);
																inputState->guessing--;
															}
															if ( synPredMatched416 ) {
																mPredefined_operator(false);
															}
															else if ((_tokenSet_9.member(LA(1))) && (true) && (true) && (true)) {
																mOperator(false);
															}
															else {
																throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
															}
															
															}
															if ( inputState->guessing==0 ) {
																
																std::string s = text.substr(_begin,text.length()-_begin);
																if (s == ":")                           { _ttype = TOK_colon; }
																else if (s == "::")                     { _ttype = TOK_dcolon; }
																else if (s == "=>")                     { _ttype = TOK_double_arrow; }
																else if (s == "->")                     { _ttype = TOK_arrow; }
																else if (s == "=")                      { _ttype = TOK_equal; }
																else if (s == "_")                      { _ttype = TOK_underscore; }
																else if (s == "+")                      { _ttype = TOK_plus; }
																else if (s == "-")                      { _ttype = TOK_minus; }            
																else if (s == "<")                      { _ttype = TOK_langle; }
																else if (s == ">")                      { _ttype = TOK_rangle; }
																else if (s == ".<")                     { _ttype = TOK_dot_langle; }
																else if (s == "*")                      { _ttype = TOK_star; }
																else if (s == ".")                      { _ttype = TOK_dot; }
																
																else                                    { _ttype = TOK_operator; }
																
															}
														}
														else {
															bool synPredMatched418 = false;
															if (((_tokenSet_13.member(LA(1))) && (true) && (true) && (true))) {
																int _m418 = mark();
																synPredMatched418 = true;
																inputState->guessing++;
																try {
																	{
																	mUTF8_lead_byte(false);
																	}
																}
																catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
																	(void) pe; /* suppress warnings about unreferenced var 'pe' */
																	synPredMatched418 = false;
																}
																rewind(_m418);
																inputState->guessing--;
															}
															if ( synPredMatched418 ) {
																{
																if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))&&( is_opening_bracket(check_next_code_point()) )) {
																	mUTF8_multibyte_char(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_opening_bracket;
																	}
																}
																else if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))&&( is_closing_bracket(check_next_code_point()) )) {
																	mUTF8_multibyte_char(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_closing_bracket;
																	}
																}
																else if (((_tokenSet_10.member(LA(1))) && (true) && (true) && (true))&&( is_white_space(check_next_code_point()) )) {
																	mWhite_space(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = White_space;
																	}
																}
																else if (((_tokenSet_11.member(LA(1))) && (true) && (true) && (true))&&( is_identifier_start(check_next_code_point()) )) {
																	mIdentifier(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_identifier;
																	}
																}
																else if (((_tokenSet_9.member(LA(1))) && (true) && (true) && (true))&&( is_operator_char(check_next_code_point()) )) {
																	mOperator(false);
																	if ( inputState->guessing==0 ) {
																		_ttype = TOK_operator;
																	}
																}
																else {
																	throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
																}
																
																}
															}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}}}}}}}}}}}}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mUTF8_bom(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UTF8_bom;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match(static_cast<unsigned char>('\357') /* charlit */ );
		match(static_cast<unsigned char>('\273') /* charlit */ );
		match(static_cast<unsigned char>('\277') /* charlit */ );
		if ( inputState->guessing==0 ) {
			setColumn(getColumn() - 3); /* 0 width */
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mShebang(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Shebang;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("#!");
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_14.member(LA(1)))) {
				{
				match(_tokenSet_14);
				}
			}
			else {
				bool synPredMatched449 = false;
				if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)))) {
					int _m449 = mark();
					synPredMatched449 = true;
					inputState->guessing++;
					try {
						{
						mUTF8_not_line_break(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched449 = false;
					}
					rewind(_m449);
					inputState->guessing--;
				}
				if ( synPredMatched449 ) {
					mUTF8_not_line_break(false);
				}
			else {
				goto _loop450;
			}
			}
		}
		_loop450:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mWhite_space(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = White_space;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt430=0;
		for (;;) {
			bool synPredMatched429 = false;
			if (((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))&&( is_white_space(check_next_code_point()) ))) {
				int _m429 = mark();
				synPredMatched429 = true;
				inputState->guessing++;
				try {
					{
					mUTF8_lead_byte(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched429 = false;
				}
				rewind(_m429);
				inputState->guessing--;
			}
			if ( synPredMatched429 ) {
				mUTF8_multibyte_char(false);
			}
			else if ((LA(1) == 0x9 /* '\t' */ )) {
				match('\t' /* charlit */ );
			}
			else if ((LA(1) == 0x20 /* ' ' */ )) {
				match(' ' /* charlit */ );
			}
			else {
				bool synPredMatched427 = false;
				if (((_tokenSet_15.member(LA(1))) && (true) && (true) && (true))) {
					int _m427 = mark();
					synPredMatched427 = true;
					inputState->guessing++;
					try {
						{
						mLine_break(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched427 = false;
					}
					rewind(_m427);
					inputState->guessing--;
				}
				if ( synPredMatched427 ) {
					mLine_break(false);
				}
			else {
				if ( _cnt430>=1 ) { goto _loop430; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			}
			_cnt430++;
		}
		_loop430:;
		}  // ( ... )+
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mIdentifier_start(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Identifier_start;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((_tokenSet_16.member(LA(1)))) {
			mIdentifier_ascii_start(false);
		}
		else {
			bool synPredMatched556 = false;
			if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)))) {
				int _m556 = mark();
				synPredMatched556 = true;
				inputState->guessing++;
				try {
					{
					mUTF8_lead_byte(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched556 = false;
				}
				rewind(_m556);
				inputState->guessing--;
			}
			if ( synPredMatched556 ) {
				mUTF8_multibyte_char(false);
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_17);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mIdentifier(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Identifier;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mIdentifier_start(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_18.member(LA(1)))) {
				mIdentifier_ascii(false);
			}
			else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mDec_digit(false);
			}
			else {
				bool synPredMatched552 = false;
				if (((((LA(1) >= 0xc2 && LA(1) <= 0xf4)))&&( is_identifier_part(check_next_code_point()) ))) {
					int _m552 = mark();
					synPredMatched552 = true;
					inputState->guessing++;
					try {
						{
						mUTF8_lead_byte(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched552 = false;
					}
					rewind(_m552);
					inputState->guessing--;
				}
				if ( synPredMatched552 ) {
					mUTF8_multibyte_char(false);
				}
			else {
				goto _loop553;
			}
			}
		}
		_loop553:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mComment(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Comment;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2f /* '/' */ )) {
			mLine_comment(false);
		}
		else if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2a /* '*' */ )) {
			mBlock_comment(false);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mString_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = String_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		bool synPredMatched474 = false;
		if (((LA(1) == 0x22 /* '\"' */ ) && (LA(2) == 0x22 /* '\"' */ ) && (LA(3) == 0x22 /* '\"' */ ) && (_tokenSet_19.member(LA(4))))) {
			int _m474 = mark();
			synPredMatched474 = true;
			inputState->guessing++;
			try {
				{
				match("\"\"\"");
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched474 = false;
			}
			rewind(_m474);
			inputState->guessing--;
		}
		if ( synPredMatched474 ) {
			mWysiwyg_string(false);
		}
		else {
			bool synPredMatched476 = false;
			if (((LA(1) == 0x22 /* '\"' */ ) && (_tokenSet_19.member(LA(2))) && (true) && (true))) {
				int _m476 = mark();
				synPredMatched476 = true;
				inputState->guessing++;
				try {
					{
					match('\"' /* charlit */ );
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched476 = false;
				}
				rewind(_m476);
				inputState->guessing--;
			}
			if ( synPredMatched476 ) {
				mDouble_quoted_string(false);
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBin_prefix(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Bin_prefix;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('0' /* charlit */ );
		{
		if ((LA(1) == 0x62 /* 'b' */ )) {
			match('b' /* charlit */ );
		}
		else if ((LA(1) == 0x42 /* 'B' */ )) {
			match('B' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_20);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBin_digit(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Bin_digit;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((LA(1) == 0x30 /* '0' */ )) {
			match('0' /* charlit */ );
		}
		else if ((LA(1) == 0x31 /* '1' */ )) {
			match('1' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_21);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBin_integer_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Bin_integer_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mBin_prefix(false);
		mBin_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOct_prefix(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Oct_prefix;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('0' /* charlit */ );
		{
		if ((LA(1) == 0x6f /* 'o' */ )) {
			match('o' /* charlit */ );
		}
		else if ((LA(1) == 0x4f /* 'O' */ )) {
			match('O' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_22);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOct_digit(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Oct_digit;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		matchRange('0','7');
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_23);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOct_integer_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Oct_integer_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mOct_prefix(false);
		mOct_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_floating_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_floating_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		bool synPredMatched541 = false;
		if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x58 /* 'X' */  || LA(2) == 0x78 /* 'x' */ ) && (_tokenSet_2.member(LA(3))) && (_tokenSet_24.member(LA(4))))) {
			int _m541 = mark();
			synPredMatched541 = true;
			inputState->guessing++;
			try {
				{
				mHex_prefix(false);
				mHex_integer(false);
				match('.' /* charlit */ );
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched541 = false;
			}
			rewind(_m541);
			inputState->guessing--;
		}
		if ( synPredMatched541 ) {
			mHex_prefix(false);
			mHex_integer(false);
			match('.' /* charlit */ );
			mHex_integer(false);
			{
			if ((LA(1) == 0x50 /* 'P' */  || LA(1) == 0x70 /* 'p' */ )) {
				mHex_exponent(false);
			}
			else {
			}
			
			}
		}
		else {
			bool synPredMatched545 = false;
			if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x58 /* 'X' */  || LA(2) == 0x78 /* 'x' */ ) && (_tokenSet_2.member(LA(3))) && (_tokenSet_25.member(LA(4))))) {
				int _m545 = mark();
				synPredMatched545 = true;
				inputState->guessing++;
				try {
					{
					mHex_prefix(false);
					mHex_integer(false);
					{
					if ((LA(1) == 0x70 /* 'p' */ )) {
						match('p' /* charlit */ );
					}
					else if ((LA(1) == 0x50 /* 'P' */ )) {
						match('P' /* charlit */ );
					}
					else {
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					
					}
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched545 = false;
				}
				rewind(_m545);
				inputState->guessing--;
			}
			if ( synPredMatched545 ) {
				mHex_prefix(false);
				mHex_integer(false);
				mHex_exponent(false);
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDec_floating_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Dec_floating_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		bool synPredMatched531 = false;
		if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_26.member(LA(2))) && (_tokenSet_26.member(LA(3))) && (true))) {
			int _m531 = mark();
			synPredMatched531 = true;
			inputState->guessing++;
			try {
				{
				mDec_integer(false);
				match('.' /* charlit */ );
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched531 = false;
			}
			rewind(_m531);
			inputState->guessing--;
		}
		if ( synPredMatched531 ) {
			mDec_integer(false);
			match('.' /* charlit */ );
			mDec_integer(false);
			{
			if ((LA(1) == 0x45 /* 'E' */  || LA(1) == 0x65 /* 'e' */ )) {
				mDec_exponent(false);
			}
			else {
			}
			
			}
		}
		else {
			bool synPredMatched535 = false;
			if ((((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (_tokenSet_27.member(LA(2))) && (_tokenSet_28.member(LA(3))) && (true))) {
				int _m535 = mark();
				synPredMatched535 = true;
				inputState->guessing++;
				try {
					{
					mDec_integer(false);
					{
					if ((LA(1) == 0x65 /* 'e' */ )) {
						match('e' /* charlit */ );
					}
					else if ((LA(1) == 0x45 /* 'E' */ )) {
						match('E' /* charlit */ );
					}
					else {
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					
					}
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched535 = false;
				}
				rewind(_m535);
				inputState->guessing--;
			}
			if ( synPredMatched535 ) {
				mDec_integer(false);
				mDec_exponent(false);
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_prefix(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_prefix;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('0' /* charlit */ );
		{
		if ((LA(1) == 0x78 /* 'x' */ )) {
			match('x' /* charlit */ );
		}
		else if ((LA(1) == 0x58 /* 'X' */ )) {
			match('X' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_digit(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_digit;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			matchRange('0','9');
		}
		else if (((LA(1) >= 0x61 /* 'a' */  && LA(1) <= 0x66 /* 'f' */ ))) {
			matchRange('a','f');
		}
		else if (((LA(1) >= 0x41 /* 'A' */  && LA(1) <= 0x46 /* 'F' */ ))) {
			matchRange('A','F');
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_integer_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_integer_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mHex_prefix(false);
		mHex_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDec_integer_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Dec_integer_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mDec_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mCharacter_literal(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Character_literal;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('\'' /* charlit */ );
		{
		if ((_tokenSet_29.member(LA(1)))) {
			{
			match(_tokenSet_29);
			}
		}
		else if ((LA(1) == 0x5c /* '\\' */ )) {
			mEscape_sequence(false);
		}
		else if (((LA(1) >= 0xc2 && LA(1) <= 0xf4))) {
			mUTF8_not_line_break(false);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
		match('\'' /* charlit */ );
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOperator(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Operator;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt561=0;
		for (;;) {
			if ((_tokenSet_9.member(LA(1)))) {
				mOperator_char(false);
			}
			else {
				if ( _cnt561>=1 ) { goto _loop561; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt561++;
		}
		_loop561:;
		}  // ( ... )+
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mUTF8_lead_byte(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UTF8_lead_byte;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		matchRange(static_cast<unsigned char>('\302'),static_cast<unsigned char>('\364'));
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mUTF8_multibyte_char(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UTF8_multibyte_char;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if (((LA(1) >= 0xc2 && LA(1) <= 0xdf))) {
			matchRange(static_cast<unsigned char>('\302'),static_cast<unsigned char>('\337'));
			mUTF8_next_byte(false);
			if ( inputState->guessing==0 ) {
				setColumn(getColumn() - 1);
			}
		}
		else if (((LA(1) >= 0xe0 && LA(1) <= 0xef))) {
			matchRange(static_cast<unsigned char>('\340'),static_cast<unsigned char>('\357'));
			mUTF8_next_byte(false);
			mUTF8_next_byte(false);
			if ( inputState->guessing==0 ) {
				setColumn(getColumn() - 2);
			}
		}
		else if (((LA(1) >= 0xf0 && LA(1) <= 0xf4))) {
			matchRange(static_cast<unsigned char>('\360'),static_cast<unsigned char>('\364'));
			mUTF8_next_byte(false);
			mUTF8_next_byte(false);
			mUTF8_next_byte(false);
			if ( inputState->guessing==0 ) {
				setColumn(getColumn() - 3);
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOperator_ascii_char(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Operator_ascii_char;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		switch ( LA(1)) {
		case 0x7e /* '~' */ :
		{
			match('~' /* charlit */ );
			break;
		}
		case 0x60 /* '`' */ :
		{
			match('`' /* charlit */ );
			break;
		}
		case 0x21 /* '!' */ :
		{
			match('!' /* charlit */ );
			break;
		}
		case 0x40 /* '@' */ :
		{
			match('@' /* charlit */ );
			break;
		}
		case 0x23 /* '#' */ :
		{
			match('#' /* charlit */ );
			break;
		}
		case 0x24 /* '$' */ :
		{
			match('$' /* charlit */ );
			break;
		}
		case 0x25 /* '%' */ :
		{
			match('%' /* charlit */ );
			break;
		}
		case 0x5e /* '^' */ :
		{
			match('^' /* charlit */ );
			break;
		}
		case 0x26 /* '&' */ :
		{
			match('&' /* charlit */ );
			break;
		}
		case 0x2a /* '*' */ :
		{
			match('*' /* charlit */ );
			break;
		}
		case 0x2d /* '-' */ :
		{
			match('-' /* charlit */ );
			break;
		}
		case 0x2b /* '+' */ :
		{
			match('+' /* charlit */ );
			break;
		}
		case 0x3d /* '=' */ :
		{
			match('=' /* charlit */ );
			break;
		}
		case 0x2f /* '/' */ :
		{
			match('/' /* charlit */ );
			break;
		}
		case 0x7c /* '|' */ :
		{
			match('|' /* charlit */ );
			break;
		}
		case 0x5c /* '\\' */ :
		{
			match('\\' /* charlit */ );
			break;
		}
		case 0x3a /* ':' */ :
		{
			match(':' /* charlit */ );
			break;
		}
		case 0x27 /* '\'' */ :
		{
			match('\'' /* charlit */ );
			break;
		}
		case 0x3c /* '<' */ :
		{
			match('<' /* charlit */ );
			break;
		}
		case 0x3e /* '>' */ :
		{
			match('>' /* charlit */ );
			break;
		}
		case 0x2e /* '.' */ :
		{
			match('.' /* charlit */ );
			break;
		}
		case 0x3f /* '?' */ :
		{
			match('?' /* charlit */ );
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mPredefined_operator(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Predefined_operator;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ )) {
			match("::");
		}
		else if ((LA(1) == 0x3d /* '=' */ ) && (LA(2) == 0x3e /* '>' */ )) {
			match("=>");
		}
		else if ((LA(1) == 0x2d /* '-' */ ) && (LA(2) == 0x3e /* '>' */ )) {
			match("->");
		}
		else if ((LA(1) == 0x2e /* '.' */ ) && (LA(2) == 0x3c /* '<' */ )) {
			match(".<");
		}
		else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
			match(":");
		}
		else if ((LA(1) == 0x3d /* '=' */ ) && (true)) {
			match("=");
		}
		else if ((LA(1) == 0x2b /* '+' */ )) {
			match("+");
		}
		else if ((LA(1) == 0x2d /* '-' */ ) && (true)) {
			match("-");
		}
		else if ((LA(1) == 0x3c /* '<' */ )) {
			match("<");
		}
		else if ((LA(1) == 0x3e /* '>' */ )) {
			match(">");
		}
		else if ((LA(1) == 0x2a /* '*' */ )) {
			match("*");
		}
		else if ((LA(1) == 0x2e /* '.' */ ) && (true)) {
			match(".");
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mUTF8_next_byte(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UTF8_next_byte;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		matchRange(static_cast<unsigned char>('\200'),static_cast<unsigned char>('\277'));
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_30);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mLine_break(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Line_break;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((LA(1) == 0xe2) && (LA(2) == 0x80) && (LA(3) == 0xa8)) {
			match(static_cast<unsigned char>('\342') /* charlit */ );
			match(static_cast<unsigned char>('\200') /* charlit */ );
			match(static_cast<unsigned char>('\250') /* charlit */ );
			if ( inputState->guessing==0 ) {
				newline();
			}
		}
		else if ((LA(1) == 0xe2) && (LA(2) == 0x80) && (LA(3) == 0xa9)) {
			match(static_cast<unsigned char>('\342') /* charlit */ );
			match(static_cast<unsigned char>('\200') /* charlit */ );
			match(static_cast<unsigned char>('\251') /* charlit */ );
			if ( inputState->guessing==0 ) {
				newline();
			}
		}
		else {
			bool synPredMatched433 = false;
			if (((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ ) && (true) && (true))) {
				int _m433 = mark();
				synPredMatched433 = true;
				inputState->guessing++;
				try {
					{
					match("\r\n");
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched433 = false;
				}
				rewind(_m433);
				inputState->guessing--;
			}
			if ( synPredMatched433 ) {
				match("\r\n");
				if ( inputState->guessing==0 ) {
					newline();
				}
			}
			else if ((LA(1) == 0xa /* '\n' */ )) {
				match('\n' /* charlit */ );
				if ( inputState->guessing==0 ) {
					newline();
				}
			}
			else if ((LA(1) == 0xc /* '\14' */ )) {
				match('\14' /* charlit */ );
				if ( inputState->guessing==0 ) {
					newline();
				}
			}
			else if ((LA(1) == 0xd /* '\r' */ ) && (true) && (true) && (true)) {
				match("\r");
				if ( inputState->guessing==0 ) {
					newline();
				}
			}
			else if ((LA(1) == 0xc2)) {
				match(static_cast<unsigned char>('\302') /* charlit */ );
				match(static_cast<unsigned char>('\205') /* charlit */ );
				if ( inputState->guessing==0 ) {
					newline();
				}
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mUTF8_not_line_break(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = UTF8_not_line_break;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		bool synPredMatched437 = false;
		if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))) {
			int _m437 = mark();
			synPredMatched437 = true;
			inputState->guessing++;
			try {
				{
				{
				match(_tokenSet_31);
				}
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched437 = false;
			}
			rewind(_m437);
			inputState->guessing--;
		}
		if ( synPredMatched437 ) {
			mUTF8_multibyte_char(false);
		}
		else {
			bool synPredMatched439 = false;
			if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))) {
				int _m439 = mark();
				synPredMatched439 = true;
				inputState->guessing++;
				try {
					{
					match(static_cast<unsigned char>('\302') /* charlit */ );
					matchNot(static_cast<unsigned char>('\205') /* charlit */ );
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched439 = false;
				}
				rewind(_m439);
				inputState->guessing--;
			}
			if ( synPredMatched439 ) {
				mUTF8_multibyte_char(false);
			}
			else {
				bool synPredMatched441 = false;
				if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))) {
					int _m441 = mark();
					synPredMatched441 = true;
					inputState->guessing++;
					try {
						{
						match(static_cast<unsigned char>('\342') /* charlit */ );
						matchNot(static_cast<unsigned char>('\200') /* charlit */ );
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched441 = false;
					}
					rewind(_m441);
					inputState->guessing--;
				}
				if ( synPredMatched441 ) {
					mUTF8_multibyte_char(false);
				}
				else {
					bool synPredMatched444 = false;
					if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true))) {
						int _m444 = mark();
						synPredMatched444 = true;
						inputState->guessing++;
						try {
							{
							match(static_cast<unsigned char>('\342') /* charlit */ );
							match(static_cast<unsigned char>('\200') /* charlit */ );
							{
							match(_tokenSet_32);
							}
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							(void) pe; /* suppress warnings about unreferenced var 'pe' */
							synPredMatched444 = false;
						}
						rewind(_m444);
						inputState->guessing--;
					}
					if ( synPredMatched444 ) {
						mUTF8_multibyte_char(false);
					}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_33);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mLine_comment(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Line_comment;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("//");
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_14.member(LA(1)))) {
				{
				match(_tokenSet_14);
				}
			}
			else {
				bool synPredMatched456 = false;
				if ((((LA(1) >= 0xc2 && LA(1) <= 0xf4)))) {
					int _m456 = mark();
					synPredMatched456 = true;
					inputState->guessing++;
					try {
						{
						mUTF8_not_line_break(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched456 = false;
					}
					rewind(_m456);
					inputState->guessing--;
				}
				if ( synPredMatched456 ) {
					mUTF8_not_line_break(false);
				}
			else {
				goto _loop457;
			}
			}
		}
		_loop457:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBlock_comment(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Block_comment;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("/*");
		mBlock_comment_content(false);
		{
		if ((LA(1) == 0x2a /* '*' */ ) && (LA(2) == 0x2f /* '/' */ ) && (true) && (true)) {
			match("*/");
		}
		else if (true&&( LA(1) == EOF && allow_unterminated_comments )) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBlock_comment_content(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Block_comment_content;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if (((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (true) && (true)) {
				mUTF8_multibyte_char(false);
			}
			else if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x2a /* '*' */ ) && (true) && (true)) {
				mBlock_comment(false);
			}
			else if ((_tokenSet_34.member(LA(1))) && (true) && (true) && (true)) {
				{
				match(_tokenSet_34);
				}
			}
			else {
				bool synPredMatched465 = false;
				if (((LA(1) == 0x2a /* '*' */ ) && (true) && (true) && (true))) {
					int _m465 = mark();
					synPredMatched465 = true;
					inputState->guessing++;
					try {
						{
						match('*' /* charlit */ );
						{
						match(_tokenSet_35);
						}
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched465 = false;
					}
					rewind(_m465);
					inputState->guessing--;
				}
				if ( synPredMatched465 ) {
					match('*' /* charlit */ );
				}
				else {
					bool synPredMatched468 = false;
					if (((LA(1) == 0x2f /* '/' */ ) && (true) && (true) && (true))) {
						int _m468 = mark();
						synPredMatched468 = true;
						inputState->guessing++;
						try {
							{
							match('/' /* charlit */ );
							{
							match(_tokenSet_36);
							}
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							(void) pe; /* suppress warnings about unreferenced var 'pe' */
							synPredMatched468 = false;
						}
						rewind(_m468);
						inputState->guessing--;
					}
					if ( synPredMatched468 ) {
						match('/' /* charlit */ );
					}
					else {
						bool synPredMatched470 = false;
						if (((_tokenSet_15.member(LA(1))) && (true) && (true) && (true))) {
							int _m470 = mark();
							synPredMatched470 = true;
							inputState->guessing++;
							try {
								{
								mLine_break(false);
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								(void) pe; /* suppress warnings about unreferenced var 'pe' */
								synPredMatched470 = false;
							}
							rewind(_m470);
							inputState->guessing--;
						}
						if ( synPredMatched470 ) {
							mLine_break(false);
						}
			else {
				goto _loop471;
			}
			}}}
		}
		_loop471:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mWysiwyg_string(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Wysiwyg_string;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match("\"\"\"");
		{ // ( ... )*
		for (;;) {
			bool synPredMatched487 = false;
			if (((LA(1) == 0x22 /* '\"' */ ) && (_tokenSet_19.member(LA(2))) && (_tokenSet_30.member(LA(3))) && (_tokenSet_30.member(LA(4))))) {
				int _m487 = mark();
				synPredMatched487 = true;
				inputState->guessing++;
				try {
					{
					match('\"' /* charlit */ );
					matchNot('\"' /* charlit */ );
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched487 = false;
				}
				rewind(_m487);
				inputState->guessing--;
			}
			if ( synPredMatched487 ) {
				match('\"' /* charlit */ );
			}
			else {
				bool synPredMatched489 = false;
				if (((LA(1) == 0x22 /* '\"' */ ) && (LA(2) == 0x22 /* '\"' */ ) && (_tokenSet_19.member(LA(3))) && (_tokenSet_30.member(LA(4))))) {
					int _m489 = mark();
					synPredMatched489 = true;
					inputState->guessing++;
					try {
						{
						match('\"' /* charlit */ );
						match('\"' /* charlit */ );
						matchNot('\"' /* charlit */ );
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched489 = false;
					}
					rewind(_m489);
					inputState->guessing--;
				}
				if ( synPredMatched489 ) {
					match('\"' /* charlit */ );
					match('\"' /* charlit */ );
				}
				else {
					bool synPredMatched491 = false;
					if (((_tokenSet_15.member(LA(1))) && (_tokenSet_37.member(LA(2))) && (_tokenSet_30.member(LA(3))) && (_tokenSet_30.member(LA(4))))) {
						int _m491 = mark();
						synPredMatched491 = true;
						inputState->guessing++;
						try {
							{
							mLine_break(false);
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							(void) pe; /* suppress warnings about unreferenced var 'pe' */
							synPredMatched491 = false;
						}
						rewind(_m491);
						inputState->guessing--;
					}
					if ( synPredMatched491 ) {
						mLine_break(false);
					}
					else if (((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (_tokenSet_30.member(LA(3))) && (_tokenSet_30.member(LA(4)))) {
						mUTF8_multibyte_char(false);
					}
					else if ((_tokenSet_38.member(LA(1)))) {
						{
						match(_tokenSet_38);
						}
					}
			else {
				goto _loop492;
			}
			}}
		}
		_loop492:;
		} // ( ... )*
		match("\"\"\"");
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDouble_quoted_string(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Double_quoted_string;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('\"' /* charlit */ );
		{ // ( ... )*
		for (;;) {
			if (((LA(1) >= 0xc2 && LA(1) <= 0xf4)) && ((LA(2) >= 0x80 && LA(2) <= 0xbf)) && (_tokenSet_30.member(LA(3))) && (true)) {
				mUTF8_multibyte_char(false);
			}
			else {
				bool synPredMatched481 = false;
				if (((_tokenSet_15.member(LA(1))) && (_tokenSet_37.member(LA(2))) && (true) && (true))) {
					int _m481 = mark();
					synPredMatched481 = true;
					inputState->guessing++;
					try {
						{
						mLine_break(false);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched481 = false;
					}
					rewind(_m481);
					inputState->guessing--;
				}
				if ( synPredMatched481 ) {
					mLine_break(false);
				}
				else if ((_tokenSet_39.member(LA(1)))) {
					{
					match(_tokenSet_39);
					}
				}
				else if ((LA(1) == 0x5c /* '\\' */ )) {
					mEscape_sequence(false);
				}
			else {
				goto _loop482;
			}
			}
		}
		_loop482:;
		} // ( ... )*
		match('\"' /* charlit */ );
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mEscape_sequence(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Escape_sequence;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		match('\\' /* charlit */ );
		{
		if ((_tokenSet_40.member(LA(1)))) {
			mEscaped_char(false);
		}
		else if ((LA(1) == 0x75 /* 'u' */ )) {
			match('u' /* charlit */ );
			mHex_digit(false);
			mHex_digit(false);
			mHex_digit(false);
			mHex_digit(false);
		}
		else if ((LA(1) == 0x55 /* 'U' */ )) {
			match('U' /* charlit */ );
			mBin_digit(false);
			mHex_digit(false);
			mHex_digit(false);
			mHex_digit(false);
			mHex_digit(false);
			mHex_digit(false);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mEscaped_char(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Escaped_char;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		switch ( LA(1)) {
		case 0x22 /* '\"' */ :
		{
			match('\"' /* charlit */ );
			break;
		}
		case 0x27 /* '\'' */ :
		{
			match('\'' /* charlit */ );
			break;
		}
		case 0x5c /* '\\' */ :
		{
			match('\\' /* charlit */ );
			break;
		}
		case 0x61 /* 'a' */ :
		{
			match('a' /* charlit */ );
			break;
		}
		case 0x62 /* 'b' */ :
		{
			match('b' /* charlit */ );
			break;
		}
		case 0x74 /* 't' */ :
		{
			match('t' /* charlit */ );
			break;
		}
		case 0x6e /* 'n' */ :
		{
			match('n' /* charlit */ );
			break;
		}
		case 0x76 /* 'v' */ :
		{
			match('v' /* charlit */ );
			break;
		}
		case 0x66 /* 'f' */ :
		{
			match('f' /* charlit */ );
			break;
		}
		case 0x72 /* 'r' */ :
		{
			match('r' /* charlit */ );
			break;
		}
		case 0x65 /* 'e' */ :
		{
			match('e' /* charlit */ );
			break;
		}
		case 0x30 /* '0' */ :
		{
			match('0' /* charlit */ );
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_19);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDec_integer(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Dec_integer;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mDec_digit(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_41.member(LA(1)))) {
				{
				if ((LA(1) == 0x5f /* '_' */ )) {
					match('_' /* charlit */ );
				}
				else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				
				}
				mDec_digit(false);
			}
			else {
				goto _loop503;
			}
			
		}
		_loop503:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_42);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDec_digit(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Dec_digit;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		matchRange('0','9');
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_43);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mBin_integer(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Bin_integer;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mBin_digit(false);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */  || LA(1) == 0x5f /* '_' */ )) {
				{
				if ((LA(1) == 0x5f /* '_' */ )) {
					match('_' /* charlit */ );
				}
				else if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				
				}
				mBin_digit(false);
			}
			else {
				goto _loop511;
			}
			
		}
		_loop511:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOct_integer(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Oct_integer;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mOct_digit(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_23.member(LA(1)))) {
				{
				if ((LA(1) == 0x5f /* '_' */ )) {
					match('_' /* charlit */ );
				}
				else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				
				}
				mOct_digit(false);
			}
			else {
				goto _loop519;
			}
			
		}
		_loop519:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_integer(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_integer;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		mHex_digit(false);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_21.member(LA(1)))) {
				{
				if ((LA(1) == 0x5f /* '_' */ )) {
					match('_' /* charlit */ );
				}
				else if ((_tokenSet_2.member(LA(1)))) {
				}
				else {
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				
				}
				mHex_digit(false);
			}
			else {
				goto _loop527;
			}
			
		}
		_loop527:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_44);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mDec_exponent(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Dec_exponent;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		{
		if ((LA(1) == 0x65 /* 'e' */ )) {
			match('e' /* charlit */ );
		}
		else if ((LA(1) == 0x45 /* 'E' */ )) {
			match('E' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
		{
		if ((LA(1) == 0x2b /* '+' */ )) {
			match('+' /* charlit */ );
		}
		else if ((LA(1) == 0x2d /* '-' */ )) {
			match('-' /* charlit */ );
		}
		else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
		mDec_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mHex_exponent(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Hex_exponent;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		{
		if ((LA(1) == 0x50 /* 'P' */ )) {
			match('P' /* charlit */ );
		}
		else if ((LA(1) == 0x70 /* 'p' */ )) {
			match('p' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
		{
		if ((LA(1) == 0x2b /* '+' */ )) {
			match('+' /* charlit */ );
		}
		else if ((LA(1) == 0x2d /* '-' */ )) {
			match('-' /* charlit */ );
		}
		else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
		mDec_integer(false);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mIdentifier_ascii(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Identifier_ascii;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if (((LA(1) >= 0x61 /* 'a' */  && LA(1) <= 0x7a /* 'z' */ ))) {
			matchRange('a','z');
		}
		else if (((LA(1) >= 0x41 /* 'A' */  && LA(1) <= 0x5a /* 'Z' */ ))) {
			matchRange('A','Z');
		}
		else if ((LA(1) == 0x5f /* '_' */ )) {
			match('_' /* charlit */ );
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_17);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mIdentifier_ascii_start(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Identifier_ascii_start;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if (((LA(1) >= 0x61 /* 'a' */  && LA(1) <= 0x7a /* 'z' */ ))) {
			matchRange('a','z');
		}
		else if (((LA(1) >= 0x41 /* 'A' */  && LA(1) <= 0x5a /* 'Z' */ ))) {
			matchRange('A','Z');
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_17);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mOperator_char(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = Operator_char;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
		if ((_tokenSet_45.member(LA(1)))) {
			mOperator_ascii_char(false);
		}
		else {
			bool synPredMatched565 = false;
			if (((((LA(1) >= 0xc2 && LA(1) <= 0xf4)))&&( is_operator_char(check_next_code_point()) ))) {
				int _m565 = mark();
				synPredMatched565 = true;
				inputState->guessing++;
				try {
					{
					mUTF8_lead_byte(false);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched565 = false;
				}
				rewind(_m565);
				inputState->guessing--;
			}
			if ( synPredMatched565 ) {
				mUTF8_multibyte_char(false);
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void lexer_arrow::mEOF(bool _createToken) {
	int _ttype; ANTLR_USE_NAMESPACE(antlr)RefToken _token; ANTLR_USE_NAMESPACE(std)string::size_type _begin = text.length();
	_ttype = ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE;
	ANTLR_USE_NAMESPACE(std)string::size_type _saveIndex;
	
	try {      // for error handling
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	if ( _createToken && _token==ANTLR_USE_NAMESPACE(antlr)nullToken && _ttype!=ANTLR_USE_NAMESPACE(antlr)Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long lexer_arrow::_tokenSet_0_data_[] = { 67122689UL, 4160744703UL, 3623878655UL, 1476395007UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x9 0xa 0xc 0xd 0x1a   ! \" # $ % & \' * + - . / 0 1 2 3 4 5 6 7 
// 8 9 : < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
// 0x5c ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long lexer_arrow::_tokenSet_1_data_[] = { 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_1(_tokenSet_1_data_,10);
const unsigned long lexer_arrow::_tokenSet_2_data_[] = { 0UL, 67043328UL, 126UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 A B C D E F a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_2(_tokenSet_2_data_,10);
const unsigned long lexer_arrow::_tokenSet_3_data_[] = { 0UL, 67059712UL, 2147549310UL, 65662UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 A B C D E F P _ a b c d e f p 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long lexer_arrow::_tokenSet_4_data_[] = { 0UL, 67059712UL, 2147483680UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 E _ e 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long lexer_arrow::_tokenSet_5_data_[] = { 0UL, 67069952UL, 2147483680UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// + - . 0 1 2 3 4 5 6 7 8 9 E _ e 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long lexer_arrow::_tokenSet_6_data_[] = { 4294953983UL, 4294967167UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # 
// $ % & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g 
// h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_6(_tokenSet_6_data_,16);
const unsigned long lexer_arrow::_tokenSet_7_data_[] = { 0UL, 65668UL, 270532608UL, 7618662UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \" \' 0 U 0x5c a b e f n r t u v 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_7(_tokenSet_7_data_,12);
const unsigned long lexer_arrow::_tokenSet_8_data_[] = { 0UL, 0UL, 0UL, 0UL, 4294967295UL, 4294967295UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_8(_tokenSet_8_data_,16);
const unsigned long lexer_arrow::_tokenSet_9_data_[] = { 0UL, 4093701370UL, 1342177281UL, 1342177281UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ! # $ % & \' * + - . / : < = > ? @ 0x5c ^ ` 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_9(_tokenSet_9_data_,16);
const unsigned long lexer_arrow::_tokenSet_10_data_[] = { 13824UL, 1UL, 0UL, 0UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x9 0xa 0xc 0xd   
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_10(_tokenSet_10_data_,16);
const unsigned long lexer_arrow::_tokenSet_11_data_[] = { 0UL, 0UL, 134217726UL, 134217726UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_11(_tokenSet_11_data_,16);
const unsigned long lexer_arrow::_tokenSet_12_data_[] = { 0UL, 1946184704UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// * + - . : < = > 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_12(_tokenSet_12_data_,10);
const unsigned long lexer_arrow::_tokenSet_13_data_[] = { 13824UL, 4093701371UL, 1476395007UL, 1476395007UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x9 0xa 0xc 0xd   ! # $ % & \' * + - . / : < = > ? @ A B C D E F G H 
// I J K L M N O P Q R S T U V W X Y Z 0x5c ^ ` a b c d e f g h i j k l 
// m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_13(_tokenSet_13_data_,16);
const unsigned long lexer_arrow::_tokenSet_14_data_[] = { 4227845118UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_14(_tokenSet_14_data_,16);
const unsigned long lexer_arrow::_tokenSet_15_data_[] = { 13312UL, 0UL, 0UL, 0UL, 0UL, 0UL, 4UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0xa 0xc 0xd 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_15(_tokenSet_15_data_,16);
const unsigned long lexer_arrow::_tokenSet_16_data_[] = { 0UL, 0UL, 134217726UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_16(_tokenSet_16_data_,10);
const unsigned long lexer_arrow::_tokenSet_17_data_[] = { 0UL, 67043328UL, 2281701374UL, 134217726UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X 
// Y Z _ a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_17(_tokenSet_17_data_,16);
const unsigned long lexer_arrow::_tokenSet_18_data_[] = { 0UL, 0UL, 2281701374UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z _ a b c d e f g 
// h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_18(_tokenSet_18_data_,10);
const unsigned long lexer_arrow::_tokenSet_19_data_[] = { 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > 
// ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ 
// ` a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_19(_tokenSet_19_data_,16);
const unsigned long lexer_arrow::_tokenSet_20_data_[] = { 0UL, 196608UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_20(_tokenSet_20_data_,10);
const unsigned long lexer_arrow::_tokenSet_21_data_[] = { 0UL, 67043328UL, 2147483774UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 A B C D E F _ a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_21(_tokenSet_21_data_,10);
const unsigned long lexer_arrow::_tokenSet_22_data_[] = { 0UL, 16711680UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_22(_tokenSet_22_data_,10);
const unsigned long lexer_arrow::_tokenSet_23_data_[] = { 0UL, 16711680UL, 2147483648UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 _ 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_23(_tokenSet_23_data_,10);
const unsigned long lexer_arrow::_tokenSet_24_data_[] = { 0UL, 67059712UL, 2147483774UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 A B C D E F _ a b c d e f 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_24(_tokenSet_24_data_,10);
const unsigned long lexer_arrow::_tokenSet_25_data_[] = { 0UL, 67043328UL, 2147549310UL, 65662UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 A B C D E F P _ a b c d e f p 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_25(_tokenSet_25_data_,10);
const unsigned long lexer_arrow::_tokenSet_26_data_[] = { 0UL, 67059712UL, 2147483648UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 _ 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_26(_tokenSet_26_data_,10);
const unsigned long lexer_arrow::_tokenSet_27_data_[] = { 0UL, 67043328UL, 2147483680UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 E _ e 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_27(_tokenSet_27_data_,10);
const unsigned long lexer_arrow::_tokenSet_28_data_[] = { 0UL, 67053568UL, 2147483680UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// + - 0 1 2 3 4 5 6 7 8 9 E _ e 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_28(_tokenSet_28_data_,10);
const unsigned long lexer_arrow::_tokenSet_29_data_[] = { 4294953983UL, 4294967167UL, 4026531839UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # 
// $ % & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ ] ^ _ ` a b c d e f g h i 
// j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_29(_tokenSet_29_data_,16);
const unsigned long lexer_arrow::_tokenSet_30_data_[] = { 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > 
// ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ 
// ` a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_30(_tokenSet_30_data_,16);
const unsigned long lexer_arrow::_tokenSet_31_data_[] = { 0UL, 0UL, 0UL, 0UL, 4294967295UL, 4294967295UL, 4294967291UL, 4294967291UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_31(_tokenSet_31_data_,16);
const unsigned long lexer_arrow::_tokenSet_32_data_[] = { 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294966527UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > 
// ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ 
// ` a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_32(_tokenSet_32_data_,16);
const unsigned long lexer_arrow::_tokenSet_33_data_[] = { 4227845118UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_33(_tokenSet_33_data_,16);
const unsigned long lexer_arrow::_tokenSet_34_data_[] = { 4294953983UL, 4294933503UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # 
// $ % & \' ( ) + , - . 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_34(_tokenSet_34_data_,16);
const unsigned long lexer_arrow::_tokenSet_35_data_[] = { 4294967295UL, 4294934527UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) * + , - . 0 1 2 3 4 5 6 7 8 9 : ; < = > ? 
// @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` 
// a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_35(_tokenSet_35_data_,16);
const unsigned long lexer_arrow::_tokenSet_36_data_[] = { 4294967295UL, 4294966271UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? 
// @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` 
// a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_36(_tokenSet_36_data_,16);
const unsigned long lexer_arrow::_tokenSet_37_data_[] = { 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 33UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 
// 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 
// 0x1f   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > 
// ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ 
// ` a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_37(_tokenSet_37_data_,16);
const unsigned long lexer_arrow::_tokenSet_38_data_[] = { 4294953983UL, 4294967291UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g 
// h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_38(_tokenSet_38_data_,16);
const unsigned long lexer_arrow::_tokenSet_39_data_[] = { 4294953983UL, 4294967291UL, 4026531839UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S T U V W X Y Z [ ] ^ _ ` a b c d e f g h i 
// j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_39(_tokenSet_39_data_,16);
const unsigned long lexer_arrow::_tokenSet_40_data_[] = { 0UL, 65668UL, 268435456UL, 5521510UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \" \' 0 0x5c a b e f n r t v 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_40(_tokenSet_40_data_,10);
const unsigned long lexer_arrow::_tokenSet_41_data_[] = { 0UL, 67043328UL, 2147483648UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 _ 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_41(_tokenSet_41_data_,10);
const unsigned long lexer_arrow::_tokenSet_42_data_[] = { 0UL, 16384UL, 32UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . E e 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_42(_tokenSet_42_data_,10);
const unsigned long lexer_arrow::_tokenSet_43_data_[] = { 0UL, 67059712UL, 2281701374UL, 134217726UL, 0UL, 0UL, 4294967292UL, 2097151UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W 
// X Y Z _ a b c d e f g h i j k l m n o p q r s t u v w x y z 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_43(_tokenSet_43_data_,16);
const unsigned long lexer_arrow::_tokenSet_44_data_[] = { 0UL, 16384UL, 65536UL, 65536UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . P p 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_44(_tokenSet_44_data_,10);
const unsigned long lexer_arrow::_tokenSet_45_data_[] = { 0UL, 4093701370UL, 1342177281UL, 1342177281UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ! # $ % & \' * + - . / : < = > ? @ 0x5c ^ ` 
const ANTLR_USE_NAMESPACE(antlr)BitSet lexer_arrow::_tokenSet_45(_tokenSet_45_data_,10);

