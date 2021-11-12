#include <racket/chezscheme.h>
#include <racket/racketcs.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef RACKET_BASE_IMPORT_STATIC
// this is just to get rid of compiler warning about missing declaration
#include "racket-base.c"
#endif

//#else: the library is compiled withtout embedding the racket base; 
//       it is instead loaded at runtime with racket_embedded_load_file().

void boot_custom_racket(int argc, char *argv[]){
    racket_boot_arguments_t ba;

    memset(&ba, 0, sizeof(ba));

    const char * s1_petite_boot = getenv("PETITE_BOOT");
    const char * s2_scheme_boot = getenv("SCHEME_BOOT");
    const char * s3_racket_boot = getenv("RACKET_BOOT");

    if((s1_petite_boot == NULL) || (s2_scheme_boot == NULL) || (s3_racket_boot == NULL)){
        fprintf(stderr, "Missing required env var (PETITE_BOOT=%s, SCHEME_BOOT=%s, RACKET_BOOT=%s)\n",
                (s1_petite_boot ? s1_petite_boot : "<not-found>"),
                (s2_scheme_boot ? s2_scheme_boot : "<not-found>"),
                (s3_racket_boot ? s3_racket_boot : "<not-found>"));
        abort();
    }

    ba.boot1_path = s1_petite_boot;
    ba.boot2_path = s2_scheme_boot;
    ba.boot3_path = s3_racket_boot;;

    ba.exec_file = argv[0];

    racket_boot(&ba);

#ifdef RACKET_BASE_IMPORT_STATIC
    declare_modules();
#else
    const char * RACKET_BASE_BIN = "RACKET_BASE_BIN";
    const char * racket_base_bin = getenv(RACKET_BASE_BIN);
    if(racket_base_bin == NULL){
        fprintf(stderr, "Missing env var %s\n", RACKET_BASE_BIN);
        abort();
    }

    // racket-base.bin is a placeholder, this should be passed 
    // as argument or via some other mechanism
    // TODO becaues C argparsing is fucking awful
    racket_embedded_load_file(racket_base_bin, 1);
#endif

    racket_namespace_require(Sstring_to_symbol("racket"));
}
