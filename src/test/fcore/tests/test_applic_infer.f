r       : All[a,b,c] (a->int->b) -> c;
u       : int -> int -> int;
g4      = r u;

Pair    : All[a,b] a->b->(a,b);

x       : int;
y       : bool;

g3      = Pair x (Pair x (Pair y));

fst : All[a,b] (a, b) -> a;
g1      = fst (3, true);

type List[a];
Nil     : All[a] List a;
Cons    : All[a] a -> List a -> List a;

map     : All[a,b] (a -> b) -> List a -> List b;
treble  : int -> int;
g2      = map treble;

r2      : All[a,b] (int->a) -> string;
u2      : int -> bool;
g5      = r2 u2;

g6      = Pair.<int,bool>;
g7      = Pair.<int, bool> x;
g8      = Pair x;
g9      = (Pair x).<bool>;
g10     = (Pair x).<bool> y;
g11     = Pair x y;
