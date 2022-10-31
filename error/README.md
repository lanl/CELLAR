# eap-error
The EAP Error Library is a tool for easily identifying error conditions in EAP
code and raising structured error records containing detailed information on the
error.

## Adding Dependency
To use the eap-error library, you need to add it to your
`target_link_libraries`.

```cmake
# CMakeLists.txt

# Use PUBLIC if error-macros.hpp will be used from a header file, otherwise use
# PRIVATE.
target_link_libraries(<your-library> PUBLIC eap-error)
```

## Usage
In your C++ library, first add the `error-macros.hpp` header:
```c++
#include <error-macros.hpp>
```

This header file contains the EAP Error macro defintions.

### Prelude/Postscript Macros
Before using any EAP Error macro in a function, you must start the function
with, at minimum, a "prelude" macro invocation and end it with a matching
"postscript" macro.

#### EE_PRELUDE
If you just want to raise errors, but don't care about raising function-level
failure information, you must simply start the routine with `EE_PRELUDE`.

For example:
```c++
void Object::Function(int a) {
    EE_PRELUDE

    EE_ASSERT_EQ(7, a, "7 is the one true number!");
}
```

You do not need a postscript macro for `EE_PRELUDE`.

#### EE_DIAG
If you want to add function-level context to an error, you can use the
`EE_DIAG` macros.

Start the function with `EE_DIAG_PRE`. This implies `EE_PRELUDE`.

End the function with `EE_DIAG_POST` or `EE_DIAG_POST_MSG`. The
message stream expression passed to `EE_DIAG_POST_MSG` is only evaluated
if an error occurs.

For example:
```c++
void Object::Function(int a, std::string b) {
    EE_DIAG_PRE

    EE_RAISE("Lots of problems in this function!");

    EE_DIAG_POST_MSG("Ran into a problem (a = " << a << ", b = " << b ")")
}
```

#### EAP_EXTERN
When defining a C++ FFI routine, exceptions must be caught before returning.
The `EAP_EXTERN` macros simplify this.

For example:
```c++
extern "C" bool eap_foo_bar_do_it(eap_foo_bar_t const *obj) {
    EAP_EXTERN_PRE

    return BarFromFFI(obj)->DoIt();

    EAP_EXTERN_POST
}
```

If an error occurs, a "stack trace" will be printed and the job will be aborted:
```
ERROR: Terminating (rank = 0) due to C++ Exception at C++/FFI/Fortran Boundary:
        comm/ffi/src/comm-ffi-token.cpp:257 (comm_token_builder_build_global) -> Failed: Uncaught C++ exception at FFI Boundary
        comm/src/comm-token.cpp:323 (eap::comm::TokenBuilder::build_global) -> Failed: Routine failed
        comm/src/comm-token.cpp:351 (eap::comm::TokenBuilder::build_local) -> Failed: Routine failed
        comm/src/comm-token.cpp:430 (eap::comm::TokenBuilder::build_local) -> Failed (vector<local_index_t>(home_counts.size, 0)): Could not allocate global_index with 109951162777600 addresses
        std::bad_alloc
```

### Raising Errors
#### EE_RAISE(msg)
Originates an error with the message passed to it. This is essentially the same
as a `badboy` in xRage.

#### EE_CHECK(expression, msg)
Evaluates to the result of `expression`. If `expression` throws, then the raised
exception is wrapped with another one, appending the message that
`msg` evaluates to.

For example:
```c++
auto global_index =
    EE_CHECK(vector<local_index_t>(home_counts.size, 0),
             "Could not allocate global_index with " << home_counts.size << " addresses");
```

#### EE_ASSERT(expr, msg?)
Raises an error if `expr` not does evaluate to true. Takes optional message
stream.

#### EE_ASSERT_NOT_NULL(parameter, msg?)
Raises an error if `parameter` is `nullptr`. Takes optional message stream.

#### EE_ASSERT_EQ(a, b, msg?)
Raises an error if `a` does not evaluate to an `==` value to `b`. Takes optional
message stream.

#### EE_ASSERT_NE(a, b, msg?)
Raises an error if `a` does not evaluate to a `!=` value to `b`. Takes optional
message stream.