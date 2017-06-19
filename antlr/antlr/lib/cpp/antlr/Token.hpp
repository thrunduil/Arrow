#ifndef INC_Token_hpp__
#define INC_Token_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/antlr/Token.hpp#2 $
 */

#include <antlr/config.hpp>
#include <antlr/TokenRefCount.hpp>
#include <string>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

/** A token is minimally a token type.  Subclasses can add the text matched
 *  for the token and line info.
 */
class ANTLR_API Token
{
public:
	// constants
	enum {
		MIN_USER_TYPE = 4,
		NULL_TREE_LOOKAHEAD = 3,
		INVALID_TYPE = 0,
		EOF_TYPE = 1,
		SKIP = -1
	};

	Token()
	: type(INVALID_TYPE)
	{
	}
	Token(int t)
	: type(t)
	{
	}
	Token(int t, const std::string& txt)
	: type(t)
	{
		setText(txt);
	}
	virtual ~Token()
	{
	}

	virtual int getColumn() const;
	virtual int getLine() const;
    virtual std::string getText() const;
	virtual const std::string& getFilename() const;
	virtual int getType() const;

	virtual void setColumn(int c);

	virtual void setLine(int l);
	virtual void setText(const std::string& t);
	virtual void setType(int t);

	virtual void setFilename( const std::string& file );

	virtual std::string toString() const;

private:
	int type; 							///< the type of the token

	Token(RefToken other);
	Token& operator=(const Token& other);
	Token& operator=(RefToken other);

	Token(const Token&);
};

extern ANTLR_API RefToken nullToken;

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_Token_hpp__
