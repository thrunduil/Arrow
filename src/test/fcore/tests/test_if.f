Kind[k]
type true2  : k -> k -> k
            = Fun[t : k, f : k] => t;
            
Kind[k]            
type false2  : k -> k -> k
            = Fun[t : k, f : k] => f;

Kind[k]
type If2    = Fun[b : k -> k -> k] => b;
            
type test1  = If2 true2.Kind[*] int bool;
type test2  = If2 false2.Kind[*] int bool;
