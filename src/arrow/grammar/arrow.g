header
{
    //parser.hpp includes
    #include "grammar/parser_include.h"
}

options 
{
    language        = "Cpp";
    genHashLines    = false; 
}
{
    //parser.cpp includes
}

//------------------------------------------------------------------------------
//                  PARSER
//------------------------------------------------------------------------------
class           parser_arrow extends Parser;
options     
{       
    buildAST            = false;  
    exportVocab         = arrow_vocabulary;
    k 				    = 1;
    defaultErrorHandler = false;

    codeGenMakeSwitchThreshold 			= 5; 
    codeGenBitsetTestThreshold			= 5;
}                               

{	
    //parser members
    #include "grammar/parser_members.h"
}

//------------------------------------------------------------------------------
//                  START
//------------------------------------------------------------------------------

//-----------------------------------------------
//          TOPLEVEL
//-----------------------------------------------
toplevel returns[ast::module_definition defs]
{}  
    :   defs = module[false]
;
exception catch [unwind_all& e]                     {}

toplevel_shell returns[ast::module_definition defs]
{}  
    :   defs = module[true]
;
exception catch [unwind_all& e]                     {}
    
module[bool is_shell] returns[ast::module_definition x]
{
    location loc    = get_location();
    std::string name;
    std::vector<string_loc> imp;

    ast::definitions defs = make_definitions(loc);
}
    :	(
            {is_shell == false}?
            (
                name = em:module_name
                s3:TOK_semi
            )?
        
            imports[imp]
        |
        )

        (
            (
                ec:command[defs] 
                s1:TOK_semi
            |   s2:TOK_semi
            )

            exception catch [unwind_until_command& e] {}
        ) *

        EOF
                                                    { x = make_module_definition(name, imp, defs); }
;
exception[em] catch [antlr::RecognitionException& e]{ reportError(e); consume_until_semi();}
exception[ec] catch [antlr::RecognitionException& e]{ reportError(e); consume_until_semi();}
exception[s1] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[s2] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[s3] catch [antlr::RecognitionException& e]{ reportError(e);}
exception catch [antlr::RecognitionException& e]    { reportError(e);}

module_name returns[std::string x]
{
    std::string mod;
}
    :   
        TOK_module
        string_identifier[x]
;

imports[std::vector<string_loc>& imp]
{
    string_loc id;
}
    :   (
            options {greedy = true;}:
            id = e:import 
            s1:TOK_semi                             { imp.push_back(id); }

        |   s2:TOK_semi
        )*
;
exception[s1] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[s2] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[e] catch [antlr::RecognitionException& e] { reportError(e); consume_until_semi();}

import returns[parser_arrow::string_loc x]
{
    std::string import, import2;
    location loc = get_location();
}
    :   TOK_import
        import_name[import]
        (
            TOK_as
            string_identifier[import2]
        )?
                                                    { x = make_import(import, import2, loc); }
;

command[ast::definitions& defs]
{    
    ast::definition d;
    std::vector<ast::identifier> vars;
}
    :   (
            kind_abstraction[vars]
        )?
        (
            d = term_definition[vars]               { defs.add(d); }        
        |   d = type_definition[vars]               { defs.add(d); }
        |   d = term_use_definition[vars]           { defs.add(d); }
        |   d = operator_declaration                { defs.add(d); }
        )
;

term_use_definition[const kind_id_vec& vars] returns[ast::definition d]
{
    ast::term e;
}
    :   TOK_val e = term                            { d = make_term_use(e, vars); }
;

term_definition[const kind_id_vec& vars] returns[ast::term_definition td]
{
    ast::identifier id;
    ast::term e;
    ast::type t;
    bool over = false;
}
    :   (   
            TOK_overload                            { over = true; }
        )?
    
        id = symbol_name

        (
            {over == true}?
                                                    { td = make_overload_def(id, vars); }

        |   t = type_annot_id
            (
                                                    { td = make_term_constant(id, vars, t, over); }
            |   TOK_equal
                (
                    e = term                        { td = make_term_def(id, vars, t, e, over); }
                |   TOK_extern                      { td = make_extern_def(id, vars, t, over); }
                )
            )

        |   TOK_equal 
            e = term                                { td = make_term_def(id, vars, e, over); }

        |   td = term_function_def[id, vars, over]

        )
;

term_function_def[const ast::identifier& id, const kind_id_vec& vars, bool over]
    returns[ast::term_definition d]
{
    std::vector<ast::annotated_identifier> args;
    ast::annotated_identifier ai;
    ast::term e;
    ast::type t;
}
    :   TOK_lbracket
            ai = annotated_identifier_or_under      { args.push_back(ai); }
            (
                TOK_comma
                ai = annotated_identifier_or_under  { args.push_back(ai); }
            )*
        TOK_rbracket

        (
            TOK_colon
            t = type
        )?

        TOK_double_arrow
        e = term                                    { d = make_term_def(id, vars, args, t, e, over); }
;

type_definition[const kind_id_vec& vars] returns[ast::definition d]
{
    ast::identifier id;
    ast::type t;
    ast::kind ki;
    ast::polarity_type at;
    bool over = false;
}
    :   TOK_type 
        (   
            TOK_overload                            { over = true; }
        )?

        id = symbol_name

        (
            {over == true}?
                                                    { d = make_type_def_over(id, vars); }
        |   ki = kind_annot_id
            (
                TOK_equal
                (
                    t = type                        { d = make_type_def(id, vars, ki, t, over); }
                |   TOK_extern                      { d = make_type_def_extern(id, vars, ki, over); }
                )
            |                                       { d = make_type_def(id, vars, ki, t, over); }    
            )

        |   TOK_equal
            t = type                                { d = make_type_def(id, vars, ki, t, over); }

        |   d = type_function_def[id, vars, over]
        )
;

type_function_def[const ast::identifier& id, const kind_id_vec& vars, bool over]
    returns[ast::definition d]
{
    ast::annotated_type_identifier ati;    
    ast::polarity_type at;
    ast::type t;

    ast::type_definition_var td = make_type_def_var(id, vars, over);
    d = td;
    ast::kind ki;
}
    :   TOK_lbracket
            at = kind_arrow_prefix
            ati = annotated_type_identifier         { td.add_var(ati, at); }
            (
                TOK_comma
                at = kind_arrow_prefix
                ati = annotated_type_identifier     { td.add_var(ati, at); }
            )*
        TOK_rbracket

        (
            TOK_colon
            ki = kind
        )?

        (
            TOK_double_arrow
            (
                t = type
            |   TOK_extern                      { t = make_type_constant(id, ki, true);}
            )
        |                                       { t = make_type_constant(id, ki, false);}
        )                                       { set_type_def_body(td, ki, t); }
;

operator_declaration returns[ast::definition x]
{
    location loc = get_location();
    ast::symbol_sequence        syms;
    ast::operator_property      properties;
}
    :   
        TOK_operator_declaration

        TOK_lcurly
        
        (
            operator_property[properties]
            (
                    (TOK_comma TOK_rcurly) => 
                    TOK_comma
                |   TOK_comma 
                    operator_property[properties]
            )*
        )

        TOK_rcurly

        symbol_sequence[syms]                       { x = make_fixity_definition(loc, properties, syms); }
;

operator_property[ast::operator_property& prop]
{
    location loc = get_location();
    std::string s1, s2;
}
    :   string_identifier[s1]
        TOK_equal

        (
                string_identifier[s2]               { add_operator_property_string(loc, prop, s1, s2);}
            |   i3:TOK_integer                      { add_operator_property_int(loc, prop, s1, get_int64(i3));}
        )
;

symbol_sequence[ast::symbol_sequence& seq]
{
    location loc = get_location();
    seq          = ast::symbol_sequence(loc);
    ast::identifier id;
}
    :   id = operator_definition_name               { seq.add(id); }

        (
            TOK_comma
            id = operator_definition_name           { seq.add(id); }
        )*
;

operator_definition_name returns[ast::identifier vi]
{
    location loc = get_location();
}
    :     vi = basic_identifier[true]
        | vi = operator_atom
        | vi = operator_bracket_dolar_atom
;

//-----------------------------------------------
//          KINDS
//-----------------------------------------------

kind_annot_id returns[ast::kind ki]
{}
    :   TOK_colon
        ki = kind
;
kind_annot_type returns[ast::kind ki]
{}
    :   TOK_dcolon
        ki = kind
;

kind returns[ast::kind ki]
{}  
    :   ki = arrow_kind
;

arrow_kind returns[ast::kind ki]
{
    ast::kind k2;
    ast::polarity_type at;
} 
    :   ki = kind_atom 
        (   
            at = kind_arrow_prefix
            TOK_arrow 
            k2 = arrow_kind                         { ki = make_arrow_kind(ki, at, k2); }
        )?
;

