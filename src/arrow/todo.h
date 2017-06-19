/*
extensions:
- add effect system (for example similar to Haskel's monads)
- injective type constructors
- unify computations on type level and term level
- introduce general classes on type and term level
- add record updates (records are stil immutable)
- overloading of type functions

required features
- remove references? decide after adding effect system
- add ADT's; this is mostly syntactic problem since we have variants
- int paths from int_type

errors:
- overloading failure in some cases in detected in runtime only => add static checks

LOW PRIORITY
- relax instantiation condition for local symbols in pack and unpack terms,
    not instantiation is possible only in runtime

TODO:
- implement import in shell
- recursive type guards can be relaxed => add escape checks
    e.g. for F:k->int, F g can be a guard if rec kind is not int
- add patterns
- star kind identifier
- check value kinds and type extern functions
*/