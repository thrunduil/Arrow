/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/src/LLkParser.cpp#2 $
 */

#include "antlr/LLkParser.hpp"
#include <iostream>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

/**An LL(k) parser.
 *
 * @see antlr.Token
 * @see antlr.TokenBuffer
 * @see antlr.LL1Parser
 */

//	LLkParser(int k_);

LLkParser::LLkParser(pool_parser pool,const ParserSharedInputState& state, int k_)
: Parser(pool,state), k(k_)
{
}

LLkParser::LLkParser(pool_parser pool,TokenBuffer& tokenBuf, int k_)
: Parser(pool,tokenBuf), k(k_)
{
}

LLkParser::LLkParser(pool_parser pool,TokenStream& lexer, int k_)
: Parser(pool,new(*pool,get_ptr<TokenBuffer>()) TokenBuffer(lexer)), k(k_)
{
}

void LLkParser::trace(const char* ee, const char* rname)
{
	traceIndent();

	std::cout << ee << rname << ((inputState->guessing>0)?"; [guessing]":"; ");

	for (int i = 1; i <= k; i++)
	{
		if (i != 1) {
			std::cout << ", ";
		}
		std::cout << "LA(" << i << ")==";

		std::string temp;

		try {
			temp = LT(i)->getText().c_str();
		}
		catch( ANTLRException& ae )
		{
			temp = "[error: ";
			temp += ae.toString();
			temp += ']';
		}
		std::cout << temp;
	}

	std::cout << std::endl;
}

void LLkParser::traceIn(const char* rname)
{
	traceDepth++;
	trace("> ",rname);
}

void LLkParser::traceOut(const char* rname)
{
	trace("< ",rname);
	traceDepth--;
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif
