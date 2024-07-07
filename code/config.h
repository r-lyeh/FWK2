// -----------------------------------------------------------------------------
// config directives

#if 0

#ifndef ENABLE_FASTCALL_LUA
#define ENABLE_FASTCALL_LUA     1 ///+
#endif

#ifndef ENABLE_PROFILER
#define ENABLE_PROFILER         ifdef(retail, 0, 1)  ///+
#endif

#ifndef ENABLE_SELFIES
#define ENABLE_SELFIES          0 ///+
#endif

#ifndef ENABLE_MEMORY_POISON
#define ENABLE_MEMORY_POISON    ifdef(debug, 1, 0) ///+
#endif

#ifndef ENABLE_MEMORY_LEAKS
#define ENABLE_MEMORY_LEAKS     0 ///+
#endif

#ifndef ENABLE_LINUX_CALLSTACKS
#define ENABLE_LINUX_CALLSTACKS 0 ///+
#endif

#ifndef ENABLE_AUTOTESTS
#define ENABLE_AUTOTESTS        ifdef(debug, ifndef(ems, 1, 0), 0) ///+
#endif

#ifndef ENABLE_RETAIL
#define ENABLE_RETAIL           0 // ifdef(retail, 1, 0) ///+
#endif

#ifndef ENABLE_COOK
#define ENABLE_COOK             ifdef(retail, 0, 1) ///+
#endif

#ifndef ENABLE_RPMALLOC
#define ENABLE_RPMALLOC         0 // ifdef(tcc, 0, 1) // forbidden on tcc because of lacking TLS support
#endif

#endif
