#ifndef INC_ASTRefCount_hpp__
# define INC_ASTRefCount_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/antlr/ASTRefCount.hpp#2 $
 */

# include <antlr/config.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class AST;

template<class T> class ANTLR_API ASTRefCount
{
	private:
		AST* ref;

		template<class Y> friend class ASTRefCount;

	public:
		ASTRefCount()								: ref(0) { }
		ASTRefCount(AST* p)							: ref(p) { }
		ASTRefCount(const ASTRefCount<T>& other)	: ref(other.ref) {}
		~ASTRefCount()								{};
		ASTRefCount<T>& set(AST* other)
		{
			ref=other;
			return *this;
		}
		ASTRefCount<T>& operator=(const ASTRefCount<T>& other)
		{
			ref=other.ref;
			return *this;
		}
		template<typename Y>
		ASTRefCount<T>& operator=(const ASTRefCount<Y>& other)
		{
			ref=other.ref;
			return *this;
		}

		operator T* ()  const { return static_cast<T*>(ref); }
		T* operator->() const { return static_cast<T*>(ref); }
		T* get()        const { return static_cast<T*>(ref); }
};

typedef ASTRefCount<AST> RefAST;

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_ASTRefCount_hpp__
