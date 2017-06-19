Kind[k]
type ID : k -> *;

Kind[k]
id          : All[X:k] (ID X);

type R1_1   = Rec[X] Ref (Ref X);
type R1_2   = Ref (Ref (Rec[X] Ref (Ref X)));
type R2_1   = Rec[X] Rec[Y] (X, Y);
type R2_2   = Rec[Y] (Rec[X] Rec[Y1] (X, Y1), Y);

test1       : ID R1_1
            = id.<R1_2>;

test2       : ID R2_1
            = id.<R2_2>;
            
type x1     = Fun[f: * +-> *] => (Rec[X] Ref (f X));
type x2     = x1 (Fun[+X] => X);

type F5     = Rec[X] (X -> int) -> F4;
type F4     = Rec[X] (X -> int) -> F3;
type F3     = Rec[X] (X -> int) -> F2;
type F2     = Rec[X] (X -> int) -> F1;
type F1     = Rec[X] (int,X);

//type H5   = Rec[X] (X -> int) -> H4;
type H5     = ((Rec[X] (X -> int) -> H4) -> int) -> H4;
type H4     = Rec[X] (X -> int) -> H3;
type H3     = Rec[X] (X -> int) -> H2;
type H2     = Rec[X] (X -> int) -> H1;
type H1     = Rec[X] (int, X );

test        : ID F5
            = id.<H5>;

type Prod : * +-> * +-> *
    = Fun[+X,+Y] => All[Z] (X -> Y -> Z) -> Z;

type List3      = Fun[A : *] => Rec[List3 : * +-> *] Prod (Ref (List3 A));
test5           = fun[x : List3 int int] => x;
    
type List2      = All[A] Rec[T : *] Variant{l1 = (), l2 = (A,T)};

type T      = Fun[X : {*,*,*,*,*,*,*,*,*}] => {X.0, X.1, X.8};
type R1     = Fun[X : {*,*}] => {(X.0,X.1), (X.1, X.0)}.0;
type R2     = R1 {int,bool};
type R3     = Rec[X : {*,*,*}] {Ref X.1, Ref X.2, Ref X.0};
type R4     = Rec[X : {*,*,*}] {Ref X.1, Ref X.0, Ref X.1};
type R5     = { Ref (Rec[X : {*,*,*}] {Ref X.1, Ref X.2, Ref X.0}).1, 
                Ref (Rec[X : {*,*,*}] {Ref X.1, Ref X.2, Ref X.0}).2, 
                Ref (Rec[X : {*,*,*}] {Ref X.1, Ref X.2, Ref X.0}).0
              };

test10     : ID R3.0
            = id.<R5.0>;
            
type List[A] :* => Variant {nil = (), cons = (A, List A) };
nil             = All[A] variant[List A]{nil = ()};
cons            = All[A] fun[head : A, tail : List A] => variant[List A]{cons = (head, tail)};
val cons 3 (cons 2 (cons 1 nil.<int>));