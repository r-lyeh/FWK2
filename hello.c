// playground tests
// - rlyeh, public domain
//
// # quickstart
// - win/vc       : cl hello.c
// - win/clang-cl : clang-cl  hello.c
// - win/tcc      : tools\tcc hello.c -m64
// - win/mingw    : gcc   hello.c -lws2_32 -lwinmm -ldbghelp -lole32 -luser32 -lgdi32 -lcomdlg32
// - win/clang    : clang hello.c -lws2_32 -lwinmm -ldbghelp -lole32 -luser32 -lgdi32 -lcomdlg32
// - linux        : cc  hello.c -lm -ldl -lpthread -lX11
// - linux/tcc    : tcc hello.c -lm -ldl -lpthread -lX11 -D__STDC_NO_VLA__
// - osx          : cc -ObjC hello.c -framework cocoa -framework iokit -framework audiotoolbox

#include "engine.h"

#pragma warning(disable : 4716)
#define main CONCAT(app,__COUNTER__)
#define appname CONCAT(appname,__COUNTER__)

#include "art/demos/00-null.c"
#include "art/demos/01-window.c"
#include "art/demos/02-opengl.c"
#include "art/demos/03-imgui.c"

#undef appname
#undef main

const char *trim(const char *x) {
    const char *a = strrchr(x, '\\');
    const char *b = strrchr(x, '/');
    if( a && b ) return a > b ? a+1 : b+1;
    return a ? a+1 : (b ? b+1 : x);
}

int main() {
    int (*demos[])() = {
        app1,app4,app7,app9,
    };
    const char *names[] = {
        appname2,appname5,appname8,appname10,
    };

    int demo = argc() > 1 && argv(1)[0] >= '0' && argv(1)[0] <= '9' ? atoi(argv(1)) : -1;
    if( demo >= 0 && demo < COUNTOF(demos) ) return demos[demo]();

    window_debug(0);
    window_create(100, WINDOW_TRANSPARENT);

    static int open = 1;
    while( window_swap() && !input(KEY_ESC) && open ) {
        if( ui_window("Demo Launcher", &open) ) {
            for( int i = 0; i < COUNTOF(demos); ++i)
            if( ui_button(trim(names[i])) ) system(va("%s %d", argv(0), i));
            ui_separator();
            ui_window_end();
        }
    }
    return 0;
}
