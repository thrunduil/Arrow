#ifndef INC_TokenWithIndex_hpp__
#define INC_TokenWithIndex_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id:$
 */

#include <antlr/config.hpp>
#include <antlr/CommonToken.hpp>
#include <antlr/String.hpp>
#include <antlr/pool_parser.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API TokenWithIndex : public ANTLR_USE_NAMESPACE(antlr)CommonToken {
public:
	// static size_t count;
	TokenWithIndex() : CommonToken(), index(0)
	{
		// std::cout << __PRETTY_FUNCTION__ << std::endl;
		// count++;
	}
	TokenWithIndex(int t, const std::string& txt)
	: CommonToken(t,txt)
	, index(0)
	{
		// std::cout << __PRETTY_FUNCTION__ << std::endl;
		// count++;
	}
	TokenWithIndex(const std::string& s)
	: CommonToken(s)
	, index(0)
	{
		// std::cout << __PRETTY_FUNCTION__ << std::endl;
		// count++;
	}
	~TokenWithIndex()
	{
		// count--;
	}
	void setIndex( size_t idx )
	{
		index = idx;
	}
	size_t getIndex( void ) const
	{
		return index;
	}

	std::string toString() const
	{
		return std::string("[")+
			index+
			":\""+
			getText()+"\",<"+
			getType()+">,line="+
			getLine()+",column="+
			getColumn()+"]";
	}

	static RefToken factory(pool_parser pool)
	{
		return RefToken(new(*pool,get_ptr<TokenWithIndex>()) TokenWithIndex());
	}

protected:
	size_t index;

private:
	TokenWithIndex(const TokenWithIndex&);
	const TokenWithIndex& operator=(const TokenWithIndex&);
};

typedef TokenRefCount<TokenWithIndex> RefTokenWithIndex;

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_CommonToken_hpp__
