
# Principles

* Zero-overhead abstraction -- Don't pay for what you don't use
* Opt-in abstraction costs -- If there is a cost, the user must announce it
* No unnecessary syntax -- Heavy syntax for advanced usage
* No lower language -- don't leave room for C.


## Declarations

### Value declarations

Binds a name to a value (possibly a reference).

```
val x : int = 42;
&x; // ill-formed: x has type int.

val r : int& = some_other_var;
&x; // OK: r is a reference.
```

Note that in LLVM, we can simply bind x and r to the expressions that compute the value. 

The type of the value declaration can be omitted to indicate that it is deduced from the initializer.

```
val x = 0.3; //  x has type double
```

### Variable declarations

Declares (mutable) storage for a value.

```
var x : int = 42;
```

Note that the type can be omitted to indicate deduction from the initializer.

```
var x = 42; // x has type int
```

### Functions

```
fn f(a : int) -> int 
{ }
```

### Parameters

Are value declarations by default. Can be declared with var to indicate mutability.

```
fn f(a: int) -> int {
  a = a + 1; // error: cannot assign to the value a.
};
```

```
fn f(var a: int) -> int {
  a = a + 1; // OK:
};
```


## Overloading

I think I might prefer both overloading and traits/named concepts. Traits provide customization points, overloading allows algorithms to be extended.

