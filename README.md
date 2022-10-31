<<<<<<< HEAD
# CELLAR
The CELL Adaptive mesh Refinement (CELLAR) application provides cell-based adaptive mesh refinement data structures and execution for parallel computing architectures.
=======
# EAP Core
EAP Core is a C++ project that forms the foundation of cell based AMR for applications.

It provides the following:
- AMR Mesh Datastructure
- AMR Mesh Reconstruction
- Communication Patterns
- C++ Error Handling and Tracing
- Performance Monitoring
- C++/Fortran Interop


## Building
The easiest way to install dependencies is using [Spack](https://spack.io).
After
[installing Spack](https://spack.readthedocs.io/en/latest/getting_started.html),
you can start build dependencies.

The following instructions assume that you have Spack 0.13 or newer. You can check your
Spack version like so:
```
$ spack --version
0.13.0
```

First, add [lanl/cellar-spack](https://github.com/lanl/cellar-spack)
to your list of spack repos.

Once you have the `lanl/cellar-spack` installed, then you can install all
dependencies using
[Spack environments](https://spack.readthedocs.io/en/latest/tutorial_environments.html#).
You'll need to use a modern-ish C++ compiler that supports C++14:
```
$ module load gcc/9.3.0
$ spack compiler find
$ cd path/to/eap-core
```

Then issue the following commands. This will build all of eap-core's dependencies.:
```
$ spack env create -d spack/default
$ spack env activate -d $PWD/spack/default
$ spack install
```

Any time you open a new shell, you'll need to re-activate the Spack environment:
```
$ spack env activate -d $PWD/spack/default
```

Now you're ready to build eap-core. First configure the project using CMake:
```
mkdir build && cd build
cmake ..
```

And then build:
```
make -j
```
For snow, substitute in spack/snow in the above instructions in place of spack/default. If you need
to change the environment use "spack env deactivate".


## Contributing
Code contributors should read the [Developers Guide](DEVELOPERS.md) prior to
sending a pull request.
>>>>>>> 93b7fa8 (initial commit)
