# Arrow

Progamming language based on the system F-omega with polymorphic kinds, 
(weakly) equirecursive types, general function overloading, type inference, and flexible syntax.

## Code example

Equality encoding:
```fortran
	Kind[k] 
	type Eq : k -> k -> *
			= Fun[A:k, B:k] => All[Phi : k->*] (Phi A -> Phi B);

	Kind[k]
	refl	: All[A:k] Eq A A
			= All[A:k, Phi : k->*] fun[x : Phi A] => x;

	Kind[k]
	symm	= All[A:k, B:k] fun[e : All[P : k -> *] P A -> P B] 
		     => e.< Fun[X : k] => Eq X A> refl.<A>;

	Kind[k]
	trans	= All[A:k, B:k, C:k] fun[ab : Eq A B, bc : Eq B C] 
			  => bc.<Eq A> ab;

	cast	: All[A, B] (Eq A B) -> A -> B
			= All[A, B] fun[p: Eq A B, x : A] => p.<Fun[X] => X> x;

	// type equality assumption
	eq_IB	: Eq int bool;

	test	: bool 
			= cast eq_IB 1;
```            

More examples and simple tests can be found in scr/test/fcore.

## Status

The Arrow language is at early stage of developments. Core type system
is mostly finished, but a few extensions are required, especially some
kind of overloading of type functions, type constraints (in order to 
type check overload resolution in compile time), and effect system.
A simple interpreter and shell is implemented. However implementation is
not well tested.

Currently Arrow language is not documented. 