#ifndef INC_RefCount_hpp__
#define INC_RefCount_hpp__
/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/antlr/RefCount.hpp#2 $
 */

#include <antlr/config.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

template<class T>
class ANTLR_API RefCount 
{
	private:
		T*	ptr;

	public:		
		RefCount()								: ptr(NULL)	{ };
		explicit RefCount(T* p)					: ptr(p)	{ };
		RefCount(const RefCount<T>& other)		: ptr(other.ptr) {};
		~RefCount(){}
		RefCount<T>& operator=(const RefCount<T>& other)
		{
			ptr = other.ptr;
			return *this;
		}

		operator T* () const					{ return ptr;}

		T* operator->() const					{ return ptr;}
		T* get() const							{ return ptr; }

		template<class newType> operator RefCount<newType>()
		{
			return RefCount<newType>(ptr);
		}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_RefCount_hpp__
