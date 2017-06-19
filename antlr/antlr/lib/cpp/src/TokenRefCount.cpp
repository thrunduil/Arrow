/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id:$
 */
#include "antlr/TokenRefCount.hpp"
#include "antlr/Token.hpp"
#include "antlr/pool_parser.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

template<class T>
TokenRefCount<T>::TokenRef::TokenRef(Token* p)
: ptr(p)
{
	if (p && !p->ref)
	{
		p->ref = this;
	};
}
template<class T>
TokenRefCount<T>::TokenRef::~TokenRef()
{
}
template<class T>
typename TokenRefCount<T>::TokenRef* TokenRefCount<T>::TokenRef::getRef(pool_parser pool,Token* pp)
{
	if (pp)
	{
		if (pp->ref)
		{
			return pp->ref;
		}
		else
		{
			return new(pool,pool->mark<TokenRef>()) TokenRef(pp);
		};
	}
	else
	{
		return 0;
	};
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

