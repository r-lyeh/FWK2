void (*die)(const char *error);
void (*throw)(const char *exception);

#if TEST
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void my_die(const char *msg) { exit(-fprintf(stderr, "die: %s\n", msg)); }
void my_throw(const char *ex) { fprintf(stderr, "throw: %s\n", ex); assert(!"attach debugger now"); die(ex); }

main() {
    die = my_die;
    throw = my_throw;

    char *p = malloc(~0ull);
    if( !p ) throw("out of memory");
    *p = 0;

    puts("all good");
}
#endif
