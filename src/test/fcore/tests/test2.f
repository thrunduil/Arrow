type Arrow_k  : * --> * +-> *;

Kind[k] 
type All_t  : (k --> *) +-> *;

Kind[l]
type T1     = All_t.Kind[*->l];

type Arrow  : * --> * +-> *;

Kind[k] 
type Mu     : int +-> (k +-> k) +-> k;

Kind[k] 
type Nu     : int --> (k +-> k) +-> k;

type S      : int +-> int
            = Fun[+X : int] => 1;
type Inf    : int
            = 1;

Kind[k]
type All_t2[+X :(k --> *)];

type Prod : * +-> * +-> *
    = Fun[+X,+Y] => All[Z] (X -> Y -> Z) -> Z;

type Sum : * +-> * +-> *
    = Fun[+X,+Y] => All[Z] (X -> Z) -> (Y -> Z) -> Z;
    
type Exi : (* -> *) +-> *
    = Fun[+F : *->*] => All[Z] (All[X] F X -> Z) -> Z;
    
Kind[k]          
type Exi2 : (k -> *) +-> *
    = Fun[+F : k -> *]  => All[Z] (All[X : k] F X -> Z) -> Z;    

type Nat : int +-> *;
type List: int +-> * +-> *;
type Tree: int +-> * --> * +-> *;
type Ord : int +-> *;

type PList : int +-> * +-> *;
type Bush: int +-> * +-> *;
type Lam: int +-> * +-> *;

type Prod2 : * +-> * +-> *
    = Fun[+X : *,+Y]  => All[Z] (X -> Y -> Z) -> Z;

type List3[A] : * => Prod A (List3 A,);

type List4[A :*];
type primRec = All[A,B] B -> (A -> List4 A -> B -> B) -> List4 A -> B;

type Cow[a] => ((a -> bool) -> bool);

type Func1  : (* --> * +-> *) -> *;
type E1     = Func1 (Fun[-Arg1, +Ret] => bool);

type Func   : * --> * +-> *;
type App    = Fun[Arg1, Ret] => Func Arg1 Ret;

type Stream : int --> * +-> *;          

//power list: data Powl a = PCons a (Powl(a,a)) | PNil
type Powl   = Fun[A] => All[X:*->*] (A -> X (A, A) -> X A) -> X A -> X A;
    
//Leibniz equality
Kind[k]
type Eq[a :k, b :k] => All[f:k->*] f a -> f b;

Kind[l]
type L1     : l -> *
            = Fun[Y : l] => All_t (Fun[-X] => X -> int);
type L2     = L1 int;
t51         = fun[x: L2] => x;

type Arrow_k2[-F : * --> *, -Arg];
type H      = Fun[X, -Y] => Y -> X;
type K1     = Fun[X] => Arrow_k2 (H X) X;
type K1_2   = Fun[X] => Arrow_k2 ((Fun[X1, -Y] => Y -> X1) X) X;

type K3     = Fun[X] => Arrow_k2 (Fun[-Y] => Y -> X) X;
type K4     = K3 int;
t5          = fun[x: K4] => x;
            
Kind[k]
type F      : k -> k
            = Fun[X:k] => X;
type G      : *->*
            = Fun[X] => X;
t6          = fun[x : (F G) int] => x;
            
r2          : All[a,b] (int->a) -> string;

Kind[l]
t7          = All[Y : l] fun[x : (Fun[T : l] => int) Y] => x;
t8          = t7.<int>;

Kind[k] 
t1          = All[X : k->*, Y : k, Z] fun[x : Z -> X Y, z : Z] => x z;

Kind[k] 
t2          = All[X : k->*, Y : k, Z] fun[x : Z -> X Y, z : Z] => x z;
t3          = t2.<Fun[X] =>int>;

y1      : All[T] T
        = let 
        {
            a1 : All[T] T 
                = if true then y1 else a1;
        }
        in a1;
        
y2      : All[T] T
        = y2;

type Id = All[T] T;

x1      : All[T] T
        = let 
        {
            a1 : Id = let x = if true then x1 else a2 in x;
            a2 : Id = let x = a3 in x;
            a3 : Id = let x = a4 in x;
            a4 : Id = let x = a1 in x;
        }
        in a4;
        
h1      : All[T] T -> T
        = All[T] fun[x : T] => h1 x;

f       : All[T] T
        = g;
g       : All[T] T 
        = f;
        
zipWith         : All[A,B,C] (A -> B -> C) -> (All[l:int] Stream l A -> Stream l B -> Stream l C);

//TODO: add to global defs
operator{kind = infix, prec = assign} :=;

:=      = All[X] fun[lhs : Ref X, rhs : X] => ();

succ            = fun[x:int] => x;
r4              = let rh = All[X] ref (fun[x:X] => x) in 
                    (rh.<int> := (fun[x:int] => succ x), deref rh.<bool> true);
val r4;        

f3      = All[A] fun[x : A] => (x, x);
val f3.<string> "abc";
    
f2      : (All[a] list[a] -> a) -> (int, char)
        = fun[get: All[a] list[a] -> a] => (get list[1,2], get list['a','b','c']);
    
length  : All[a] list[a] -> int;
ids     : list [All[a] a -> a];
ap1     = length ids;

g2      : list[All[c] c->c] -> list[All[c] c->c];
Nil     : All[a] list[a];
h       = g2 Nil.<All[c] c->c>;

type X  = Record{i = int, j = All[a] a->a};
t       = (fun[x:X] => x) record{i = 1, j = All[T] fun[x:T] => x};

type Arrow_k3[-X, -F :* --> *, -Arg];

type K31    = Fun[X] => Arrow_k3 X (Fun[-Y] => Y -> X) X;
type K41    = K31 int;
t52         = fun[x: K41] => x;

type List2[a, b];
type X2 = List2 int;

type Lit1 : int = 1;
r5          = val[Lit1];
