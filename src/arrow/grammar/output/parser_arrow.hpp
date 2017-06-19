#ifndef INC_parser_arrow_hpp_
#define INC_parser_arrow_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "arrow.g" -> "parser_arrow.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "arrow_vocabularyTokenTypes.hpp"
#include <antlr/LLkParser.hpp>


    //parser.hpp includes
    #include "grammar/parser_include.h"

class CUSTOM_API parser_arrow : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public arrow_vocabularyTokenTypes
{
	
    //parser members
    #include "grammar/parser_members.h"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	parser_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	parser_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	parser_arrow(antlr::pool_parser pool, ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	parser_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	parser_arrow(antlr::pool_parser pool, const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return parser_arrow::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return parser_arrow::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return parser_arrow::tokenNames;
	}
	public: ast::module_definition  toplevel();
	public: ast::module_definition  module(
		bool is_shell
	);
	public: ast::module_definition  toplevel_shell();
	public: std::string  module_name();
	public: void imports(
		std::vector<string_loc>& imp
	);
	public: void command(
		ast::definitions& defs
	);
	public: void string_identifier(
		std::string& x
	);
	public: parser_arrow::string_loc  import();
	public: void import_name(
		std::string& x
	);
	public: void kind_abstraction(
		std::vector<ast::identifier>& vars
	);
	public: ast::term_definition  term_definition(
		const kind_id_vec& vars
	);
	public: ast::definition  type_definition(
		const kind_id_vec& vars
	);
	public: ast::definition  term_use_definition(
		const kind_id_vec& vars
	);
	public: ast::definition  operator_declaration();
	public: ast::term  term();
	public: ast::identifier  symbol_name();
	public: ast::type  type_annot_id();
	public: ast::term_definition  term_function_def(
		const ast::identifier& id, const kind_id_vec& vars, bool over
	);
	public: ast::annotated_identifier  annotated_identifier_or_under();
	public: ast::type  type();
	public: ast::kind  kind_annot_id();
	public: ast::definition  type_function_def(
		const ast::identifier& id, const kind_id_vec& vars, bool over
	);
	public: ast::polarity_type  kind_arrow_prefix();
	public: ast::annotated_type_identifier  annotated_type_identifier();
	public: ast::kind  kind();
	public: void operator_property(
		ast::operator_property& prop
	);
	public: void symbol_sequence(
		ast::symbol_sequence& seq
	);
	public: ast::identifier  operator_definition_name();
	public: ast::identifier  basic_identifier(
		bool build
	);
	public: ast::identifier  operator_atom();
	public: ast::identifier  operator_bracket_dolar_atom();
	public: ast::kind  kind_annot_type();
	public: ast::kind  arrow_kind();
	public: ast::kind  kind_atom();
	public: ast::kind  star_kind();
	public: ast::kind  kind_var();
	public: ast::kind  product_kind();
	public: ast::kind  value_kind();
	public: ast::identifier  kind_identifier();
	public: ast::kind  detailed_star_kind();
	public: ast::kind  star_kind_all();
	public: ast::kind  star_kind_some();
	public: ast::kind  star_kind_arrow();
	public: ast::kind  star_kind_prefix();
	public: ast::kind  star_kind_rref();
	public: ast::kind  star_kind_atom();
	public: ast::kind  star_kind_atom_predefined();
	public: ast::kind  star_kind_atom_record();
	public: ast::kind  star_kind_atom_variant();
	public: ast::kind  star_kind_atom_tuple();
	public: ast::kind  star_kind_atom_list();
	public: void star_kind_atom_record_field(
		ast::star_record_kind& rk
	);
	public: void star_kind_atom_record_field_var(
		ast::star_variant_kind& rk
	);
	public: ast::type  type_annot_term();
	public: ast::identifier  type_identifier(
		bool build
	);
	public: ast::type  all_type();
	public: ast::some_type  some_type();
	public: ast::type  lambda_type();
	public: ast::type  mu_type();
	public: ast::type  if_type();
	public: ast::type  arrow_type();
	public: ast::type  type_applic_simple();
	public: ast::type  type_applic_general();
	public: void prefix_type_simple(
		type_collector& col
	);
	public: void type_applic_tail_simple(
		type_collector& col
	);
	public: void prefix_type_general(
		type_collector& col
	);
	public: void type_applic_tail_general(
		type_collector& col
	);
	public: ast::type  rref_type_simple();
	public: void type_path_predicate();
	public: void type_path_identifier(
		type_collector& col
	);
	public: void type_postfix_predicate();
	public: void postfix_type_simple(
		type_collector& col
	);
	public: ast::type  rref_type_general();
	public: void postfix_type_general(
		type_collector& col
	);
	public: ast::type  prefix_type_general_ret();
	public: ast::type  prefix_type_simple_ret();
	public: void atomic_type_simple(
		type_collector& col
	);
	public: void type_postfix_item(
		type_collector& col
	);
	public: void atomic_type_general(
		type_collector& col
	);
	public: antlr::RefToken  opening_bracket_type();
	public: ast::int_literal  atom_int(
		bool build
	);
	public: void type_type_app(
		type_collector& col
	);
	public: void type_kind_applic(
		type_collector& col
	);
	public: ast::tuple_type  tuple_type();
	public: ast::bracket_type  bracket_type();
	public: void type_projection(
		type_collector& col
	);
	public: ast::type  predefined_types();
	public: ast::type  record_type();
	public: ast::type  variant_type();
	public: ast::type  list_type();
	public: ast::type  product_of_types();
	public: ast::type  value_type();
	public: ast::identifier  symbol_name_atom();
	public: void symbol_name_predicate();
	public: ast::type  symbol_name_type();
	public: ast::type  annotated_type();
	public: antlr::RefToken  closing_bracket_type();
	public: void field_type(
		ast::record_type& rt
	);
	public: void field_type_var(
		ast::variant_type& vt
	);
	public: ast::bool_literal  atom_bool();
	public: ast::char_literal  atom_char();
	public: ast::string_literal  atom_string();
	public: ast::identifier  term_identifier(
		bool build
	);
	public: ast::annotated_identifier  possibly_annotated_identifier();
	public: ast::annotated_identifier  annotated_identifier();
	public: ast::term  annotated_term();
	public: ast::term  app_term();
	public: ast::term  lambda_term();
	public: ast::term  mu_term();
	public: ast::term  all_term();
	public: ast::term  if_term();
	public: ast::term  variant_case_term();
	public: ast::term  pack_term();
	public: ast::term  unpack_term();
	public: ast::term  let_term();
	public: void patterns(
		ast::variant_case_term& x
	);
	public: void pattern(
		ast::variant_case_term& x
	);
	public: ast::definition  local_definition(
		const kind_id_vec& vars
	);
	public: void binds(
		ast::definitions& defs
	);
	public: void prefix_term(
		term_collector& col
	);
	public: void app_term_tail(
		term_collector& col
	);
	public: ast::term  prefix_term_ret();
	public: ast::term  ref_term();
	public: ast::term  deref_term();
	public: void path_predicate();
	public: void path_identifier(
		term_collector& col
	);
	public: void postfix_predicate();
	public: void postfix(
		term_collector& col
	);
	public: void postfix_term(
		term_collector& col
	);
	public: void atomic_term(
		term_collector& col
	);
	public: antlr::RefToken  opening_bracket();
	public: void term_type_app(
		term_collector& col
	);
	public: ast::bracket_term  bracket_term();
	public: ast::tuple_term  tuple_term();
	public: void term_projection(
		term_collector& col
	);
	public: void term_kind_applic(
		term_collector& col
	);
	public: ast::term  record_term();
	public: ast::term  variant_term();
	public: ast::term  list_term();
	public: ast::term  val_term();
	public: ast::term  literals();
	public: ast::term  symbol_name_term();
	public: antlr::RefToken  closing_bracket();
	public: void field_term(
		ast::record_term& x
	);
	public: ast::term  atom_real();
	public: ast::identifier  root_symbol_name();
	public: void operator_atom_1(
		std::string& name, bool& can_cont
	);
	public: void predefined_operator(
		std::string& name, bool& can_cont
	);
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 123;
#else
	enum {
		NUM_TOKENS = 123
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_17;
	static const unsigned long _tokenSet_18_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_18;
	static const unsigned long _tokenSet_19_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_19;
	static const unsigned long _tokenSet_20_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_20;
	static const unsigned long _tokenSet_21_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_21;
	static const unsigned long _tokenSet_22_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_22;
	static const unsigned long _tokenSet_23_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_23;
	static const unsigned long _tokenSet_24_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_24;
	static const unsigned long _tokenSet_25_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_25;
	static const unsigned long _tokenSet_26_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_26;
	static const unsigned long _tokenSet_27_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_27;
	static const unsigned long _tokenSet_28_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_28;
	static const unsigned long _tokenSet_29_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_29;
	static const unsigned long _tokenSet_30_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_30;
	static const unsigned long _tokenSet_31_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_31;
	static const unsigned long _tokenSet_32_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_32;
	static const unsigned long _tokenSet_33_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_33;
	static const unsigned long _tokenSet_34_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_34;
	static const unsigned long _tokenSet_35_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_35;
	static const unsigned long _tokenSet_36_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_36;
};

#endif /*INC_parser_arrow_hpp_*/
