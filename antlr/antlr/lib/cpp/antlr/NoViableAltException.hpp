#ifndef INC_NoViableAltException_hpp__
#define INC_NoViableAltException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/antlr/NoViableAltException.hpp#2 $
 */

#include <antlr/config.hpp>
#include <antlr/RecognitionException.hpp>
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API NoViableAltException : public RecognitionException {
public:
	const RefToken token;
	const RefAST node; // handles parsing and treeparsing

	NoViableAltException(RefAST t);
	NoViableAltException(RefToken t,const std::string& fileName_);

	~NoViableAltException() throw() {}

	/**
	 * Returns a clean error message (no line number/column information)
	 */
	std::string getMessage() const;

    NoViableAltException(const NoViableAltException& o);
private:    
    NoViableAltException& operator=(const NoViableAltException&);
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_NoViableAltException_hpp__
