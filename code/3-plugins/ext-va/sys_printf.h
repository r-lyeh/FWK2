
#define PRINTF(...)  PRINTF(va(""__VA_ARGS__), 1[""#__VA_ARGS__] == '!' ? callstack(+48) : "", __FILE__, __LINE__, __FUNCTION__)
API int (PRINTF)(const char *text, const char *stack, const char *file, int line, const char *function);

#if ENABLE_RETAIL
#undef  PRINTF
#define PRINTF(...) 0
#endif

// ----------------------------------------------------------------------------
// logger

//static int __thread _thread_id;
//#define PRINTF(...)      (printf("%03d %07.3fs|%-16s|", (((unsigned)(uintptr_t)&_thread_id)>>8) % 1000, time_ss(), __FUNCTION__), printf(__VA_ARGS__), printf("%s", 1[#__VA_ARGS__] == '!' ? callstack(+48) : "")) // verbose logger

int (PRINTF)(const char *text, const char *stack, const char *file, int line, const char *function) {
    double secs = time_ss();
    uint32_t color = 0;
    /**/ if( strstri(text, "fail") || strstri(text, "error") ) color = RED;
    else if( strstri(text, "warn") || strstri(text, "not found") ) color = YELLOW;
    #if is(cl)
    char *slash = strrchr(file, '\\'); if(slash) file = slash + 1;
    #endif
    char *location = va("|%s|%s:%d", /*errno?strerror(errno):*/function, file, line);
    int cols = tty_cols() + 1 - (int)strlen(location);

    flockfile(stdout);

    tty_color(color);
    printf("\r%*.s%s", cols, "", location);
    printf("\r%07.3fs|%s%s", secs, text, stack);
    tty_color(0);

    funlockfile(stdout);

    return 1;
}
