operator {kind = infix, assoc = left, prec = add} op1; 
operator {kind = prefix} op1;  
operator {kind = postfix} op1;  

op1 [x:int, y:int] => (x,y);
_ op1 [x:int] => (x, "postfix");
val (op1) 1;
val (_ op1) 1;

operator {kind = infix, assoc = left, prec = add} op2; 
operator {kind = prefix} op2;  
operator {kind = postfix} op2;  

op2 [x:int, y:int] => (x,y);
_ op2   [x:int] => (x, "postfix");
op2 _   [x:int] => (x, "prefix");

val (_ op2 _) 1 2;
val (_ op2) 1;
val (op2 _) 1;
val (op2) 1 2;
val 1 op2 2;

operator {kind = indexing} $.();
_$.()   = fun[x: int, arg : string] => (x,arg);
y1      = 1 .("a");

operator {kind = indexing} $.()=;
_$.()=  = fun[x: int, arg : string, rhs:bool] => (x,arg, rhs);
y2      = 1 .("a") = true;

// test binders (missing value test)
f1      = All[X] fun[x:X, y:int] => (x,y);
g1      = f1 f1;
val g1 1;

int_minus   : int->int->int = extern;
int_cmp_eeq : int->int->bool = extern;

operator{kind = infix, prec = add} -;
operator{kind = infix, prec = compare} ==;

- = int_minus;
== = int_cmp_eeq;

f2 : All[X] int->int = All[X] fun[x:int] => if x == 0 then 0 else f2.<list[X]>(x-1);
val f2.<string> 4;

type List : *->*   
            = Fun[A] => Variant{cons = (A, List A), nil = ()};
cons        = All[A] fun[head : A, tail : List A] => variant[List A]{cons = (head, tail)};
nil         = All[A] variant[List A]{nil = ()};
val cons(1, cons(2, cons(4, nil.<int>)));

Kind[k] type H[X:k] => int;
Kind[k] f3  = All[X:k] fun[x : H X] => 1;
g3          = f3.Kind[*] 1;

f4          = All[Y] fun[y:Y] => let l = All[X] fun[x:X] => (x,y) in l 1;
val f4 "a";

f5          = All[Y] fun[y:Y] => let l = All[X] fun[x:X] => (x,y) in (l 1, l "b");
val f5 "a";

operator {kind = prefix} ++;
f6          = fun[x1:int, x2:int] => let {++_ = fun[x:int] => x1; ++_ = fun[x:string] => x2;} in (++"a", ++1);
val f6 1 2;

f7          = fun[x1:int, x2:int] => let {over g7; g7 = fun[y:int] => (x1,y);g7 = fun[y:string] => (x2,y);}in (g7 1, g7 "a");
val f7 1 2;

over g8;
f8          = fun[x1:int, x2:int] => let {g8 = fun[y:int] => (x1,y);g8 = fun[y:string] => (x2,y);}in (g8 "a", g8 1);
