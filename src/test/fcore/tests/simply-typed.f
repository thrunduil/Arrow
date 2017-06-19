type Base : *;

id = fun[x : Base] => x;

idf = fun[x : Base->Base] => x;

compose = fun[f : Base->Base, g : Base->Base, x : Base] => f (g x);
