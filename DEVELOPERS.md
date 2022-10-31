# Development Guide
This guide intends to serve as an on-ramp for new EAP Core developers.

## Setting Up Build Environment
TODO

## Languages
All core code is written in C++. Some of these packages may be used by Fortran
code, and so there is also Fortran code present to interop with C++ code and
present a Fortran-first interface. But all business logic should be written in
C++.

## Code Style
### C++
EAP Core follows (or attempts to follow) the
[EAP C++ Style Guide](https://gitlab.lanl.gov/EAP/eap-cpp-style). When in doubt,
please refer to the style guide.

### Fortran
Currently we have no formal style guide for Fortran. Just try to follow the
existing code.

## Code Layout
This section describes the layout of the EAP Core code.

### Packages
Groups of logically-related code go into separate folders located at the root
of the project. Each of these packages will build their own libraries that may
get linked together later. Packages can have strictly directed, non-acyclic
dependencies, which are documented in the CMakeLists.txt in each folder.

The code layout is as follows:
```yaml
<package-name>/ # root of package
  include/ # Public header files
  internal/ # Header files only used internally by package - not a public interface
  src/ # C++ source files
  test/ # C++ GTest unit tests
    CMakeLists.txt # Builds the unit tests 
  ffi/ # C Foreign Function Interface (FFI) to core C++ business logic (optional)
    include/ # Public header files that support interop across FFI boundary
      base/ # Header files declaring common types for both sides of the C/C++ interop boundary
      interop/ # Header files declaring routines that are only used by C++ libraries that need to convert types from base
      library/ # Header files used by a C application using this package
    internal / # Header files only used internally by FFI implementation
    src/ # C++ source files implementing the C interface
    test/ # C "example" tests (optional)
      CMakeLists.txt # Builds examples
    CMakeLists.txt # Builds static library depending on library at root of package
  fortran/ # Fortran interface implemented on top of core C++ business logic (optional)
    src/ # Fortran .F90 files
    test/ # Fortran "example" tests
    CMakeLists.txt # Builds static library depending on library from ffi
  CMakeLists.txt # Builds and exports libraries in package, with optional disablement for features
```

### File Names
Primarily due to downstream build system limitations, all source and header
files must have unique names. This requirement does not apply to tests or
examples.

#### C++ (Implementation)
Each header file covering some "topic" should have the name:
`<package>-<topic>.hpp`. For example `comm-token.hpp`, which contains the
interface for the Token component in the comm package. You may also have a
`<package>.hpp` file, which should typically include all other `<package>-*.hpp`
files.

Sometimes you'll have header files that need to be public, but you don't want
to be considered a "public" API. In this case, these files should have the name
`<package>-internal-<topic>.hpp`.

Similarly, all source files should have the name `<package>-<topic>.cpp`. There
does not need to be a 1:1 relationship between header and source files. Often
there will be header files with no matching source file since the entire
implementation may need to be in the header (due to performance considerations
or templates).

#### FFI
All C-useable header files should have the name: `<package>-ffi-<topic>.h>`.
A `<package>-ffi.h` file is also allowed.

Header files that are meant purely for C++-side interop, they should be called:
`<package>-ffi-<topic>.hpp`.

Source files should be named: `<package>-ffi-<topic>.cpp`.

#### Fortran
All source files should have the name `<package>-<topic>.F90`.

### Namespacing
#### C++
All public names from a package should fall under the namespace
`eap::<package>`. E.g.:

```C++
namespace eap {
namespace comm {
  // Code here
}
}
```

If you have any names that must be in a public header but aren't part of the
public API, then use the `internal` namespace, like so:

```C++
namespace eap {
namespace comm {
namespace internal {
  // "Internal" names go here
}
}
}
```

#### C (FFI)
All "type-bound" names should be of the form:
`eap_<package>_<type>_<op>(<type>_t const *this, ...)`

Free functions may have the form: `eap_<package>_<op>(...)`.

#### Fortran
For some file `<package>-<topic>.F90`, the source file should implement a module
`<package>_<topic>`. E.g.:

```fortran
module comm_patterns
  private

  public foo
contains
  function foo()
  end function foo
end module comm_patterns
```

The names exported from the module do not need to be laboriously namespaced, but
a best effort to avoid naming collisions is recommended.