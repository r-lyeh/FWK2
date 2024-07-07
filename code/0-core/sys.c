#include "sys.h"
#define CODE 1
#include "sys.h"

// -----------------------------------------------------------------------------
// pragma libs

#if is(win32) && (is(cl) || is(tcc))
//#pragma comment(lib, "advapi32")
//#pragma comment(lib, "comdlg32")
//#pragma comment(lib, "dbghelp")
//#pragma comment(lib, "gdi32")
//#pragma comment(lib, "ole32")
//#pragma comment(lib, "shell32")
//#pragma comment(lib, "user32")
//#pragma comment(lib, "winmm")
//#pragma comment(lib, "wininet")
//#pragma comment(lib, "ws2_32")
#endif

#if is(linux) && is(tcc)
#pragma comment(lib, "m")
#endif

void init0() {
    // chdir to root (if invoked as tcc -g -run)
    // chdir(app_path());
}

void quit0() {
}
