Kind[k]
type ID     : k -> *;

Kind[k]
id          : All[X :k] (ID X);

//beta type equality test
Kind[k2]    
type Y      : k2;

Kind[k1, k2]
type F1     = Fun[X : k1] => (Y :: k2); 

Kind[k1, k2]
type F2     = Fun[X : k1] => (F1 :: k1->k2) X; 

test        : ID F1.Kind[*,*]
            = id.<F2.Kind[*,*]>;