# Find library manually, because racket does not provide CMake/pkg-config modules..
find_library(LIBRACKETCS NAMES racketcs)

# TODO: find_library(... REQUIRED ...) only provided since CMake 3.18.
# Fallback to manual check
# NOTE: the variable LIBRACKETCS is not used directly, but the library is
# linked with relative name "racketcs" in target_link_library() definitions (see below).

if(NOT LIBRACKETCS)
    message(FATAL_ERROR "Could not find library: racketcs")
endif()

find_package(PkgConfig REQUIRED)

# Racket 8.2 - These are the library dependency versions in Ubuntu 20.04.
# Newer versions might work, I haven't tested.
pkg_check_modules(LZ4       liblz4>=1.9 REQUIRED IMPORTED_TARGET)
pkg_check_modules(ZLIB      zlib>=1     REQUIRED IMPORTED_TARGET)
pkg_check_modules(NCURSES   ncurses>=6  REQUIRED IMPORTED_TARGET)

# racketcs depends on pthread, lets use this 
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# NOTE: only libc dependency is implicit (dl m)

# Place library dependencies in variable for later use.
# NOTE we could use ${CMAKE_DL_LIBS} in place of dl. But lets not.
set(racket_embed_library_deps
    racketcs dl m Threads::Threads
    PkgConfig::LZ4 PkgConfig::ZLIB PkgConfig::NCURSES
)

#-------------------------------------------------------------------------

# Specify which racket modules you want to embed
# NOTE: the more modules you add, the larger the base image will be 
# (= generation/compilation takes much longer)
set(racket_embed_modules racket racket/lang/reader racket/runtime-config)
set(racket_embed_user_modules ${CMAKE_CURRENT_SOURCE_DIR}/run.rkt ${CMAKE_CURRENT_SOURCE_DIR}/testi.rkt)

# Places ++lib in front of each module target. 
# NOTE: the semicolon is important here! It turns the result into a 
# list (++lib a ++lib b ...) instead of ("++lib a" "++lib b"). The latter
# form is not understood by raco.
list(TRANSFORM racket_embed_modules PREPEND "++lib;" )

# NOTE: When using -G Ninja, the build "cmake ... --verbose" does not produce
# the "generating racket-base.bin" messages from CMake.
# So it may seem like build step is hanging during the racket-base generation...
# Build without --verbose shows the messages in Ninja feed.

# generate base image with racket base stuff and embedded custom modules
add_custom_command(
    OUTPUT
        racket-base.bin
    COMMAND
        raco ctool --mods racket-base.bin 
            ${racket_embed_modules}
            ${racket_embed_user_modules}
    DEPENDS
            ${racket_embed_user_modules}
    VERBATIM
)

# generate base image with racket base stuff, encoded as massive C byte array
add_custom_command(
    OUTPUT
        racket-base.c
    COMMAND 
        raco ctool -v --c-mods racket-base.c
            ${racket_embed_modules}
            ${racket_embed_user_modules}
    DEPENDS
            ${racket_embed_user_modules}
    VERBATIM
)


#-------------------------------------------------------------------------
# IMMEDIATE load method
#-------------------------------------------------------------------------

# Base library for racket, this imports all the crap in compiled form.
# Also, let's not build it unless needed, because it might be slllllllooww
add_library(boot-racket-bundled STATIC EXCLUDE_FROM_ALL boot-racket.c)

# Defines change how the boot-racket.c behaves.
target_compile_definitions(boot-racket-bundled PRIVATE RACKET_BASE_IMPORT_STATIC)

# If 'racket-base.c' contained proper includes an declarations, it would be easily
# turned into a library target that compiles without warnings. However, that is not the 
# case, so now we need to do clever trickery and embed that source code via 
# '#include "racket-base.c"' in the boot-racket.c file.
# Downside is that 'racket-base.c' is now an implicit dependency of 'boot-racket-bundled'
# target. We need to make that dependency explicit. Unfortunately CMake does not support 
# files as dependency targets, so we must create this silly dummy target in between...
add_custom_target(racket-base-c DEPENDS racket-base.c)
add_dependencies(boot-racket-bundled racket-base-c)

target_include_directories(boot-racket-bundled 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_libraries(boot-racket-bundled
    PRIVATE
        ${racket_embed_library_deps}
)

#-------------------------------------------------------------------------
# LAZY load method
#-------------------------------------------------------------------------

add_library(boot-racket STATIC EXCLUDE_FROM_ALL boot-racket.c)
target_include_directories(boot-racket
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(boot-racket
    PRIVATE
        ${racket_embed_library_deps}
)
#target_compile_definitions(boot-racket PRIVATE -DRACKET_BOOT_LOAD_COMPILED)

if(NOT EMBED_BASE_MODULES_IMMEDIATE)
    # We are in lazy mode - require generation of racket-base.bin as dependency to ALL-target.
    # The racket-base.bin is loaded at runtime, and should not trigger recompilation of 'boot-racket'
    # or any of its dependants whenever it changes (because boot-racket does not really know what's
    # in racket-base.bin
    add_custom_target(racket-base-bin ALL DEPENDS racket-base.bin)

    # add_dependencies(boot-racket racket-base-bin) # Dont!
endif()


