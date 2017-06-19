#ifndef INC_TokenRefCount_hpp__
# define INC_TokenRefCount_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id:$
 */

# include <antlr/config.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif


class Token;

template<class T> class ANTLR_API TokenRefCount
{
	private:

		Token* ref;		

		template<class Y> friend class TokenRefCount;

	public:
		TokenRefCount()									: ref(0) { }
		TokenRefCount(Token* p)							: ref(p) { }
		TokenRefCount(const TokenRefCount<T>& other)	: ref(other.ref) { };
		~TokenRefCount()								{};
		TokenRefCount<T>& set(Token* other)
		{
			ref=other;
			return *this;
		}
		TokenRefCount<T>& operator=(const TokenRefCount<T>& other)
		{
			ref=other.ref;
			return *this;
		}
		template<typename Y>
		TokenRefCount<T>& operator=(const TokenRefCount<Y>& other)
		{
			ref=other.ref;
			return *this;
		}

		operator T* ()  const { return static_cast<T*>(ref); }
		T* operator->() const { return static_cast<T*>(ref); }
		T* get()        const { return static_cast<T*>(ref); }
};

typedef TokenRefCount<Token> RefToken;

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_TokenRefCount_hpp__
