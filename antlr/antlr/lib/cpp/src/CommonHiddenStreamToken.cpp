/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/src/CommonHiddenStreamToken.cpp#2 $
 */
#include "antlr/CommonHiddenStreamToken.hpp"
#include "antlr/pool_parser.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

CommonHiddenStreamToken::CommonHiddenStreamToken()
: CommonToken()
{
}

CommonHiddenStreamToken::CommonHiddenStreamToken(int t, const std::string& txt)
: CommonToken(t,txt)
{
}

CommonHiddenStreamToken::CommonHiddenStreamToken(const std::string& s)
: CommonToken(s)
{
}

RefToken CommonHiddenStreamToken::getHiddenAfter()
{
	return hiddenAfter;
}

RefToken CommonHiddenStreamToken::getHiddenBefore()
{
	return hiddenBefore;
}

RefToken CommonHiddenStreamToken::factory(pool_parser pool)
{
	return RefToken(new(*pool,get_ptr<CommonHiddenStreamToken>()) CommonHiddenStreamToken);
}

void CommonHiddenStreamToken::setHiddenAfter(RefToken t)
{
	hiddenAfter = t;
}

void CommonHiddenStreamToken::setHiddenBefore(RefToken t)
{
	hiddenBefore = t;
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