kind_arrow_prefix returns[ast::polarity_type at]
{
    at = ast::polarity_type::neutral;
}
    :   TOK_plus                                    { at = ast::polarity_type::positive; }
    |   TOK_minus                                   { at = ast::polarity_type::negative; }
    |                                               { at = ast::polarity_type::neutral; }
;

kind_atom returns[ast::kind ki]
{
    location loc    = get_location();
} 
    :   ki = star_kind
    |   ki = kind_var
    |   TOK_lparen 
            ki = kind
        TOK_rparen
    |   ki = product_kind
    |   ki = value_kind
;

kind_var returns[ast::kind ki]
{
    location loc = get_location();
    ast::identifier id;
}
    :   id = kind_identifier                        { ki = make_kind_identifier(loc, id); }
;

kind_abstraction[std::vector<ast::identifier>& vars]
{
    ast::identifier id;
}
    :   TOK_kind

        TOK_lbracket 
            id = kind_identifier                    { vars.push_back(id); }
            (
                TOK_comma
                id = kind_identifier                { vars.push_back(id); }
            )*
        TOK_rbracket
;

value_kind returns[ast::kind ki]
{
    location loc    = get_location();
}
    :   TOK_type_int                                { ki = make_int_kind(loc); }
    |   TOK_type_bool                               { ki = make_bool_kind(loc); }
    |   TOK_type_char                               { ki = make_char_kind(loc); }
    |   TOK_type_string                             { ki = make_string_kind(loc); }
;

product_kind returns[ast::kind ki]
{
    location loc            = get_location();
    ast::product_kind pk    = ast::product_kind(loc, true); 
    ki = pk;   
    ast::kind k1;
}
    :   TOK_lcurly

        (
            k1 = kind                               { pk.add(k1); } 

            (
                TOK_comma
                k1 = kind                           { pk.add(k1); } 
            )*
        )?

        TOK_rcurly                                  { ki = pk; }
;

star_kind returns[ast::kind ki]
{
    location loc    = get_location();
}
    :   TOK_star
        (
            TOK_lcurly
                ki = detailed_star_kind
            TOK_rcurly
        |                                           { ki = make_star_kind(loc); }
        )
;

detailed_star_kind returns[ast::kind ki]
{
    location loc    = get_location();
}
    :   ki = star_kind_all
    |   ki = star_kind_some
    |   ki = star_kind_arrow
;

star_kind_all returns[ast::kind ki]
{
    location loc        = get_location();
    std::vector<ast::kind> kind_vec;
}
    :   TOK_all

        TOK_lbracket
            ki = kind                               { kind_vec.push_back(ki); }
            (
                TOK_comma
                ki = kind                           { kind_vec.push_back(ki); }
            )*
        TOK_rbracket

        ki = detailed_star_kind                     { ki = make_all_kind_star(loc, kind_vec, ki); }
;

star_kind_some returns[ast::kind ki]
{
    location loc        = get_location();
    std::vector<ast::kind> kind_vec;
}
    :   TOK_some

        TOK_lbracket
            ki = kind                               { kind_vec.push_back(ki); }
            (
                TOK_comma
                ki = kind                           { kind_vec.push_back(ki); }
            )*
        TOK_rbracket

        ki = detailed_star_kind                     { ki = make_some_kind_star(loc, kind_vec, ki); }
;

star_kind_arrow returns[ast::kind ki]
{
    ast::kind k2;
}
    :   (
            ki = star_kind_prefix
        )
        (
            TOK_arrow
            k2 = detailed_star_kind                 { ki = make_arrow_kind_star(ki, k2); }
        )?
;

star_kind_prefix returns[ast::kind ki]
{}
    :   ki = star_kind_rref
    |   ki = star_kind_atom
;

star_kind_atom returns[ast::kind ki]
{
    location loc    = get_location();
} 
    :   ki = star_kind_atom_predefined
    |   ki = star_kind_atom_record
    |   ki = star_kind_atom_variant
    |   ki = star_kind_atom_tuple
    |   ki = star_kind_atom_list
    |   TOK_star                                    { ki = make_star_kind(loc); }
;

star_kind_rref returns[ast::kind ki]
{
    location loc    = get_location();
}
    :   TOK_rref 
        ki = star_kind_atom                         { ki = make_ref_kind_star(loc, ki); }
;

star_kind_atom_predefined returns[ast::kind ki]
{
    location loc    = get_location();
}
    :   TOK_type_bool                               { ki = make_bool_kind_star(loc); }
    |   TOK_type_int                                { ki = make_int_kind_star(loc); }
    |   TOK_type_float                              { ki = make_float_kind_star(loc); }
    |   TOK_type_string                             { ki = make_string_kind_star(loc); }
    |   TOK_type_char                               { ki = make_char_kind_star(loc); }
;

star_kind_atom_record returns[ast::kind ki]
{
    location loc    = get_location();
    ast::star_record_kind rk = ast::star_record_kind(loc, ast::star_kind_state::fixed);
    ki              = rk;
}
    :   TOK_Record
        TOK_lcurly
        (
            star_kind_atom_record_field[rk]
            (
                TOK_comma
                star_kind_atom_record_field[rk]
            )*
        )?
        TOK_rcurly
;

star_kind_atom_variant returns[ast::kind ki]
{
    location loc    = get_location();
    ast::star_variant_kind vk = ast::star_variant_kind(loc, ast::star_kind_state::fixed);
    ki              = vk;
}
    :   TOK_Variant
        TOK_lcurly
        (
            star_kind_atom_record_field_var[vk]
            (
                TOK_comma
                star_kind_atom_record_field_var[vk]
            )*
        )?
        TOK_rcurly
;

star_kind_atom_tuple returns[ast::kind ki]
{
    location loc    = get_location();
    std::vector<ast::kind> kind_vec;
    bool fixed      = false;
}
    :   TOK_lparen 
        (   
            //empty tuple

        |   ki = detailed_star_kind                 { kind_vec.push_back(ki); }

            (   
                //single element tuple
                (TOK_comma TOK_rparen) =>
                TOK_comma                           { fixed = true; }
            |
                //multi element tuple
                (
                    TOK_comma
                    ki = detailed_star_kind         { kind_vec.push_back(ki); }
                )+
            |
                //paranthesized kind
            )
        )

        TOK_rparen                                  { ki = make_tuple_kind_star(loc, kind_vec, fixed); }
;

star_kind_atom_list returns[ast::kind ki]
{
    location loc = get_location();
}
    :   TOK_list
        TOK_lbracket
        ki = detailed_star_kind
        TOK_rbracket                                { ki = make_list_kind_star(loc, ki); }
;

star_kind_atom_record_field[ast::star_record_kind& rk]
{
    ast::identifier id;
    ast::kind ki;
}
    :   id = kind_identifier
        TOK_equal
        ki = detailed_star_kind                     { rk.add(id, ki); }
;
star_kind_atom_record_field_var[ast::star_variant_kind& rk]
{
    ast::identifier id;
    ast::kind ki;
}
    :   id = kind_identifier
        TOK_equal
        ki = detailed_star_kind                     { rk.add(id, ki); }
;

//-----------------------------------------------
//          TYPES
//-----------------------------------------------
type_annot_id returns[ast::type t]
{}
    :   TOK_colon
        t = type
;
type_annot_term returns[ast::type t]
{}
    :   TOK_dcolon
        t = type
;

annotated_type_identifier returns[ast::annotated_type_identifier ati]
{
    ast::identifier id;
    ast::kind ki;
}
    :   id = type_identifier[true]
        (
                                                    { ati = make_annotated_type_id(id); }
        |    ki = kind_annot_id                     { ati = make_annotated_type_id(id, ki); }
        )
;

type returns[ast::type t]
{}
    :   t = all_type 
    |   t = some_type   
    |   t = lambda_type
    |   t = mu_type    
    |   t = if_type
    |   t = arrow_type
;

arrow_type returns[ast::type t]
{
    ast::type t2;
}
    :   (
            t = type_applic_simple

        |   TOK_val
            TOK_lbracket
            t = type_applic_general
            TOK_rbracket
        )
        (
            TOK_arrow
            t2 = type                               { t = make_arrow_type(t, t2); }
        )?
;

type_applic_simple returns[ast::type t]
{
    location loc = get_location();
    type_collector col(loc);
}
    :   prefix_type_simple[col]
        (
            type_applic_tail_simple[col]
        )?                                          { t = col.make_type(); }
;
type_applic_tail_simple[type_collector& col]
{
    location loc = get_location();
}
    :   prefix_type_simple[col]
        (
            type_applic_tail_simple[col]
        )?
;

type_applic_general returns[ast::type t]
{
    location loc = get_location();
    type_collector col(loc);
}
    :   prefix_type_general[col]
        (
            type_applic_tail_general[col]
        )?                                          { t = col.make_type(); }
