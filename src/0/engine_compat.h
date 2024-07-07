// ----------------------------------------------------------------------------
// non-gnu fixes

#if is(msc)
#define __thread                   __declspec(thread)
#elif is(tcc) && is(win32)
#define __thread                   __declspec(thread) // compiles fine apparently, but does not work
#elif is(tcc)
#define __thread
#endif

// -----------------------------------------------------------------------------
// tcc fixes

#if is(win32) && is(tcc) && is(x64)
// missing libm symbols on tinycc HEAD repo (tcc-x64 pre-0.9.28)
#include <tgmath.h>
#define sqrtf sqrt
#define sinf sin
#define asinf asin
#define cosf cos
#define acosf acos
#define tanf tan
#define atan2f atan2
#define powf pow
#define floorf floor
#define logf log
#define ceilf ceil
#define copysignf copysign
#define expf exp
#define fmodf fmod
#define log10f log10
#define hypotf hypot
// `tcc0.9.27 -m64` workaround (win) for all functions with ending bool argument
#undef  bool
typedef char bool; ///-
#endif

#if defined __TINYC__ && defined __linux
#   ifndef __builtin_alloca
#   define __builtin_alloca alloca
#   endif
#endif
