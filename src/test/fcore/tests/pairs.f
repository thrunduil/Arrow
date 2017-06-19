
type Pair = Fun[A, B] => All[C] (A -> B -> C) -> C;

pair    = All[A, B] fun[x : A, y : B] => All[C] fun[k : A -> B -> C] => k x y;
fst     = All[A, B] fun[p : Pair A B] => p (fun[x : A, y : B] => x);
snd     = All[A, B] fun[p : Pair A B] => p (fun[x : A, y : B] => y);
