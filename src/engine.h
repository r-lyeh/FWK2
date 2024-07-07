/* Game framework.
 * - rlyeh, public domain
 *
 * ## License
 *
 * This software is available under 3 licenses. Choose whichever you prefer.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE A - Public Domain (https://unlicense.org/)
 * ------------------------------------------------------------------------------
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE B - 0-BSD License (https://opensource.org/licenses/FPL-1.0.0)
 * ------------------------------------------------------------------------------
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE C - MIT-0 (No Attribution clause)
 * ------------------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ## License: Contributed Code ------------------------------------------------
 *
 * Dear Contributor,
 *
 * In order to ensure this project remains completely free and unencumbered by
 * anyone's copyright monopoly, it is advisable that you dedicate your code-base
 * contributions to the three licensing terms above. This removes any possible
 * ambiguity as to what terms somebody might have thought they were contributing
 * under, in case of a future dispute. These concerns are not unique to public
 * domain software. Most large, established open-source projects have a
 * Contributor License Agreement (CLA) process, of varying degrees of formality.
 *
 * Please add yourself to the list below before contributing.
 * Thanks.
 *
 * --
 *
 * "I dedicate any and all copyright interest in this software to the three
 * licensing terms listed above. I make this dedication for the benefit of the
 * public at large and to the detriment of my heirs and successors. I intend
 * this dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law."
 *
 * Author (name)              I agree (YES/NO)    Files/Features (optional)
 * ------------------------------------------------------------------------------
 * @r-lyeh                    YES                 Initial codebase
 * @zak@v4.games              YES                 N/A
 * ------------------------------------------------------------------------------
 */

#ifndef ENGINE_H
#define ENGINE_H

// -----------------------------------------------------------------------------
// system defines

#ifndef _GNU_SOURCE
#define _GNU_SOURCE ///- linux
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS ///- win32
#define _CRT_NONSTDC_NO_DEPRECATE ///- win32
#define _WIN32_WINNT 0x0600  ///- win32 GetInfoAddrW/FreeAddrInfoW for X86
#define _WINSOCK_DEPRECATED_NO_WARNINGS ///- win32
#endif

// -----------------------------------------------------------------------------
// system headers

#ifdef _MSC_VER
#include <omp.h>      // compile with /openmp to speed up some internal computations
#endif
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifdef __cplusplus
extern "C" {
#endif

#if __has_include("0/module.h")
#include "0/module.h"
#endif

#if __has_include("1/module.h")
#include "1/module.h"
#endif

#if __has_include("2/module.h")
#include "2/module.h"
#endif

#if __has_include("3/module.h")
#include "3/module.h"
#endif

#if __has_include("4/module.h")
#include "4/module.h"
#endif

#ifdef __cplusplus
}
#endif

// expose glfw/glad apis
#if is(ems)
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #define gladLoadGL(func) (glewExperimental = true, glewInit() == GLEW_OK) ///-
#else
    #if is(win32) /*&& is(tcc)*/ // && ENABLE_DLL
    #ifdef GLAD_API_CALL
    #undef GLAD_API_CALL
    #endif
    #define GLAD_API_CALL extern API ///-
    #endif
    #ifndef GLAD_GL_H_
    #include "engine.3"
    #endif
#endif

#endif // ENGINE_H
