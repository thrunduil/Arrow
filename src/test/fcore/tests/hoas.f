// Embedding of simply-typed lambda calculus using higher-order abstract syntax. 

// The dictionary for lambda terms
type ExpDict = Fun[E : *->*] => All[C] 
         ((All[A, B] (E A -> E B) -> E (A -> B)) -> (All[A, B] E (A -> B) -> (E A -> E B)) -> C) -> C;


// term constructors
abs : All[A, B, E : *->*] ExpDict E -> (E A -> E B) -> E (A -> B)
  = All[A, B, E : *->*] fun[dict : ExpDict E, f : E A -> E B] => 
      dict (fun[abs : All[A1, B1] (E A1 -> E B1) -> E (A1 -> B1), app : All[A1, B1] E (A1 -> B1) -> (E A1 -> E B1)] => abs) f
;

app : All[A, B, E : *->*] ExpDict E -> E (A -> B) -> E A -> E B
  = All[A, B, E : *->*] fun[dict : ExpDict E, f : E (A -> B), p : E A] => 
      dict (fun[abs : All[A1, B1] (E A1 -> E B1) -> E (A1 -> B1), app : All[A1, B1] E (A1 -> B1) -> (E A1-> E B1)] => app) f p
;

// A type of unevaluated expressions
type Expression = Fun[A] => (All[E : *->*] ExpDict E -> E A);

// Evaluator: interpret lambda terms as functions

evalDict : ExpDict (Fun[A]=>A)
  = All[A] fun[k : (All[A1, B1] (A1 -> B1) -> (A1 -> B1)) -> (All[A1, B1] (A1 -> B1) -> (A1 -> B1)) -> A] 
        => k (All[A1, B1] fun[f : A1->B1] => f) (All[A1, B1] fun[f : A1->B1] => f)
;
      
eval : All[A] Expression A -> A
  = All[A] fun[e : Expression A] => e.<Fun[A1] => A1> evalDict;

// natural numbers
type Nat = All[A] A -> (A -> A) -> A;

zero : Nat
  = All[A] fun[z : A, s : A -> A] => z;

succ : Nat -> Nat
  = fun[n : Nat] => All[A] fun[z : A, s : A -> A] => s (n z s);

add : Nat -> Nat -> Nat
  = fun[m : Nat, n : Nat] => m n succ;

// Term size measurer: interpret lambda terms as integers representing their size
sizeDict : ExpDict (Fun[A] => Nat)
  = All[A] fun[k : (All[A1, B1] (Nat -> Nat) -> Nat) -> (All[A1, B1] Nat -> Nat -> Nat) -> A]
        => k (All[A1, B1] fun[f : Nat -> Nat] => succ (f (succ zero)))
          (All[A1, B1] fun[f : Nat, p : Nat] => add f p)
;

size : All[A] Expression A -> Nat
  = All[A] fun[e : Expression A] => e.<Fun[A1]=>Nat> sizeDict;
