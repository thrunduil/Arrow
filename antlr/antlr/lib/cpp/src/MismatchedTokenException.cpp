/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/src/MismatchedTokenException.cpp#2 $
 */

#include "antlr/MismatchedTokenException.hpp"
#include "antlr/String.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

MismatchedTokenException::MismatchedTokenException()
  : RecognitionException("Mismatched Token: expecting any AST node","<AST>",-1,-1)
  , token()
  , node(nullASTptr)
  , tokenNames(0)
  , numTokens(0)
{
}

MismatchedTokenException::MismatchedTokenException(const MismatchedTokenException& o)
:RecognitionException(o),token(o.token),node(o.node),tokenText(o.tokenText),mismatchType(o.mismatchType),
expecting(o.expecting),upper(o.upper),set(o.set),tokenNames(o.tokenNames),numTokens(o.numTokens)
{};

// Expected range / not range
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefAST node_,
	int lower,
	int upper_,
	bool matchNot
) : RecognitionException("Mismatched Token","<AST>",-1,-1)
  , token()
  , node(node_)
  , tokenText( (node_ ? node_->toString(): std::string("<empty tree>")) )
  , mismatchType(matchNot ? NOT_RANGE : RANGE)
  , expecting(lower)
  , upper(upper_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

// Expected token / not token
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefAST node_,
	int expecting_,
	bool matchNot
) : RecognitionException("Mismatched Token","<AST>",-1,-1)
  , token()
  , node(node_)
  , tokenText( (node_ ? node_->toString(): std::string("<empty tree>")) )
  , mismatchType(matchNot ? NOT_TOKEN : TOKEN)
  , expecting(expecting_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

// Expected BitSet / not BitSet
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefAST node_,
	BitSet set_,
	bool matchNot
) : RecognitionException("Mismatched Token","<AST>",-1,-1)
  , token()
  , node(node_)
  , tokenText( (node_ ? node_->toString(): std::string("<empty tree>")) )
  , mismatchType(matchNot ? NOT_SET : SET)
  , set(set_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

// Expected range / not range
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefToken token_,
	int lower,
	int upper_,
	bool matchNot,
	const std::string& fileName_
) : RecognitionException("Mismatched Token",fileName_,token_->getLine(),token_->getColumn())
  , token(token_)
  , node(nullASTptr)
  , tokenText(token_->getText())
  , mismatchType(matchNot ? NOT_RANGE : RANGE)
  , expecting(lower)
  , upper(upper_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

// Expected token / not token
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefToken token_,
	int expecting_,
	bool matchNot,
	const std::string& fileName_
) : RecognitionException("Mismatched Token",fileName_,token_->getLine(),token_->getColumn())
  , token(token_)
  , node(nullASTptr)
  , tokenText(token_->getText())
  , mismatchType(matchNot ? NOT_TOKEN : TOKEN)
  , expecting(expecting_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

// Expected BitSet / not BitSet
MismatchedTokenException::MismatchedTokenException(
	const char* const* tokenNames_,
	const int numTokens_,
	RefToken token_,
	BitSet set_,
	bool matchNot,
	const std::string& fileName_
) : RecognitionException("Mismatched Token",fileName_,token_->getLine(),token_->getColumn())
  , token(token_)
  , node(nullASTptr)
  , tokenText(token_->getText())
  , mismatchType(matchNot ? NOT_SET : SET)
  , set(set_)
  , tokenNames(tokenNames_)
  , numTokens(numTokens_)
{
}

std::string MismatchedTokenException::getMessage() const
{
	std::string s;
	switch (mismatchType) {
	case TOKEN:
		s += "expecting " + tokenName(expecting) + ", found \"" + tokenText + "\"";
		break;
	case NOT_TOKEN:
		s += "expecting anything but " + tokenName(expecting) + "; got it anyway";
		break;
	case RANGE:
		s += "expecting token in range: " + tokenName(expecting) + ".." + tokenName(upper) + ", found \"" + tokenText + "\"";
		break;
	case NOT_RANGE:
		s += "expecting token NOT in range: " + tokenName(expecting) + ".." + tokenName(upper) + ", found \"" + tokenText + "\"";
		break;
	case SET:
	case NOT_SET:
		{
			s += std::string("expecting ") + (mismatchType == NOT_SET ? "NOT " : "") + "one of (";
			std::vector<unsigned int> elems = set.toArray();
			for ( unsigned int i = 0; i < elems.size(); i++ )
			{
				s += " ";
				s += tokenName(elems[i]);
			}
			s += "), found \"" + tokenText + "\"";
		}
		break;
	default:
		s = RecognitionException::getMessage();
		break;
	}
	return s;
}

std::string MismatchedTokenException::tokenName(int tokenType) const
{
	if (tokenType == Token::INVALID_TYPE)
		return "<Set of tokens>";
	else if (tokenType < 0 || tokenType >= numTokens)
		return std::string("<") + tokenType + ">";
	else
		return tokenNames[tokenType];
}


#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif
