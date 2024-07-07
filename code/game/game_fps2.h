//-----------------------------------------------------------------------------
// fps locking

API float fps(); // current fps
API float fps_get_target();
API void  fps_set_target(float target); // [0] vsync off, [1] vsync on ... [N] specific rate

#if CODE

extern volatile float target_fps;
static volatile unsigned fps_limiter_isrunning, fps_limiter_count, fps_limiter_numloops;
static
int fps_limit_thread(void *arg) {
    int64_t ns_excess = 0;
    while( fps_limiter_isrunning ) {
        if( target_fps <= 1 ) {
            fps_limiter_numloops = fps_limiter_count = 0;
            SDL_Delay(250);
        } else {
            fps_limiter_count++;
            int64_t tt = (int64_t)(1e9/(float)target_fps) - ns_excess;
            uint64_t took = -SDL_GetTicksNS();
            SDL_DelayPrecise( tt > 0 ? (float)tt : 0.f );
            took += SDL_GetTicksNS();
            ns_excess = took - tt;
            if( ns_excess < 0 ) ns_excess = 0;
            //puts( strf("%lld", ns_excess) );
        }
    }
    fps_limiter_isrunning = 1;

    (void)arg;
    return 0;
}

//-----------------------------------------------------------------------------

// function that locks render to desired `FPS` framerate.
// - returns true if must render, else 0.

static
int _fps_limit(float fps) {
    target_fps = fps; // fps * (fps > 0)
    if( target_fps <= 0 ) return 1;

    do_once {
        // private threaded timer
        fps_limiter_isrunning = 1, fps_limiter_count = fps_limiter_numloops = 0;
        SDL_CreateThread( fps_limit_thread, "fps_limit_thread()", NULL );
    }

    // if we throttled too much, cpu idle wait
    while( fps_limiter_isrunning && (fps_limiter_numloops > fps_limiter_count) ) {
        //thread_yield();
        SDL_DelayPrecise(100);
    }

    // max auto frameskip is 10: ie, even if speed is low paint at least one frame every 10
    enum { maxframeskip = 10 };
    if( fps_limiter_count > fps_limiter_numloops + maxframeskip ) {
        fps_limiter_numloops = fps_limiter_count;
    }
    fps_limiter_numloops++;

    // only draw if we are fast enough, otherwise skip the frame
    return fps_limiter_numloops >= fps_limiter_count;
}

static
void _fps_tick() {
    _fps_limit( target_fps );
}

float fps() {
    return render_fps();
}
void  fps_set_target(float fps) {
    target_fps = fps;
}
float fps_get_target() {
    return target_fps;
}

// this is handled in game_app inner loop for now
// AUTORUN { hooks("tick", _fps_tick); }

#endif
