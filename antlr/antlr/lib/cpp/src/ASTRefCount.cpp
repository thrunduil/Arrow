/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/src/ASTRefCount.cpp#2 $
 */
#include "antlr/ASTRefCount.hpp"
#include "antlr/AST.hpp"
#include "antlr/pool_parser.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

ASTRef::ASTRef(AST* p)
: ptr(p)
{
	if (p && !p->ref)
	{
		p->ref = this;
	};
}

ASTRef::~ASTRef()
{
}

ASTRef* ASTRef::getRef(pool_parser pool,AST* pp)
{
	if (pp)
	{
		if (pp->ref)
		{
			return pp->ref;
		}
		else
		{
			return new(pool,pool->mark<ASTRef>()) ASTRef(pp);
		}
	}
	else
	{
		return 0;
	};
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