;
type_applic_tail_general[type_collector& col]
{
    location loc = get_location();
}
    :   prefix_type_general[col]
        (
            type_applic_tail_general[col]
        )?
;

prefix_type_simple[type_collector& col]
{
    ast::type t;
    ast::identifier id;
}
    :   t = rref_type_simple                        { col.add(t); }

    |   (type_path_predicate) =>
        
        TOK_dot
        type_path_identifier[col]

        (
            (type_postfix_predicate) =>
            postfix_type_simple[col]
        )?

    |   postfix_type_simple[col]
;

prefix_type_general[type_collector& col]
{
    ast::type t;
    ast::identifier id;
}
    :   t = rref_type_general                       { col.add(t); }

    |   (type_path_predicate) =>
        
        TOK_dot
        type_path_identifier[col]

        (
            (type_postfix_predicate) =>
            postfix_type_general[col]
        )?

    | postfix_type_general[col]
;

prefix_type_general_ret returns[ast::type t]
{
    type_collector col = type_collector(get_location());
}
    : prefix_type_general[col]                      { t = col.make_type(); }
;
prefix_type_simple_ret returns[ast::type t]
{
    type_collector col = type_collector(get_location());
}
    : prefix_type_simple[col]                       { t = col.make_type(); }
;

postfix_type_simple[type_collector& col]
{}
    :   atomic_type_simple[col]        
        ( 
            (type_postfix_predicate) =>
            type_postfix_item[col]            
        )?
;
postfix_type_general[type_collector& col]
{}
    :   atomic_type_general[col]
        ( 
            (type_postfix_predicate) =>
            type_postfix_item[col]
        )?
;

type_postfix_predicate
{
    ast::int_literal il;
    ast::identifier id;
    antlr::RefToken s;
}
    :   TOK_dot
        (
            TOK_kind                    //kind application
        |   TOK_lparen                  //tuple_type
        |   s = opening_bracket_type    //bracket_type
        |   il = atom_int[false]        //type_projection
        |   id = type_identifier[false] //path_identifier
        |   TOK_operator                //path_identifier
        |   TOK_langle                  //type_type_app
        )
    |   TOK_dot_langle                  //type_type_app
;

type_postfix_item[type_collector& col]
{
    ast::location loc   = get_location();

    ast::kind ki;
    ast::int_literal pos;
    ast::identifier id;
    ast::tuple_type tt;
    ast::bracket_type bt;
    std::vector<ast::kind> args;
}
    :   (
            (TOK_dot TOK_langle) =>
            type_type_app[col]

        |   (TOK_dot_langle) =>
            type_type_app[col]  
              
        |   TOK_dot   
            (
                type_kind_applic[col]
            |   tt = tuple_type                     {col.add_dot_tuple(tt); }
            |   bt = bracket_type                   {col.add_dot_brack(bt); }
            |   type_projection[col]
            |   type_path_identifier[col]
            )
        )
        (   
            (type_postfix_predicate) =>         
            type_postfix_item[col]            
        )?
;

type_type_app[type_collector& col]
{
    ast::location loc = get_location();
    ast::type t;
    std::vector<ast::type> args;
}
    :   (TOK_dot_langle | TOK_dot TOK_langle)
            t = type                                { args.push_back(t); }
            (
                TOK_comma
                t = type                            { args.push_back(t); }
            )*
        TOK_rangle                                  { col.add_type_applic(loc, args); }
;

type_path_predicate
{
    ast::identifier id;
}
    :   TOK_dot (id = type_identifier[false] | TOK_operator)
;
type_path_identifier[type_collector& col]
{
    location loc = get_location();
    ast::identifier id;
}
    :   id = type_identifier[true]                  { col.add_dot(loc, id); }

    |   TOK_operator
        TOK_lparen
            id = symbol_name                        { mark_type_identifier(id); }
        TOK_rparen                                  { col.add_dot(loc, id); }
;

type_kind_applic[type_collector& col]
{
    ast::location loc = get_location();
    ast::kind ki;
    std::vector<ast::kind> args;
}
    :   TOK_kind
        TOK_lbracket
        ki = kind                                   { args.push_back(ki); }
        (
            TOK_comma
            ki = kind                               { args.push_back(ki); }
        )*
        TOK_rbracket                                { col.add_kind_applic(loc, args); }
;

type_projection[type_collector& col]
{
    ast::location loc = get_location();
    ast::int_literal pos;
}
    :   pos = atom_int[true]                        { col.add_proj(loc, pos); }
;

atomic_type_simple[type_collector& col]
{
    ast::identifier id;
    ast::type t;
} 
    :   id = type_identifier[true]                  { t = make_identifier_type(id); col.add(t); }
    |   t = predefined_types                        { col.add(t); }
    |   t = record_type                             { col.add(t); }
    |   t = variant_type                            { col.add(t); }
    |   t = tuple_type                              { col.add(t); }
    |   t = bracket_type                            { col.add(t); }
    |   t = list_type                               { col.add(t); }
    |   t = product_of_types                        { col.add(t); }
    |   t = value_type                              { col.add(t); }
;

atomic_type_general[type_collector& col]
{
    ast::identifier id;
    ast::type t;
} 
    :   id = symbol_name_atom                       { t = make_identifier_type(id); col.add(t); }
    |   t = predefined_types                        { col.add(t); }
    |   t = record_type                             { col.add(t); }
    |   t = variant_type                            { col.add(t); }
    |   t = tuple_type                              { col.add(t); }
    |   t = bracket_type                            { col.add(t); }
    |   t = list_type                               { col.add(t); }
    |   t = product_of_types                        { col.add(t); }
    |   t = value_type                              { col.add(t); }
;

all_type returns[ast::type t]
{
    location loc        = get_location();
    ast::type body;
    ast::annotated_type_identifier ai;
    type_id_vec2 vars;
}
    :   TOK_all

        TOK_lbracket
            ai = annotated_type_identifier          { vars.push_back(ai); }
            (
                TOK_comma
                ai = annotated_type_identifier      { vars.push_back(ai); }
            )*
        TOK_rbracket
        body = type                                 { t = make_all_type(loc, vars,body); }
;

some_type returns[ast::some_type t]
{
    location loc    = get_location();
    ast::type body;
    ast::annotated_type_identifier ai;
    type_id_vec2 vars;
}
    :   TOK_some

        TOK_lbracket
            ai = annotated_type_identifier          { vars.push_back(ai); }
            (
                TOK_comma
                ai = annotated_type_identifier      { vars.push_back(ai); }
            )*
        TOK_rbracket
        body = type                                 { t = make_some_type(loc, vars, body); }
;

lambda_type returns[ast::type t]
{
    location loc        = get_location();
    ast::type body;
    ast::annotated_type_identifier ai;
    type_id_vec vars;
    ast::polarity_type at;
    ast::kind ki;
}
    :   TOK_Fun
        TOK_lbracket
            at = kind_arrow_prefix
            ai = annotated_type_identifier          { vars.push_back(annot_polarity(ai,at)); }
            (
                TOK_comma
                at = kind_arrow_prefix
                ai = annotated_type_identifier      { vars.push_back(annot_polarity(ai,at)); }
            )*
        TOK_rbracket

        (
            TOK_colon
            ki = kind
        )?

        TOK_double_arrow
        body = type                          { t = make_lambda_type(loc, vars, ki, body); }
;

mu_type returns[ast::type t]
{
    location loc        = get_location();
    ast::type body;
    type_id_vec2 vars;
    ast::annotated_type_identifier ai;
}
    :   TOK_Rec
        TOK_lbracket
            ai = annotated_type_identifier          { vars.push_back(ai); }
            (
                TOK_comma
                ai = annotated_type_identifier      { vars.push_back(ai); }
            )*
        TOK_rbracket
        body = type                                 { t = make_mu_type(loc, vars, body); }
;

rref_type_general returns[ast::type t]
{
    location loc    = get_location();
    ast::type ta;
}
    :   TOK_rref 
        ta = prefix_type_general_ret                { t = make_ref_type(loc, ta); }
;
rref_type_simple returns[ast::type t]
{
    location loc    = get_location();
    ast::type ta;
}
    :   TOK_rref 
        ta = prefix_type_simple_ret                { t = make_ref_type(loc, ta); }
;

if_type returns[ast::type x]
{
    location loc    = get_location();
    ast::type t1, t2, t3;
}
    :   TOK_If      t1 = type
        TOK_Then    t2 = type
        TOK_Else    t3 = type                       { x = make_if_type(loc, t1, t2, t3); }
;

