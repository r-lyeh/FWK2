// ----------------------------------------------------------------------------

void ddraw_init() {}
void sprite_init() {}
void sprite_flush() {}
void profiler_init() {}
void scene_init() {}
void audio_init() {}
void midi_init() {}
void midi_quit() {}
void network_init() {}
void ui_engine() {}
API int engine_tick() { return 0; }

// ----------------------------------------------------------------------------

int window_frame_begin() {
    glfwPollEvents();

#if HAS_COOK
    // we cannot simply terminate threads on some OSes. also, aborted cook jobs could leave temporary files on disc.
    // so let's try to be polite: we will be disabling any window closing briefly until all cook is either done or canceled.
    static bool has_cook; do_once has_cook = !COOK_ON_DEMAND && have_tools() && cook_jobs();
    if( has_cook ) {
        has_cook = cook_progress() < 100;
        if( glfwWindowShouldClose(g->window) ) cook_cancel();
        glfwSetWindowShouldClose(g->window, GLFW_FALSE);
    }
#endif

    if( glfwWindowShouldClose(g->window) ) {
        return 0;
    }

    glNewFrame();

    ui_create();

#if !ENABLE_RETAIL
    bool has_menu = ui_has_menubar();
    bool may_render_debug_panel = 1;

#if HAS_COOK
    if( have_tools() ) {
        static int cook_has_progressbar; do_once cook_has_progressbar = !COOK_ON_DEMAND;
        if( cook_has_progressbar) {
            // render profiler, unless we are in the cook progress screen
            static unsigned frames = 0; if(frames <= 0) frames += cook_progress() >= 100;
            may_render_debug_panel = (frames > 0);
        }
    }
#endif

    if (!win_debug_visible)
        may_render_debug_panel = 0;

    // generate Debug panel contents
    if( may_render_debug_panel ) {
        if( has_menu ? ui_window("Debug " ICON_MD_SETTINGS, 0) : ui_panel("Debug " ICON_MD_SETTINGS, 0) ) {
            ui_engine();

            (has_menu ? ui_window_end : ui_panel_end)();
        }

        API int engine_tick();
        engine_tick();
    }
#endif // ENABLE_RETAIL

#if 0 // deprecated
    // run user-defined hooks
    for(int i = 0; i < 64; ++i) {
        if( hooks[i] ) hooks[i]( userdatas[i] );
    }
#endif

    double now = paused ? t : glfwGetTime();
    dt = now - t;
    t = now;

#if !ENABLE_RETAIL
    char *st = window_stats();
    static double timer = 0;
    timer += window_delta();
    if( timer >= 0.25 ) {
        glfwSetWindowTitle(window, st);
        timer = 0;
    }
#else
    glfwSetWindowTitle(window, title);
#endif

    void input_update();
    input_update();

    return 1;
}

void window_frame_end() {
    // flush batching systems that need to be rendered before frame swapping. order matters.
    {
        font_goto(0,0);
        void touch_flush();
        touch_flush();
        sprite_flush();

#if HAS_DD
        // flush all debugdraw calls before swap
        dd_ontop = 0;
        ddraw_flush();
        glClear(GL_DEPTH_BUFFER_BIT);
        dd_ontop = 1;
        ddraw_flush();
#endif

        ui_render();
    }

#if !is(ems)
    // save screenshot if queued
    if( screenshot_file[0] ) {
        int n = 3;
        void *rgb = screenshot(n);
        stbi_flip_vertically_on_write(true);
        if(!stbi_write_png(screenshot_file, w, h, n, rgb, n * w) ) {
            PANIC("!could not write screenshot file `%s`\n", screenshot_file);
        }
        screenshot_file[0] = 0;
    }
    if( record_active() ) {
        void record_frame();
        record_frame();
    }
#endif
}

void window_frame_swap() {
    // glFinish();
#if !is(ems)
    void window_vsync(float);
    window_vsync(hz);
#endif
    glfwSwapBuffers(window);
    // emscripten_webgl_commit_frame();

    static int delay = 0; do_once delay = optioni("--delay", 0);
    if( delay &&
#if HAS_COOK
        !COOK_ON_DEMAND &&
#endif
        cook_progress() >= 100
    ) sleep_ms( delay );
}

// ----------------------------------------------------------------------------

void lib_frame_end() {
#if HAS_FX
    if (GLOBAL_FX_PASS_ENABLED && !postfx_backbuffer_draw) {
        postfx_end(&fx, 0, 0);
    }
#endif
    window_frame_end();
}

void lib_frame_swap() {
    window_frame_swap();
}

int lib_frame_begin() {
#if HAS_FX
    if (GLOBAL_FX_PASS_ENABLED && !postfx_backbuffer_draw) {
        if (!postfx_begin(&fx, window_width(), window_height())) {
            fbo_unbind();
        }
    }
#endif
    return window_frame_begin();
}


void lib_post(float refreshRate) {
    int i;

    // init subsystems that DO NOT depend on cooked assets
    // camera inits for lib_pre_init() -> ddraw_flush() -> get_active_camera()
    // static camera_t cam = {0}; id44(cam.view); id44(cam.proj); extern camera_t *last_camera; last_camera = &cam;

        precook0();
        precook1();
        precook2();

        #pragma omp parallel for
        for( i = 0; i <= 3; ++i) {
            /**/ if( i == 0 ) ddraw_init();// init this on thread#0 since it will be compiling shaders, and shaders need to be compiled from the very same thread than glfwMakeContextCurrent() was set up
            else if( i == 1 ) sprite_init();
            else if( i == 2 ) profiler_init();
        }

        // window_swap();

    // display a progress bar meanwhile cook is working in the background
    // Sleep(500);
#if HAS_COOK
    if( !COOK_ON_DEMAND )
    if( have_tools() && cook_jobs() )
    while( cook_progress() < 100 ) {
        for( int frames = 0; frames < 2/*10*/ && window_swap(); frames += cook_progress() >= 100 ) {
            window_title(va("%s %.2d%%", cook_cancelling ? "Aborting" : "Cooking assets", cook_progress()));
            if( input(KEY_ESC) ) cook_cancel();

            glNewFrame();

            cook2();

            do_once window_visible(1);

            // render progress bar at 30Hz + give the cook threads more time to actually cook the assets.
            // no big deal since progress bar is usually quiet when cooking assets most of the time.
            // also, make the delay even larger when window is minimized or hidden.
            // shaved cook times: 88s -> 57s (tcc), 50s -> 43s (vc)
            sleep_ms( window_has_visible() && window_has_focus() ? 8 : 16 );
        }
        // set black screen
        glNewFrame();
        window_swap();
#if !ENABLE_RETAIL
        window_title("");
#endif
    }

    if(cook_cancelling) cook_stop(), exit(-1);
    cook_done = true;
#endif

    // <-- assets were cooked at this point !!

    // init subsystems that DO depend on cooked assets now

    #pragma omp parallel for
    for( i = 0; i <= 3; ++i ) {
        if(i == 0) scene_init(); // init these on thread #0, since both will be compiling shaders, and shaders need to be compiled from the very same thread than glfwMakeContextCurrent() was set up
        if(i == 1) audio_init(0);
        if(i == 2) kit_init(), midi_init();
        if(i == 3) network_init();
    }

    postcook2();
    postcook1(refreshRate);
    postcook0();
}

// ----------------------------------------------------------------------------

void lib_quit(void) {
    quit2();
    quit1();
    quit0();

    midi_quit();
}

void lib_init() {
    do_once {
        init0();
        init1();
        init2();

        atexit(lib_quit);
    }
}
