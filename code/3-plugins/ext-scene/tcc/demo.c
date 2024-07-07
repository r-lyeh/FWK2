// Minimal libtcc sample
// - rlyeh, public domain

// header

#if 1

#ifdef _WIN32
#   include <winsock2.h>
#   define dlopen(name,mode)    (void*)( (name) ? LoadLibraryA(name) : GetModuleHandle(NULL))
#   define dlsym(handle,symbol) GetProcAddress((HMODULE)handle, symbol )
#   define dlclose(handle)      0
#else
#   include <dlfcn.h>
#endif

void* dll(const char *filename, const char *symbol) {
    void *dll = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    return dll ? dlsym(dll, symbol) : 0;
}


#define LIBTCCAPI

struct TCCState;
typedef struct TCCState TCCState;
/* create a new TCC compilation context */
LIBTCCAPI TCCState *(*tcc_new)(void);
/* free a TCC compilation context */
LIBTCCAPI void (*tcc_delete)(TCCState *s);
/* set output type. MUST BE CALLED before any compilation */
LIBTCCAPI int (*tcc_set_output_type)(TCCState *s, int output_type);
#define TCC_OUTPUT_MEMORY   1 /* output will be run in memory (default) */
#define TCC_OUTPUT_EXE      2 /* executable file */
#define TCC_OUTPUT_DLL      3 /* dynamic library */
#define TCC_OUTPUT_OBJ      4 /* object file */
#define TCC_OUTPUT_PREPROCESS 5 /* only preprocess (used internally) */
/* compile a string containing a C source. Return -1 if error. */
LIBTCCAPI int (*tcc_compile_string)(TCCState *s, const char *buf);
/* do all relocations (needed before using tcc_get_symbol()) */
LIBTCCAPI int (*tcc_relocate)(TCCState *s1, void *ptr);
#define TCC_RELOCATE_AUTO (void*)1
/* return symbol value or NULL if not found */
LIBTCCAPI void *(*tcc_get_symbol)(TCCState *s, const char *name);

void tcc_dlopen() {
    tcc_new = dll("libtcc.dll", "tcc_new");
    tcc_relocate = dll("libtcc.dll", "tcc_relocate");
    tcc_get_symbol = dll("libtcc.dll", "tcc_get_symbol");
    tcc_set_output_type = dll("libtcc.dll", "tcc_set_output_type");
    tcc_compile_string = dll("libtcc.dll", "tcc_compile_string");
    tcc_delete = dll("libtcc.dll", "tcc_delete");
}

#endif

// demo

#include <stdio.h>
#include <stdlib.h>

#define die(s) exit((puts(s), -__LINE__))

void* c_symbol(const char *program, const char *sym) {
    static int open = 0; if(!open) open = 1, tcc_dlopen();

    TCCState *s = tcc_new();
    if( !s ) die("Cannot create a tcc context");

    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    if( tcc_compile_string(s, program) > 0 ) {
        die("C compilation error");
    }

    tcc_relocate(s, TCC_RELOCATE_AUTO);
    return tcc_get_symbol(s, sym);
    // tcc_delete(s); // @leak
}

int main() {
    const char* program =
    "int foo(int value){ return printf(\"this is a test: %d\n\", value); }";

    int (*foo)(int in_value) = c_symbol(program, "foo");
    foo(32);
}
