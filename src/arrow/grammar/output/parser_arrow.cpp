/* $ANTLR 2.7.7 (2006-11-01): "arrow.g" -> "parser_arrow.cpp"$ */
#include "parser_arrow.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>

//parser.cpp includes

parser_arrow::parser_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,tokenBuf,k)
{
}

parser_arrow::parser_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,tokenBuf,1)
{
}

parser_arrow::parser_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,lexer,k)
{
}

parser_arrow::parser_arrow(antlr::pool_parser pool,ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,lexer,1)
{
}

parser_arrow::parser_arrow(antlr::pool_parser pool,const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(pool,state,1)
{
}

ast::module_definition  parser_arrow::toplevel() {
	ast::module_definition defs;
	
	try {      // for error handling
		defs=module(false);
	}
	catch (unwind_all& e) {
		if (inputState->guessing==0) {
		} else {
			throw;
		}
	}
	return defs;
}

ast::module_definition  parser_arrow::module(
	bool is_shell
) {
	ast::module_definition x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc    = get_location();
	std::string name;
	std::vector<string_loc> imp;
	
	ast::definitions defs = make_definitions(loc);
	
	
	try {      // for error handling
		{
		if (((_tokenSet_0.member(LA(1))))&&(is_shell == false)) {
			{
			if ((LA(1) == TOK_module)) {
				try { // for error handling
					name=module_name();
				}
				catch (antlr::RecognitionException& e) {
					if (inputState->guessing==0) {
						reportError(e); consume_until_semi();
					} else {
						throw;
					}
				}
				try { // for error handling
					s3 = LT(1);
					match(TOK_semi);
				}
				catch (antlr::RecognitionException& e) {
					if (inputState->guessing==0) {
						reportError(e);
					} else {
						throw;
					}
				}
			}
			else if ((_tokenSet_1.member(LA(1)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			imports(imp);
		}
		else if ((_tokenSet_2.member(LA(1)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				try {      // for error handling
					{
					if ((_tokenSet_4.member(LA(1)))) {
						try { // for error handling
							command(defs);
						}
						catch (antlr::RecognitionException& e) {
							if (inputState->guessing==0) {
								reportError(e); consume_until_semi();
							} else {
								throw;
							}
						}
						try { // for error handling
							s1 = LT(1);
							match(TOK_semi);
						}
						catch (antlr::RecognitionException& e) {
							if (inputState->guessing==0) {
								reportError(e);
							} else {
								throw;
							}
						}
					}
					else if ((LA(1) == TOK_semi)) {
						try { // for error handling
							s2 = LT(1);
							match(TOK_semi);
						}
						catch (antlr::RecognitionException& e) {
							if (inputState->guessing==0) {
								reportError(e);
							} else {
								throw;
							}
						}
					}
					else {
						throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
				catch (unwind_until_command& e) {
					if (inputState->guessing==0) {
					} else {
						throw;
					}
				}
			}
			else {
				goto _loop8;
			}
			
		}
		_loop8:;
		} // ( ... )*
		match(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
		if ( inputState->guessing==0 ) {
			x = make_module_definition(name, imp, defs);
		}
	}
	catch (antlr::RecognitionException& e) {
		if (inputState->guessing==0) {
			reportError(e);
		} else {
			throw;
		}
	}
	return x;
}

ast::module_definition  parser_arrow::toplevel_shell() {
	ast::module_definition defs;
	
	try {      // for error handling
		defs=module(true);
	}
	catch (unwind_all& e) {
		if (inputState->guessing==0) {
		} else {
			throw;
		}
	}
	return defs;
}

std::string  parser_arrow::module_name() {
	std::string x;
	
	std::string mod;
	
	
	match(TOK_module);
	string_identifier(x);
	return x;
}

void parser_arrow::imports(
	std::vector<string_loc>& imp
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  s1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	string_loc id;
	
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_import)) {
			try { // for error handling
				id=import();
			}
			catch (antlr::RecognitionException& e) {
				if (inputState->guessing==0) {
					reportError(e); consume_until_semi();
				} else {
					throw;
				}
			}
			try { // for error handling
				s1 = LT(1);
				match(TOK_semi);
			}
			catch (antlr::RecognitionException& e) {
				if (inputState->guessing==0) {
					reportError(e);
				} else {
					throw;
				}
			}
			if ( inputState->guessing==0 ) {
				imp.push_back(id);
			}
		}
		else if ((LA(1) == TOK_semi)) {
			try { // for error handling
				s2 = LT(1);
				match(TOK_semi);
			}
			catch (antlr::RecognitionException& e) {
				if (inputState->guessing==0) {
					reportError(e);
				} else {
					throw;
				}
			}
		}
		else {
			goto _loop12;
		}
		
	}
	_loop12:;
	} // ( ... )*
}

void parser_arrow::command(
	ast::definitions& defs
) {
	
	ast::definition d;
	std::vector<ast::identifier> vars;
	
	
	{
	if ((LA(1) == TOK_kind)) {
		kind_abstraction(vars);
	}
	else if ((_tokenSet_5.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	if ((_tokenSet_6.member(LA(1)))) {
		d=term_definition(vars);
		if ( inputState->guessing==0 ) {
			defs.add(d);
		}
	}
	else if ((LA(1) == TOK_type)) {
		d=type_definition(vars);
		if ( inputState->guessing==0 ) {
			defs.add(d);
		}
	}
	else if ((LA(1) == TOK_val)) {
		d=term_use_definition(vars);
		if ( inputState->guessing==0 ) {
			defs.add(d);
		}
	}
	else if ((LA(1) == TOK_operator_declaration)) {
		d=operator_declaration();
		if ( inputState->guessing==0 ) {
			defs.add(d);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::string_identifier(
	std::string& x
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	i1 = LT(1);
	match(TOK_identifier);
	if ( inputState->guessing==0 ) {
		x = get_text(i1);
	}
}

parser_arrow::string_loc  parser_arrow::import() {
	parser_arrow::string_loc x;
	
	std::string import, import2;
	location loc = get_location();
	
	
	match(TOK_import);
	import_name(import);
	{
	if ((LA(1) == TOK_as)) {
		match(TOK_as);
		string_identifier(import2);
	}
	else if ((LA(1) == TOK_semi)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		x = make_import(import, import2, loc);
	}
	return x;
}

void parser_arrow::import_name(
	std::string& x
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	if ((LA(1) == TOK_identifier)) {
		string_identifier(x);
	}
	else if ((LA(1) == TOK_string)) {
		i1 = LT(1);
		match(TOK_string);
		if ( inputState->guessing==0 ) {
			x = get_text(i1);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void parser_arrow::kind_abstraction(
	std::vector<ast::identifier>& vars
) {
	
	ast::identifier id;
	
	
	match(TOK_kind);
	match(TOK_lbracket);
	id=kind_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(id);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			id=kind_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(id);
			}
		}
		else {
			goto _loop61;
		}
		
	}
	_loop61:;
	} // ( ... )*
	match(TOK_rbracket);
}

ast::term_definition  parser_arrow::term_definition(
	const kind_id_vec& vars
) {
	ast::term_definition td;
	
	ast::identifier id;
	ast::term e;
	ast::type t;
	bool over = false;
	
	
	{
	if ((LA(1) == TOK_overload)) {
		match(TOK_overload);
		if ( inputState->guessing==0 ) {
			over = true;
		}
	}
	else if ((_tokenSet_7.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	id=symbol_name();
	{
	if (((LA(1) == TOK_semi || LA(1) == TOK_in))&&(over == true)) {
		if ( inputState->guessing==0 ) {
			td = make_overload_def(id, vars);
		}
	}
	else if ((LA(1) == TOK_colon)) {
		t=type_annot_id();
		{
		if ((LA(1) == TOK_semi || LA(1) == TOK_in)) {
			if ( inputState->guessing==0 ) {
				td = make_term_constant(id, vars, t, over);
			}
		}
		else if ((LA(1) == TOK_equal)) {
			match(TOK_equal);
			{
			if ((_tokenSet_8.member(LA(1)))) {
				e=term();
				if ( inputState->guessing==0 ) {
					td = make_term_def(id, vars, t, e, over);
				}
			}
			else if ((LA(1) == TOK_extern)) {
				match(TOK_extern);
				if ( inputState->guessing==0 ) {
					td = make_extern_def(id, vars, t, over);
				}
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else if ((LA(1) == TOK_equal)) {
		match(TOK_equal);
		e=term();
		if ( inputState->guessing==0 ) {
			td = make_term_def(id, vars, e, over);
		}
	}
	else if ((LA(1) == TOK_lbracket)) {
		td=term_function_def(id, vars, over);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return td;
}

ast::definition  parser_arrow::type_definition(
	const kind_id_vec& vars
) {
	ast::definition d;
	
	ast::identifier id;
	ast::type t;
	ast::kind ki;
	ast::polarity_type at;
	bool over = false;
	
	
	match(TOK_type);
	{
	if ((LA(1) == TOK_overload)) {
		match(TOK_overload);
		if ( inputState->guessing==0 ) {
			over = true;
		}
	}
	else if ((_tokenSet_7.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	id=symbol_name();
	{
	if (((LA(1) == TOK_semi))&&(over == true)) {
		if ( inputState->guessing==0 ) {
			d = make_type_def_over(id, vars);
		}
	}
	else if ((LA(1) == TOK_colon)) {
		ki=kind_annot_id();
		{
		if ((LA(1) == TOK_equal)) {
			match(TOK_equal);
			{
			if ((_tokenSet_9.member(LA(1)))) {
				t=type();
				if ( inputState->guessing==0 ) {
					d = make_type_def(id, vars, ki, t, over);
				}
			}
			else if ((LA(1) == TOK_extern)) {
				match(TOK_extern);
				if ( inputState->guessing==0 ) {
					d = make_type_def_extern(id, vars, ki, over);
				}
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1) == TOK_semi)) {
			if ( inputState->guessing==0 ) {
				d = make_type_def(id, vars, ki, t, over);
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else if ((LA(1) == TOK_equal)) {
		match(TOK_equal);
		t=type();
		if ( inputState->guessing==0 ) {
			d = make_type_def(id, vars, ki, t, over);
		}
	}
	else if ((LA(1) == TOK_lbracket)) {
		d=type_function_def(id, vars, over);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return d;
}

ast::definition  parser_arrow::term_use_definition(
	const kind_id_vec& vars
) {
	ast::definition d;
	
	ast::term e;
	
	
	match(TOK_val);
	e=term();
	if ( inputState->guessing==0 ) {
		d = make_term_use(e, vars);
	}
	return d;
}

ast::definition  parser_arrow::operator_declaration() {
	ast::definition x;
	
	location loc = get_location();
	ast::symbol_sequence        syms;
	ast::operator_property      properties;
	
	
	match(TOK_operator_declaration);
	match(TOK_lcurly);
	{
	operator_property(properties);
	{ // ( ... )*
	for (;;) {
		bool synPredMatched43 = false;
		if (((LA(1) == TOK_comma))) {
			int _m43 = mark();
			synPredMatched43 = true;
			inputState->guessing++;
			try {
				{
				match(TOK_comma);
				match(TOK_rcurly);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched43 = false;
			}
			rewind(_m43);
			inputState->guessing--;
		}
		if ( synPredMatched43 ) {
			match(TOK_comma);
		}
		else if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			operator_property(properties);
		}
		else {
			goto _loop44;
		}
		
	}
	_loop44:;
	} // ( ... )*
	}
	match(TOK_rcurly);
	symbol_sequence(syms);
	if ( inputState->guessing==0 ) {
		x = make_fixity_definition(loc, properties, syms);
	}
	return x;
}

ast::term  parser_arrow::term() {
	ast::term x;
	
	switch ( LA(1)) {
	case TOK_val:
	case TOK_equal:
	case TOK_lbracket:
	case TOK_colon:
	case TOK_double_arrow:
	case TOK_lcurly:
	case TOK_integer:
	case TOK_arrow:
	case TOK_plus:
	case TOK_minus:
	case TOK_lparen:
	case TOK_star:
	case TOK_list:
	case TOK_dot:
	case TOK_operator:
	case TOK_langle:
	case TOK_rangle:
	case TOK_ref:
	case TOK_deref:
	case TOK_record:
	case TOK_variant:
	case TOK_identifier:
	case TOK_string:
	case TOK_bracket_dolar:
	case TOK_bracket_assign_dolar:
	case TOK_dot_bracket_dolar:
	case TOK_dot_bracket_assign_dolar:
	case TOK_opening_bracket:
	case TOK_char:
	case LITERAL_true:
	case LITERAL_false:
	case TOK_number:
	{
		x=app_term();
		break;
	}
	case TOK_fun:
	{
		x=lambda_term();
		break;
	}
	case TOK_rec:
	{
		x=mu_term();
		break;
	}
	case TOK_all:
	{
		x=all_term();
		break;
	}
	case TOK_if:
	{
		x=if_term();
		break;
	}
	case TOK_case:
	{
		x=variant_case_term();
		break;
	}
	case TOK_some:
	{
		x=pack_term();
		break;
	}
	case TOK_open:
	{
		x=unpack_term();
		break;
	}
	case TOK_let:
	{
		x=let_term();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return x;
}

ast::identifier  parser_arrow::symbol_name() {
	ast::identifier vi;
	
	bool pref_under = false;
	bool post_under = false;
	
	
	{
	if ((LA(1) == TOK_underscore)) {
		match(TOK_underscore);
		if ( inputState->guessing==0 ) {
			pref_under = true;
		}
	}
	else if ((_tokenSet_10.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	vi=symbol_name_atom();
	{
	if ((LA(1) == TOK_underscore)) {
		match(TOK_underscore);
		if ( inputState->guessing==0 ) {
			post_under = true;
		}
	}
	else if ((_tokenSet_11.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		vi = make_fixity_name(vi, pref_under, post_under);
	}
	return vi;
}

ast::type  parser_arrow::type_annot_id() {
	ast::type t;
	
	match(TOK_colon);
	t=type();
	return t;
}

ast::term_definition  parser_arrow::term_function_def(
	const ast::identifier& id, const kind_id_vec& vars, bool over
) {
	ast::term_definition d;
	
	std::vector<ast::annotated_identifier> args;
	ast::annotated_identifier ai;
	ast::term e;
	ast::type t;
	
	
	match(TOK_lbracket);
	ai=annotated_identifier_or_under();
	if ( inputState->guessing==0 ) {
		args.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_identifier_or_under();
			if ( inputState->guessing==0 ) {
				args.push_back(ai);
			}
		}
		else {
			goto _loop26;
		}
		
	}
	_loop26:;
	} // ( ... )*
	match(TOK_rbracket);
	{
	if ((LA(1) == TOK_colon)) {
		match(TOK_colon);
		t=type();
	}
	else if ((LA(1) == TOK_double_arrow)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_double_arrow);
	e=term();
	if ( inputState->guessing==0 ) {
		d = make_term_def(id, vars, args, t, e, over);
	}
	return d;
}

ast::annotated_identifier  parser_arrow::annotated_identifier_or_under() {
	ast::annotated_identifier ai;
	
	location loc    = get_location();
	ast::identifier id;
	ast::type ta;
	
	
	if ((LA(1) == TOK_identifier)) {
		id=term_identifier(true);
		ta=type_annot_id();
		if ( inputState->guessing==0 ) {
			ai = make_annotated_id(loc, id, ta);
		}
	}
	else if ((LA(1) == TOK_underscore)) {
		match(TOK_underscore);
		ta=type_annot_id();
		if ( inputState->guessing==0 ) {
			ai = make_annotated_underscore(loc, ta);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return ai;
}

ast::type  parser_arrow::type() {
	ast::type t;
	
	switch ( LA(1)) {
	case TOK_all:
	{
		t=all_type();
		break;
	}
	case TOK_some:
	{
		t=some_type();
		break;
	}
	case TOK_Fun:
	{
		t=lambda_type();
		break;
	}
	case TOK_Rec:
	{
		t=mu_type();
		break;
	}
	case TOK_If:
	{
		t=if_type();
		break;
	}
	case TOK_val:
	case TOK_lbracket:
	case TOK_lcurly:
	case TOK_integer:
	case TOK_lparen:
	case TOK_type_int:
	case TOK_type_bool:
	case TOK_type_char:
	case TOK_type_string:
	case TOK_rref:
	case TOK_type_float:
	case TOK_Record:
	case TOK_Variant:
	case TOK_list:
	case TOK_dot:
	case TOK_identifier:
	case TOK_string:
	case TOK_opening_bracket:
	case TOK_char:
	case LITERAL_true:
	case LITERAL_false:
	{
		t=arrow_type();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return t;
}

ast::kind  parser_arrow::kind_annot_id() {
	ast::kind ki;
	
	match(TOK_colon);
	ki=kind();
	return ki;
}

ast::definition  parser_arrow::type_function_def(
	const ast::identifier& id, const kind_id_vec& vars, bool over
) {
	ast::definition d;
	
	ast::annotated_type_identifier ati;    
	ast::polarity_type at;
	ast::type t;
	
	ast::type_definition_var td = make_type_def_var(id, vars, over);
	d = td;
	ast::kind ki;
	
	
	match(TOK_lbracket);
	at=kind_arrow_prefix();
	ati=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		td.add_var(ati, at);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			at=kind_arrow_prefix();
			ati=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				td.add_var(ati, at);
			}
		}
		else {
			goto _loop35;
		}
		
	}
	_loop35:;
	} // ( ... )*
	match(TOK_rbracket);
	{
	if ((LA(1) == TOK_colon)) {
		match(TOK_colon);
		ki=kind();
	}
	else if ((LA(1) == TOK_semi || LA(1) == TOK_double_arrow)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	if ((LA(1) == TOK_double_arrow)) {
		match(TOK_double_arrow);
		{
		if ((_tokenSet_9.member(LA(1)))) {
			t=type();
		}
		else if ((LA(1) == TOK_extern)) {
			match(TOK_extern);
			if ( inputState->guessing==0 ) {
				t = make_type_constant(id, ki, true);
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else if ((LA(1) == TOK_semi)) {
		if ( inputState->guessing==0 ) {
			t = make_type_constant(id, ki, false);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		set_type_def_body(td, ki, t);
	}
	return d;
}

ast::polarity_type  parser_arrow::kind_arrow_prefix() {
	ast::polarity_type at;
	
	at = ast::polarity_type::neutral;
	
	
	if ((LA(1) == TOK_plus)) {
		match(TOK_plus);
		if ( inputState->guessing==0 ) {
			at = ast::polarity_type::positive;
		}
	}
	else if ((LA(1) == TOK_minus)) {
		match(TOK_minus);
		if ( inputState->guessing==0 ) {
			at = ast::polarity_type::negative;
		}
	}
	else if ((LA(1) == TOK_arrow || LA(1) == TOK_identifier)) {
		if ( inputState->guessing==0 ) {
			at = ast::polarity_type::neutral;
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return at;
}

ast::annotated_type_identifier  parser_arrow::annotated_type_identifier() {
	ast::annotated_type_identifier ati;
	
	ast::identifier id;
	ast::kind ki;
	
	
	id=type_identifier(true);
	{
	if ((LA(1) == TOK_equal || LA(1) == TOK_comma || LA(1) == TOK_rbracket)) {
		if ( inputState->guessing==0 ) {
			ati = make_annotated_type_id(id);
		}
	}
	else if ((LA(1) == TOK_colon)) {
		ki=kind_annot_id();
		if ( inputState->guessing==0 ) {
			ati = make_annotated_type_id(id, ki);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return ati;
}

ast::kind  parser_arrow::kind() {
	ast::kind ki;
	
	ki=arrow_kind();
	return ki;
}

void parser_arrow::operator_property(
	ast::operator_property& prop
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	std::string s1, s2;
	
	
	string_identifier(s1);
	match(TOK_equal);
	{
	if ((LA(1) == TOK_identifier)) {
		string_identifier(s2);
		if ( inputState->guessing==0 ) {
			add_operator_property_string(loc, prop, s1, s2);
		}
	}
	else if ((LA(1) == TOK_integer)) {
		i3 = LT(1);
		match(TOK_integer);
		if ( inputState->guessing==0 ) {
			add_operator_property_int(loc, prop, s1, get_int64(i3));
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::symbol_sequence(
	ast::symbol_sequence& seq
) {
	
	location loc = get_location();
	seq          = ast::symbol_sequence(loc);
	ast::identifier id;
	
	
	id=operator_definition_name();
	if ( inputState->guessing==0 ) {
		seq.add(id);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			id=operator_definition_name();
			if ( inputState->guessing==0 ) {
				seq.add(id);
			}
		}
		else {
			goto _loop49;
		}
		
	}
	_loop49:;
	} // ( ... )*
}

ast::identifier  parser_arrow::operator_definition_name() {
	ast::identifier vi;
	
	location loc = get_location();
	
	
	if ((LA(1) == TOK_identifier)) {
		vi=basic_identifier(true);
	}
	else if ((_tokenSet_12.member(LA(1)))) {
		vi=operator_atom();
	}
	else if (((LA(1) >= TOK_bracket_dolar && LA(1) <= TOK_dot_bracket_assign_dolar))) {
		vi=operator_bracket_dolar_atom();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return vi;
}

ast::identifier  parser_arrow::basic_identifier(
	bool build
) {
	ast::identifier x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	
	
	i1 = LT(1);
	match(TOK_identifier);
	if ( inputState->guessing==0 ) {
		if (build) x = make_basic_identifier(loc, get_text(i1));
	}
	return x;
}

ast::identifier  parser_arrow::operator_atom() {
	ast::identifier op;
	
	location loc = get_location();
	std::string name;
	bool can_cont;
	
	
	operator_atom_1(name, can_cont);
	if ( inputState->guessing==0 ) {
		op = make_operator_identifier(loc, name);
	}
	return op;
}

ast::identifier  parser_arrow::operator_bracket_dolar_atom() {
	ast::identifier op;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i4 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	ast::identifier vi;
	
	
	if ((LA(1) == TOK_bracket_dolar)) {
		i1 = LT(1);
		match(TOK_bracket_dolar);
		if ( inputState->guessing==0 ) {
			op = make_brack_op(loc, get_text(i1), false);
		}
	}
	else if ((LA(1) == TOK_bracket_assign_dolar)) {
		i2 = LT(1);
		match(TOK_bracket_assign_dolar);
		if ( inputState->guessing==0 ) {
			op = make_brack_assign_op(loc, get_text(i2), false);
		}
	}
	else if ((LA(1) == TOK_dot_bracket_dolar)) {
		i3 = LT(1);
		match(TOK_dot_bracket_dolar);
		if ( inputState->guessing==0 ) {
			op = make_brack_assign_op(loc, get_text(i3), true);
		}
	}
	else if ((LA(1) == TOK_dot_bracket_assign_dolar)) {
		i4 = LT(1);
		match(TOK_dot_bracket_assign_dolar);
		if ( inputState->guessing==0 ) {
			op = make_brack_assign_op(loc, get_text(i4), true);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return op;
}

ast::kind  parser_arrow::kind_annot_type() {
	ast::kind ki;
	
	match(TOK_dcolon);
	ki=kind();
	return ki;
}

ast::kind  parser_arrow::arrow_kind() {
	ast::kind ki;
	
	ast::kind k2;
	ast::polarity_type at;
	
	
	ki=kind_atom();
	{
	if (((LA(1) >= TOK_arrow && LA(1) <= TOK_minus))) {
		at=kind_arrow_prefix();
		match(TOK_arrow);
		k2=arrow_kind();
		if ( inputState->guessing==0 ) {
			ki = make_arrow_kind(ki, at, k2);
		}
	}
	else if ((_tokenSet_13.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return ki;
}

ast::kind  parser_arrow::kind_atom() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	switch ( LA(1)) {
	case TOK_star:
	{
		ki=star_kind();
		break;
	}
	case TOK_identifier:
	{
		ki=kind_var();
		break;
	}
	case TOK_lparen:
	{
		match(TOK_lparen);
		ki=kind();
		match(TOK_rparen);
		break;
	}
	case TOK_lcurly:
	{
		ki=product_kind();
		break;
	}
	case TOK_type_int:
	case TOK_type_bool:
	case TOK_type_char:
	case TOK_type_string:
	{
		ki=value_kind();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return ki;
}

ast::kind  parser_arrow::star_kind() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	match(TOK_star);
	{
	if ((LA(1) == TOK_lcurly)) {
		match(TOK_lcurly);
		ki=detailed_star_kind();
		match(TOK_rcurly);
	}
	else if ((_tokenSet_14.member(LA(1)))) {
		if ( inputState->guessing==0 ) {
			ki = make_star_kind(loc);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return ki;
}

ast::kind  parser_arrow::kind_var() {
	ast::kind ki;
	
	location loc = get_location();
	ast::identifier id;
	
	
	id=kind_identifier();
	if ( inputState->guessing==0 ) {
		ki = make_kind_identifier(loc, id);
	}
	return ki;
}

ast::kind  parser_arrow::product_kind() {
	ast::kind ki;
	
	location loc            = get_location();
	ast::product_kind pk    = ast::product_kind(loc, true); 
	ki = pk;   
	ast::kind k1;
	
	
	match(TOK_lcurly);
	{
	if ((_tokenSet_15.member(LA(1)))) {
		k1=kind();
		if ( inputState->guessing==0 ) {
			pk.add(k1);
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				k1=kind();
				if ( inputState->guessing==0 ) {
					pk.add(k1);
				}
			}
			else {
				goto _loop66;
			}
			
		}
		_loop66:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	if ( inputState->guessing==0 ) {
		ki = pk;
	}
	return ki;
}

ast::kind  parser_arrow::value_kind() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	if ((LA(1) == TOK_type_int)) {
		match(TOK_type_int);
		if ( inputState->guessing==0 ) {
			ki = make_int_kind(loc);
		}
	}
	else if ((LA(1) == TOK_type_bool)) {
		match(TOK_type_bool);
		if ( inputState->guessing==0 ) {
			ki = make_bool_kind(loc);
		}
	}
	else if ((LA(1) == TOK_type_char)) {
		match(TOK_type_char);
		if ( inputState->guessing==0 ) {
			ki = make_char_kind(loc);
		}
	}
	else if ((LA(1) == TOK_type_string)) {
		match(TOK_type_string);
		if ( inputState->guessing==0 ) {
			ki = make_string_kind(loc);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return ki;
}

ast::identifier  parser_arrow::kind_identifier() {
	ast::identifier x;
	
	x=basic_identifier(true);
	if ( inputState->guessing==0 ) {
		mark_kind_identifier(x);
	}
	return x;
}

ast::kind  parser_arrow::detailed_star_kind() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	if ((LA(1) == TOK_all)) {
		ki=star_kind_all();
	}
	else if ((LA(1) == TOK_some)) {
		ki=star_kind_some();
	}
	else if ((_tokenSet_16.member(LA(1)))) {
		ki=star_kind_arrow();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return ki;
}

ast::kind  parser_arrow::star_kind_all() {
	ast::kind ki;
	
	location loc        = get_location();
	std::vector<ast::kind> kind_vec;
	
	
	match(TOK_all);
	match(TOK_lbracket);
	ki=kind();
	if ( inputState->guessing==0 ) {
		kind_vec.push_back(ki);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ki=kind();
			if ( inputState->guessing==0 ) {
				kind_vec.push_back(ki);
			}
		}
		else {
			goto _loop72;
		}
		
	}
	_loop72:;
	} // ( ... )*
	match(TOK_rbracket);
	ki=detailed_star_kind();
	if ( inputState->guessing==0 ) {
		ki = make_all_kind_star(loc, kind_vec, ki);
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_some() {
	ast::kind ki;
	
	location loc        = get_location();
	std::vector<ast::kind> kind_vec;
	
	
	match(TOK_some);
	match(TOK_lbracket);
	ki=kind();
	if ( inputState->guessing==0 ) {
		kind_vec.push_back(ki);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ki=kind();
			if ( inputState->guessing==0 ) {
				kind_vec.push_back(ki);
			}
		}
		else {
			goto _loop75;
		}
		
	}
	_loop75:;
	} // ( ... )*
	match(TOK_rbracket);
	ki=detailed_star_kind();
	if ( inputState->guessing==0 ) {
		ki = make_some_kind_star(loc, kind_vec, ki);
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_arrow() {
	ast::kind ki;
	
	ast::kind k2;
	
	
	{
	ki=star_kind_prefix();
	}
	{
	if ((LA(1) == TOK_arrow)) {
		match(TOK_arrow);
		k2=detailed_star_kind();
		if ( inputState->guessing==0 ) {
			ki = make_arrow_kind_star(ki, k2);
		}
	}
	else if ((LA(1) == TOK_comma || LA(1) == TOK_rbracket || LA(1) == TOK_rcurly || LA(1) == TOK_rparen)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_prefix() {
	ast::kind ki;
	
	if ((LA(1) == TOK_rref)) {
		ki=star_kind_rref();
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		ki=star_kind_atom();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return ki;
}

ast::kind  parser_arrow::star_kind_rref() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	match(TOK_rref);
	ki=star_kind_atom();
	if ( inputState->guessing==0 ) {
		ki = make_ref_kind_star(loc, ki);
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_atom() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	switch ( LA(1)) {
	case TOK_type_int:
	case TOK_type_bool:
	case TOK_type_char:
	case TOK_type_string:
	case TOK_type_float:
	{
		ki=star_kind_atom_predefined();
		break;
	}
	case TOK_Record:
	{
		ki=star_kind_atom_record();
		break;
	}
	case TOK_Variant:
	{
		ki=star_kind_atom_variant();
		break;
	}
	case TOK_lparen:
	{
		ki=star_kind_atom_tuple();
		break;
	}
	case TOK_list:
	{
		ki=star_kind_atom_list();
		break;
	}
	case TOK_star:
	{
		match(TOK_star);
		if ( inputState->guessing==0 ) {
			ki = make_star_kind(loc);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_atom_predefined() {
	ast::kind ki;
	
	location loc    = get_location();
	
	
	switch ( LA(1)) {
	case TOK_type_bool:
	{
		match(TOK_type_bool);
		if ( inputState->guessing==0 ) {
			ki = make_bool_kind_star(loc);
		}
		break;
	}
	case TOK_type_int:
	{
		match(TOK_type_int);
		if ( inputState->guessing==0 ) {
			ki = make_int_kind_star(loc);
		}
		break;
	}
	case TOK_type_float:
	{
		match(TOK_type_float);
		if ( inputState->guessing==0 ) {
			ki = make_float_kind_star(loc);
		}
		break;
	}
	case TOK_type_string:
	{
		match(TOK_type_string);
		if ( inputState->guessing==0 ) {
			ki = make_string_kind_star(loc);
		}
		break;
	}
	case TOK_type_char:
	{
		match(TOK_type_char);
		if ( inputState->guessing==0 ) {
			ki = make_char_kind_star(loc);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_atom_record() {
	ast::kind ki;
	
	location loc    = get_location();
	ast::star_record_kind rk = ast::star_record_kind(loc, ast::star_kind_state::fixed);
	ki              = rk;
	
	
	match(TOK_Record);
	match(TOK_lcurly);
	{
	if ((LA(1) == TOK_identifier)) {
		star_kind_atom_record_field(rk);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				star_kind_atom_record_field(rk);
			}
			else {
				goto _loop86;
			}
			
		}
		_loop86:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	return ki;
}

ast::kind  parser_arrow::star_kind_atom_variant() {
	ast::kind ki;
	
	location loc    = get_location();
	ast::star_variant_kind vk = ast::star_variant_kind(loc, ast::star_kind_state::fixed);
	ki              = vk;
	
	
	match(TOK_Variant);
	match(TOK_lcurly);
	{
	if ((LA(1) == TOK_identifier)) {
		star_kind_atom_record_field_var(vk);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				star_kind_atom_record_field_var(vk);
			}
			else {
				goto _loop90;
			}
			
		}
		_loop90:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	return ki;
}

ast::kind  parser_arrow::star_kind_atom_tuple() {
	ast::kind ki;
	
	location loc    = get_location();
	std::vector<ast::kind> kind_vec;
	bool fixed      = false;
	
	
	match(TOK_lparen);
	{
	if ((LA(1) == TOK_rparen)) {
	}
	else if ((_tokenSet_18.member(LA(1)))) {
		ki=detailed_star_kind();
		if ( inputState->guessing==0 ) {
			kind_vec.push_back(ki);
		}
		{
		bool synPredMatched95 = false;
		if (((LA(1) == TOK_comma))) {
			int _m95 = mark();
			synPredMatched95 = true;
			inputState->guessing++;
			try {
				{
				match(TOK_comma);
				match(TOK_rparen);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched95 = false;
			}
			rewind(_m95);
			inputState->guessing--;
		}
		if ( synPredMatched95 ) {
			match(TOK_comma);
			if ( inputState->guessing==0 ) {
				fixed = true;
			}
		}
		else if ((LA(1) == TOK_comma)) {
			{ // ( ... )+
			int _cnt97=0;
			for (;;) {
				if ((LA(1) == TOK_comma)) {
					match(TOK_comma);
					ki=detailed_star_kind();
					if ( inputState->guessing==0 ) {
						kind_vec.push_back(ki);
					}
				}
				else {
					if ( _cnt97>=1 ) { goto _loop97; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt97++;
			}
			_loop97:;
			}  // ( ... )+
		}
		else if ((LA(1) == TOK_rparen)) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rparen);
	if ( inputState->guessing==0 ) {
		ki = make_tuple_kind_star(loc, kind_vec, fixed);
	}
	return ki;
}

ast::kind  parser_arrow::star_kind_atom_list() {
	ast::kind ki;
	
	location loc = get_location();
	
	
	match(TOK_list);
	match(TOK_lbracket);
	ki=detailed_star_kind();
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		ki = make_list_kind_star(loc, ki);
	}
	return ki;
}

void parser_arrow::star_kind_atom_record_field(
	ast::star_record_kind& rk
) {
	
	ast::identifier id;
	ast::kind ki;
	
	
	id=kind_identifier();
	match(TOK_equal);
	ki=detailed_star_kind();
	if ( inputState->guessing==0 ) {
		rk.add(id, ki);
	}
}

void parser_arrow::star_kind_atom_record_field_var(
	ast::star_variant_kind& rk
) {
	
	ast::identifier id;
	ast::kind ki;
	
	
	id=kind_identifier();
	match(TOK_equal);
	ki=detailed_star_kind();
	if ( inputState->guessing==0 ) {
		rk.add(id, ki);
	}
}

ast::type  parser_arrow::type_annot_term() {
	ast::type t;
	
	match(TOK_dcolon);
	t=type();
	return t;
}

ast::identifier  parser_arrow::type_identifier(
	bool build
) {
	ast::identifier x;
	
	x=basic_identifier(build);
	if ( inputState->guessing==0 ) {
		mark_type_identifier(x);
	}
	return x;
}

ast::type  parser_arrow::all_type() {
	ast::type t;
	
	location loc        = get_location();
	ast::type body;
	ast::annotated_type_identifier ai;
	type_id_vec2 vars;
	
	
	match(TOK_all);
	match(TOK_lbracket);
	ai=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(ai);
			}
		}
		else {
			goto _loop166;
		}
		
	}
	_loop166:;
	} // ( ... )*
	match(TOK_rbracket);
	body=type();
	if ( inputState->guessing==0 ) {
		t = make_all_type(loc, vars,body);
	}
	return t;
}

ast::some_type  parser_arrow::some_type() {
	ast::some_type t;
	
	location loc    = get_location();
	ast::type body;
	ast::annotated_type_identifier ai;
	type_id_vec2 vars;
	
	
	match(TOK_some);
	match(TOK_lbracket);
	ai=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(ai);
			}
		}
		else {
			goto _loop169;
		}
		
	}
	_loop169:;
	} // ( ... )*
	match(TOK_rbracket);
	body=type();
	if ( inputState->guessing==0 ) {
		t = make_some_type(loc, vars, body);
	}
	return t;
}

ast::type  parser_arrow::lambda_type() {
	ast::type t;
	
	location loc        = get_location();
	ast::type body;
	ast::annotated_type_identifier ai;
	type_id_vec vars;
	ast::polarity_type at;
	ast::kind ki;
	
	
	match(TOK_Fun);
	match(TOK_lbracket);
	at=kind_arrow_prefix();
	ai=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(annot_polarity(ai,at));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			at=kind_arrow_prefix();
			ai=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(annot_polarity(ai,at));
			}
		}
		else {
			goto _loop172;
		}
		
	}
	_loop172:;
	} // ( ... )*
	match(TOK_rbracket);
	{
	if ((LA(1) == TOK_colon)) {
		match(TOK_colon);
		ki=kind();
	}
	else if ((LA(1) == TOK_double_arrow)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_double_arrow);
	body=type();
	if ( inputState->guessing==0 ) {
		t = make_lambda_type(loc, vars, ki, body);
	}
	return t;
}

ast::type  parser_arrow::mu_type() {
	ast::type t;
	
	location loc        = get_location();
	ast::type body;
	type_id_vec2 vars;
	ast::annotated_type_identifier ai;
	
	
	match(TOK_Rec);
	match(TOK_lbracket);
	ai=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(ai);
			}
		}
		else {
			goto _loop176;
		}
		
	}
	_loop176:;
	} // ( ... )*
	match(TOK_rbracket);
	body=type();
	if ( inputState->guessing==0 ) {
		t = make_mu_type(loc, vars, body);
	}
	return t;
}

ast::type  parser_arrow::if_type() {
	ast::type x;
	
	location loc    = get_location();
	ast::type t1, t2, t3;
	
	
	match(TOK_If);
	t1=type();
	match(TOK_Then);
	t2=type();
	match(TOK_Else);
	t3=type();
	if ( inputState->guessing==0 ) {
		x = make_if_type(loc, t1, t2, t3);
	}
	return x;
}

ast::type  parser_arrow::arrow_type() {
	ast::type t;
	
	ast::type t2;
	
	
	{
	if ((_tokenSet_19.member(LA(1)))) {
		t=type_applic_simple();
	}
	else if ((LA(1) == TOK_val)) {
		match(TOK_val);
		match(TOK_lbracket);
		t=type_applic_general();
		match(TOK_rbracket);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	if ((LA(1) == TOK_arrow)) {
		match(TOK_arrow);
		t2=type();
		if ( inputState->guessing==0 ) {
			t = make_arrow_type(t, t2);
		}
	}
	else if ((_tokenSet_20.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return t;
}

ast::type  parser_arrow::type_applic_simple() {
	ast::type t;
	
	location loc = get_location();
	type_collector col(loc);
	
	
	prefix_type_simple(col);
	{
	if ((_tokenSet_19.member(LA(1)))) {
		type_applic_tail_simple(col);
	}
	else if ((_tokenSet_21.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		t = col.make_type();
	}
	return t;
}

ast::type  parser_arrow::type_applic_general() {
	ast::type t;
	
	location loc = get_location();
	type_collector col(loc);
	
	
	prefix_type_general(col);
	{
	if ((_tokenSet_22.member(LA(1)))) {
		type_applic_tail_general(col);
	}
	else if ((LA(1) == TOK_rbracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		t = col.make_type();
	}
	return t;
}

void parser_arrow::prefix_type_simple(
	type_collector& col
) {
	
	ast::type t;
	ast::identifier id;
	
	
	if ((LA(1) == TOK_rref)) {
		t=rref_type_simple();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
	}
	else {
		bool synPredMatched119 = false;
		if (((LA(1) == TOK_dot))) {
			int _m119 = mark();
			synPredMatched119 = true;
			inputState->guessing++;
			try {
				{
				type_path_predicate();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched119 = false;
			}
			rewind(_m119);
			inputState->guessing--;
		}
		if ( synPredMatched119 ) {
			match(TOK_dot);
			type_path_identifier(col);
			{
			bool synPredMatched122 = false;
			if (((_tokenSet_23.member(LA(1))))) {
				int _m122 = mark();
				synPredMatched122 = true;
				inputState->guessing++;
				try {
					{
					type_postfix_predicate();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched122 = false;
				}
				rewind(_m122);
				inputState->guessing--;
			}
			if ( synPredMatched122 ) {
				postfix_type_simple(col);
			}
			else if ((_tokenSet_24.member(LA(1)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((_tokenSet_23.member(LA(1)))) {
			postfix_type_simple(col);
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void parser_arrow::type_applic_tail_simple(
	type_collector& col
) {
	
	location loc = get_location();
	
	
	prefix_type_simple(col);
	{
	if ((_tokenSet_19.member(LA(1)))) {
		type_applic_tail_simple(col);
	}
	else if ((_tokenSet_21.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::prefix_type_general(
	type_collector& col
) {
	
	ast::type t;
	ast::identifier id;
	
	
	if ((LA(1) == TOK_rref)) {
		t=rref_type_general();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
	}
	else {
		bool synPredMatched125 = false;
		if (((LA(1) == TOK_dot))) {
			int _m125 = mark();
			synPredMatched125 = true;
			inputState->guessing++;
			try {
				{
				type_path_predicate();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched125 = false;
			}
			rewind(_m125);
			inputState->guessing--;
		}
		if ( synPredMatched125 ) {
			match(TOK_dot);
			type_path_identifier(col);
			{
			bool synPredMatched128 = false;
			if (((_tokenSet_25.member(LA(1))))) {
				int _m128 = mark();
				synPredMatched128 = true;
				inputState->guessing++;
				try {
					{
					type_postfix_predicate();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched128 = false;
				}
				rewind(_m128);
				inputState->guessing--;
			}
			if ( synPredMatched128 ) {
				postfix_type_general(col);
			}
			else if ((_tokenSet_26.member(LA(1)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((_tokenSet_25.member(LA(1)))) {
			postfix_type_general(col);
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void parser_arrow::type_applic_tail_general(
	type_collector& col
) {
	
	location loc = get_location();
	
	
	prefix_type_general(col);
	{
	if ((_tokenSet_22.member(LA(1)))) {
		type_applic_tail_general(col);
	}
	else if ((LA(1) == TOK_rbracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

ast::type  parser_arrow::rref_type_simple() {
	ast::type t;
	
	location loc    = get_location();
	ast::type ta;
	
	
	match(TOK_rref);
	ta=prefix_type_simple_ret();
	if ( inputState->guessing==0 ) {
		t = make_ref_type(loc, ta);
	}
	return t;
}

void parser_arrow::type_path_predicate() {
	
	ast::identifier id;
	
	
	match(TOK_dot);
	{
	if ((LA(1) == TOK_identifier)) {
		id=type_identifier(false);
	}
	else if ((LA(1) == TOK_operator)) {
		match(TOK_operator);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::type_path_identifier(
	type_collector& col
) {
	
	location loc = get_location();
	ast::identifier id;
	
	
	if ((LA(1) == TOK_identifier)) {
		id=type_identifier(true);
		if ( inputState->guessing==0 ) {
			col.add_dot(loc, id);
		}
	}
	else if ((LA(1) == TOK_operator)) {
		match(TOK_operator);
		match(TOK_lparen);
		id=symbol_name();
		if ( inputState->guessing==0 ) {
			mark_type_identifier(id);
		}
		match(TOK_rparen);
		if ( inputState->guessing==0 ) {
			col.add_dot(loc, id);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void parser_arrow::type_postfix_predicate() {
	
	ast::int_literal il;
	ast::identifier id;
	antlr::RefToken s;
	
	
	if ((LA(1) == TOK_dot)) {
		match(TOK_dot);
		{
		switch ( LA(1)) {
		case TOK_kind:
		{
			match(TOK_kind);
			break;
		}
		case TOK_lparen:
		{
			match(TOK_lparen);
			break;
		}
		case TOK_lbracket:
		case TOK_opening_bracket:
		{
			s=opening_bracket_type();
			break;
		}
		case TOK_integer:
		{
			il=atom_int(false);
			break;
		}
		case TOK_identifier:
		{
			id=type_identifier(false);
			break;
		}
		case TOK_operator:
		{
			match(TOK_operator);
			break;
		}
		case TOK_langle:
		{
			match(TOK_langle);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	else if ((LA(1) == TOK_dot_langle)) {
		match(TOK_dot_langle);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void parser_arrow::postfix_type_simple(
	type_collector& col
) {
	
	atomic_type_simple(col);
	{
	bool synPredMatched134 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m134 = mark();
		synPredMatched134 = true;
		inputState->guessing++;
		try {
			{
			type_postfix_predicate();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched134 = false;
		}
		rewind(_m134);
		inputState->guessing--;
	}
	if ( synPredMatched134 ) {
		type_postfix_item(col);
	}
	else if ((_tokenSet_24.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

ast::type  parser_arrow::rref_type_general() {
	ast::type t;
	
	location loc    = get_location();
	ast::type ta;
	
	
	match(TOK_rref);
	ta=prefix_type_general_ret();
	if ( inputState->guessing==0 ) {
		t = make_ref_type(loc, ta);
	}
	return t;
}

void parser_arrow::postfix_type_general(
	type_collector& col
) {
	
	atomic_type_general(col);
	{
	bool synPredMatched138 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m138 = mark();
		synPredMatched138 = true;
		inputState->guessing++;
		try {
			{
			type_postfix_predicate();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched138 = false;
		}
		rewind(_m138);
		inputState->guessing--;
	}
	if ( synPredMatched138 ) {
		type_postfix_item(col);
	}
	else if ((_tokenSet_26.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

ast::type  parser_arrow::prefix_type_general_ret() {
	ast::type t;
	
	type_collector col = type_collector(get_location());
	
	
	prefix_type_general(col);
	if ( inputState->guessing==0 ) {
		t = col.make_type();
	}
	return t;
}

ast::type  parser_arrow::prefix_type_simple_ret() {
	ast::type t;
	
	type_collector col = type_collector(get_location());
	
	
	prefix_type_simple(col);
	if ( inputState->guessing==0 ) {
		t = col.make_type();
	}
	return t;
}

void parser_arrow::atomic_type_simple(
	type_collector& col
) {
	
	ast::identifier id;
	ast::type t;
	
	
	switch ( LA(1)) {
	case TOK_identifier:
	{
		id=type_identifier(true);
		if ( inputState->guessing==0 ) {
			t = make_identifier_type(id); col.add(t);
		}
		break;
	}
	case TOK_type_int:
	case TOK_type_bool:
	case TOK_type_char:
	case TOK_type_string:
	case TOK_type_float:
	{
		t=predefined_types();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_Record:
	{
		t=record_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_Variant:
	{
		t=variant_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lparen:
	{
		t=tuple_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lbracket:
	case TOK_opening_bracket:
	{
		t=bracket_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_list:
	{
		t=list_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lcurly:
	{
		t=product_of_types();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_integer:
	case TOK_string:
	case TOK_char:
	case LITERAL_true:
	case LITERAL_false:
	{
		t=value_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void parser_arrow::type_postfix_item(
	type_collector& col
) {
	
	ast::location loc   = get_location();
	
	ast::kind ki;
	ast::int_literal pos;
	ast::identifier id;
	ast::tuple_type tt;
	ast::bracket_type bt;
	std::vector<ast::kind> args;
	
	
	{
	bool synPredMatched144 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m144 = mark();
		synPredMatched144 = true;
		inputState->guessing++;
		try {
			{
			match(TOK_dot);
			match(TOK_langle);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched144 = false;
		}
		rewind(_m144);
		inputState->guessing--;
	}
	if ( synPredMatched144 ) {
		type_type_app(col);
	}
	else {
		bool synPredMatched146 = false;
		if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
			int _m146 = mark();
			synPredMatched146 = true;
			inputState->guessing++;
			try {
				{
				match(TOK_dot_langle);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched146 = false;
			}
			rewind(_m146);
			inputState->guessing--;
		}
		if ( synPredMatched146 ) {
			type_type_app(col);
		}
		else if ((LA(1) == TOK_dot)) {
			match(TOK_dot);
			{
			switch ( LA(1)) {
			case TOK_kind:
			{
				type_kind_applic(col);
				break;
			}
			case TOK_lparen:
			{
				tt=tuple_type();
				if ( inputState->guessing==0 ) {
					col.add_dot_tuple(tt);
				}
				break;
			}
			case TOK_lbracket:
			case TOK_opening_bracket:
			{
				bt=bracket_type();
				if ( inputState->guessing==0 ) {
					col.add_dot_brack(bt);
				}
				break;
			}
			case TOK_integer:
			{
				type_projection(col);
				break;
			}
			case TOK_operator:
			case TOK_identifier:
			{
				type_path_identifier(col);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	bool synPredMatched150 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m150 = mark();
		synPredMatched150 = true;
		inputState->guessing++;
		try {
			{
			type_postfix_predicate();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched150 = false;
		}
		rewind(_m150);
		inputState->guessing--;
	}
	if ( synPredMatched150 ) {
		type_postfix_item(col);
	}
	else if ((_tokenSet_27.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::atomic_type_general(
	type_collector& col
) {
	
	ast::identifier id;
	ast::type t;
	
	
	switch ( LA(1)) {
	case TOK_equal:
	case TOK_colon:
	case TOK_double_arrow:
	case TOK_arrow:
	case TOK_plus:
	case TOK_minus:
	case TOK_star:
	case TOK_dot:
	case TOK_operator:
	case TOK_langle:
	case TOK_rangle:
	case TOK_identifier:
	case TOK_bracket_dolar:
	case TOK_bracket_assign_dolar:
	case TOK_dot_bracket_dolar:
	case TOK_dot_bracket_assign_dolar:
	{
		id=symbol_name_atom();
		if ( inputState->guessing==0 ) {
			t = make_identifier_type(id); col.add(t);
		}
		break;
	}
	case TOK_type_int:
	case TOK_type_bool:
	case TOK_type_char:
	case TOK_type_string:
	case TOK_type_float:
	{
		t=predefined_types();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_Record:
	{
		t=record_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_Variant:
	{
		t=variant_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lparen:
	{
		t=tuple_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lbracket:
	case TOK_opening_bracket:
	{
		t=bracket_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_list:
	{
		t=list_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_lcurly:
	{
		t=product_of_types();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	case TOK_integer:
	case TOK_string:
	case TOK_char:
	case LITERAL_true:
	case LITERAL_false:
	{
		t=value_type();
		if ( inputState->guessing==0 ) {
			col.add(t);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

antlr::RefToken  parser_arrow::opening_bracket_type() {
	antlr::RefToken t;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	if ((LA(1) == TOK_lbracket)) {
		i2 = LT(1);
		match(TOK_lbracket);
		if ( inputState->guessing==0 ) {
			t = i2;
		}
	}
	else if ((LA(1) == TOK_opening_bracket)) {
		i3 = LT(1);
		match(TOK_opening_bracket);
		if ( inputState->guessing==0 ) {
			t = i3;
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t;
}

ast::int_literal  parser_arrow::atom_int(
	bool build
) {
	ast::int_literal x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	
	
	i1 = LT(1);
	match(TOK_integer);
	if ( inputState->guessing==0 ) {
		if (build) x = make_int(loc, get_text(i1));
	}
	return x;
}

void parser_arrow::type_type_app(
	type_collector& col
) {
	
	ast::location loc = get_location();
	ast::type t;
	std::vector<ast::type> args;
	
	
	{
	if ((LA(1) == TOK_dot_langle)) {
		match(TOK_dot_langle);
	}
	else if ((LA(1) == TOK_dot)) {
		match(TOK_dot);
		match(TOK_langle);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	t=type();
	if ( inputState->guessing==0 ) {
		args.push_back(t);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			t=type();
			if ( inputState->guessing==0 ) {
				args.push_back(t);
			}
		}
		else {
			goto _loop154;
		}
		
	}
	_loop154:;
	} // ( ... )*
	match(TOK_rangle);
	if ( inputState->guessing==0 ) {
		col.add_type_applic(loc, args);
	}
}

void parser_arrow::type_kind_applic(
	type_collector& col
) {
	
	ast::location loc = get_location();
	ast::kind ki;
	std::vector<ast::kind> args;
	
	
	match(TOK_kind);
	match(TOK_lbracket);
	ki=kind();
	if ( inputState->guessing==0 ) {
		args.push_back(ki);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ki=kind();
			if ( inputState->guessing==0 ) {
				args.push_back(ki);
			}
		}
		else {
			goto _loop160;
		}
		
	}
	_loop160:;
	} // ( ... )*
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		col.add_kind_applic(loc, args);
	}
}

ast::tuple_type  parser_arrow::tuple_type() {
	ast::tuple_type t;
	
	location loc        = get_location();
	ast::tuple_type tt  = ast::tuple_type(loc, ast::tuple_type::can_expand); 
	t                   = tt;
	ast::type t1;
	ast::kind ki;
	
	
	match(TOK_lparen);
	{
	if ((LA(1) == TOK_rparen)) {
	}
	else {
		bool synPredMatched183 = false;
		if (((_tokenSet_7.member(LA(1))))) {
			int _m183 = mark();
			synPredMatched183 = true;
			inputState->guessing++;
			try {
				{
				symbol_name_predicate();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched183 = false;
			}
			rewind(_m183);
			inputState->guessing--;
		}
		if ( synPredMatched183 ) {
			t1=symbol_name_type();
			{
			if ((LA(1) == TOK_colon)) {
				ki=kind_annot_id();
				if ( inputState->guessing==0 ) {
					t1 = make_annotated_type(t1, ki);
				}
			}
			else if ((LA(1) == TOK_rparen)) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			if ( inputState->guessing==0 ) {
				tt.add(t1);
			}
		}
		else if ((_tokenSet_9.member(LA(1)))) {
			t1=annotated_type();
			if ( inputState->guessing==0 ) {
				tt.add(t1);
			}
			{
			bool synPredMatched187 = false;
			if (((LA(1) == TOK_comma))) {
				int _m187 = mark();
				synPredMatched187 = true;
				inputState->guessing++;
				try {
					{
					match(TOK_comma);
					match(TOK_rparen);
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched187 = false;
				}
				rewind(_m187);
				inputState->guessing--;
			}
			if ( synPredMatched187 ) {
				match(TOK_comma);
				if ( inputState->guessing==0 ) {
					tt.set_state(ast::tuple_type::fixed);
				}
			}
			else if ((LA(1) == TOK_comma)) {
				{ // ( ... )+
				int _cnt189=0;
				for (;;) {
					if ((LA(1) == TOK_comma)) {
						match(TOK_comma);
						t1=annotated_type();
						if ( inputState->guessing==0 ) {
							tt.add(t1);
						}
					}
					else {
						if ( _cnt189>=1 ) { goto _loop189; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
					}
					
					_cnt189++;
				}
				_loop189:;
				}  // ( ... )+
			}
			else if ((LA(1) == TOK_rparen)) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(TOK_rparen);
	return t;
}

ast::bracket_type  parser_arrow::bracket_type() {
	ast::bracket_type bt;
	
	location loc = get_location();
	bt = ast::bracket_type(loc);
	ast::type t;
	antlr::RefToken start, end;
	
	
	start=opening_bracket_type();
	{
	if ((_tokenSet_9.member(LA(1)))) {
		t=annotated_type();
		if ( inputState->guessing==0 ) {
			bt.add(t);
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				t=annotated_type();
				if ( inputState->guessing==0 ) {
					bt.add(t);
				}
			}
			else {
				goto _loop193;
			}
			
		}
		_loop193:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_closing_bracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	end=closing_bracket_type();
	if ( inputState->guessing==0 ) {
		bt = make_bracket_type(start, bt, end);
	}
	return bt;
}

void parser_arrow::type_projection(
	type_collector& col
) {
	
	ast::location loc = get_location();
	ast::int_literal pos;
	
	
	pos=atom_int(true);
	if ( inputState->guessing==0 ) {
		col.add_proj(loc, pos);
	}
}

ast::type  parser_arrow::predefined_types() {
	ast::type t;
	
	location loc    = get_location();
	
	
	switch ( LA(1)) {
	case TOK_type_bool:
	{
		match(TOK_type_bool);
		if ( inputState->guessing==0 ) {
			t = make_type_bool(loc);
		}
		break;
	}
	case TOK_type_int:
	{
		match(TOK_type_int);
		if ( inputState->guessing==0 ) {
			t = make_type_int(loc);
		}
		break;
	}
	case TOK_type_float:
	{
		match(TOK_type_float);
		if ( inputState->guessing==0 ) {
			t = make_type_float(loc);
		}
		break;
	}
	case TOK_type_string:
	{
		match(TOK_type_string);
		if ( inputState->guessing==0 ) {
			t = make_type_string(loc);
		}
		break;
	}
	case TOK_type_char:
	{
		match(TOK_type_char);
		if ( inputState->guessing==0 ) {
			t = make_type_char(loc);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return t;
}

ast::type  parser_arrow::record_type() {
	ast::type t;
	
	location loc        = get_location();
	ast::record_type tt = make_record_type(loc);
	t = tt;
	
	
	match(TOK_Record);
	match(TOK_lcurly);
	{
	if ((LA(1) == TOK_identifier)) {
		field_type(tt);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				field_type(tt);
			}
			else {
				goto _loop204;
			}
			
		}
		_loop204:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	return t;
}

ast::type  parser_arrow::variant_type() {
	ast::type t;
	
	location loc            = get_location();
	ast::variant_type tt    = make_variant_type(loc);
	t = tt;
	
	
	match(TOK_Variant);
	match(TOK_lcurly);
	{
	if ((LA(1) == TOK_identifier)) {
		field_type_var(tt);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				field_type_var(tt);
			}
			else {
				goto _loop208;
			}
			
		}
		_loop208:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	return t;
}

ast::type  parser_arrow::list_type() {
	ast::type t;
	
	location loc        = get_location();
	
	
	match(TOK_list);
	match(TOK_lbracket);
	t=type();
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		t = make_list_type(loc, t);
	}
	return t;
}

ast::type  parser_arrow::product_of_types() {
	ast::type t;
	
	location loc            = get_location();
	ast::product_of_types tt= ast::product_of_types(loc); 
	t = tt;   
	ast::type t1;
	
	
	match(TOK_lcurly);
	{
	if ((_tokenSet_9.member(LA(1)))) {
		t1=annotated_type();
		if ( inputState->guessing==0 ) {
			tt.add(t1);
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				t1=annotated_type();
				if ( inputState->guessing==0 ) {
					tt.add(t1);
				}
			}
			else {
				goto _loop197;
			}
			
		}
		_loop197:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	if ( inputState->guessing==0 ) {
		t = tt;
	}
	return t;
}

ast::type  parser_arrow::value_type() {
	ast::type t;
	
	ast::int_literal il;
	ast::bool_literal bl;
	ast::char_literal cl;
	ast::string_literal sl;
	
	
	if ((LA(1) == TOK_integer)) {
		il=atom_int(true);
		if ( inputState->guessing==0 ) {
			t = make_int_type(il);
		}
	}
	else if ((LA(1) == LITERAL_true || LA(1) == LITERAL_false)) {
		bl=atom_bool();
		if ( inputState->guessing==0 ) {
			t = make_bool_type(bl);
		}
	}
	else if ((LA(1) == TOK_char)) {
		cl=atom_char();
		if ( inputState->guessing==0 ) {
			t = make_char_type(cl);
		}
	}
	else if ((LA(1) == TOK_string)) {
		sl=atom_string();
		if ( inputState->guessing==0 ) {
			t = make_string_type(sl);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t;
}

ast::identifier  parser_arrow::symbol_name_atom() {
	ast::identifier vi;
	
	if ((LA(1) == TOK_identifier)) {
		vi=basic_identifier(true);
	}
	else if ((_tokenSet_12.member(LA(1)))) {
		vi=operator_atom();
	}
	else if (((LA(1) >= TOK_bracket_dolar && LA(1) <= TOK_dot_bracket_assign_dolar))) {
		vi=operator_bracket_dolar_atom();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return vi;
}

void parser_arrow::symbol_name_predicate() {
	
	ast::identifier vi;
	
	
	{
	if ((LA(1) == TOK_underscore)) {
		match(TOK_underscore);
	}
	else if ((_tokenSet_10.member(LA(1)))) {
		vi=symbol_name_atom();
		match(TOK_underscore);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

ast::type  parser_arrow::symbol_name_type() {
	ast::type t;
	
	ast::identifier id;
	
	
	id=symbol_name();
	if ( inputState->guessing==0 ) {
		t = make_identifier_type(id);
	}
	return t;
}

ast::type  parser_arrow::annotated_type() {
	ast::type t;
	
	ast::kind ki;
	
	
	t=type();
	{
	if ((LA(1) == TOK_dcolon)) {
		ki=kind_annot_type();
		if ( inputState->guessing==0 ) {
			t = make_annotated_type(t,ki);
		}
	}
	else if ((LA(1) == TOK_comma || LA(1) == TOK_rcurly || LA(1) == TOK_rparen || LA(1) == TOK_closing_bracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return t;
}

antlr::RefToken  parser_arrow::closing_bracket_type() {
	antlr::RefToken t;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	i3 = LT(1);
	match(TOK_closing_bracket);
	if ( inputState->guessing==0 ) {
		t = i3;
	}
	return t;
}

void parser_arrow::field_type(
	ast::record_type& rt
) {
	
	ast::annotated_type_identifier id;
	ast::type t;
	
	
	id=annotated_type_identifier();
	match(TOK_equal);
	t=type();
	if ( inputState->guessing==0 ) {
		rt.add(id,t);
	}
}

void parser_arrow::field_type_var(
	ast::variant_type& vt
) {
	
	ast::annotated_type_identifier id;
	ast::type t;
	
	
	id=annotated_type_identifier();
	match(TOK_equal);
	t=type();
	if ( inputState->guessing==0 ) {
		vt.add(id,t);
	}
}

ast::bool_literal  parser_arrow::atom_bool() {
	ast::bool_literal x;
	
	location loc = get_location();
	
	
	if ((LA(1) == LITERAL_true)) {
		match(LITERAL_true);
		if ( inputState->guessing==0 ) {
			x = make_boolean(loc,true);
		}
	}
	else if ((LA(1) == LITERAL_false)) {
		match(LITERAL_false);
		if ( inputState->guessing==0 ) {
			x = make_boolean(loc,false);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return x;
}

ast::char_literal  parser_arrow::atom_char() {
	ast::char_literal x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	
	
	i1 = LT(1);
	match(TOK_char);
	if ( inputState->guessing==0 ) {
		x = make_char(loc, get_text(i1));
	}
	return x;
}

ast::string_literal  parser_arrow::atom_string() {
	ast::string_literal x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	
	
	i1 = LT(1);
	match(TOK_string);
	if ( inputState->guessing==0 ) {
		x = make_string(loc, get_text(i1));
	}
	return x;
}

ast::identifier  parser_arrow::term_identifier(
	bool build
) {
	ast::identifier x;
	
	x=basic_identifier(build);
	if ( inputState->guessing==0 ) {
		mark_term_identifier(x);
	}
	return x;
}

ast::annotated_identifier  parser_arrow::possibly_annotated_identifier() {
	ast::annotated_identifier ai;
	
	location loc    = get_location();
	ast::identifier id;
	ast::type ta;
	
	
	id=term_identifier(true);
	{
	if ((LA(1) == TOK_colon)) {
		ta=type_annot_id();
	}
	else if ((LA(1) == TOK_equal || LA(1) == TOK_double_arrow)) {
		if ( inputState->guessing==0 ) {
			ta = make_type_var(get_location());
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		ai = make_annotated_id(loc, id, ta);
	}
	return ai;
}

ast::annotated_identifier  parser_arrow::annotated_identifier() {
	ast::annotated_identifier ai;
	
	location loc    = get_location();
	ast::identifier id;
	ast::type ta;
	
	
	id=term_identifier(true);
	ta=type_annot_id();
	if ( inputState->guessing==0 ) {
		ai = make_annotated_id(loc, id, ta);
	}
	return ai;
}

ast::term  parser_arrow::annotated_term() {
	ast::term x;
	
	ast::type t;
	
	
	x=term();
	{
	if ((LA(1) == TOK_dcolon)) {
		t=type_annot_term();
		if ( inputState->guessing==0 ) {
			x = make_annotated_term(x, t);
		}
	}
	else if ((_tokenSet_28.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return x;
}

ast::term  parser_arrow::app_term() {
	ast::term x;
	
	location loc = get_location();
	term_collector col(loc);
	
	
	prefix_term(col);
	{
	if ((_tokenSet_29.member(LA(1)))) {
		app_term_tail(col);
	}
	else if ((_tokenSet_30.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		x = col.make_term();
	}
	return x;
}

ast::term  parser_arrow::lambda_term() {
	ast::term x;
	
	location loc        = get_location();
	ast::term body;    
	ast::annotated_identifier ai;
	std::vector<ast::annotated_identifier> vars;
	ast::type t;
	
	
	match(TOK_fun);
	match(TOK_lbracket);
	ai=annotated_identifier_or_under();
	if ( inputState->guessing==0 ) {
		vars.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_identifier_or_under();
			if ( inputState->guessing==0 ) {
				vars.push_back(ai);
			}
		}
		else {
			goto _loop243;
		}
		
	}
	_loop243:;
	} // ( ... )*
	match(TOK_rbracket);
	{
	if ((LA(1) == TOK_colon)) {
		match(TOK_colon);
		t=type();
	}
	else if ((LA(1) == TOK_double_arrow)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_double_arrow);
	body=term();
	if ( inputState->guessing==0 ) {
		x = make_lambda_term(loc, vars, t, body);
	}
	return x;
}

ast::term  parser_arrow::mu_term() {
	ast::term t;
	
	location loc        = get_location();
	ast::term body;
	term_id_vec vars;
	ast::annotated_identifier ai;
	
	
	match(TOK_rec);
	match(TOK_lbracket);
	ai=annotated_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(ai);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ai=annotated_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(ai);
			}
		}
		else {
			goto _loop247;
		}
		
	}
	_loop247:;
	} // ( ... )*
	match(TOK_rbracket);
	body=term();
	if ( inputState->guessing==0 ) {
		t = make_mu_term(loc, vars, body);
	}
	return t;
}

ast::term  parser_arrow::all_term() {
	ast::term x;
	
	location loc                = get_location();
	ast::annotated_type_identifier it;
	ast::term t;
	type_id_vec vars;
	ast::polarity_type at = ast::polarity_type::neutral;
	
	
	match(TOK_all);
	match(TOK_lbracket);
	it=annotated_type_identifier();
	if ( inputState->guessing==0 ) {
		vars.push_back(annot_polarity(it,at));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			it=annotated_type_identifier();
			if ( inputState->guessing==0 ) {
				vars.push_back(annot_polarity(it,at));
			}
		}
		else {
			goto _loop222;
		}
		
	}
	_loop222:;
	} // ( ... )*
	match(TOK_rbracket);
	t=term();
	if ( inputState->guessing==0 ) {
		x = make_all_term(loc, vars, t);
	}
	return x;
}

ast::term  parser_arrow::if_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::term t1, t2, t3;
	
	
	match(TOK_if);
	t1=term();
	match(TOK_then);
	t2=term();
	match(TOK_else);
	t3=term();
	if ( inputState->guessing==0 ) {
		x = make_if_term(loc, t1, t2, t3);
	}
	return x;
}

ast::term  parser_arrow::variant_case_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::annotated_identifier id;
	ast::term t_case;
	ast::variant_case_term vt = ast::variant_case_term(loc, ast::variant_case_term::fresh());
	x = vt;
	
	
	match(TOK_case);
	t_case=term();
	if ( inputState->guessing==0 ) {
		vt.set_term(t_case);
	}
	match(TOK_in);
	match(TOK_lcurly);
	patterns(vt);
	match(TOK_rcurly);
	return x;
}

ast::term  parser_arrow::pack_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::annotated_type_identifier it;
	ast::term e;
	ast::type et;
	
	id_type_vec vars;
	
	
	match(TOK_some);
	match(TOK_lbracket);
	it=annotated_type_identifier();
	match(TOK_equal);
	et=type();
	if ( inputState->guessing==0 ) {
		vars.push_back(id_type(it,et));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			it=annotated_type_identifier();
			match(TOK_equal);
			et=type();
			if ( inputState->guessing==0 ) {
				vars.push_back(id_type(it,et));
			}
		}
		else {
			goto _loop231;
		}
		
	}
	_loop231:;
	} // ( ... )*
	match(TOK_rbracket);
	e=term();
	if ( inputState->guessing==0 ) {
		x = make_pack(loc, vars, e);
	}
	return x;
}

ast::term  parser_arrow::unpack_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::annotated_type_identifier  id1;
	ast::annotated_identifier       id2;
	ast::term t1, t2;
	
	
	match(TOK_open);
	match(TOK_lbracket);
	id1=annotated_type_identifier();
	match(TOK_rbracket);
	id2=possibly_annotated_identifier();
	match(TOK_equal);
	t1=term();
	match(TOK_in);
	t2=term();
	if ( inputState->guessing==0 ) {
		x = make_unpack(loc, t1, id1, id2, t2);
	}
	return x;
}

ast::term  parser_arrow::let_term() {
	ast::term x;
	
	location loc = get_location();
	ast::term body;
	ast::definitions defs(loc);
	
	
	match(TOK_let);
	binds(defs);
	match(TOK_in);
	body=term();
	if ( inputState->guessing==0 ) {
		x = make_let(loc, defs, body);
	}
	return x;
}

void parser_arrow::patterns(
	ast::variant_case_term& x
) {
	
	pattern(x);
	{
	if ((LA(1) == TOK_semi)) {
		match(TOK_semi);
		{
		if ((LA(1) == TOK_identifier)) {
			patterns(x);
		}
		else if ((LA(1) == TOK_rcurly)) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::pattern(
	ast::variant_case_term& x
) {
	
	ast::annotated_identifier id;
	ast::term t_alt;
	
	
	id=possibly_annotated_identifier();
	match(TOK_double_arrow);
	t_alt=term();
	if ( inputState->guessing==0 ) {
		x.add_pattern(id, t_alt);
	}
}

ast::definition  parser_arrow::local_definition(
	const kind_id_vec& vars
) {
	ast::definition d;
	
	if ((_tokenSet_6.member(LA(1)))) {
		d=term_definition(vars);
	}
	else if ((LA(1) == TOK_operator_declaration)) {
		d=operator_declaration();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return d;
}

void parser_arrow::binds(
	ast::definitions& defs
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  s1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	std::vector<ast::identifier> vars;
	ast::definition d;
	
	
	{
	if ((LA(1) == TOK_kind)) {
		kind_abstraction(vars);
	}
	else if ((_tokenSet_31.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	if ((_tokenSet_32.member(LA(1)))) {
		d=local_definition(vars);
		if ( inputState->guessing==0 ) {
			if (d) defs.add(d);
		}
	}
	else if ((LA(1) == TOK_lcurly)) {
		match(TOK_lcurly);
		{
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_6.member(LA(1)))) {
				try { // for error handling
					d=term_definition(vars);
				}
				catch (antlr::RecognitionException& e) {
					if (inputState->guessing==0) {
						reportError(e); consume_until_semi();
					} else {
						throw;
					}
				}
				try { // for error handling
					s1 = LT(1);
					match(TOK_semi);
				}
				catch (antlr::RecognitionException& e) {
					if (inputState->guessing==0) {
						reportError(e);
					} else {
						throw;
					}
				}
				if ( inputState->guessing==0 ) {
					if (d) defs.add(d);
				}
			}
			else if ((LA(1) == TOK_semi)) {
				try { // for error handling
					s2 = LT(1);
					match(TOK_semi);
				}
				catch (antlr::RecognitionException& e) {
					if (inputState->guessing==0) {
						reportError(e);
					} else {
						throw;
					}
				}
			}
			else {
				goto _loop239;
			}
			
		}
		_loop239:;
		} // ( ... )*
		}
		match(TOK_rcurly);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::prefix_term(
	term_collector& col
) {
	
	ast::term x;
	ast::identifier id;
	
	
	if ((LA(1) == TOK_ref)) {
		x=ref_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
	}
	else if ((LA(1) == TOK_deref)) {
		x=deref_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
	}
	else {
		bool synPredMatched255 = false;
		if (((LA(1) == TOK_dot))) {
			int _m255 = mark();
			synPredMatched255 = true;
			inputState->guessing++;
			try {
				{
				path_predicate();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched255 = false;
			}
			rewind(_m255);
			inputState->guessing--;
		}
		if ( synPredMatched255 ) {
			match(TOK_dot);
			path_identifier(col);
			{
			bool synPredMatched258 = false;
			if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
				int _m258 = mark();
				synPredMatched258 = true;
				inputState->guessing++;
				try {
					{
					postfix_predicate();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					(void) pe; /* suppress warnings about unreferenced var 'pe' */
					synPredMatched258 = false;
				}
				rewind(_m258);
				inputState->guessing--;
			}
			if ( synPredMatched258 ) {
				postfix(col);
			}
			else if ((_tokenSet_33.member(LA(1)))) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((_tokenSet_34.member(LA(1)))) {
			postfix_term(col);
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void parser_arrow::app_term_tail(
	term_collector& col
) {
	
	prefix_term(col);
	{
	if ((_tokenSet_29.member(LA(1)))) {
		app_term_tail(col);
	}
	else if ((_tokenSet_30.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

ast::term  parser_arrow::prefix_term_ret() {
	ast::term x;
	
	location loc = get_location();
	term_collector col(loc);
	
	
	prefix_term(col);
	if ( inputState->guessing==0 ) {
		x = col.make_term();
	}
	return x;
}

ast::term  parser_arrow::ref_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::term t;
	
	
	match(TOK_ref);
	t=prefix_term_ret();
	if ( inputState->guessing==0 ) {
		x = make_ref_term(loc, t);
	}
	return x;
}

ast::term  parser_arrow::deref_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::term t;
	
	
	match(TOK_deref);
	t=prefix_term_ret();
	if ( inputState->guessing==0 ) {
		x = make_deref_term(loc, t);
	}
	return x;
}

void parser_arrow::path_predicate() {
	
	ast::identifier id;
	
	
	match(TOK_dot);
	{
	if ((LA(1) == TOK_identifier)) {
		id=term_identifier(false);
	}
	else if ((LA(1) == TOK_operator)) {
		match(TOK_operator);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::path_identifier(
	term_collector& col
) {
	
	location loc = get_location();
	ast::identifier id;
	
	
	if ((LA(1) == TOK_identifier)) {
		id=term_identifier(true);
		if ( inputState->guessing==0 ) {
			col.add_dot(loc, id);
		}
	}
	else if ((LA(1) == TOK_operator)) {
		match(TOK_operator);
		match(TOK_lparen);
		id=symbol_name();
		if ( inputState->guessing==0 ) {
			mark_term_identifier(id);
		}
		match(TOK_rparen);
		if ( inputState->guessing==0 ) {
			col.add_dot(loc, id);
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void parser_arrow::postfix_predicate() {
	
	ast::identifier id;
	antlr::RefToken s;
	ast::int_literal il;
	
	
	if ((LA(1) == TOK_dot)) {
		match(TOK_dot);
		{
		switch ( LA(1)) {
		case TOK_langle:
		{
			match(TOK_langle);
			break;
		}
		case TOK_lbracket:
		case TOK_lcurly:
		case TOK_opening_bracket:
		{
			s=opening_bracket();
			break;
		}
		case TOK_lparen:
		{
			match(TOK_lparen);
			break;
		}
		case TOK_integer:
		{
			il=atom_int(false);
			break;
		}
		case TOK_identifier:
		{
			id=term_identifier(false);
			break;
		}
		case TOK_operator:
		{
			match(TOK_operator);
			break;
		}
		case TOK_kind:
		{
			match(TOK_kind);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	else if ((LA(1) == TOK_dot_langle)) {
		match(TOK_dot_langle);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void parser_arrow::postfix(
	term_collector& col
) {
	
	ast::identifier id;
	ast::bracket_term bt;
	ast::tuple_term tt;
	
	
	{
	bool synPredMatched272 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m272 = mark();
		synPredMatched272 = true;
		inputState->guessing++;
		try {
			{
			match(TOK_dot);
			match(TOK_langle);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched272 = false;
		}
		rewind(_m272);
		inputState->guessing--;
	}
	if ( synPredMatched272 ) {
		term_type_app(col);
	}
	else {
		bool synPredMatched274 = false;
		if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
			int _m274 = mark();
			synPredMatched274 = true;
			inputState->guessing++;
			try {
				{
				match(TOK_dot_langle);
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched274 = false;
			}
			rewind(_m274);
			inputState->guessing--;
		}
		if ( synPredMatched274 ) {
			term_type_app(col);
		}
		else if ((LA(1) == TOK_dot)) {
			match(TOK_dot);
			{
			switch ( LA(1)) {
			case TOK_lbracket:
			case TOK_lcurly:
			case TOK_opening_bracket:
			{
				bt=bracket_term();
				if ( inputState->guessing==0 ) {
					col.add_dot_brack(bt);
				}
				break;
			}
			case TOK_lparen:
			{
				tt=tuple_term();
				if ( inputState->guessing==0 ) {
					col.add_dot_tuple(tt);
				}
				break;
			}
			case TOK_operator:
			case TOK_identifier:
			{
				path_identifier(col);
				break;
			}
			case TOK_integer:
			{
				term_projection(col);
				break;
			}
			case TOK_kind:
			{
				term_kind_applic(col);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	bool synPredMatched278 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m278 = mark();
		synPredMatched278 = true;
		inputState->guessing++;
		try {
			{
			postfix_predicate();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched278 = false;
		}
		rewind(_m278);
		inputState->guessing--;
	}
	if ( synPredMatched278 ) {
		postfix(col);
	}
	else if ((_tokenSet_33.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::postfix_term(
	term_collector& col
) {
	
	ast::term x;
	
	
	atomic_term(col);
	{
	bool synPredMatched264 = false;
	if (((LA(1) == TOK_dot || LA(1) == TOK_dot_langle))) {
		int _m264 = mark();
		synPredMatched264 = true;
		inputState->guessing++;
		try {
			{
			postfix_predicate();
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			(void) pe; /* suppress warnings about unreferenced var 'pe' */
			synPredMatched264 = false;
		}
		rewind(_m264);
		inputState->guessing--;
	}
	if ( synPredMatched264 ) {
		postfix(col);
	}
	else if ((_tokenSet_33.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::atomic_term(
	term_collector& col
) {
	
	ast::identifier id;
	ast::term x;
	
	
	switch ( LA(1)) {
	case TOK_lparen:
	{
		x=tuple_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_record:
	{
		x=record_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_variant:
	{
		x=variant_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_list:
	{
		x=list_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_lbracket:
	case TOK_lcurly:
	case TOK_opening_bracket:
	{
		x=bracket_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_val:
	{
		x=val_term();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_integer:
	case TOK_string:
	case TOK_char:
	case LITERAL_true:
	case LITERAL_false:
	case TOK_number:
	{
		x=literals();
		if ( inputState->guessing==0 ) {
			col.add(x);
		}
		break;
	}
	case TOK_equal:
	case TOK_colon:
	case TOK_double_arrow:
	case TOK_arrow:
	case TOK_plus:
	case TOK_minus:
	case TOK_star:
	case TOK_dot:
	case TOK_operator:
	case TOK_langle:
	case TOK_rangle:
	case TOK_identifier:
	case TOK_bracket_dolar:
	case TOK_bracket_assign_dolar:
	case TOK_dot_bracket_dolar:
	case TOK_dot_bracket_assign_dolar:
	{
		id=symbol_name_atom();
		if ( inputState->guessing==0 ) {
			x = make_identifier_term(id); col.add(x);
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

antlr::RefToken  parser_arrow::opening_bracket() {
	antlr::RefToken t;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	if ((LA(1) == TOK_lcurly)) {
		i1 = LT(1);
		match(TOK_lcurly);
		if ( inputState->guessing==0 ) {
			t = i1;
		}
	}
	else if ((LA(1) == TOK_lbracket)) {
		i2 = LT(1);
		match(TOK_lbracket);
		if ( inputState->guessing==0 ) {
			t = i2;
		}
	}
	else if ((LA(1) == TOK_opening_bracket)) {
		i3 = LT(1);
		match(TOK_opening_bracket);
		if ( inputState->guessing==0 ) {
			t = i3;
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t;
}

void parser_arrow::term_type_app(
	term_collector& col
) {
	
	ast::location loc = get_location();
	ast::type t;
	std::vector<ast::type> args;
	
	
	{
	if ((LA(1) == TOK_dot_langle)) {
		match(TOK_dot_langle);
	}
	else if ((LA(1) == TOK_dot)) {
		match(TOK_dot);
		match(TOK_langle);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	t=type();
	if ( inputState->guessing==0 ) {
		args.push_back(t);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			t=type();
			if ( inputState->guessing==0 ) {
				args.push_back(t);
			}
		}
		else {
			goto _loop283;
		}
		
	}
	_loop283:;
	} // ( ... )*
	match(TOK_rangle);
	if ( inputState->guessing==0 ) {
		col.add_type_applic(loc, args);
	}
}

ast::bracket_term  parser_arrow::bracket_term() {
	ast::bracket_term bt;
	
	location loc = get_location();
	bt = ast::bracket_term(loc);
	ast::term t;
	antlr::RefToken start, end;
	
	
	start=opening_bracket();
	{
	if ((_tokenSet_8.member(LA(1)))) {
		t=annotated_term();
		if ( inputState->guessing==0 ) {
			bt.add(t);
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				t=annotated_term();
				if ( inputState->guessing==0 ) {
					bt.add(t);
				}
			}
			else {
				goto _loop302;
			}
			
		}
		_loop302:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rbracket || LA(1) == TOK_rcurly || LA(1) == TOK_closing_bracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	end=closing_bracket();
	if ( inputState->guessing==0 ) {
		bt = make_bracket_expr(start, bt, end);
	}
	return bt;
}

ast::tuple_term  parser_arrow::tuple_term() {
	ast::tuple_term tt;
	
	location loc        = get_location();
	tt                  = ast::tuple_term(loc, ast::tuple_term::can_expand);
	ast::term t;  
	ast::type ty;  
	
	
	match(TOK_lparen);
	{
	if ((LA(1) == TOK_rparen)) {
	}
	else {
		bool synPredMatched292 = false;
		if (((_tokenSet_7.member(LA(1))))) {
			int _m292 = mark();
			synPredMatched292 = true;
			inputState->guessing++;
			try {
				{
				symbol_name_predicate();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				(void) pe; /* suppress warnings about unreferenced var 'pe' */
				synPredMatched292 = false;
			}
			rewind(_m292);
			inputState->guessing--;
		}
		if ( synPredMatched292 ) {
			t=symbol_name_term();
			{
			if ((LA(1) == TOK_dcolon)) {
				ty=type_annot_term();
				if ( inputState->guessing==0 ) {
					t = make_annotated_term(t, ty);
				}
			}
			else if ((LA(1) == TOK_rparen)) {
			}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			
			}
			if ( inputState->guessing==0 ) {
				tt.add(t);
			}
		}
		else if ((_tokenSet_8.member(LA(1)))) {
			t=annotated_term();
			if ( inputState->guessing==0 ) {
				tt.add(t);
			}
			{
			if ((LA(1) == TOK_rparen)) {
			}
			else {
				bool synPredMatched296 = false;
				if (((LA(1) == TOK_comma))) {
					int _m296 = mark();
					synPredMatched296 = true;
					inputState->guessing++;
					try {
						{
						match(TOK_comma);
						match(TOK_rparen);
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						(void) pe; /* suppress warnings about unreferenced var 'pe' */
						synPredMatched296 = false;
					}
					rewind(_m296);
					inputState->guessing--;
				}
				if ( synPredMatched296 ) {
					match(TOK_comma);
					if ( inputState->guessing==0 ) {
						tt.set_state(ast::tuple_term::fixed);
					}
				}
				else if ((LA(1) == TOK_comma)) {
					{ // ( ... )+
					int _cnt298=0;
					for (;;) {
						if ((LA(1) == TOK_comma)) {
							match(TOK_comma);
							t=annotated_term();
							if ( inputState->guessing==0 ) {
								tt.add(t);
							}
						}
						else {
							if ( _cnt298>=1 ) { goto _loop298; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
						}
						
						_cnt298++;
					}
					_loop298:;
					}  // ( ... )+
				}
			else {
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(TOK_rparen);
	return tt;
}

void parser_arrow::term_projection(
	term_collector& col
) {
	
	ast::location loc = get_location();
	ast::int_literal pos;
	
	
	pos=atom_int(true);
	if ( inputState->guessing==0 ) {
		col.add_proj(loc, pos);
	}
}

void parser_arrow::term_kind_applic(
	term_collector& col
) {
	
	ast::location loc = get_location();
	ast::kind ki;
	std::vector<ast::kind> args;
	
	
	match(TOK_kind);
	match(TOK_lbracket);
	ki=kind();
	if ( inputState->guessing==0 ) {
		args.push_back(ki);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == TOK_comma)) {
			match(TOK_comma);
			ki=kind();
			if ( inputState->guessing==0 ) {
				args.push_back(ki);
			}
		}
		else {
			goto _loop287;
		}
		
	}
	_loop287:;
	} // ( ... )*
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		col.add_kind_applic(loc, args);
	}
}

ast::term  parser_arrow::record_term() {
	ast::term x;
	
	location loc        = get_location();
	ast::record_term rt = ast::record_term(loc);
	x = rt;
	
	
	match(TOK_record);
	match(TOK_lcurly);
	{
	if ((LA(1) == TOK_identifier)) {
		field_term(rt);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				field_term(rt);
			}
			else {
				goto _loop311;
			}
			
		}
		_loop311:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rcurly)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rcurly);
	return x;
}

ast::term  parser_arrow::variant_term() {
	ast::term x;
	
	location loc            = get_location();
	ast::annotated_identifier id;
	ast::type t;
	ast::term ex;
	
	
	match(TOK_variant);
	match(TOK_lbracket);
	t=type();
	match(TOK_rbracket);
	match(TOK_lcurly);
	id=possibly_annotated_identifier();
	match(TOK_equal);
	ex=term();
	match(TOK_rcurly);
	if ( inputState->guessing==0 ) {
		x = make_variant_term(loc, t, id, ex);
	}
	return x;
}

ast::term  parser_arrow::list_term() {
	ast::term x;
	
	location loc        = get_location();
	ast::list_term lt   = ast::list_term(loc);    
	x = lt;
	ast::term t;    
	
	
	match(TOK_list);
	match(TOK_lbracket);
	{
	if ((_tokenSet_8.member(LA(1)))) {
		t=annotated_term();
		if ( inputState->guessing==0 ) {
			lt.add(t);
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == TOK_comma)) {
				match(TOK_comma);
				t=annotated_term();
				if ( inputState->guessing==0 ) {
					lt.add(t);
				}
			}
			else {
				goto _loop307;
			}
			
		}
		_loop307:;
		} // ( ... )*
	}
	else if ((LA(1) == TOK_rbracket)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		x = lt;
	}
	return x;
}

ast::term  parser_arrow::val_term() {
	ast::term x;
	
	location loc    = get_location();
	ast::type t;
	
	
	match(TOK_val);
	match(TOK_lbracket);
	t=type();
	match(TOK_rbracket);
	if ( inputState->guessing==0 ) {
		x = make_val_term(loc, t);
	}
	return x;
}

ast::term  parser_arrow::literals() {
	ast::term x;
	
	switch ( LA(1)) {
	case TOK_char:
	{
		x=atom_char();
		break;
	}
	case TOK_integer:
	{
		x=atom_int(true);
		break;
	}
	case TOK_string:
	{
		x=atom_string();
		break;
	}
	case LITERAL_true:
	case LITERAL_false:
	{
		x=atom_bool();
		break;
	}
	case TOK_number:
	{
		x=atom_real();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return x;
}

ast::term  parser_arrow::symbol_name_term() {
	ast::term x;
	
	ast::identifier id;
	
	
	id=symbol_name();
	if ( inputState->guessing==0 ) {
		x = make_identifier_term(id);
	}
	return x;
}

antlr::RefToken  parser_arrow::closing_bracket() {
	antlr::RefToken t;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	if ((LA(1) == TOK_rcurly)) {
		i1 = LT(1);
		match(TOK_rcurly);
		if ( inputState->guessing==0 ) {
			t = i1;
		}
	}
	else if ((LA(1) == TOK_rbracket)) {
		i2 = LT(1);
		match(TOK_rbracket);
		if ( inputState->guessing==0 ) {
			t = i2;
		}
	}
	else if ((LA(1) == TOK_closing_bracket)) {
		i3 = LT(1);
		match(TOK_closing_bracket);
		if ( inputState->guessing==0 ) {
			t = i3;
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return t;
}

void parser_arrow::field_term(
	ast::record_term& x
) {
	
	location loc = get_location();
	ast::annotated_identifier id;
	ast::term t;
	
	
	id=possibly_annotated_identifier();
	match(TOK_equal);
	t=term();
	if ( inputState->guessing==0 ) {
		x.add(id,t);
	}
}

ast::term  parser_arrow::atom_real() {
	ast::term x;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	location loc = get_location();
	
	
	i1 = LT(1);
	match(TOK_number);
	if ( inputState->guessing==0 ) {
		x = make_float(loc, get_text(i1));
	}
	return x;
}

ast::identifier  parser_arrow::root_symbol_name() {
	ast::identifier vi;
	
	vi=symbol_name();
	{
	if ((LA(1) == TOK_semi)) {
		match(TOK_semi);
	}
	else if ((LA(1) == ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE)) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	return vi;
}

void parser_arrow::operator_atom_1(
	std::string& name, bool& can_cont
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	can_cont = false;
	
	
	{
	if ((_tokenSet_35.member(LA(1)))) {
		predefined_operator(name, can_cont);
	}
	else if ((LA(1) == TOK_operator)) {
		i1 = LT(1);
		match(TOK_operator);
		if ( inputState->guessing==0 ) {
			name += get_text(i1); can_cont = false;
		}
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	if (((_tokenSet_36.member(LA(1))))&&(can_cont == true)) {
		{
		if ((_tokenSet_12.member(LA(1)))) {
			operator_atom_1(name, can_cont);
		}
		else if ((_tokenSet_36.member(LA(1)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
	}
	else if ((_tokenSet_36.member(LA(1)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
}

void parser_arrow::predefined_operator(
	std::string& name, bool& can_cont
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i3 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i4 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i5 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i6 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i7 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i8 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i9 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i10 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case TOK_colon:
	{
		i1 = LT(1);
		match(TOK_colon);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i1, can_cont));
		}
		break;
	}
	case TOK_double_arrow:
	{
		i2 = LT(1);
		match(TOK_double_arrow);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i2, can_cont));
		}
		break;
	}
	case TOK_arrow:
	{
		i3 = LT(1);
		match(TOK_arrow);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i3, can_cont));
		}
		break;
	}
	case TOK_equal:
	{
		i4 = LT(1);
		match(TOK_equal);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i4, can_cont));
		}
		break;
	}
	case TOK_plus:
	{
		i5 = LT(1);
		match(TOK_plus);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i5, can_cont));
		}
		break;
	}
	case TOK_minus:
	{
		i6 = LT(1);
		match(TOK_minus);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i6, can_cont));
		}
		break;
	}
	case TOK_langle:
	{
		i7 = LT(1);
		match(TOK_langle);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i7, can_cont));
		}
		break;
	}
	case TOK_rangle:
	{
		i8 = LT(1);
		match(TOK_rangle);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i8, can_cont));
		}
		break;
	}
	case TOK_star:
	{
		i9 = LT(1);
		match(TOK_star);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i9, can_cont));
		}
		break;
	}
	case TOK_dot:
	{
		i10 = LT(1);
		match(TOK_dot);
		if ( inputState->guessing==0 ) {
			name += (get_text_cont(i10, can_cont));
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void parser_arrow::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* parser_arrow::tokenNames[] = {
	"<0>",
	"end of file",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	";",
	"\"module\"",
	"\"import\"",
	"TOK_as",
	"\"val\"",
	"\"over\"",
	"=",
	"\"extern\"",
	"\"left bracket\"",
	",",
	"\"right bracket\"",
	":",
	"=>",
	"\"type\"",
	"\"operator\"",
	"\"left curly bracket\"",
	"\"right curly bracket\"",
	"\"integer literal\"",
	"::",
	"->",
	"+",
	"-",
	"\"left paren\"",
	"\"right paren\"",
	"\"Kind\"",
	"\"int\"",
	"\"bool\"",
	"\"char\"",
	"\"string\"",
	"*",
	"\"All\"",
	"\"Some\"",
	"\"Ref\"",
	"\"float\"",
	"\"Record\"",
	"\"Variant\"",
	"\"list\"",
	".",
	"\"operator symbol\"",
	"\"left angle bracket\"",
	".<",
	"\"right angle bracket\"",
	"\"Fun\"",
	"\"Rec\"",
	"\"If\"",
	"\"Then\"",
	"\"Else\"",
	"_",
	"\"if\"",
	"\"then\"",
	"\"else\"",
	"\"case\"",
	"\"in\"",
	"\"open\"",
	"\"let\"",
	"\"fun\"",
	"\"rec\"",
	"\"ref\"",
	"\"deref\"",
	"\"record\"",
	"\"variant\"",
	"\"an identifier\"",
	"\"string literal\"",
	"\"bracket definition\"",
	"\"bracket assign definition\"",
	"\"dot bracket definition\"",
	"\"dot bracket assign definition\"",
	"\"opening bracket\"",
	"\"closing bracket\"",
	"\"character literal\"",
	"\"true\"",
	"\"false\"",
	"\"number literal\"",
	"\"any token\"",
	"\"utf8 lead byte\"",
	"\"utf8 next byte\"",
	"\"utf8 multibyte\"",
	"\"utf8 not line break\"",
	"\"utf8 bom\"",
	"\"shebang char\"",
	"\"whitespace char\"",
	"\"line break\"",
	"\"comment section\"",
	"\"line comment\"",
	"\"block comment\"",
	"\"double quoted string\"",
	"\"wysiwig string\"",
	"\"escape sequence\"",
	"\"escape char\"",
	"\"decimal integer\"",
	"\"binary integer\"",
	"\"octal integer\"",
	"\"hexal iteger\"",
	"\"decimal integer value\"",
	"\"decimal digit\"",
	"\"bin prefix\"",
	"\"bin integer\"",
	"\"bin digit\"",
	"\"octal prefix\"",
	"\"octal integer value\"",
	"\"octal digit\"",
	"\"hexal prefix\"",
	"\"hexal integer\"",
	"\"hexal digit\"",
	"\"decimal number\"",
	"\"hexal number\"",
	"\"decimal exponent\"",
	"\"hexal exponent\"",
	"\"string literal rule\"",
	"\"char literal\"",
	"\"an id\"",
	"\"id start\"",
	"\"block comment content\"",
	"\"identifier ascii\"",
	"\"identifier ascii start\"",
	"\"operator ascii character\"",
	"\"an operator\"",
	"\"operator character\"",
	"Predefined_operator",
	0
};

const unsigned long parser_arrow::_tokenSet_0_data_[] = { 327649138UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF TOK_semi "module" "import" "val" "over" TOK_equal TOK_colon TOK_double_arrow 
// "type" "operator" TOK_arrow TOK_plus TOK_minus "Kind" TOK_star TOK_dot 
// "operator symbol" "left angle bracket" "right angle bracket" TOK_underscore 
// "an identifier" "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long parser_arrow::_tokenSet_1_data_[] = { 327649106UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF TOK_semi "import" "val" "over" TOK_equal TOK_colon TOK_double_arrow 
// "type" "operator" TOK_arrow TOK_plus TOK_minus "Kind" TOK_star TOK_dot 
// "operator symbol" "left angle bracket" "right angle bracket" TOK_underscore 
// "an identifier" "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_1(_tokenSet_1_data_,8);
const unsigned long parser_arrow::_tokenSet_2_data_[] = { 327649042UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF TOK_semi "val" "over" TOK_equal TOK_colon TOK_double_arrow "type" 
// "operator" TOK_arrow TOK_plus TOK_minus "Kind" TOK_star TOK_dot "operator symbol" 
// "left angle bracket" "right angle bracket" TOK_underscore "an identifier" 
// "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long parser_arrow::_tokenSet_3_data_[] = { 327649040UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi "val" "over" TOK_equal TOK_colon TOK_double_arrow "type" "operator" 
// TOK_arrow TOK_plus TOK_minus "Kind" TOK_star TOK_dot "operator symbol" 
// "left angle bracket" "right angle bracket" TOK_underscore "an identifier" 
// "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long parser_arrow::_tokenSet_4_data_[] = { 327649024UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" "over" TOK_equal TOK_colon TOK_double_arrow "type" "operator" 
// TOK_arrow TOK_plus TOK_minus "Kind" TOK_star TOK_dot "operator symbol" 
// "left angle bracket" "right angle bracket" TOK_underscore "an identifier" 
// "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long parser_arrow::_tokenSet_5_data_[] = { 59213568UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" "over" TOK_equal TOK_colon TOK_double_arrow "type" "operator" 
// TOK_arrow TOK_plus TOK_minus TOK_star TOK_dot "operator symbol" "left angle bracket" 
// "right angle bracket" TOK_underscore "an identifier" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_5(_tokenSet_5_data_,8);
const unsigned long parser_arrow::_tokenSet_6_data_[] = { 58820096UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "over" TOK_equal TOK_colon TOK_double_arrow TOK_arrow TOK_plus TOK_minus 
// TOK_star TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// TOK_underscore "an identifier" "bracket definition" "bracket assign definition" 
// "dot bracket definition" "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_6(_tokenSet_6_data_,8);
const unsigned long parser_arrow::_tokenSet_7_data_[] = { 58819584UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_equal TOK_colon TOK_double_arrow TOK_arrow TOK_plus TOK_minus TOK_star 
// TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// TOK_underscore "an identifier" "bracket definition" "bracket assign definition" 
// "dot bracket definition" "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_7(_tokenSet_7_data_,8);
const unsigned long parser_arrow::_tokenSet_8_data_[] = { 128554240UL, 4270862094UL, 7935UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" TOK_equal "left bracket" TOK_colon TOK_double_arrow "left curly bracket" 
// "integer literal" TOK_arrow TOK_plus TOK_minus "left paren" TOK_star 
// "All" "Some" "list" TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "if" "case" "open" "let" "fun" "rec" "ref" "deref" "record" "variant" 
// "an identifier" "string literal" "bracket definition" "bracket assign definition" 
// "dot bracket definition" "dot bracket assign definition" "opening bracket" 
// "character literal" "true" "false" "number literal" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_8(_tokenSet_8_data_,8);
const unsigned long parser_arrow::_tokenSet_9_data_[] = { 3827831040UL, 115709UL, 3718UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" "left bracket" "left curly bracket" "integer literal" "left paren" 
// "int" "bool" "char" "string" "All" "Some" "Ref" "float" "Record" "Variant" 
// "list" TOK_dot "Fun" "Rec" "If" "an identifier" "string literal" "opening bracket" 
// "character literal" "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_9(_tokenSet_9_data_,8);
const unsigned long parser_arrow::_tokenSet_10_data_[] = { 58819584UL, 11778UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_equal TOK_colon TOK_double_arrow TOK_arrow TOK_plus TOK_minus TOK_star 
// TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "an identifier" "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_10(_tokenSet_10_data_,8);
const unsigned long parser_arrow::_tokenSet_11_data_[] = { 138449938UL, 16777216UL, 0UL, 0UL };
// EOF TOK_semi TOK_equal "left bracket" TOK_colon TOK_dcolon "right paren" 
// "in" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_11(_tokenSet_11_data_,4);
const unsigned long parser_arrow::_tokenSet_12_data_[] = { 58819584UL, 11778UL, 0UL, 0UL };
// TOK_equal TOK_colon TOK_double_arrow TOK_arrow TOK_plus TOK_minus TOK_star 
// TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_12(_tokenSet_12_data_,4);
const unsigned long parser_arrow::_tokenSet_13_data_[] = { 135357456UL, 0UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal TOK_comma "right bracket" TOK_double_arrow "right curly bracket" 
// "right paren" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_13(_tokenSet_13_data_,8);
const unsigned long parser_arrow::_tokenSet_14_data_[] = { 194077712UL, 0UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal TOK_comma "right bracket" TOK_double_arrow "right curly bracket" 
// TOK_arrow TOK_plus TOK_minus "right paren" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_14(_tokenSet_14_data_,8);
const unsigned long parser_arrow::_tokenSet_15_data_[] = { 3825729536UL, 3UL, 2UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "left curly bracket" "left paren" "int" "bool" "char" "string" TOK_star 
// "an identifier" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_15(_tokenSet_15_data_,8);
const unsigned long parser_arrow::_tokenSet_16_data_[] = { 3825205248UL, 499UL, 0UL, 0UL };
// "left paren" "int" "bool" "char" "string" TOK_star "Ref" "float" "Record" 
// "Variant" "list" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_16(_tokenSet_16_data_,4);
const unsigned long parser_arrow::_tokenSet_17_data_[] = { 3825205248UL, 483UL, 0UL, 0UL };
// "left paren" "int" "bool" "char" "string" TOK_star "float" "Record" 
// "Variant" "list" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_17(_tokenSet_17_data_,4);
const unsigned long parser_arrow::_tokenSet_18_data_[] = { 3825205248UL, 511UL, 0UL, 0UL };
// "left paren" "int" "bool" "char" "string" TOK_star "All" "Some" "Ref" 
// "float" "Record" "Variant" "list" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_18(_tokenSet_18_data_,4);
const unsigned long parser_arrow::_tokenSet_19_data_[] = { 3827830784UL, 1009UL, 3718UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "left bracket" "left curly bracket" "integer literal" "left paren" "int" 
// "bool" "char" "string" "Ref" "float" "Record" "Variant" "list" TOK_dot 
// "an identifier" "string literal" "opening bracket" "character literal" 
// "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_19(_tokenSet_19_data_,8);
const unsigned long parser_arrow::_tokenSet_20_data_[] = { 139551760UL, 17178624UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal TOK_comma "right bracket" TOK_double_arrow "right curly bracket" 
// TOK_dcolon "right paren" "right angle bracket" "Then" "Else" "in" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_20(_tokenSet_20_data_,8);
const unsigned long parser_arrow::_tokenSet_21_data_[] = { 147940368UL, 17178624UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal TOK_comma "right bracket" TOK_double_arrow "right curly bracket" 
// TOK_dcolon TOK_arrow "right paren" "right angle bracket" "Then" "Else" 
// "in" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_21(_tokenSet_21_data_,8);
const unsigned long parser_arrow::_tokenSet_22_data_[] = { 3886650368UL, 12275UL, 3838UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_equal "left bracket" TOK_colon TOK_double_arrow "left curly bracket" 
// "integer literal" TOK_arrow TOK_plus TOK_minus "left paren" "int" "bool" 
// "char" "string" TOK_star "Ref" "float" "Record" "Variant" "list" TOK_dot 
// "operator symbol" "left angle bracket" "right angle bracket" "an identifier" 
// "string literal" "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" "opening bracket" "character literal" 
// "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_22(_tokenSet_22_data_,8);
const unsigned long parser_arrow::_tokenSet_23_data_[] = { 3827830784UL, 481UL, 3718UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "left bracket" "left curly bracket" "integer literal" "left paren" "int" 
// "bool" "char" "string" "float" "Record" "Variant" "list" "an identifier" 
// "string literal" "opening bracket" "character literal" "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_23(_tokenSet_23_data_,8);
const unsigned long parser_arrow::_tokenSet_24_data_[] = { 3975771152UL, 17179633UL, 3974UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal "left bracket" TOK_comma "right bracket" TOK_double_arrow 
// "left curly bracket" "right curly bracket" "integer literal" TOK_dcolon 
// TOK_arrow "left paren" "right paren" "int" "bool" "char" "string" "Ref" 
// "float" "Record" "Variant" "list" TOK_dot "right angle bracket" "Then" 
// "Else" "in" "an identifier" "string literal" "opening bracket" "closing bracket" 
// "character literal" "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_24(_tokenSet_24_data_,8);
const unsigned long parser_arrow::_tokenSet_25_data_[] = { 3886650368UL, 12259UL, 3838UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_equal "left bracket" TOK_colon TOK_double_arrow "left curly bracket" 
// "integer literal" TOK_arrow TOK_plus TOK_minus "left paren" "int" "bool" 
// "char" "string" TOK_star "float" "Record" "Variant" "list" TOK_dot "operator symbol" 
// "left angle bracket" "right angle bracket" "an identifier" "string literal" 
// "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" "opening bracket" "character literal" 
// "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_25(_tokenSet_25_data_,8);
const unsigned long parser_arrow::_tokenSet_26_data_[] = { 3886666752UL, 12275UL, 3838UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_equal "left bracket" "right bracket" TOK_colon TOK_double_arrow 
// "left curly bracket" "integer literal" TOK_arrow TOK_plus TOK_minus 
// "left paren" "int" "bool" "char" "string" TOK_star "Ref" "float" "Record" 
// "Variant" "list" TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "an identifier" "string literal" "bracket definition" "bracket assign definition" 
// "dot bracket definition" "dot bracket assign definition" "opening bracket" 
// "character literal" "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_26(_tokenSet_26_data_,8);
const unsigned long parser_arrow::_tokenSet_27_data_[] = { 4026135568UL, 17182707UL, 4094UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_equal "left bracket" TOK_comma "right bracket" TOK_colon 
// TOK_double_arrow "left curly bracket" "right curly bracket" "integer literal" 
// TOK_dcolon TOK_arrow TOK_plus TOK_minus "left paren" "right paren" "int" 
// "bool" "char" "string" TOK_star "Ref" "float" "Record" "Variant" "list" 
// TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "Then" "Else" "in" "an identifier" "string literal" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
// "opening bracket" "closing bracket" "character literal" "true" "false" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_27(_tokenSet_27_data_,8);
const unsigned long parser_arrow::_tokenSet_28_data_[] = { 135290880UL, 0UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_comma "right bracket" "right curly bracket" "right paren" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_28(_tokenSet_28_data_,8);
const unsigned long parser_arrow::_tokenSet_29_data_[] = { 128554240UL, 3758108418UL, 7935UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" TOK_equal "left bracket" TOK_colon TOK_double_arrow "left curly bracket" 
// "integer literal" TOK_arrow TOK_plus TOK_minus "left paren" TOK_star 
// "list" TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "ref" "deref" "record" "variant" "an identifier" "string literal" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
// "opening bracket" "character literal" "true" "false" "number literal" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_29(_tokenSet_29_data_,8);
const unsigned long parser_arrow::_tokenSet_30_data_[] = { 139485200UL, 23068672UL, 256UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi TOK_comma "right bracket" "right curly bracket" TOK_dcolon 
// "right paren" "then" "else" "in" "closing bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_30(_tokenSet_30_data_,8);
const unsigned long parser_arrow::_tokenSet_31_data_[] = { 59606528UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "over" TOK_equal TOK_colon TOK_double_arrow "operator" "left curly bracket" 
// TOK_arrow TOK_plus TOK_minus TOK_star TOK_dot "operator symbol" "left angle bracket" 
// "right angle bracket" TOK_underscore "an identifier" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_31(_tokenSet_31_data_,8);
const unsigned long parser_arrow::_tokenSet_32_data_[] = { 59082240UL, 536066UL, 122UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "over" TOK_equal TOK_colon TOK_double_arrow "operator" TOK_arrow TOK_plus 
// TOK_minus TOK_star TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// TOK_underscore "an identifier" "bracket definition" "bracket assign definition" 
// "dot bracket definition" "dot bracket assign definition" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_32(_tokenSet_32_data_,8);
const unsigned long parser_arrow::_tokenSet_33_data_[] = { 268039440UL, 3781177090UL, 8191UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// TOK_semi "val" TOK_equal "left bracket" TOK_comma "right bracket" TOK_colon 
// TOK_double_arrow "left curly bracket" "right curly bracket" "integer literal" 
// TOK_dcolon TOK_arrow TOK_plus TOK_minus "left paren" "right paren" TOK_star 
// "list" TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "then" "else" "in" "ref" "deref" "record" "variant" "an identifier" 
// "string literal" "bracket definition" "bracket assign definition" "dot bracket definition" 
// "dot bracket assign definition" "opening bracket" "closing bracket" 
// "character literal" "true" "false" "number literal" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_33(_tokenSet_33_data_,8);
const unsigned long parser_arrow::_tokenSet_34_data_[] = { 128554240UL, 2147495682UL, 7935UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "val" TOK_equal "left bracket" TOK_colon TOK_double_arrow "left curly bracket" 
// "integer literal" TOK_arrow TOK_plus TOK_minus "left paren" TOK_star 
// "list" TOK_dot "operator symbol" "left angle bracket" "right angle bracket" 
// "record" "variant" "an identifier" "string literal" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
// "opening bracket" "character literal" "true" "false" "number literal" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_34(_tokenSet_34_data_,8);
const unsigned long parser_arrow::_tokenSet_35_data_[] = { 58819584UL, 10754UL, 0UL, 0UL };
// TOK_equal TOK_colon TOK_double_arrow TOK_arrow TOK_plus TOK_minus TOK_star 
// TOK_dot "left angle bracket" "right angle bracket" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_35(_tokenSet_35_data_,4);
const unsigned long parser_arrow::_tokenSet_36_data_[] = { 4026135826UL, 3781705715UL, 8191UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF TOK_semi "val" TOK_equal "left bracket" TOK_comma "right bracket" 
// TOK_colon TOK_double_arrow "left curly bracket" "right curly bracket" 
// "integer literal" TOK_dcolon TOK_arrow TOK_plus TOK_minus "left paren" 
// "right paren" "int" "bool" "char" "string" TOK_star "Ref" "float" "Record" 
// "Variant" "list" TOK_dot "operator symbol" "left angle bracket" TOK_dot_langle 
// "right angle bracket" TOK_underscore "then" "else" "in" "ref" "deref" 
// "record" "variant" "an identifier" "string literal" "bracket definition" 
// "bracket assign definition" "dot bracket definition" "dot bracket assign definition" 
// "opening bracket" "closing bracket" "character literal" "true" "false" 
// "number literal" 
const ANTLR_USE_NAMESPACE(antlr)BitSet parser_arrow::_tokenSet_36(_tokenSet_36_data_,8);


