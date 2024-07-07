// -----------------------------------------------------------------------------
// visibility
// win32 users would need to -DAPI=EXPORT/IMPORT as needed when building/using ENGINE as DLL.

#ifndef IMPORT
#define IMPORT ifdef(win32, ifdef(gcc, __attribute__ ((dllimport)), __declspec(dllimport)))
#endif
#ifndef EXPORT
#define EXPORT ifdef(win32, ifdef(gcc, __attribute__ ((dllexport)), __declspec(dllexport)))
#endif
#ifndef STATIC
#define STATIC
#endif

#ifndef API
#define API STATIC
#endif

// -----------------------------------------------------------------------------
// system defines

#ifndef _GNU_SOURCE
#define _GNU_SOURCE ///- linux
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS ///- win32
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE ///- win32
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  ///- win32 GetInfoAddrW/FreeAddrInfoW for X86
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS ///- win32
#endif

// -----------------------------------------------------------------------------
// system headers

#ifdef _MSC_VER
#include <omp.h>      // compile with /openmp to speed up some internal computations. will require VC redistributables installed, though.
#endif
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

//

#include "sys_macros.h" // first
#include "sys_glue.h"  // after macros, cross-platform glue code

#include "sys_ansi.h"
#include "sys_args.h"
#include "sys_debug.h"
#include "sys_die.h"
#include "sys_flag.h"
#include "sys_folder.h"
#include "sys_sleep.h"
#include "sys_string.h"
#include "sys_test.h"
#include "sys_time.h"
#include "sys_trace.h"
#include "sys_trap.h"
