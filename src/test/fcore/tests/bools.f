type Bool2 = All[a] a -> a -> a;

true2   = All[a] fun[t : a, f : a] => t;
false2  = All[a] fun[t : a, f : a] => f;
if2     = fun[b : Bool2] => b;
if3     = fun[b : Bool2] => b.<Bool2>;

and     = fun[x : Bool2, y : Bool2] => x y false2;
or      = fun[x : Bool2, y : Bool2] => x true2 y;
not     = fun[b : Bool2]            => if2 b false2 true2;
