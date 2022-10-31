# EAP Perf (eap-perf)
This module implements functionality for recording performance statistics
in EAP.

eap-perf uses the STL `chrono` library. I recommend familiarizing yourself with
this library on [cppreference.com](https://en.cppreference.com/w/cpp/chrono).

## Features
### Timers
EAP Perf has a Timer feature for aggregating timer statistics over many runs
of some function, including average, sum, min, and max. It can use any clock
implementation that implements the
[TrivialClock](https://en.cppreference.com/w/cpp/named_req/TrivialClock)
requirement.

EAP Perf's Timer is available in C++ as `eap::perf::Timer<Clock>`. When Clock is
unspecified, i.e. `eap::perf::Timer<>`, then `eap::perf::DefaultClock` is used
for the Clock. This is `std::chrono::steady_clock` by default.

```cpp
#include <chrono>
#include <perf-timer.hpp>

using std::chrono::steady_clock;

int main() {
    // eap::perf::Timer<> is equivalent to eap::perf::Timer<steady_clock>
    eap::perf::Timer<steady_clock> timer;

    for (auto i = 0; i < 100; i++) {
        timer.start();
        // some expensive operation
        timer.stop();
    }

    // avg_time, min_time, and max_time return values only when there is at least one time sample.
    if (nonstd::optional<steady_clock::duration> avg_time = timer.avg_time()) {
        // do something with avg_time
    }
}
```

NOTE: EAP Perf's Timer is NOT intended for micro-benchmarking - it's only useful
for timing computations on the order of tens of microseconds. If you're
interested in micro-benchmarking, try a micro-benchmarking framework like
[Google Benchmark](https://github.com/google/benchmark).

### Timer Registry
`eap::perf::Timer` is useful if you have just a few unrelated timers. However,
if you have many timers it's useful to have a centralized store for these timers
so that you can go to a single point to dump all timers at the end of a program
run.

`eap::perf::TimerRegistry<Clock>` allows a caller to register named timers. The
`TimerRegistry` does not offer mutable access to the underlying `Timer` object -
the timers must be started and stopped indirectly through `TimerRegistry`. This
allows the registry to track nested timers. However, only one timer is ever
running at a time and that is always the outermost timer.

Timers inside of `TimerRegistry` are pushed and popped in a stack pattern -
timers are pushed to the stack and must then be popped in the reverse order.
Currently only the outermost or "highest" timer is started and stopped -
all timers pushed after the outermost one point are ignored (except for the
fact that they have to be popped in the reverse order they were pushed in).

See [examples/registry.cpp](examples/registry.cpp) for a detailed example of
using `TimerRegistry`.

A possible output of this example is the following:
```
Timer: foo timer, count = 1000, sum_time = 624610 ns, avg_time = 624 ns
Timer: bar timer, count = 1000, sum_time = 260127 ns, avg_time = 260 ns
Timer: zap timer, count = 0, sum_time = 0 ns
```

As you'll see, "bar timer" is only counted 1000 times since it's only called at
the "outermost level" 1000 times, even though the timer is called 2000 total
times. Equivalently, "zap timer" has a count of 0 because it's only used when
another timer is already running.

## Dependencies
### Public Dependencies
These dependencies are always required, even if the supplied CMake build system
is not used.

| Package Name     | Spack Repo | Package Location                                |
| ---------------- | ---------- | ----------------------------------------------- |
| optional-lite    | builtin    | https://github.com/martinmoene/optional-lite    |
| string-view-lite | builtin    | https://github.com/martinmoene/string-view-lite |

### Build Dependencies
These dependencies are used by the supplied CMake build system to build the
core library.

| Package Name   | Spack Repo         | Package Location                               |
| -------------- | ------------------ | ---------------------------------------------- |
| stl-lite-cmake | agaspar/spack-repo | https://gitlab.lanl.gov/agaspar/stl-lite-cmake |

### Dev Dependencies
These dependencies are used for testing and other developer workflows. These are
only needed if you're planning on making changes to the code.

| Package Name | Spack Repo | Package Location                     |
| ------------ | ---------- | ------------------------------------ |
| googletest   | builtin    | https://github.com/google/googletest |
| doxygen      | builtin    | https://github.com/doxygen/doxygen   |

### Third Party Spack Repos
For convenience in building and using dependencies, the following third-party
Spack repos can assist:

| Spack Repo         | Location                                   |
| ------------------ | ------------------------------------------ |
| agaspar/spack-repo | https://gitlab.lanl.gov/agaspar/spack-repo |

## Building
eap-perf uses CMake. For example, when working with Spack, CMake, and Make:
```
mkdir build && cd build
spack view symlink deps googletest stl-lite-cmake doxygen
CMAKE_PREFIX_PATH=deps cmake ..
make -j
```

## Documentation
For now, documentation must be built to be browsed. To build the Doxygen docs,
install Doxygen, configure a CMake Build directory, and build the documentation
target.

```
make eap-perf-docs
```

Documentation will be available in the "html" directory.

## Testing
eap-perf uses GTest for unit testing, and it is integrated with CTest. To run
all tests, run `make test` or `ctest` from your `build` directory.

## Formatting
Code should be formatted using `clang-format` prior to check-in. Run
`./format.sh` in the root directory to format all code in the workspace.
