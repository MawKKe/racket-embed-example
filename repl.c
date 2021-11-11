#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <racket/chezscheme.h>
#include <racket/racketcs.h>


#include "boot-racket.h"


int main(int argc, char *argv[])

{
    boot_custom_racket(argc, argv);

    {
        // Runs a function 'hello-repl' defined in "rkt/run.rkt"
 
        fprintf(stderr, "[Evaluating ((dynamic-require ''run 'hello-repl)) ]\n");

        ptr rbase_sym = Scons(Sstring_to_symbol("quote"), Scons(Sstring_to_symbol("run"), Snil));
        ptr repl_sym = Sstring_to_symbol("hello-repl");
        racket_apply(Scar(racket_dynamic_require(rbase_sym, repl_sym)), Snil); 

    }

    {

        // Runs a function 'pythagoras' defined in "rkt/testi.rkt", passing two integers
        // from this parent C application
        ptr a = Sfixnum(4);
        ptr b = Sfixnum(5);

        ptr rbase_sym = Scons(Sstring_to_symbol("quote"), Scons(Sstring_to_symbol("testi"), Snil));
        ptr repl_sym = Sstring_to_symbol("pythagoras");


#ifdef REPL_DEBUG
        if(Spairp(rbase_sym)){
            fprintf(stderr, "rbase_sym is a pair\n");
        }
#endif

        fprintf(stderr, "[Evaluating ((dynamic-require ''testi 'pythagoras) %ld %ld) ]\n", 
                Sfixnum_value(a), Sfixnum_value(b));

        // For some reason the dynamic require returns a 'pair' with 
        // only one element: the procedure. Scar reads that
        ptr proc_pair = racket_dynamic_require(rbase_sym, repl_sym);

        ptr pythagoras = Scar(proc_pair);

        //assert(Sprocedurep(pythagoras));

#ifdef REPL_DEBUG
        if(!Spairp(proc_pair)){
            fprintf(stderr, "proc_pair is NOT a 'pair'\n");
            return -1;
        }
        if(!Sprocedurep(pythagoras)){
            fprintf(stderr, "pythagoras is NOT procedure?\n");
            return -1;
        }
#endif


        // Result from the function call
        ptr ret = Scall2(pythagoras, a, b); 


        // Must check the return value
        if(Sfixnump(ret)){
            fprintf(stderr, "=> %ld\n", Sfixnum_value(ret));
        }
        else if(Sflonump(ret)){
            fprintf(stderr, "=> %lf\n", Sflonum_value(ret));
        }
        else {
            fprintf(stderr, "pythagoras returned some weird value type???\n");
        }
    }

    {
        // Enter Racket REPL
        ptr rbase_sym = Sstring_to_symbol("racket/base");
        ptr repl_sym = Sstring_to_symbol("read-eval-print-loop");


        racket_apply(Scar(racket_dynamic_require(rbase_sym, repl_sym)), Snil); 
    }

    return 0;

}
