#if !CODE

API extern FILE *stdlog;

//#define PRINTF(fmt,...) fprintf(stdlog, __FUNCTION__ "() " __VA_ARGS__)

#ifndef FPRINTF
#define FPRINTF(log,fmt,...) ( flockfile(log), \
    fprintf(log, ANSI_GREY("%07.3f|"), time_ss()), \
    fprintf(log, strstri(fmt, "fail") || strstri(fmt,"error") ? ANSI_RED(__FUNCTION__ "() " fmt) : \
                    strstri(fmt,"warn") || strstri(fmt,"not ") ? ANSI_YELLOW(__FUNCTION__ "() " fmt) : \
                    __FUNCTION__ "() " fmt, __VA_ARGS__), \
    0[fmt] == '!' && fprintf(log, "\n%s", trace(+16)), \
    funlockfile(log), 1)
#endif

#ifndef PRINTF
#define PRINTF(fmt,...) FPRINTF(stdlog ? stdlog : stdout, fmt, __VA_ARGS__)
#endif

#else

FILE *stdlog;

AUTORUN {
    // to console
    stdlog = stdout;

    // to logfile
    if( flag("--logfile") )
    stdlog = fopen(".log.txt","a+");

    fprintf(stdlog, "---\nnew session\n---\n");
}

// @todo:
// 1[""#__VA_ARGS__] == '!' ? callstack(+48) : "", __FILE__, __LINE__, __FUNCTION__

// @todo:
// static int __thread _thread_id;
// #define PRINTF(...)      (printf("%03d %07.3fs|%-16s|", (((unsigned)(uintptr_t)&_thread_id)>>8) % 1000, time_ss(), __FUNCTION__), printf(__VA_ARGS__), printf("%s", 1[#__VA_ARGS__] == '!' ? callstack(+48) : "")) // verbose logger

// @todo:
// #if ENABLE_RETAIL
// #undef  PRINTF
// #define PRINTF(...) 0
// #endif

// @todo: enable this block
// ifdef(retail, AUTORUN {
//     fclose(stderr);
//     fclose(stdout);
//     const char* null_stream = ifdef(win32, "nul:", "/dev/null");
//     if (!freopen(null_stream, "a", stdout)) die("cannot recreate standard streams");
//     if (!freopen(null_stream, "a", stderr)) die("cannot recreate standard streams");
// } )

#endif
