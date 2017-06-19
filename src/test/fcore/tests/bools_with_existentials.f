type Unit = All[A] A -> A;

unit = All[A] fun[a : A] => a;

// an abstract type of bools, packaged as an existential
// there are three elements representing 'true', 'false', 'if'
type Bool2      = Variant{t = Unit, f = Unit};
bool_package    : Some[b] (b, b, b -> (All[a] a -> a -> a))
                = Some[X = Bool2]
                    (variant[Bool2]{f = unit} :: X, variant[Bool2]{t = unit} :: X, 
                        fun[b : X] => All[a] fun[r : a, s : a] => case b in {t => s; f => r} );

// simple example of using the abstract bool type
// open the package and evaluate a term corresponding to the following:
//   if true then Bool2.t else Bool2.f
val open[b] b = bool_package in
    (b.2 b.0)
    variant[Bool2]{t = unit}    // "then" expression
    variant[Bool2]{f = unit}    // "else" expression
;
