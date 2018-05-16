
## Physical conventions

A module is a collection of declarations. Some declarations can be exported,
meaning that their names and properties are made available to other modules.

A module can import another module.

A module is defined by a set of source files that are translated simultaneously.

TODO: Talk about modules w.r.t. the file system. Do a python type thing? Map
names onto paths?

## Template modules

A module can have parameters:

```
module[a : type, b : int] x.y.z;
```

When imported, arguments must be provided:

```
import x.y.x[int, 0];
```

Generic modules may not have binary components.

