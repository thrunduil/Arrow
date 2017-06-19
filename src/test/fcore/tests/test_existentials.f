pack    = Some[X = int] record{a : X = 0, f = fun[x:X] => 0};

x1      = open[X] x = pack in (x.f x.a);
x2      = open[X] x = pack in (fun[y : X] => x.f y) x.a;
x3      = open[X] x = pack in Some[Y = X] (x.a :: Y);

//errors:
//succ    = fun[x : int] => x;
//x4      = open[X] x = pack in succ x.a;
//x5      = open[X] x = pack in x.a;
