// [ref] https://gafferongames.com/post/floating_point_determinism/
// @todo:
// - Make sure the FPU state is consistent across platforms / threads.
//   - Check the floating point rounding behavior (should be nearest).
//   - Check that the denormals are zero (DAZ) and flush to zero (FTZ) flags are set consistently.

#if CODE
#include <float.h> // fenv.h
//#pragma fenv_access (on)

void x87_tick() {
    do_once {
        _controlfp(_PC_24, _MCW_PC);
        _controlfp(_RC_NEAR, _MCW_RC);
    }
    ASSERT( (_controlfp(0, 0) & _MCW_PC) == _PC_24 );
    ASSERT( (_controlfp(0, 0) & _MCW_RC) == _RC_NEAR );
}

AUTORUN {
    if( is(x32) ) hooks("tick", x87_tick);
}
#endif