tuple_type returns[ast::tuple_type t]
{
    location loc        = get_location();
    ast::tuple_type tt  = ast::tuple_type(loc, ast::tuple_type::can_expand); 
    t                   = tt;
    ast::type t1;
    ast::kind ki;
}
    :   TOK_lparen 
        (   
            //empty tuple

        |   //explicit fixity annotation

            (symbol_name_predicate) =>
            t1 = symbol_name_type

            (
                ki = kind_annot_id                  {t1 = make_annotated_type(t1, ki); }
            )?                                      {tt.add(t1);}

        |   t1 = annotated_type                     {tt.add(t1);}

            (   
                //single element tuple
                (TOK_comma TOK_rparen) =>
                TOK_comma                           { tt.set_state(ast::tuple_type::fixed); }
            |
                //multi element tuple
                (
                    TOK_comma
                    t1 = annotated_type             { tt.add(t1); } 
                )+
            |
                //paranthesized type
            )
        )

        TOK_rparen
;

bracket_type returns[ast::bracket_type bt]
{
    location loc = get_location();
    bt = ast::bracket_type(loc);
    ast::type t;
    antlr::RefToken start, end;
}
    :   start = opening_bracket_type
        
        (
            t = annotated_type                      { bt.add(t); }

            (
                TOK_comma 
                t = annotated_type                  { bt.add(t); }
            )*
        )?

        end = closing_bracket_type                  { bt = make_bracket_type(start, bt, end); }
;

product_of_types returns[ast::type t]
{
    location loc            = get_location();
    ast::product_of_types tt= ast::product_of_types(loc); 
    t = tt;   
    ast::type t1;
}
    :   TOK_lcurly

        (
            t1 = annotated_type                     { tt.add(t1); } 

            (
                TOK_comma
                t1 = annotated_type                 { tt.add(t1); } 
            )*
        )?

        TOK_rcurly                                  { t = tt; }
;

annotated_type returns[ast::type t]
{
    ast::kind ki;
}
    :   t = type
        (
            ki = kind_annot_type                    { t = make_annotated_type(t,ki); }
        )?
;

list_type returns[ast::type t]
{
    location loc        = get_location();
}
    :   TOK_list
        TOK_lbracket
        t = type
        TOK_rbracket                                { t = make_list_type(loc, t); }
;

record_type returns[ast::type t]
{
    location loc        = get_location();
    ast::record_type tt = make_record_type(loc);
    t = tt;
}
    :   TOK_Record
        TOK_lcurly
        (
            field_type[tt]
            (
                TOK_comma
                field_type[tt]
            )*
        )?
        TOK_rcurly
;

variant_type returns[ast::type t]
{
    location loc            = get_location();
    ast::variant_type tt    = make_variant_type(loc);
    t = tt;
}
    :   TOK_Variant
        TOK_lcurly
        (
            field_type_var[tt]
            (
                TOK_comma
                field_type_var[tt]
            )*
        )?
        TOK_rcurly
;

field_type[ast::record_type& rt]
{
    ast::annotated_type_identifier id;
    ast::type t;
}
    :   id = annotated_type_identifier
        TOK_equal
        t = type                                    { rt.add(id,t); }
;
field_type_var[ast::variant_type& vt]
{
    ast::annotated_type_identifier id;
    ast::type t;
}
    :   id = annotated_type_identifier
        TOK_equal
        t = type                                    { vt.add(id,t); }
;

predefined_types returns[ast::type t]
{
    location loc    = get_location();
}
    :   TOK_type_bool                               { t = make_type_bool(loc); }
    |   TOK_type_int                                { t = make_type_int(loc); }
    |   TOK_type_float                              { t = make_type_float(loc); }
    |   TOK_type_string                             { t = make_type_string(loc); }
    |   TOK_type_char                               { t = make_type_char(loc); }
;

value_type returns[ast::type t]
{
    ast::int_literal il;
    ast::bool_literal bl;
    ast::char_literal cl;
    ast::string_literal sl;
}
    :   il = atom_int[true]                         { t = make_int_type(il); }
    |   bl = atom_bool                              { t = make_bool_type(bl); }
    |   cl = atom_char                              { t = make_char_type(cl); }
    |   sl = atom_string                            { t = make_string_type(sl); }
;

//-----------------------------------------------
//          TERMS
//-----------------------------------------------

annotated_identifier_or_under returns[ast::annotated_identifier ai]
{
    location loc    = get_location();
    ast::identifier id;
    ast::type ta;
}
    :   id = term_identifier[true]
        ta = type_annot_id                          { ai = make_annotated_id(loc, id, ta); }
    |   TOK_underscore 
        ta = type_annot_id                          { ai = make_annotated_underscore(loc, ta); }
;

possibly_annotated_identifier returns[ast::annotated_identifier ai]
{
    location loc    = get_location();
    ast::identifier id;
    ast::type ta;
}
    :   id = term_identifier[true]
        (
            ta = type_annot_id
            |                                       { ta = make_type_var(get_location()); }
        )                                           { ai = make_annotated_id(loc, id, ta); }
;
annotated_identifier returns[ast::annotated_identifier ai]
{
    location loc    = get_location();
    ast::identifier id;
    ast::type ta;
}
    :   id = term_identifier[true]
        ta = type_annot_id                          { ai = make_annotated_id(loc, id, ta); }
;

annotated_term returns[ast::term x]
{
    ast::type t;
}
    :   x = term
        (
            t = type_annot_term                     { x = make_annotated_term(x, t); }
        )?
;

term returns[ast::term x]
{}
    :   x = app_term
    |   x = lambda_term
    |   x = mu_term
    |   x = all_term
    |   x = if_term
    |   x = variant_case_term
    |   x = pack_term
    |   x = unpack_term
    |   x = let_term
;

all_term returns[ast::term x]
{
    location loc                = get_location();
    ast::annotated_type_identifier it;
    ast::term t;
    type_id_vec vars;
    ast::polarity_type at = ast::polarity_type::neutral;
}
    :   TOK_all
        TOK_lbracket
            it = annotated_type_identifier          { vars.push_back(annot_polarity(it,at)); }
            (
                TOK_comma
                it = annotated_type_identifier      { vars.push_back(annot_polarity(it,at)); }
            )*
        TOK_rbracket

        t = term                                    { x = make_all_term(loc, vars, t); }
;

if_term returns[ast::term x]
{
    location loc    = get_location();
    ast::term t1, t2, t3;
}
    :   TOK_if      t1 = term
        TOK_then    t2 = term
        TOK_else    t3 = term                       { x = make_if_term(loc, t1, t2, t3); }
;

variant_case_term returns[ast::term x]
{
    location loc    = get_location();
    ast::annotated_identifier id;
    ast::term t_case;
    ast::variant_case_term vt = ast::variant_case_term(loc, ast::variant_case_term::fresh());
    x = vt;
}
    :   TOK_case 
            t_case = term                           { vt.set_term(t_case); }
        TOK_in 
        TOK_lcurly
            patterns[vt]        
        TOK_rcurly
;

patterns[ast::variant_case_term& x]
{}
    :   pattern[x]
        (
            TOK_semi
            (
                patterns[x]
            |
            )
        )?
;

pattern[ast::variant_case_term& x]
{
    ast::annotated_identifier id;
    ast::term t_alt;
}
    :   id = possibly_annotated_identifier
        TOK_double_arrow
        t_alt = term                                { x.add_pattern(id, t_alt); }
;

pack_term returns[ast::term x]
{
    location loc    = get_location();
    ast::annotated_type_identifier it;
    ast::term e;
    ast::type et;

    id_type_vec vars;
}
    :   TOK_some
        TOK_lbracket
            
            it = annotated_type_identifier
            TOK_equal
            et = type                               { vars.push_back(id_type(it,et)); }

            (
                TOK_comma
                it = annotated_type_identifier
                TOK_equal
                et = type                           { vars.push_back(id_type(it,et)); }
            )*
        TOK_rbracket

        e = term                                    { x = make_pack(loc, vars, e); }
;

unpack_term returns[ast::term x]
{
    location loc    = get_location();
    ast::annotated_type_identifier  id1;
    ast::annotated_identifier       id2;
    ast::term t1, t2;
}
    :   TOK_open
        TOK_lbracket
            id1 = annotated_type_identifier
        TOK_rbracket

        id2 = possibly_annotated_identifier 
        TOK_equal
        t1 = term 
                    
        TOK_in 
        t2 = term                                   { x = make_unpack(loc, t1, id1, id2, t2); }
;

local_definition[const kind_id_vec& vars] returns[ast::definition d]
{}
    :   d = term_definition[vars]
    |   d = operator_declaration
;

binds[ast::definitions& defs]
{
    std::vector<ast::identifier> vars;
    ast::definition d;
}
    :   (
            kind_abstraction[vars]
        )?
        (
            d = local_definition[vars]              { if (d) defs.add(d); }

        |   TOK_lcurly
            (
                (
                      d = e2:term_definition[vars] 
                      s1:TOK_semi                   { if (d) defs.add(d); }
                    | s2:TOK_semi
                )*
            )
            TOK_rcurly
        )
