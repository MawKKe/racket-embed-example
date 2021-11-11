racket_lib=/usr/lib/racket

# static, common boot files 
# loaded by the embedded application at startup
export PETITE_BOOT=$racket_lib/petite.boot
export SCHEME_BOOT=$racket_lib/scheme.boot
export RACKET_BOOT=$racket_lib/racket.boot

# Our "custom" base library
# - Generated at compile time (see rkt/CMakeLists.txt)
# - boot-racket.c loads it into the racket VM at startup
# You can do without this by compiling the base into the
# application, see RACKET_EMBED_LINK_BASE_AS_BUNDLED in CMakeLists.txt
export RACKET_BASE_BIN=$PWD/build/rkt/racket-base.bin
