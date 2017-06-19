type Nat = All[a] a -> (a -> a) -> a;
  
// succ : Nat
zero = All[a] fun[z : a, s : a -> a] => z;

// succ : Nat -> Nat
succ = fun[n : Nat] =>  All[a] fun[z : a, s : a -> a] => s (n z s);

// add : Nat -> Nat -> Nat
add = fun[m : Nat, n : Nat] => m n succ;
  
// pred : Nat -> Nat
pred =
  fun[n : Nat] => (n (zero, zero) (fun[p : (Nat,Nat)] => (p.1, succ p.1))).0;