;
exception[s1] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[s2] catch [antlr::RecognitionException& e]{ reportError(e);}
exception[e2] catch [antlr::RecognitionException& e]{ reportError(e); consume_until_semi();}

let_term returns[ast::term x]
{
    location loc = get_location();
    ast::term body;
    ast::definitions defs(loc);
}
    :   TOK_let
        
        binds[defs]

        TOK_in
        body = term                                 { x = make_let(loc, defs, body); }
;

lambda_term returns[ast::term x]
{
    location loc        = get_location();
    ast::term body;    
    ast::annotated_identifier ai;
    std::vector<ast::annotated_identifier> vars;
    ast::type t;
}
    :   TOK_fun
        TOK_lbracket
            
            ai = annotated_identifier_or_under      { vars.push_back(ai); }
            (
                TOK_comma
                ai = annotated_identifier_or_under  { vars.push_back(ai); }
            )*

        TOK_rbracket

        (
            TOK_colon
            t = type
        )?

        TOK_double_arrow
        body = term                                 { x = make_lambda_term(loc, vars, t, body); }
;

mu_term returns[ast::term t]
{
    location loc        = get_location();
    ast::term body;
    term_id_vec vars;
    ast::annotated_identifier ai;
}
    :   TOK_rec
        TOK_lbracket
            ai = annotated_identifier               { vars.push_back(ai); }
            (
                TOK_comma
                ai = annotated_identifier           { vars.push_back(ai); }
            )*
        TOK_rbracket
        body = term                                 { t = make_mu_term(loc, vars, body); }
;

app_term returns[ast::term x]
{
    location loc = get_location();
    term_collector col(loc);
} 
    :   prefix_term[col]
        (
            app_term_tail[col]
        )?                                          { x = col.make_term(); }
;

app_term_tail[term_collector& col]
{}   
    :   prefix_term[col]
        (
            app_term_tail[col]
        )?
;

prefix_term_ret returns[ast::term x]
{
    location loc = get_location();
    term_collector col(loc);
}
    :   prefix_term[col]                            { x = col.make_term(); }
;

prefix_term[term_collector& col]
{
    ast::term x;
    ast::identifier id;
}
    :   x = ref_term                                { col.add(x); }
    |   x = deref_term                              { col.add(x); }

    |   (path_predicate) =>
        
        TOK_dot
        path_identifier[col]

        (
            (postfix_predicate) =>
            postfix[col]
        )?
    
    |   postfix_term[col]
;

ref_term returns[ast::term x]
{
    location loc    = get_location();
    ast::term t;
}
    :   TOK_ref
        t = prefix_term_ret                         { x = make_ref_term(loc, t); }
;

deref_term returns[ast::term x]
{
    location loc    = get_location();
    ast::term t;
}
    :   TOK_deref
        t = prefix_term_ret                         { x = make_deref_term(loc, t); }
;

postfix_term[term_collector& col]
{
    ast::term x;
}
    :   atomic_term[col]
        (
            (postfix_predicate) =>
            postfix[col]
        )?
;

path_predicate
{
    ast::identifier id;
}
    :   TOK_dot (id = term_identifier[false] | TOK_operator)
;

postfix_predicate
{
    ast::identifier id;
    antlr::RefToken s;
    ast::int_literal il;
}
    :   TOK_dot
        (
            TOK_langle                  //term_type_app
        |   s = opening_bracket         //bracket_term
        |   TOK_lparen                  //tuple_term
        |   il = atom_int[false]        //term_projection
        |   id = term_identifier[false] //path_identifier
        |   TOK_operator                //path_identifier
        |   TOK_kind                    //kind application
        )
    |   TOK_dot_langle                  //term_type_app
;

postfix[term_collector& col]
{
    ast::identifier id;
    ast::bracket_term bt;
    ast::tuple_term tt;
}
    :
    (
        (TOK_dot TOK_langle) =>
        term_type_app[col]

    |   (TOK_dot_langle) =>
        term_type_app[col]

    |   TOK_dot
        (
            bt = bracket_term                       {col.add_dot_brack(bt); }
        |   tt = tuple_term                         {col.add_dot_tuple(tt); }
        |   path_identifier[col]
        |   term_projection[col]
        |   term_kind_applic[col]
        )
    ) 
    (
        (postfix_predicate) =>
        postfix[col]
    )?
;

path_identifier[term_collector& col]
{
    location loc = get_location();
    ast::identifier id;
}
    :   id = term_identifier[true]                  { col.add_dot(loc, id); }

    |   TOK_operator
        TOK_lparen
            id = symbol_name                        { mark_term_identifier(id); }
        TOK_rparen                                  { col.add_dot(loc, id); }
;

term_type_app[term_collector& col]
{
    ast::location loc = get_location();
    ast::type t;
    std::vector<ast::type> args;
}
    :   (TOK_dot_langle | TOK_dot TOK_langle)
            t = type                                { args.push_back(t); }
            (
                TOK_comma
                t = type                            { args.push_back(t); }
            )*
        TOK_rangle                                  { col.add_type_applic(loc, args); }
;

term_projection[term_collector& col]
{
    ast::location loc = get_location();
    ast::int_literal pos;
}
    :   pos = atom_int[true]                        { col.add_proj(loc, pos); }
;

term_kind_applic[term_collector& col]
{
    ast::location loc = get_location();
    ast::kind ki;
    std::vector<ast::kind> args;
}
    :   TOK_kind
        TOK_lbracket
        ki = kind                                   { args.push_back(ki); }
        (
            TOK_comma
            ki = kind                               { args.push_back(ki); }
        )*
        TOK_rbracket                                { col.add_kind_applic(loc, args); }
;

atomic_term[term_collector& col]
{
    ast::identifier id;
    ast::term x;
}
    :   x = tuple_term                              { col.add(x); }
    |   x = record_term                             { col.add(x); }
    |   x = variant_term                            { col.add(x); }
    |   x = list_term                               { col.add(x); }
    |   x = bracket_term                            { col.add(x); }
    |   x = val_term                                { col.add(x); }
    |   x = literals                                { col.add(x); }
    |   id = symbol_name_atom                       { x = make_identifier_term(id); col.add(x);}
;

tuple_term returns[ast::tuple_term tt]
{
    location loc        = get_location();
    tt                  = ast::tuple_term(loc, ast::tuple_term::can_expand);
    ast::term t;  
    ast::type ty;  
}
    :   TOK_lparen 
        (
            //empty tuple

        |   //explicit fixity annotation
            (symbol_name_predicate) =>
            t = symbol_name_term

            (
                ty = type_annot_term                { t = make_annotated_term(t, ty); }
            )?                                      { tt.add(t); }

        |   t = annotated_term                      { tt.add(t); }

            (
                //paranthesized term
            |
                //single element tuple
                (TOK_comma TOK_rparen) =>
                TOK_comma                           { tt.set_state(ast::tuple_term::fixed); }

            |   //multi element term                
                (
                    TOK_comma 
                    t = annotated_term              { tt.add(t); }
                )+
            )
        )
        
        TOK_rparen
;

bracket_term returns[ast::bracket_term bt]
{
    location loc = get_location();
    bt = ast::bracket_term(loc);
    ast::term t;
    antlr::RefToken start, end;
}
    :   start = opening_bracket
        
        (
            t = annotated_term                      { bt.add(t); }

            (
                TOK_comma 
                t = annotated_term                  { bt.add(t); }
            )*
        )?

        end = closing_bracket                       { bt = make_bracket_expr(start, bt, end); }
;

val_term returns[ast::term x]
{
    location loc    = get_location();
    ast::type t;
}
    :   TOK_val
        TOK_lbracket
            t = type
        TOK_rbracket                                { x = make_val_term(loc, t); }
;

list_term returns[ast::term x]
{
    location loc        = get_location();
    ast::list_term lt   = ast::list_term(loc);    
    x = lt;
    ast::term t;    
}
    :   TOK_list
        TOK_lbracket
        (
            t = annotated_term                      { lt.add(t); }
            (
                TOK_comma 
                t = annotated_term                  { lt.add(t); }
            )* 
        )?
        TOK_rbracket                                { x = lt; }
;

record_term returns[ast::term x]
{
    location loc        = get_location();
    ast::record_term rt = ast::record_term(loc);
    x = rt;
}
    :   TOK_record
        TOK_lcurly
        (
            field_term[rt]
            (
                TOK_comma
                field_term[rt]
            )*
        )?
        TOK_rcurly
;

variant_term returns[ast::term x]
{
    location loc            = get_location();
    ast::annotated_identifier id;
    ast::type t;
    ast::term ex;
}
    :   TOK_variant
        TOK_lbracket
            t = type
        TOK_rbracket
        TOK_lcurly
            id = possibly_annotated_identifier
            TOK_equal
            ex = term                                    
        TOK_rcurly                                  { x = make_variant_term(loc, t, id, ex); }
;

