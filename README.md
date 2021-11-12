# racket-embed-example
Example project on how to compile and embedded Racket (Scheme Lisp) into
a C/C++ Program (using CMake).

Tested on Racket 8.2 (CS), running on Ubuntu 20.04.

# Build

    $ cmake -B build .
    $ cmake --build build --parallel

## Lazy or Immediate loading of base image
The build step generates a custom racket base image via the `racket ctool`.
The base image is then loaded into the main application via C-routines provided
by the Racket interfacing library. This CMake configuration allows the base
image to be loaded via "lazy" method at runtime, or via "immediate" method
at compile time. The default method is "lazy".

You can choose which load method to use by specifying the CMake option

    $ cmake -B build <...> -DEMBED_BASE_MODULES_IMMEDIATE=ON         # (default is OFF)

In the *lazy* version, the base image is generated as a binary file by the `raco
ctool --mods <...>` command, which is then loaded by the
`racket_embedded_load_file()` function at startup. The base image can
change, without needing to re-compile and re-link the whole library/program.

In the *immediate* version, the base image is encoded as a HUUUUUUGE[1] byte array in C
source code form.  The generated C source code is compiled and linked into the
program during the build step, and is available immediately when the program starts.
The startup routine then reads and executes the byte array via the
`racket_embedded_load_bytes()` function (indirectly by the generated `declare_modules()` function).
This version might be useful if/when you decide to bundle your application into
a single executable. However, the generation and compilation of the _massive_ C source file
is unbearably slow, so I recommend using the lazy method during development.


[1]: depending on what libraries/modules you embed into it

# Run

## tl;dr
Build the project, then you should be able to run the simple `repl` example:

    $ source test-vars.sh
    $ ./build/repl

## Explanation
The example executable 'repl' needs to read these bootfiles at startup:

- `petite.boot`
- `scheme.boot`
- `racket.boot`

On Ubuntu 20.04, these files are found under `/usr/lib/racket`. The custom
startup routine (`boot-racket.c`) attemps to locate these files via environment
variables `PETITE_BOOT`, `SCHEME_BOOT` and `RACKET_BOOT`, respectively.

Additionally, the boot routine loads the custom racket base image which should
have beeen generated via `raco ctool`. The path the base image is specified via
`RACKET_BASE_BIN` environment variable. Note, that CMake is configured to
generate this base image automatically.

So at startup, all you need to run is something like this:

    $ export PETITE_BOOT=/usr/lib/racket/petite.boot
    $ export SCHEME_BOOT=/usr/lib/racket/scheme.boot
    $ export RACKET_BOOT=/usr/lib/racket/racket.boot
    $ export RACKET_BASE_BIN=$PWD/build/rkt/racket-base.bin
    $ ./build/repl

The `export` commands need to be run only once per shell session.
Instead of running each command separately, you can run `source test-vars.sh`
once for the same effect.

# Dependencies

This setup was tested with Racket 8.2 (CS) on Ubuntu 20.04.
The `racket` package should provide the required library
(`libracketcs.a`) , header files and `raco ctool`[2].

Successful build depends on these tools and libraries:

- a recent C compiler toolchain (`GCC`, or `Clang`)
- `CMake >= 3.16`
- `pkg-config`
- `zlib1g >= 1`
- `liblz4 >= 1.9`
- Basic threading support (`pthreads`?)

More details about dependencies can be found
in `rkt/CMakeLists.txt`.

[2]: **NOTE**: The `raco` tool shipped with the Racket package must
have a `ctool` subcommand available. It is required for the base image generation (see `Build` above).  Some platforms/packages may not provide it; for example on FreeBSD 12.2 the package
`racket-minimal-8.2` does not provide `raco ctool`.

# Features

Nothing much. Mostly demonstrates the build setup via CMake. The Racket
documentation was a bit lacking in this regard. There are also very few
C code examples of how to use the embedded Racket. I will investigate more...

# TODO

Here are some random "nice to have" ideas:

- Figure out smarter way of specifying location of runtime files, perhaps a configuration file?
- Create a CMake target that bundles all these various files into a single executable.

# Author

Markus H (MawKKe)

# License

Copyright 2021 Markus Holmström (MawKKe)

The works under this repository are licenced under Apache License 2.0. See file LICENSE for more information.

# Contributing

This project is hosted at https://github.com/MawKKe/racket-embed-example

You are welcome to leave bug reports, fixes and feature requests. Thanks!
