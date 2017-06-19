module predefined;

//integer arithmetics
int_plus    : int->int->int = extern;               // x + y
int_minus   : int->int->int = extern;               // x - y
int_mult    : int->int->int = extern;               // x * y
int_div     : int->int->int = extern;               // x / y
int_uminus  : int->int = extern;                    // -x

//integer comparison
int_cmp_eeq : int->int->bool = extern;              // x == y
int_cmp_neq : int->int->bool = extern;              // x ~= y
int_cmp_leq : int->int->bool = extern;              // x <= y
int_cmp_geq : int->int->bool = extern;              // x >= y
int_cmp_lt  : int->int->bool = extern;              // x < y
int_cmp_gt  : int->int->bool = extern;              // x > y

//boolean 
bool_and    : bool->bool->bool  = extern;           // x && y
bool_or     : bool->bool->bool  = extern;           // x || y
bool_xor    : bool->bool->bool  = extern;           // x ^ y
bool_not    : bool->bool  = extern;                 // ~x
bool_cmp_eeq : bool->bool->bool = extern;           // x == y
bool_cmp_neq : bool->bool->bool = extern;           // x ~= y

//string
string_plus     : string->string->string = extern;  // x + y
string_char_plus: string->char->string = extern;    // x + y
char_string_plus: char->string->string = extern;    // x + y
string_cmp_eeq  : string->string->bool = extern;    // x == y
string_cmp_neq  : string->string->bool = extern;    // x ~= y
string_cmp_geq  : string->string->bool = extern;    // x >= y
string_cmp_leq  : string->string->bool = extern;    // x <= y
string_cmp_lt   : string->string->bool = extern;    // x > y
string_cmp_gt   : string->string->bool = extern;    // x < y

//char
char_plus       : char->char->string = extern;      // x + y
char_cmp_eeq    : char->char->bool = extern;        // x == y
char_cmp_neq    : char->char->bool = extern;        // x ~= y
char_cmp_geq    : char->char->bool = extern;        // x >= y
char_cmp_leq    : char->char->bool = extern;        // x <= y
char_cmp_lt     : char->char->bool = extern;        // x > y
char_cmp_gt     : char->char->bool = extern;        // x < y

//float arithmetics
float_plus      : float->float->float = extern;     // x + y
float_minus     : float->float->float = extern;     // x - y
float_mult      : float->float->float = extern;     // x * y
float_div       : float->float->float = extern;     // x / y
float_uminus    : float->float = extern;            // -x

//float constants
float_inf       : float = extern;                   //inf
float_nan       : float = extern;                   //nan

//float comparison
float_cmp_eeq   : float->float->bool = extern;      // x == y
float_cmp_neq   : float->float->bool = extern;      // x ~= y
float_cmp_leq   : float->float->bool = extern;      // x <= y
float_cmp_geq   : float->float->bool = extern;      // x >= y
float_cmp_lt    : float->float->bool = extern;      // x < y
float_cmp_gt    : float->float->bool = extern;      // x > y

//---------------------------------------------------------------------
//                  TYPE FUNCTIONS
//---------------------------------------------------------------------

//integer arithmetics
type ty_int_plus    : int->int->int = extern;       // x + y
type ty_int_minus   : int->int->int = extern;       // x - y
type ty_int_mult    : int->int->int = extern;       // x * y
type ty_int_div     : int->int->int = extern;       // x / y
type ty_int_uminus  : int->int = extern;            // -x

//integer comparison
type ty_int_cmp_eeq : int->int->bool = extern;      // x == y
type ty_int_cmp_neq : int->int->bool = extern;      // x ~= y
type ty_int_cmp_leq : int->int->bool = extern;      // x <= y
type ty_int_cmp_geq : int->int->bool = extern;      // x >= y
type ty_int_cmp_lt  : int->int->bool = extern;      // x < y
type ty_int_cmp_gt  : int->int->bool = extern;      // x > y

//boolean 
type ty_bool_and    : bool->bool->bool  = extern;   // x && y
type ty_bool_or     : bool->bool->bool  = extern;   // x || y
type ty_bool_xor    : bool->bool->bool  = extern;   // x ^ y
type ty_bool_not    : bool->bool        = extern;   // ~x
type ty_bool_cmp_eeq: bool->bool->bool  = extern;   // x == y
type ty_bool_cmp_neq: bool->bool->bool  = extern;   // x ~= y

