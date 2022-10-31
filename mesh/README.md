# EAP Mesh (eap-mesh)
Mesh is a distributed mesh implementation supporting arbitrary refinement.

## Dependencies
eap-mesh depends on the following:
- `mpi-cpp`
- `gtest-mpi`
- `kokkos`
- `googletest`
- `eap-comm`

It is recommended to install these using [Spack](https://spack.io/) with the
same compiler you plan to build eap-mesh with. To get access to the `mpi-cpp`
and `gtest-mpi` Spack packages, follow the instructions at
https://gitlab.lanl.gov/agaspar/spack-repo. To get access to the `eap-comm`
Spack packages, follow the instructions at
https://gitlab.lanl.gov/EAP/eap-cpp-repo.

For example:
```
spack install mpi-cpp gtest-mpi kokkos googletest eap-comm
```

## Building
eap-mesh uses CMake. If you're using Spack to install your dependencies, I
recommend creating a "Spack View" of your installed dependencies before creating
your CMake build directory.

For example:
```
spack view symlink <path/to/new-view> mpi-cpp gtest-mpi kokkos googletest eap-comm
```

Then create a CMake build directory:
```
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=<path/to/new-view> ..
```

Then build using `make`.

### Intel Compiler Note
eap-mesh uses C++14. By default, Intel will load whatever STL that is default
on the system. On a system using module files, make sure to first load a gcc
version with C++14 support prior to loading the Intel compiler.

## Testing
eap-mesh uses GTest for unit testing, and it is integrated with CTest. To run
all tests, run `make test` or `ctest` from your `build` directory.

## Formatting
Code should be formatted using `clang-format` prior to check-in. Run
`./format.sh` in the root directory to format all code in the workspace.
