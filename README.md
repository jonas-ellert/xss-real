# xss-real
Linear time Lyndon array construction.

**Build Requirements: CMake 3.10.2 or higher, GCC 7.3.0 or higher**

To clone the repository and run some tests, follow the commands below:

    git clone https://github.com/jonas-ellert/xss-real
    cd xss-real/build
    cmake ..
    make test_xss

From within the build directory, you can compile the benchmark tool and get a list of options using:

    make benchmark
    ./src/benchmark --help
    
The build system will automatically download third party libraries for you. Building may take a minute or two.

Running benchmarks is easy! The most important options are:

 - `-f path/to/file.txt` -- Build the Lyndon array / PSS tree for the specified file.
 - `--runs x` -- Execute each algorithm `x` times and take the median time as the final result.
 - `--length x` -- Consider only a prefix of length `x` of the input text. SI units can be used, e.g. `--length 17MiB`.
 - `--contains x` and `--not-contains x` -- Run only algorithms whose name contains / not contains `x`. A complete list of available algorithms can be obtained by running `./src/benchmark --list`.

For example, the following command runs each algorithm that contains `sdsl` three times on the `1KiB` prefix of the file `CMakeCache.txt`:

    ./src/benchmark --runs 3 -f CMakeCache.txt --length 1KiB --contains sdsl

**Note that the memory usage for small input files (smaller than 1MiB) may be computed incorrectly.**

If you are looking for test instances, you can use the instance generator that is included in the repository. You can build it and get a list of instance types by running the following commands from within the build directory:

    make generator
    ./src/generator --help

Enjoy!
