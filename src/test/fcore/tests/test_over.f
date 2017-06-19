//other encoding
over f;

f       = fun[x:bool] => true;
f       = fun[x:string] => "";
f       : int->bool 
        = fun[x:int] => f true;

type F : *->*
        = Fun[X] => (F X,);
y       : F int;
f1      = fun[x : F int] => x;
val f1 y;

type F1 = Fun[X : *->*] => X;
type G1 = Fun[+X] => X;
type H1 = F1 G1;

f2      = All[X : *->*] 1;
g2      = f2.<Fun[+X] => X>;

type F2 : (* +-> *) -> * = Fun[Y : *->*] => int;
type F3 : * -> * = Fun[+X] => X;

over f3;
f3      = All[X] fun[x:X] => 1;
f3      = fun[x:int] => 2;
g3      = All[Y] fun[y:Y] => f3 y;
val g3 "a";
val g3 1;
 
over f4 : All[X] X -> X;
g4      = fun[x:int] => All[Y] fun[y:Y] => (f4 y, x); 
f4      = All[X] fun[x:X] => x;
val g4 1 "a";

over f5 : All[X] X -> X;
g5      = fun[x:int] => All[Y] fun[y:Y] => (f5 y, x); 
f5      = All[X] fun[x:X] => x;
h5      = g5 1;
h5_2    = h5 "a";
val h5_2;