field_term[ast::record_term& x]
{
    location loc = get_location();
    ast::annotated_identifier id;
    ast::term t;
}
    :   id = possibly_annotated_identifier
        TOK_equal
        t = term                                    { x.add(id,t); }
;

literals returns[ast::term x]
{}
    :   x = atom_char
    |   x = atom_int[true]
    |   x = atom_string
    |   x = atom_bool
    |   x = atom_real
;

//-----------------------------------------------
//              GENERAL SYMBOL
//-----------------------------------------------
symbol_name_term returns[ast::term x]
{
    ast::identifier id;
}
    :   id = symbol_name                            { x = make_identifier_term(id); }
;
symbol_name_type returns[ast::type t]
{
    ast::identifier id;
}
    :   id = symbol_name                            { t = make_identifier_type(id); }
;

/// used in shell; symbol_name cannot be used directly
/// since ANTLR adds additional tests at the end on this rule
root_symbol_name returns[ast::identifier vi]
{}
    :   vi = symbol_name
        (TOK_semi | )
;

symbol_name returns[ast::identifier vi]
{
    bool pref_under = false;
    bool post_under = false;
} 
    :   (
            TOK_underscore                          { pref_under = true; }
        )?

        vi = symbol_name_atom

        (
            TOK_underscore                          { post_under = true; }
        )?  
                                                    { vi = make_fixity_name(vi, pref_under, post_under); }
;

symbol_name_predicate
{
    ast::identifier vi;
}
    :   (
            TOK_underscore
        |   vi = symbol_name_atom
            TOK_underscore
        )
;

symbol_name_atom returns[ast::identifier vi]
{} 
    : vi = basic_identifier[true]
    | vi = operator_atom
    | vi = operator_bracket_dolar_atom
;

//-----------------------------------------------
//              IDENTIFIERS
//-----------------------------------------------
basic_identifier[bool build] returns[ast::identifier x]
{
    location loc = get_location();
}
    :   i1:TOK_identifier                           { if (build) x = make_basic_identifier(loc, get_text(i1)); }
;
string_identifier[std::string& x]
{}
    :   i1:TOK_identifier                           { x = get_text(i1); }
;

import_name[std::string& x]
{}
    :   string_identifier[x]
    |   i1:TOK_string                               { x = get_text(i1); }
;

kind_identifier returns[ast::identifier x]
{}
    :   x = basic_identifier[true]                  { mark_kind_identifier(x); }
;
term_identifier[bool build] returns[ast::identifier x]
{}
    :   x = basic_identifier[build]                 { mark_term_identifier(x); }
;
type_identifier[bool build] returns[ast::identifier x]
{}
    :   x = basic_identifier[build]                 { mark_type_identifier(x); }
;

//-----------------------------------------------
//              OPERATORS
//-----------------------------------------------
operator_atom returns [ast::identifier op]
{
    location loc = get_location();
    std::string name;
    bool can_cont;
}
    :   operator_atom_1[name, can_cont]             { op = make_operator_identifier(loc, name); }
;

operator_atom_1[std::string& name, bool& can_cont]
{
    can_cont = false;
}
    :   (   
            predefined_operator[name, can_cont]
        |   i1 : TOK_operator                       { name += get_text(i1); can_cont = false; }
        )

        (
            {can_cont == true}?
            (
                options{greedy = true;}:
                operator_atom_1[name, can_cont]
            |
            )
        |
        )
;
predefined_operator[std::string& name, bool& can_cont]
{}
    :   
          i1 : TOK_colon                            { name += (get_text_cont(i1, can_cont)); }
        | i2 : TOK_double_arrow                     { name += (get_text_cont(i2, can_cont)); }
        | i3 : TOK_arrow                            { name += (get_text_cont(i3, can_cont)); }
        | i4 : TOK_equal                            { name += (get_text_cont(i4, can_cont)); }
        | i5 : TOK_plus                             { name += (get_text_cont(i5, can_cont)); }
        | i6 : TOK_minus                            { name += (get_text_cont(i6, can_cont)); }
        | i7 : TOK_langle                           { name += (get_text_cont(i7, can_cont)); }
        | i8 : TOK_rangle                           { name += (get_text_cont(i8, can_cont)); }
        | i9 : TOK_star                             { name += (get_text_cont(i9, can_cont)); }
        | i10 : TOK_dot                             { name += (get_text_cont(i10, can_cont)); }

        // not allowed operator names:
        // TOK_dcolon, TOK_underscore, TOK_dot_langle
;

operator_bracket_dolar_atom returns [ast::identifier op]
{
    location loc = get_location();
    ast::identifier vi;
}
    :     i1:TOK_bracket_dolar                      { op = make_brack_op(loc, get_text(i1), false); }
        | i2:TOK_bracket_assign_dolar               { op = make_brack_assign_op(loc, get_text(i2), false); }
        | i3:TOK_dot_bracket_dolar                  { op = make_brack_assign_op(loc, get_text(i3), true); }
        | i4:TOK_dot_bracket_assign_dolar           { op = make_brack_assign_op(loc, get_text(i4), true); }
;

opening_bracket returns[antlr::RefToken t]
{}
    :     i1:TOK_lcurly                             { t = i1; }
        | i2:TOK_lbracket                           { t = i2; }
        | i3:TOK_opening_bracket                    { t = i3; }
;

closing_bracket returns[antlr::RefToken t]
{}
    :     i1:TOK_rcurly                             { t = i1; }
        | i2:TOK_rbracket                           { t = i2; }
        | i3:TOK_closing_bracket                    { t = i3; }
;

opening_bracket_type returns[antlr::RefToken t]
{}
    :   //  i1:TOK_lcurly                           { t = i1; }
          i2:TOK_lbracket                           { t = i2; }
        | i3:TOK_opening_bracket                    { t = i3; }
;

closing_bracket_type returns[antlr::RefToken t]
{}
    :   //  i1:TOK_rcurly                           { t = i1; }
        //| i2:TOK_rbracket                         { t = i2; }
         i3:TOK_closing_bracket                     { t = i3; }
;

//-----------------------------------------------
//              ATOMS
//-----------------------------------------------
atom_int[bool build] returns[ast::int_literal x]
{
    location loc = get_location();
}
    :   i1:TOK_integer                              { if (build) x = make_int(loc, get_text(i1)); }
;

atom_char returns[ast::char_literal x]
{
    location loc = get_location();
}
    :	i1:TOK_char                                 { x = make_char(loc, get_text(i1)); }
;

atom_string returns[ast::string_literal x]
{
    location loc = get_location();
}
    :	i1:TOK_string                               { x = make_string(loc, get_text(i1)); }
;

atom_bool returns[ast::bool_literal x]
{
    location loc = get_location();
}
    :	"true"                                      { x = make_boolean(loc,true); }
    |   "false"                                     { x = make_boolean(loc,false); }
;

atom_real returns[ast::term x]
{
    location loc = get_location();
}
    :	i1:TOK_number                               { x = make_float(loc, get_text(i1)); }
;

//------------------------------------------------------------------------------
//                  LEXER
//------------------------------------------------------------------------------
{
    #include "grammar/lexer_include.h"
}

class  lexer_arrow extends Lexer;

options
{
    exportVocab							= arrow_vocabulary;
    charVocabulary                      ='\u0000'..'\u00FF';
    k									= 4;
    defaultErrorHandler                 = true;
    genHashLines    					= false;  
    
    codeGenMakeSwitchThreshold 			= 5; 
    codeGenBitsetTestThreshold			= 5;	
}

