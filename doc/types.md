# Types

Types describe objects 

## Fundamental types

Initially, start with simple types:

We should have a unit type.

The boolean type represents true/false.

We need a comprehensive integer suite. Ideally, 8, 16, 32, 64, and 128. The default integer type should be spelled int, it maps to one of the sizes above. All ints are signed. Use long and short to modify?

We need a corresponding unsigned integer type for binary data. Same as above. Default type is spelled uint.

Floating point types should be those supported by IEEE 754. Half, single, double, quad. Default is float and maps to single precision. Other spellings: float16, float32, float64, and float128.

Character types for various encodings. Default type is char, maps to char8. Support char16, char32.

No void type?

## Compound types

References are non-null pointers to objects (possibly with no storage). References can be const-qualified to indicate that the object type is immutable. Variables cannot have reference type.

Pointers are essentially nullable references. They can be used to declare variables. 

I would like to differentiate pointers to object from array-like pointers. Having arithmetic pointers is essential for recreating the standard library. We could use sequence types T[] to support that range of functionality:

int[] p = # some array
int[] q = p + 1;
asset q - p = 0;

Function types describe the set of functions taking the same arguments and returning the same value. They are the types of declared functions and can be used to declare values and variables.


## Aggregate types

Records, arrays, and unions are aggregate types. An object of aggregate type is comprised of subobjects. A value of aggregate type is the address of its object.




Let's say I do this...

```
func f() -> string { ... }

// later
val x : string = f();
```

What is the value of a composite type? It's structure? That's going to be very bad on registers. I think what we really want to do is say that the value of an aggregate object (i.e., an aggregate value) is really its address and that e.g., a function returning an aggregate value initializes a temporary object. Or something like that...

This gets us closer to move semantics too since a value declaration initialized by an aggregate value is essentially a reference to the initialized temporary. Note that we're going to need lifetime extension rules to do this correctly.

Note that an implementation may relax that requirement for small aggregates. It doesn't make sense treat an empty class as a by-reference entity.

```
var x : string = f();
```

Copies the aggregate value into the object.

```
class string
{
  // Assuming this is the syntax for copying.
  operator copy(x : string) 
  {
    // x binds to the aggregate value...
  }
}
```


