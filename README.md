# Arrow

Progamming language based on the system F-omega with polymorphic kinds, 
(weakly) equirecursive types, general function overloading, type inference, and flexible syntax.

```fortran
	Kind[k] 
	type Eq : k -> k -> *
			= Fun[A:k, B:k] => All[Phi : k->*] (Phi A -> Phi B);

	Kind[k]
	refl   : All[A:k] Eq A A
			= All[A:k, Phi :k->*] fun[x : Phi A] => x;

	symm	= All[A, B] fun[e : All[P : * -> *] P A -> P B] 
		     => e.< Fun[X] => Eq X A> refl.<A>;

	trans	= All[A, B, C] fun[ab : Eq A B, bc : Eq B C] => bc.<Eq A> ab;

	Kind[k]
	cast	: All[A, B] (Eq A B) -> A -> B
			= All[A, B] fun[p: Eq A B, x : A] => p.<Fun[X] => X> x;
```
## Example

            
eq_IB       : Eq int bool;
test1       : bool   
            = cast.Kind[*] eq_IB 1;
