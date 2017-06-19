
type Sum = Fun[A, B] => All[C] (A -> C) -> (B -> C) -> C;

proj    = All[A, B] fun[s : Sum A A] => (case2 s) (fun[left : A] => left) (fun[right : A] => right);
inl2    = All[A, B] fun[x : A] => All[C] fun[l : A->C, r : B->C] => l x;
inr2    = All[A, B] fun[y : B] => All[C] fun[l : A->C, r : B->C] => r y;
case2   = All[A, B] fun[s : Sum A B] => s;

bimap   = All[A, B, C] fun[f : A -> C, g : B -> C, s : Sum A B]
                        => (case2 s) (fun[left : A] => f left) (fun[right : B] =>  g right);

swap    = All[A, B] fun[s : Sum A B] 
                        => (case2 s) (fun[left : A] => inr2.<B,A> left) 
                                     (fun[right : B] =>  inl2.<B,A> right);
