#include "../engine.h"
//#include "module.h"
//#include "module.3"

// -----------------------------------------------------------------------------
// begin

ifdef(retail, AUTORUN {
    fclose(stderr);
    fclose(stdout);

    const char* null_stream = ifdef(win32, "nul:", "/dev/null");

    if (!freopen(null_stream, "a", stdout)) PANIC("cannot recreate standard streams");
    if (!freopen(null_stream, "a", stderr)) PANIC("cannot recreate standard streams");
} )

// -----------------------------------------------------------------------------

#include "render_compat.c"
#include "render_state.c"
#include "render_texture.c"

#include "engine_ui.c"
#include "engine_window_hook.c"
#include "engine_window_record.c"
#include "engine_window_tests.c"
#include "engine_window.c" // after ui
#include "engine_window_fps.c"
#include "engine_input.c"
#include "engine_vfs.c"


void init1() {
    // init glfw
    glfw_init();
}

void quit1() {

}

void precook1() {
    touch_init(); // for ems

#if ENABLE_REVERSE_Z
    if (GLAD_GL_ARB_clip_control || (GLAD_VERSION_MAJOR(gl_version) >= 4 && GLAD_VERSION_MINOR(gl_version) >= 5)) {
        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
        gl_reversez = 1;
    }
#endif

    vfs_reload();
}

void postcook1(float refreshRate) {
    ui_init(); // init these on thread #0, since both will be compiling shaders, and shaders need to be compiled from the very same thread than glfwMakeContextCurrent() was set up
    window_icon(va("%s.png", app_name())); // init on thread #0, because of glfw
    input_init(); // init on thread #0, because of glfw

    // set black screen
    glClearColor(0,0,0,1);
    glClearDepth(gl_reversez ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);

    // display window
    glfwShowWindow(window);
    glfwGetFramebufferSize(window, &w, &h); //glfwGetWindowSize(window, &w, &h);

    randset(time_ns() * !tests_captureframes());
    boot_time = -time_ss(); // measure boot time, this is continued in window_stats()

    hz = refreshRate;
    // t = glfwGetTime();

    // preload brdf LUT early
    (void)brdf_lut();

    uint64_t fps = optioni("--fps", 0);
    if( fps ) {
        window_fps_lock(fps);
    }
}
