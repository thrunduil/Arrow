#ifndef INC_AST_hpp__
#define INC_AST_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: //depot/code/org.antlr/release/antlr-2.7.7/lib/cpp/antlr/AST.hpp#2 $
 */

#include <antlr/config.hpp>
#include <antlr/ASTRefCount.hpp>
#include <antlr/Token.hpp>
#include <vector>
#include <string>
#include <antlr/pool_parser.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API AST {
public:
	AST() {};
	AST(const AST&) {};
	virtual ~AST() {};

	/// Return the type name for this AST node. (for XML output)
	virtual const char* typeName( void ) const = 0;
	/// Clone this AST node.
	virtual RefAST clone( pool_parser pool ) = 0;
   /// Is node t equal to this in terms of token type and text?
	virtual bool equals(RefAST t) const = 0;
   /** Is t an exact structural and equals() match of this tree. The
	 * 'this' reference is considered the start of a sibling list.
	 */
	virtual bool equalsList(RefAST t)  = 0;

	/** Is 't' a subtree of this list? The siblings of the root are NOT ignored.
    */
	virtual bool equalsListPartial(RefAST t)  = 0;
	/** Is tree rooted at 'this' equal to 't'?  The siblings of 'this' are
	 * ignored.
	 */
	virtual bool equalsTree(RefAST t)  = 0;
	/** Is 't' a subtree of the tree rooted at 'this'? The siblings of
	 * 'this' are ignored.
	 */
	virtual bool equalsTreePartial(RefAST t)  = 0;

	/** Walk the tree looking for all exact subtree matches.  Return
	 *  a vector of RefAST that lets the caller walk the list
	 *  of subtree roots found herein.
	 */
	virtual std::vector<RefAST> findAll(RefAST t) = 0;

   /** Walk the tree looking for all subtrees.  Return
    *  a vector of RefAST that lets the caller walk the list
    *  of subtree roots found herein.
    */
	virtual std::vector<RefAST> findAllPartial(RefAST t) = 0;

   /// Add a node to the end of the child list for this node
	virtual void addChild(RefAST c) = 0;
	/// Get the number of children. Returns 0 if the node is a leaf
	virtual size_t getNumberOfChildren() const = 0;

	/// Get the first child of this node; null if no children
	virtual RefAST getFirstChild() = 0;
	/// Get  the next sibling in line after this one
	virtual RefAST getNextSibling() = 0;

	/// Get the token text for this node
    virtual std::string getText() const = 0;
	/// Get the token type for this node
	virtual int getType() const = 0;

	/** Various initialization routines. Used by several factories to initialize
	 * an AST element.
	 */
	virtual void initialize(int t, const std::string& txt) = 0;
	virtual void initialize(RefAST t) = 0;
	virtual void initialize(RefToken t) = 0;

#ifdef ANTLR_SUPPORT_XML
	/** initialize this node from the contents of a stream.
	 * @param in the stream to read the AST attributes from.
	 */
	virtual void initialize( ANTLR_USE_NAMESPACE(std)istream& in ) = 0;
#endif

	/// Set the first child of a node.
	virtual void setFirstChild(RefAST c) = 0;
	/// Set the next sibling after this one.
	virtual void setNextSibling(RefAST n) = 0;

	/// Set the token text for this node
	virtual void setText(const std::string& txt) = 0;
	/// Set the token type for this node
	virtual void setType(int type) = 0;

	/// Return this AST node as a string
    virtual std::string toString() const = 0;

	/// Print out a child-sibling tree in LISP notation
    virtual std::string toStringList()  = 0;
    virtual std::string toStringTree()  = 0;

#ifdef ANTLR_SUPPORT_XML
	/** get attributes of this node to 'out'. Override to customize XML
	 * output.
	 * @param out the stream to write the AST attributes to.
	 * @returns if a explicit closetag should be written
	 */
	virtual bool attributesToStream( ANTLR_USE_NAMESPACE(std)ostream& out ) const = 0;

	/** Print a symbol over ostream. Overload this one to customize the XML
	 * output for AST derived AST-types
	 * @param output stream
	 */
	virtual void toStream( ANTLR_USE_NAMESPACE(std)ostream &out ) const = 0;

	/** Dump AST contents in XML format to output stream.
	 * Works in conjunction with to_stream method. Overload that one is
	 * derived classes to customize behaviour.
	 * @param output stream to write to string to put the stuff in.
	 * @param ast RefAST object to write.
	 */
	friend ANTLR_USE_NAMESPACE(std)ostream& operator<<( ANTLR_USE_NAMESPACE(std)ostream& output, const RefAST& ast );
#endif

private:
	AST(RefAST other);
	AST& operator=(const AST& other);
	AST& operator=(RefAST other);
};

#ifdef ANTLR_SUPPORT_XML
inline ANTLR_USE_NAMESPACE(std)ostream& operator<<( ANTLR_USE_NAMESPACE(std)ostream& output, const RefAST& ast )
{
	ast->toStream(output);
	return output;
}
#endif

extern ANTLR_API RefAST nullAST;
extern ANTLR_API AST* const nullASTptr;

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_AST_hpp__
