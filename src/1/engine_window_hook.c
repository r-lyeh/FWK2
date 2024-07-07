//-----------------------------------------------------------------------------

#if 0 // deprecated
static void (*hooks[64])() = {0};
static void *userdatas[64] = {0};

bool window_hook(void (*func)(), void* user) {
    window_unhook( func );
    for( int i = 0; i < 64; ++i ) {
        if( !hooks[i] ) {
            hooks[i] = func;
            userdatas[i] = user;
            return true;
        }
    }
    return false;
}
void window_unhook(void (*func)()) {
    for( int i = 0; i < 64; ++i ) {
        if(hooks[i] == func) {
            hooks[i] = 0;
            userdatas[i] = 0;
        }
    }
}
#endif
