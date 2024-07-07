//-----------------------------------------------------------------------------
// fps locking

static volatile float framerate = 0;
static volatile unsigned fps_active, timer_counter, loop_counter;
static
int fps__timing_thread(void *arg) {
    int64_t ns_excess = 0;
    while( fps_active ) {
        if( framerate <= 0 ) {
            loop_counter = timer_counter = 0;
            sleep_ms(250);
        } else {
            timer_counter++;
            int64_t tt = (int64_t)(1e9/(float)framerate) - ns_excess;
            uint64_t took = -time_ns();
            #if is(win32)
            timeBeginPeriod(1);
            #endif
            sleep_ns( tt > 0 ? (float)tt : 0.f );
            took += time_ns();
            ns_excess = took - tt;
            if( ns_excess < 0 ) ns_excess = 0;
            //puts( strf("%lld", ns_excess) );
        }
    }
    fps_active = 1;

    (void)arg;
    return thread_exit(0), 0;
}
static
void fps_locker( int on ) {
    if( on ) {
        // private threaded timer
        fps_active = 1, timer_counter = loop_counter = 0;
        thread_init( fps__timing_thread, 0, "fps__timing_thread()", 0 );
    } else {
        fps_active = 0;
    }
}
// function that locks render to desired `framerate` framerate (in FPS).
// - assumes fps_locker() was enabled beforehand.
// - returns true if must render, else 0.
static
int fps_wait() {
    if( framerate <= 0 ) return 1;
    if( !fps_active ) return 1;

    // if we throttled too much, cpu idle wait
    while( fps_active && (loop_counter > timer_counter) ) {
        //thread_yield();
        sleep_ns(100);
    }

    // max auto frameskip is 10: ie, even if speed is low paint at least one frame every 10
    enum { maxframeskip = 10 };
    if( timer_counter > loop_counter + maxframeskip ) {
        loop_counter = timer_counter;
    }
    loop_counter++;

    // only draw if we are fast enough, otherwise skip the frame
    return loop_counter >= timer_counter;
}
static
void window_vsync(float hz) {
    if( tests_captureframes() ) return;
    if( hz <= 0 ) return;
    do_once fps_locker(1);
    framerate = hz;
    fps_wait();
}

//-----------------------------------------------------------------------------

double window_fps() {
    return fps;
}
void window_fps_lock(float fps) {
    hz = fps;
}
void window_fps_unlock() {
    hz = 0;
}
double window_fps_target() {
    return hz;
}

void window_fps_vsync(int vsync) {
    glfwSwapInterval(vsync);
}

