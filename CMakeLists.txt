cmake_minimum_required(VERSION 3.16)

project(racket-embed VERSION 0.0.1)

# "Our racket" as C library
add_subdirectory(rkt)

option(EMBED_BASE_MODULES_IMMEDIATE "Link bundled instead of lazy racket boot" OFF)

# Example application
add_executable(repl repl.c)

target_compile_options(repl
    PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -pedantic -Wunused -Wshadow>
)

target_compile_definitions(repl
    PRIVATE
        $<$<CONFIG:Debug>:REPL_DEBUG>
)

# Choose which method to use

if(EMBED_BASE_MODULES_IMMEDIATE)
    target_link_libraries(repl boot-racket-bundled)
else()
    target_link_libraries(repl boot-racket)
endif()
