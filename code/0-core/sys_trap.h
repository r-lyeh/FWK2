API void        trap_install(void);
API const char *trap_name(int signal);      // helper util
API void        trap_on_ignore(int signal); // helper util
API void        trap_on_quit(int signal);   // helper util
API void        trap_on_abort(int signal);  // helper util
API void        trap_on_debug(int signal);  // helper util

#if CODE
#include <signal.h>

// trap signals ---------------------------------------------------------------

const char *trap_name(int signal) {
    if(signal == SIGABRT) return "SIGABRT - \"abort\", abnormal termination";
    if(signal == SIGFPE) return "SIGFPE - floating point exception";
    if(signal == SIGILL) return "SIGILL - \"illegal\", invalid instruction";
    if(signal == SIGSEGV) return "SIGSEGV - \"segmentation violation\", invalid memory access";
    if(signal == SIGINT) return "SIGINT - \"interrupt\", interactive attention request sent to the program";
    if(signal == SIGTERM) return "SIGTERM - \"terminate\", termination request sent to the program";
    ifndef(win32, if(signal == SIGBUS) return "SIGBUS");
    ifdef(linux, if(signal == SIGSTKFLT) return "SIGSTKFLT");
    ifndef(win32, if(signal == SIGQUIT) return "SIGQUIT");
    return "??";
}
void trap_on_ignore(int sgn) {
    signal(sgn, trap_on_ignore);
}
void trap_on_quit(int sgn) {
    signal(sgn, trap_on_quit);
    exit(0);
}
void trap_on_abort(int sgn) {
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected signal %s (%d)\n%s", trap_name(sgn), sgn, trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    signal(sgn, trap_on_abort);
    exit(-sgn);
}
void trap_on_debug(int sgn) { // @todo: rename to trap_on_choice() and ask the developer what to do next? abort, continue, debug
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected signal %s (%d)\n%s", trap_name(sgn), sgn, trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    signal(sgn, trap_on_debug);
}
#if is(win32)
LONG WINAPI trap_on_SEH(PEXCEPTION_POINTERS pExceptionPtrs) {
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected SEH exception\n%s", trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    return EXCEPTION_EXECUTE_HANDLER; // Execute default exception handler next
}
#endif
void trap_install(void) {
    // expected signals
    signal(SIGINT, trap_on_quit);
    signal(SIGTERM, trap_on_quit);
    ifndef(win32, signal(SIGQUIT, trap_on_quit));
    // unexpected signals
    signal(SIGABRT, trap_on_abort);
    signal(SIGFPE, trap_on_abort);
    signal(SIGILL, trap_on_abort);
    signal(SIGSEGV, trap_on_abort);
    ifndef(win32, signal(SIGBUS, trap_on_abort));
    ifdef(linux, signal(SIGSTKFLT, trap_on_abort));
    // others
    ifdef(win32,SetUnhandledExceptionFilter(trap_on_SEH));
}

#ifdef TRAP_DEMO
AUTORUN {
    trap_install();
    app_crash(); // app_hang();
}
#endif

AUTORUN {
    // install signal handlers
    ifdef(debug, trap_install());
}

#endif // CODE
