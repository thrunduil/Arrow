Kind[k]
type Eq     : k -> k -> *
            = Fun[A:k, B:k] => All[Phi : k->*] (Phi A -> Phi B);
            
Kind[k]
refl        : All[A:k] Eq A A
            = All[A:k, Phi :k->*] fun[x : Phi A] => x;

symm        = All[A, B] fun[e : All[P : * -> *] P A -> P B]=> e.< Fun[X] => Eq X A> refl.<A>;
trans       = All[A, B, C] fun[ab : Eq A B, bc : Eq B C] => bc.<Eq A> ab;
lift        = All[A, B, P : *->*] fun[e : Eq A B] => e.< Fun[X] => Eq (P A) (P X)> refl.<P A>;

Kind[k]
cast        : All[A:*, B:*] (Eq A B) -> A -> B
            = All[A:*, B:*] fun[p: Eq A B, x : A] => p.<Fun[X] => X> x;
            
eq_IB       : Eq int bool;
test1       : bool   
            = cast.Kind[*] eq_IB 1;

//other encoding
Kind[k]
type ID     : k -> *;

Kind[k]
id          : All[X :k] (ID X);

Kind[k]
type Eq2    : k -> k -> *
            = Fun[A:k, B:k] => All[Rho : k->k->*] (All[Gam:k] Rho Gam Gam) -> (Rho A B);

Kind[k]
refl2       : All[A:k] Eq2 A A
            = All[A:k, Rho : k->k->*] fun[f : All[Gam:k] Rho Gam Gam] => f.<A>;

Kind[k]
elim_eq     : All[A:k, B:k] (Eq2 A B) -> All[Rho:k->k->*] (All[Gam:k] Rho Gam Gam) -> Rho A B
            = All[A:k, B:k] fun[e: Eq2 A B] => All[Rho : k->k->*] fun[f : All[Gam:k] Rho Gam Gam] => e.<Rho> f;
            
Kind[k]
beta        = All[A : k, G : k->k->*] fun[f : All[B:k] G B B] => if true then (elim_eq.<A,A> refl2.<A>).<G> f else f.<A>;

Kind[k]
eta         = All[A : k, B : k] fun[z : Eq2 A B] => if true then (elim_eq.<A,B> z).<Eq2.Kind[k]> refl2.Kind[k] else z;
