// ansi console utilities,
// - rlyeh, public domain

#define ANSI_BLUE   "\x1B[34;1m" // bright: on(;1m)
#define ANSI_RED    "\x1B[31;1m" // bright: on(;1m)
#define ANSI_PURPLE "\x1B[35;1m" // bright: on(;1m)
#define ANSI_GREEN  "\x1B[32;1m" // bright: on(;1m)
#define ANSI_CYAN   "\x1B[36;1m" // bright: on(;1m)
#define ANSI_YELLOW "\x1B[33;1m" // bright: on(;1m)
#define ANSI_WHITE  "\x1B[37;1m" // bright: on(;1m)
#define ANSI_GREY   "\x1B[30;1m" // bright: on(;1m)
#define ANSI_RESET  "\x1B[m"     // "\x1B[0m"

#if CODE
#ifdef _WIN32
#include <io.h>
void ansi(void) {
    static int counter = 0;
    if( counter++ ) {
        (printf)(ANSI_RESET);
    } else {
        void *handle;
        DWORD mode;
        if (GetConsoleMode(handle = GetStdHandle(STD_OUTPUT_HANDLE), &mode)) {
            SetConsoleMode(handle, mode | 4); /* ENABLE_VIRTUAL_TERMINAL_PROCESSING. ignore errors */
        }
        _pclose(_popen("chcp 65001 >nul", "r")); // enable unicode within terminal
        atexit(ansi);
    }
}
AUTORUN {
    ansi();
}
#endif
#endif
