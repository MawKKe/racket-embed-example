# racket-embed-example
Example on how to compile and embedded Racket (Scheme Lisp) into a C/C++ Program (using CMake)

# Build

    $ cmake -B build .
    $ cmake --build build --parallel

## Lazy or Immediate loading of base image
The build step generates a custom racket base image via the `racket ctool`. 
The base image is then loaded into the main application via C-routines provided
by the Racket interfacing library. In this build setup, The base image can be loaded 
via "lazy" method (default), or via "immediate" method at compiletime.

You can choose which base image load method to use by specifying the CMake option

    $ cmake -B build <...> -DEMBED_BASE_MODULES_IMMEDIATE=ON         # (default is OFF)

In the *lazy* version, the base image is generated as a binary file by the `raco
ctool --mods <...>` command, which is then loaded by the
`racket_embedded_load_file()` function at startup. The base image can
change, without needing to re-compile and re-link the whole library/program.

In the *immediate* version, the base image is encoded as a HUUUUUUGE byte array in C
source code form.  The generated C source code is compiled and linked into the
program during the build step, and is available immediately when the program starts.
The startup routine then reads and executes the byte array via the
`racket_embedded_load_bytes()` function (indirectly by the generated `declare_modules()` function).
This version might be useful if/when you decide to bundle your application into
a single executable. However, the generation and compilation of the _massive_ C source file
is unbearably slow, so I recommend using the lazy method during development.



# Run

The example executable 'repl' needs to read these bootfiles at startup:

- petite.boot
- scheme.boot
- racket.boot

On Ubuntu 20.04, these files are found under `/usr/lib/racket`. The custom
startup routine (`boot-racket.c`) attemps to locate these files via environment 
variables `PETITE_BOOT`, `SCHEME_BOOT` and `RACKET_BOOT`, respectively. 

Additionally, the boot routine loads the custom racket base image which should be 
generated via `raco ctool`. The path the base image is specified via `RACKET_BASE_BIN` environment
variable. Note, that CMake is configured to generate this base image automatically.
So at startup, all you need to run is something like this:

    $ export PETITE_BOOT=/usr/lib/racket/petite.boot
    $ export SCHEME_BOOT=/usr/lib/racket/scheme.boot
    $ export RACKET_BOOT=/usr/lib/racket/racket.boot
    $ export RACKET_BASE_BIN=$PWD/build/rkt/racket-base.bin
    $ ./build/repl

You may run `source test-vars.sh` in the project directory to set these variables in your
current shell session.

