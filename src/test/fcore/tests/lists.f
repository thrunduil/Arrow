type List = Fun[a] => All[f : *->*] f a -> (a -> f a -> f a) -> f a;

nil     = All[a, f : *->*] fun[nil : f a, cons : a -> f a -> f a] => nil;

cons    = All[a] fun[x : a, xs : List a] => 
            All[f : *->*] fun[nil : f a, cons : a -> f a -> f a] => cons x (xs.<f> nil cons);

fold    = All[a, B] fun[c : a -> B -> B, n : B, l : List a] => l.<Fun[X]=> B> n c;
append  = All[a] fun[l : List a, r : List a] => fold cons.<a> l r;
concat  = All[a] fun[l : List (List a)] => fold append.<a> nil.<a> l;

x = cons 3 (cons 2 (cons 1 nil.<int>));
y = (append x x);