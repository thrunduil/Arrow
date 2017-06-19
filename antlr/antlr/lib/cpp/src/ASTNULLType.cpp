/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id:$
 */

#include "antlr/config.hpp"
#include "antlr/AST.hpp"
#include "antlr/ASTNULLType.hpp"

#include <iostream>

ANTLR_USING_NAMESPACE(std)

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

RefAST ASTNULLType::clone(pool_parser pool)
{
	return RefAST(this);
}

void ASTNULLType::addChild( RefAST )
{
}

size_t ASTNULLType::getNumberOfChildren() const
{
	return 0;
}

bool ASTNULLType::equals(RefAST ) const
{
	return false;
}

bool ASTNULLType::equalsList(RefAST )
{
	return false;
}

bool ASTNULLType::equalsListPartial(RefAST )
{
	return false;
}

bool ASTNULLType::equalsTree(RefAST )
{
	return false;
}

bool ASTNULLType::equalsTreePartial(RefAST )
{
	return false;
}

vector<RefAST> ASTNULLType::findAll(RefAST )
{
	return vector<RefAST>();
}

vector<RefAST> ASTNULLType::findAllPartial(RefAST )
{
	return vector<RefAST>();
}

RefAST ASTNULLType::getFirstChild()
{
	return RefAST(this);
}

RefAST ASTNULLType::getNextSibling()
{
	return RefAST(this);
}

string ASTNULLType::getText() const
{
	return "<ASTNULL>";
}

int ASTNULLType::getType() const
{
	return Token::NULL_TREE_LOOKAHEAD;
}

void ASTNULLType::initialize( int, const std::string& )
{
}

void ASTNULLType::initialize( RefAST )
{
}

void ASTNULLType::initialize( RefToken )
{
}

#ifdef ANTLR_SUPPORT_XML
void ASTNULLType::initialize( istream& )
{
}
#endif

void ASTNULLType::setFirstChild(RefAST )
{
}

void ASTNULLType::setNextSibling(RefAST )
{
}

void ASTNULLType::setText( const std::string& )
{
}

void ASTNULLType::setType( int )
{
}

string ASTNULLType::toString() const
{
	return getText();
}

string ASTNULLType::toStringList()
{
	return getText();
}

string ASTNULLType::toStringTree()
{
	return getText();
}

#ifdef ANTLR_SUPPORT_XML
bool ASTNULLType::attributesToStream( ostream& ) const
{
	return false;
}

void ASTNULLType::toStream( ostream& out ) const
{
	out << "</ASTNULL>" << endl;
}
#endif

const char* ASTNULLType::typeName( void ) const
{
	return "ASTNULLType";
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif
