#ifdef _WIN32
#   include <winsock2.h>
#   define dlopen(name,mode)    (void*)( (name) ? LoadLibraryA(name) : GetModuleHandle(NULL))
#   define dlsym(handle,symbol) GetProcAddress((HMODULE)handle, symbol )
#   define dlclose(handle)      0
#else
#   include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define DIE(s) exit((puts(s), -__LINE__))
#define PRINTF printf
#define is(x) is_##x
#define is_win32 1

unsigned char *readfile(const char *pathfile, int *size) {
    char *bin = 0;
    for( FILE *fp = fopen(pathfile,"rb"); fp; fclose(fp), fp = 0) {
        fseek(fp, 0L, SEEK_END);
        size_t len = ftell(fp);
        if(size) *size = (int)len;
        fseek(fp, 0L, SEEK_SET);
        bin = malloc(len+1);
        if( bin && fread(bin, 1, len, fp) == len ) bin[len] = '\0';
        else free(bin), bin = 0;
    }
    return bin;
}

#define LUA_IMPL
#include "src/0/3rd_lua.h"


void* dll(const char *filename, const char *symbol) {
#if 0
    if( fname && !file_exist(fname) ) {
        char *buf, *path = file_path(fname), *base = file_base(fname);
        if( file_exist(buf = va("%s%s.dll", path, base)) ||
            file_exist(buf = va("%s%s.so", path, base)) ||
            file_exist(buf = va("%slib%s.so", path, base)) ||
            file_exist(buf = va("%s%s.dylib", path, base)) ) {
            fname = (const char *)buf;
        } else {
            return NULL;
        }
    }
#endif
    void *dll = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    return dll ? dlsym(dll, symbol) : 0;
}

#if 0
void* dll(const char *fname, const char *symbol) {
#if is(win32)
    return (void*)GetProcAddress(fname ? LoadLibraryA(fname) : GetModuleHandleA(NULL), symbol);
#else
    return dlsym(fname ? dlopen(fname, RTLD_NOW|RTLD_LOCAL) : ifdef(osx, RTLD_SELF, NULL), symbol);
#endif
}
#endif



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
/* define preprocessor symbol 'sym'. Can put optional value */
LIBTCCAPI void (*tcc_define_symbol)(TCCState *s, const char *sym, const char *value);
void tcc_dlopen() {
    tcc_new = dll("libtcc.dll", "tcc_new");
    tcc_relocate = dll("libtcc.dll", "tcc_relocate");
    tcc_get_symbol = dll("libtcc.dll", "tcc_get_symbol");
    tcc_set_output_type = dll("libtcc.dll", "tcc_set_output_type");
    tcc_compile_string = dll("libtcc.dll", "tcc_compile_string");
    tcc_delete = dll("libtcc.dll", "tcc_delete");
    tcc_define_symbol = dll("libtcc.dll", "tcc_define_symbol");
    if(!tcc_new) DIE("cannot read libtcc.dll file");
}


void* c_symbol(const char *program, const char *sym) {
    static int open = 0; if(!open) open = 1, tcc_dlopen();

    TCCState *s = tcc_new();
    if( !s ) DIE("Cannot create a tcc context");
    tcc_define_symbol(s, "API", "");
    tcc_define_symbol(s, "APISTR", "\"tcc\"");
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    if( tcc_compile_string(s, program) > 0 ) {
        DIE("C compilation error");
    }

    tcc_relocate(s, TCC_RELOCATE_AUTO);
    return tcc_get_symbol(s, sym);
    // tcc_delete(s); // @leak
}


void lua_run(lua_State *L, const char *script) {
    int ret = luaL_dostring(L, script);
    if( ret != LUA_OK ) {
      PRINTF("!Script failed to run: %s\n", lua_tostring(L, -1));
      lua_pop(L, 1); // pop error message
    }
}
void lua_runfile(lua_State *L, const char *pathfile) {
    int loadResult = luaL_loadfile( L, pathfile );

    /**/ if( loadResult == LUA_OK ) {
	    lua_call(L, 0, 0);
    }
    else if( loadResult == LUA_ERRSYNTAX ) {
        PRINTF("!Script failed to load (LUA_ERRSYNTAX, '%s'): %s\n", lua_tostring( L, 1 ), pathfile );
//      lua_pop(L, 1); // pop error message
    }
    else if( loadResult == LUA_ERRMEM ) {
        PRINTF("!Script failed to load (LUA_ERRMEM): %s\n", pathfile);
    }
    else {
        PRINTF("!Script failed to load: %s\n", pathfile );
    }
}


#define API
#define APISTR "native"
#include "0/mod.c"

int main() {
	tick(); // C (native)

	void (*tick2)() = dll("plugins/0/mod.dll", "tick"); // C (dll)
	tick2();

    void (*tick3)() = c_symbol(readfile("plugins/0/mod.c",0), "tick"); // C (script)
	tick3();

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);
    lua_runfile(L, "plugins/0/mod.lua");
    lua_run(L,"mod.tick()");
}

