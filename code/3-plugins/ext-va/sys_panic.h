
#define PANIC(...)   PANIC(va(""__VA_ARGS__), __FILE__, __LINE__) // die() ?
API int (PANIC)(const char *error, const char *file, int line);


// ----------------------------------------------------------------------------
// panic

static void *panic_oom_reserve; // for out-of-memory recovery
int (PANIC)(const char *error, const char *file, int line) {
    panic_oom_reserve = SYS_MEM_REALLOC(panic_oom_reserve, 0);

    tty_color(RED);

    error += error[0] == '!';
    fprintf(stderr, "Error: %s (%s:%d) (errno:%s)\n", error, file, line, strerror(errno));
    fprintf(stderr, "%s", callstack(+16)); // no \n
    fprintf(stderr, "\n(This error has been copied to your clipboard)");
    fflush(0); // fflush(stderr);

    tty_color(0);

    glfwSetClipboardString(window_handle(), error);

    alert2("PANIC! (This error has been copied to your clipboard)", error);
    breakpoint();

    exit(-line);
    return 1;
}
