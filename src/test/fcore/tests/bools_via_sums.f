type Unit   = All[A] A -> A;
type Bool2  = Variant{t = Unit, f = Unit};

if2         = fun[b : Bool2] => All[A] fun[r : A] => (fun [s : A] => (case b in {t => s; f => r}));
unit        = All[A] fun[a : A] => a;

true2       = variant[Bool2]{t = unit}; 
false2      = variant[Bool2]{f = unit}; 

not         = fun[b : Bool2] => if2 b false2 true2;
and         = fun[x : Bool2, y : Bool2] => if2 x y false2;
or          = fun[x : Bool2, y : Bool2] => if2 x true2 y;