tokens 
{
    //keywords

    //predefined types
    TOK_type_bool           = "bool";
    TOK_type_int            = "int";
    TOK_type_float          = "float";
    TOK_type_string         = "string";
    TOK_type_char           = "char";    

    // type and term level operators
    TOK_all                 = "All";
    TOK_some                = "Some";
    TOK_kind                = "Kind";

    TOK_Fun                 = "Fun";
    TOK_fun                 = "fun";
    TOK_Rec                 = "Rec";
    TOK_rec                 = "rec";
    TOK_rref                = "Ref";
    TOK_ref                 = "ref";
    TOK_Record              = "Record";
    TOK_record              = "record"; 
    TOK_Variant             = "Variant";   
    TOK_variant             = "variant";
    TOK_If                  = "If";
    TOK_if                  = "if";
    TOK_Then                = "Then";
    TOK_then                = "then";
    TOK_Else                = "Else";
    TOK_else                = "else";

    TOK_open                = "open";
    TOK_case                = "case";
    TOK_in                  = "in";
    TOK_type                = "type";
    TOK_val                 = "val";
    TOK_module              = "module";
    TOK_import              = "import";    
    TOK_deref               = "deref";
    TOK_list                = "list";

    TOK_let                 = "let";
    TOK_extern              = "extern";
    TOK_overload            = "over";    
    TOK_operator_declaration= "operator";

    //paraphase (space required)

    TOK_identifier          = "an identifier";

    TOK_lparen              = "left paren";
    TOK_rparen              = "right paren";
    TOK_lbracket            = "left bracket";
    TOK_rbracket            = "right bracket";
    TOK_langle              = "left angle bracket";
    TOK_rangle              = "right angle bracket";
    TOK_lcurly              = "left curly bracket";
    TOK_rcurly              = "right curly bracket";

    TOK_operator            = "operator symbol";
    TOK_opening_bracket     = "opening bracket";
    TOK_closing_bracket     = "closing bracket";
    TOK_bracket_dolar       = "bracket definition";
    TOK_bracket_assign_dolar= "bracket assign definition";
    TOK_dot_bracket_dolar       = "dot bracket definition";
    TOK_dot_bracket_assign_dolar= "dot bracket assign definition";
                    
    Any                     = "any token";
    UTF8_lead_byte          = "utf8 lead byte";
    UTF8_next_byte          = "utf8 next byte";
    UTF8_multibyte_char     = "utf8 multibyte";
    UTF8_not_line_break     = "utf8 not line break";
    UTF8_bom                = "utf8 bom";
    Shebang                 = "shebang char";

    White_space             = "whitespace char";
    Line_break              = "line break";    
    Comment                 = "comment section";
    Line_comment            = "line comment";
    Block_comment           = "block comment";
    Double_quoted_string    = "double quoted string";
    Wysiwyg_string          = "wysiwig string";
    Escape_sequence         = "escape sequence";
    Escaped_char            = "escape char";
    Dec_integer_literal     = "decimal integer";
    Bin_integer_literal     = "binary integer";
    Oct_integer_literal     = "octal integer";
    Hex_integer_literal     = "hexal iteger";
    Dec_integer             = "decimal integer value";
    Dec_digit               = "decimal digit";
    Bin_prefix              = "bin prefix";
    Bin_integer             = "bin integer";
    Bin_digit               = "bin digit";
    Oct_prefix              = "octal prefix";
    Oct_integer             = "octal integer value";
    Oct_digit               = "octal digit";
    Hex_prefix              = "hexal prefix";
    Hex_integer             = "hexal integer";
    Hex_digit               = "hexal digit";
    Dec_floating_literal    = "decimal number";
    Hex_floating_literal    = "hexal number";
    Dec_exponent            = "decimal exponent";
    Hex_exponent            = "hexal exponent";

    String_literal          = "string literal rule";
    Character_literal       = "char literal";
    Identifier              = "an id";
    Identifier_start        = "id start";
    Block_comment_content   = "block comment content";

    Identifier_ascii        = "identifier ascii";
    Identifier_ascii_start  = "identifier ascii start";
    Operator_ascii_char     = "operator ascii character";
    Operator                = "an operator";
    Operator_char           = "operator character";

    TOK_integer             = "integer literal";
    TOK_char                = "character literal";
    TOK_string              = "string literal";
    TOK_number              = "number literal";
}

{
    #include "grammar/lexer_members.h"
}

TOK_lparen  : "(";
TOK_rparen  : ")";
TOK_lbracket: "[";
TOK_rbracket: "]";
//TOK_langle  : "<";
//TOK_rangle  : ">";
TOK_lcurly  : "{";
TOK_rcurly  : "}";

TOK_comma   options{ paraphrase = ","; }: ",";
TOK_semi    options{ paraphrase = ";"; }: ";";

protected TOK_arrow         options{ paraphrase = "->"; }   :;
protected TOK_double_arrow  options{ paraphrase = "=>"; }   :;
protected TOK_equal         options{ paraphrase = "="; }    :;
protected TOK_underscore    options{ paraphrase = "_"; }    :;
protected TOK_dot_langle    options{ paraphrase = ".<"; }   :;
protected TOK_star          options{ paraphrase = "*"; }    :;
protected TOK_dot           options{ paraphrase = "."; }    :;
protected TOK_plus          options{ paraphrase = "+"; }    :;
protected TOK_minus         options{ paraphrase = "-"; }    :;
protected TOK_colon         options{ paraphrase = ":"; }    :;
protected TOK_dcolon        options{ paraphrase = "::"; }   :;

Any
{}
    :   { getLine() <= 1 && getColumn() <= 1 }? 
        UTF8_bom                                    { $setType(White_space); }

    |   { getLine() <= 1 && getColumn() <= 1 }? 
        Shebang                                     { $setType(Comment); }

    |   ( ' ' | '\t' | '\n' | '\f' | '\r' ) => 
        White_space                                 { $setType(White_space); }

    |   ( Identifier_start ) => 
        Identifier                                  { $setType(TOK_identifier); }

    |   ( "//" | "/*" ) => 
        Comment                                     { $setType(Comment); }

    |   ( '"' ) => 
        String_literal                              { $setType(TOK_string); }

    |   ( Bin_prefix Bin_digit ) => 
        Bin_integer_literal                         { $setType(TOK_integer); }

    |   ( Oct_prefix Oct_digit) => 
        Oct_integer_literal                         { $setType(TOK_integer); }

    |   ( Hex_floating_literal ) => 
        Hex_floating_literal                        { $setType(TOK_number); }

    |   ( Dec_floating_literal ) => 
        Dec_floating_literal                        { $setType(TOK_number); }

    |   ( Hex_prefix Hex_digit ) => 
        Hex_integer_literal                         { $setType(TOK_integer); }

    |   Dec_integer_literal                         { $setType(TOK_integer); }

    |   ('\u0000' | '\u001a')                       { $setType(antlr::Token::EOF_TYPE); }

    |   ( Character_literal ) => 
        Character_literal                           { $setType(TOK_char); }

    |   "_"                                         { $setType(TOK_underscore); }

    |   ("$.") =>
        ("$.")
        (   
            ("()" | "{}" | "[]" | "<>") =>          { pop_symbol(2); push_symbol("."); }
            (     "()"
                | "{}"
                | "[]"
                | "<>"
            )
            (
                Operator                            { $setType(TOK_dot_bracket_assign_dolar); }
                |                                   { $setType(TOK_dot_bracket_dolar); }
            )

        |   (UTF8_lead_byte) =>                     
            { is_opening_bracket(check_next_code_point()) }?
                                                    { pop_symbol(2); push_symbol("."); }
                UTF8_multibyte_char
                
                { is_closing_bracket(check_next_code_point()) }?
                UTF8_multibyte_char
                
                (
                    Operator                        { $setType(TOK_dot_bracket_assign_dolar); }
                    |                               { $setType(TOK_dot_bracket_dolar); }
                )             

        | (Operator)?                               { $setType(TOK_operator); }
        )

    |   ('$') =>
        ('$')
        (   
            ("()" | "{}" | "[]" | "<>") =>          { pop_symbol(1); }
            (     "()"
                | "{}"
                | "[]"
                | "<>"
            )
            (
                Operator                            { $setType(TOK_bracket_assign_dolar); }
                |                                   { $setType(TOK_bracket_dolar); }
            )

        |   (UTF8_lead_byte) =>                     
            { is_opening_bracket(check_next_code_point()) }?
                                                    { pop_symbol(1); }
                UTF8_multibyte_char
                
                { is_closing_bracket(check_next_code_point()) }?
                UTF8_multibyte_char
                
                (
                    Operator                        { $setType(TOK_bracket_assign_dolar); }
                    |                               { $setType(TOK_bracket_dolar); }
                )             

        | (Operator)?                               { $setType(TOK_operator); }
        )

    |   (Operator_ascii_char ) => 
        (
            (Predefined_operator) =>
            Predefined_operator
        |   Operator
        )

        {
            std::string s = $getText;
            if (s == ":")                           { $setType(TOK_colon); }
            else if (s == "::")                     { $setType(TOK_dcolon); }
            else if (s == "=>")                     { $setType(TOK_double_arrow); }
            else if (s == "->")                     { $setType(TOK_arrow); }
            else if (s == "=")                      { $setType(TOK_equal); }
            else if (s == "_")                      { $setType(TOK_underscore); }
            else if (s == "+")                      { $setType(TOK_plus); }
            else if (s == "-")                      { $setType(TOK_minus); }            
            else if (s == "<")                      { $setType(TOK_langle); }
            else if (s == ">")                      { $setType(TOK_rangle); }
            else if (s == ".<")                     { $setType(TOK_dot_langle); }
            else if (s == "*")                      { $setType(TOK_star); }
            else if (s == ".")                      { $setType(TOK_dot); }

            else                                    { $setType(TOK_operator); }
        }

    |   ( UTF8_lead_byte ) =>

        (   
                { is_white_space(check_next_code_point()) }? 
                White_space                         { $setType(White_space); }

            |   { is_identifier_start(check_next_code_point()) }? 
                Identifier                          { $setType(TOK_identifier); }

            |   { is_operator_char(check_next_code_point()) }? 
                Operator                            { $setType(TOK_operator); }

            |   { is_opening_bracket(check_next_code_point()) }?             
                UTF8_multibyte_char                 { $setType(TOK_opening_bracket); }

            |   { is_closing_bracket(check_next_code_point()) }?
                UTF8_multibyte_char                 { $setType(TOK_closing_bracket); }

        )
