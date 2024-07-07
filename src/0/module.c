#include "../engine.h"
//#include "engine.h"
//#include "engine.3"

// -----------------------------------------------------------------------------
// pragma libs

#if is(win32) && (is(cl) || is(tcc))
#pragma comment(lib, "advapi32")
#pragma comment(lib, "comdlg32")
#pragma comment(lib, "dbghelp")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "ole32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "user32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "wininet")
#pragma comment(lib, "ws2_32")
#endif

#if is(linux) && is(tcc)
#pragma comment(lib, "dl")
#pragma comment(lib, "m")
#pragma comment(lib, "pthread")
#endif

// -----------------------------------------------------------------------------
// begin

#define do_threadlock(mutexptr) \
    for( int init_ = !!(mutexptr) || (thread_mutex_init( (mutexptr) = CALLOC(1, sizeof(thread_mutex_t)) ), 1); init_; init_ = 0) \
    for( int lock_ = (thread_mutex_lock( mutexptr ), 1); lock_; lock_ = (thread_mutex_unlock( mutexptr ), 0) )

#define AS_NKCOLOR(color) \
    ((struct nk_color){ ((color>>0))&255,((color>>8))&255,((color>>16))&255,((color>>24))&255 })

static inline
char *file_preprocess(const char *src, const char *path, char* (*)(const char *), const char *parent_function);

static bool postfx_debug_tool_enabled = false;

ifdef(retail, AUTORUN {
    fclose(stderr);
    fclose(stdout);

    const char* null_stream = ifdef(win32, "nul:", "/dev/null");

    if (!freopen(null_stream, "a", stdout)) PANIC("cannot recreate standard streams");
    if (!freopen(null_stream, "a", stderr)) PANIC("cannot recreate standard streams");
} )

// -----------------------------------------------------------------------------

#include "engine_compat.c" // early
#include "engine_ds.c"
#include "engine_extend.c"
#include "engine_file.c"
#include "engine_less.c"
#include "engine_hash.c"
#include "engine_memory.c"
#include "engine_pack.c"
#include "engine_reflect.c"
#include "engine_obj.c" // after reflect
#include "engine_string.c"
#include "engine_system.c"
#include "engine_time.c"
#include "engine_types.c"
#include "engine_math.c" // after types

void init0() {
    // install signal handlers
    ifdef(debug, trap_install());

    // init panic handler
    panic_oom_reserve = SYS_MEM_REALLOC(panic_oom_reserve, 1<<20); // 1MiB

    // enable ansi console
    tty_init();

    // chdir to root (if invoked as tcc -g -run)
    // chdir(app_path());

    // skip tcc argvs (if invoked as tcc file.c engine.c -g -run) (win)
    if( __argc > 1 ) if( strstr(__argv[0], "/tcc") || strstr(__argv[0], "\\tcc") ) {
        __argc = 0;
    }
}

void quit0() {
    storage_flush();
}

void precook0() {
    window_icon(va("%s%s.png", app_path(), app_name()));
    glfwPollEvents();
    storage_mount("save/"), storage_read(); // for ems
}

void postcook0() {
    script_init();
}
