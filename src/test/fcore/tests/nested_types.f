// Perfect :: * -> *
type Perfect = Fun[C] => All[T : *->*] (All[A] A -> T A) -> (All[A] T (A, A) -> T A) -> T C;

// zeroP : All[A] A -> Perfect A
zeroP = All[A] fun[x : A] => 
          All[T : *->*] fun[z : All[B] B -> T B, s : All[B] T (B, B) -> T B] => z x ;

// succP : All[A] Perfect (A × A) -> Perfect A
succP = All[A] fun[p : Perfect (A, A)] => 
          All[T : *->*] fun[z : All[B]  B -> T B, s : All[B]  T (B, B) -> T B] => s (p.<T> z s) ;