;

// UTF-8

protected UTF8_lead_byte
    :   '\u00C2'..'\u00F4'
;

protected UTF8_next_byte
    :   '\u0080'..'\u00BF'
;

protected UTF8_multibyte_char
    :   '\u00C2'..'\u00DF' 
        UTF8_next_byte                              { setColumn(getColumn() - 1); }

    |   '\u00E0'..'\u00EF' 
        UTF8_next_byte 
        UTF8_next_byte                              { setColumn(getColumn() - 2); }

    |   '\u00F0'..'\u00F4' 
        UTF8_next_byte
        UTF8_next_byte 
        UTF8_next_byte                              { setColumn(getColumn() - 3); }
;

protected UTF8_bom
    :   '\u00EF' '\u00BB' '\u00BF'                  { setColumn(getColumn() - 3); /* 0 width */ } // U+FEFF
    ;

// WHITESPACE

protected White_space
    :   (   
                '\t' | ' '
            |   ( Line_break ) => Line_break
            |   ( UTF8_lead_byte ) => 
                    { is_white_space(check_next_code_point()) }? 
                        UTF8_multibyte_char
        )+
;

protected Line_break
    :   '\n'                                        { newline(); }
    |   '\f'                                        { newline(); }
    |   ( "\r\n" ) => "\r\n"                        { newline(); }
    |   "\r"                                        { newline(); }
    |   '\u00c2' '\u0085'                           { newline(); } 
        // U+0085: NEXT LINE (NEL)
    |   '\u00e2' '\u0080' '\u00a8'                  { newline(); } 
        // U+2028: LINE SEPARATOR
    |   '\u00e2' '\u0080' '\u00a9'                  { newline(); } 
        // U+2029: // PARAGRAPH SEPARATOR
;

protected UTF8_not_line_break
    :   ( ~( '\u0000'..'\u007F' | '\u00c2' | '\u00e2' ) )
             => UTF8_multibyte_char
    |   ( '\u00c2' ~'\u0085' )
              => UTF8_multibyte_char
    |   ( '\u00e2' ~'\u0080' )  
            => UTF8_multibyte_char
    |   ( '\u00e2' '\u0080' ~( '\u00a8' | '\u00a9' ) ) 
            => UTF8_multibyte_char
    ;


// COMMENT

protected Shebang
    :   "#!"
        (  
             ~( '\u0000' | '\u001a' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   ( UTF8_not_line_break ) 
                    => UTF8_not_line_break
        )*
;

protected Comment
    :   Line_comment
    |   Block_comment
;

protected Line_comment
    :   "//"
        (   
            ~( '\u0000' | '\u001a' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   ( UTF8_not_line_break ) 
                    => UTF8_not_line_break
        )*
;

protected Block_comment
    :   "/*" Block_comment_content 
        ( 
            { LA(1) == EOF && allow_unterminated_comments }?
                /*nothing*/ 
            | "*/" 
        )
;

protected Block_comment_content
    :   (   
            options{greedy=true;} :   

                ~( '*' | '/' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   ('*' ~('/')) => '*'
            |   ('/' ~('*')) => '/'
            |   ( Line_break ) => Line_break
            |   UTF8_multibyte_char
            |   Block_comment
        )*
;

// STRING LITERAL

protected String_literal
{}
    :   ( "\"\"\"" ) => Wysiwyg_string
    |   ( '"' ) => Double_quoted_string
;

protected Double_quoted_string
{}
    :   '"'
        (   
                ~( '"' | '\\' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   Escape_sequence
            |   ( Line_break ) => Line_break
            |   UTF8_multibyte_char
        )*
        '"'
;

protected Wysiwyg_string
{}
    :   "\"\"\""
        (   
                ~( '"' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   ( '"' ~'"' ) => '"'
            |   ( '"' '"' ~'"' ) => '"' '"'
            |   ( Line_break ) => Line_break
            |   UTF8_multibyte_char
        )*
        "\"\"\""
;


// CHARACTER LITERAL

protected Character_literal
{}
    :   '\''
        (  
                 ~( '\\' | '\'' | '\n' | '\f' | '\r' | '\u0080'..'\u00FF' )
            |   Escape_sequence
            |   UTF8_not_line_break
        )
        '\''
;

protected Escape_sequence
    :   '\\'
        (   Escaped_char
        |   'u' Hex_digit Hex_digit Hex_digit Hex_digit
        |   'U' Bin_digit Hex_digit Hex_digit Hex_digit Hex_digit Hex_digit
        )
;

protected Escaped_char
    :   '"' | '\'' | '\\' | 'a' | 'b' | 't' | 'n' | 'v' | 'f' | 'r' | 'e' | '0'
;


// INTEGER LITERAL

protected Dec_integer_literal
    :   Dec_integer
;

protected Dec_integer
    :   Dec_digit 
        ( 
            ( '_' )? Dec_digit 
        )*
;

protected Dec_digit
    :   '0'..'9'
;

protected Bin_integer_literal
    :   Bin_prefix 
        Bin_integer
;

protected Bin_prefix
    :   '0' 
        ( 'b' | 'B' )
;

protected Bin_integer
    :   Bin_digit 
        ( 
            ( '_' )? 
            Bin_digit 
        )*
;

protected Bin_digit
    :   '0' 
    |   '1'
;

protected Oct_integer_literal
    :   Oct_prefix 
        Oct_integer
;

protected Oct_prefix
    :   '0' 
        ( 'o' | 'O' )
;

protected Oct_integer
    :   Oct_digit 
        ( 
            ( '_' )? 
            Oct_digit 
        )*
;

protected Oct_digit
    :   '0'..'7'
;

protected Hex_integer_literal
    :   Hex_prefix 
        Hex_integer
;

protected Hex_prefix
    :   '0' 
        ( 'x' | 'X' )
;

protected Hex_integer
    :   Hex_digit 
        ( 
            ( '_' )? 
            Hex_digit 
        )*
    ;

protected Hex_digit
    :   '0'..'9' 
    |   'a'..'f' 
    |   'A'..'F'
;


// FLOATING LITERAL

protected Dec_floating_literal
    :   ( Dec_integer '.' ) => 
        Dec_integer '.' Dec_integer 
        ( Dec_exponent )?
    
    |   ( Dec_integer ( 'e' | 'E' ) ) => 
        Dec_integer 
        Dec_exponent
;

protected Dec_exponent
    :   ( 'e' | 'E' ) 
        ( '+' | '-' )? 
        Dec_integer
;

protected Hex_floating_literal
    :   ( Hex_prefix Hex_integer '.' ) => 
        Hex_prefix 
        Hex_integer 
        '.' 
        Hex_integer 
        ( Hex_exponent )?

    |   ( Hex_prefix Hex_integer ( 'p' | 'P' ) ) => 
        Hex_prefix 
        Hex_integer 
        Hex_exponent
;

protected Hex_exponent
    :   ( 'P' | 'p' ) 
        ( '+' | '-' )? 
        Dec_integer
;

// IDENTIFIER

protected Identifier
    : Identifier_start
        (   
                Identifier_ascii
            |   Dec_digit
            |   ( UTF8_lead_byte ) => 
                { is_identifier_part(check_next_code_point()) }? 
                    UTF8_multibyte_char
        )*
;

protected Identifier_start
    :   Identifier_ascii_start
    |   ( UTF8_lead_byte ) => 
            UTF8_multibyte_char
;

protected Identifier_ascii
    : 'a'..'z' | 'A'..'Z' | '_'
;

protected Identifier_ascii_start
    : 'a'..'z' | 'A'..'Z'
;

// OPERATOR

protected Operator
    : ( Operator_char)+
;

protected Predefined_operator
    : ":"
    | "::"
    | "=>"
    | "->"
    | "="
    | "+"
    | "-"
    | "<"
    | ">"
    | ".<"
    | "*"
    | "."
;

protected Operator_char
    :   Operator_ascii_char
    |   ( UTF8_lead_byte ) => 
            { is_operator_char(check_next_code_point()) }? 
                UTF8_multibyte_char
;

protected Operator_ascii_char
    :   '~' | '`' | '!' | '@' | '#' | '$' | '%' | '^' | '&' | '*' | '-' | '+' | '=' 
    | '/'   | '|' | '\\'| ':' | '\''| '<' | '>' | '.' | '?'     
    
    // '(', ')', '{', '[', '}', ']' 
    //not allowed in operators: ["] [,] [;] [_]
;

protected EOF options{ paraphrase = "end of file"; }: ;