//string
type ty_string_plus     : string->string->string = extern;  // x + y
type ty_string_char_plus: string->char->string = extern;    // x + y
type ty_char_string_plus: char->string->string = extern;    // x + y
type ty_string_cmp_eeq  : string->string->bool = extern;    // x == y
type ty_string_cmp_neq  : string->string->bool = extern;    // x ~= y
type ty_string_cmp_geq  : string->string->bool = extern;    // x >= y
type ty_string_cmp_leq  : string->string->bool = extern;    // x <= y
type ty_string_cmp_lt   : string->string->bool = extern;    // x > y
type ty_string_cmp_gt   : string->string->bool = extern;    // x < y

//char
type ty_char_plus       : char->char->string = extern;      // x + y
type ty_char_cmp_eeq    : char->char->bool = extern;        // x == y
type ty_char_cmp_neq    : char->char->bool = extern;        // x ~= y
type ty_char_cmp_geq    : char->char->bool = extern;        // x >= y
type ty_char_cmp_leq    : char->char->bool = extern;        // x <= y
type ty_char_cmp_lt     : char->char->bool = extern;        // x > y
type ty_char_cmp_gt     : char->char->bool = extern;        // x < y

//---------------------------------------------------------------------
//                  OPERATORS
//---------------------------------------------------------------------
operator {kind = infix, assoc = none, prec = compare}   ==, !=, <=, >=, <, >, /=;
operator {kind = infix, assoc = left, prec = mul}       *, /;
operator {kind = infix, assoc = left, prec = add}       +, -;
operator {kind = infix, assoc = right, prec = range}    :; 
operator {kind = infix, assoc = right, prec = and}      &&;
operator {kind = infix, assoc = right, prec = or}       ||;    
operator {kind = prefix}                                -;  
  
//---------------------------------------------------------------------
//                  OPERATORS DEFINITIONS
//---------------------------------------------------------------------  
// operator ==
== = int_cmp_eeq;
== = string_cmp_eeq;
== = bool_cmp_eeq;
== = char_cmp_eeq;
== = float_cmp_eeq;

// operator !=
!= = int_cmp_neq;
!= = string_cmp_neq;
!= = bool_cmp_neq;
!= = char_cmp_neq;
!= = float_cmp_neq;

// operator >=
>= = int_cmp_geq;
>= = string_cmp_geq;
>= = char_cmp_geq;
>= = float_cmp_geq;

// operator <=
<= = int_cmp_leq;
<= = string_cmp_leq;
<= = char_cmp_leq;
<= = float_cmp_leq;

// operator >
> = int_cmp_gt;
> = string_cmp_gt;
> = char_cmp_gt;
> = float_cmp_gt;

// operator <
< = int_cmp_gt;
< = string_cmp_gt;
< = char_cmp_gt;
< = float_cmp_gt;

// operator +
+ = int_plus;
+ = string_plus;
+ = char_plus;
+ = float_plus;

// operator -
- = int_minus;
- = float_minus;

// operator *
* = int_mult;
* = float_mult;

// operator /
/ = int_div;
/ = float_div;

// operator &&
&& = bool_and;

// operator ||
|| = bool_or;

// unary -
- _ = int_uminus;
- _ = float_uminus;

/*
: :: a -> [a] -> [a];
: x y           = [x];
*/    

//---------------------------------------------------------------------
//                  TYPE OPERATORS DEFINITIONS
//---------------------------------------------------------------------  
// TODO: add overloads, when overloading of type functions will be
// allowed

type + = ty_int_plus;
type - = ty_int_minus;
type * = ty_int_mult;
type / = ty_int_div;

type == = ty_int_cmp_eeq;
type != = ty_int_cmp_neq;
type <= = ty_int_cmp_leq;
type >= = ty_int_cmp_geq;
type <  = ty_int_cmp_lt;
type >  = ty_int_cmp_gt;

type && = ty_bool_and;
type || = ty_bool_or;
type ^  = ty_bool_xor;
type ~  = ty_bool_not;
