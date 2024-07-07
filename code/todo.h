// @todo useful? before SDL_init(): SDL_SetAppMetadata("Example App", "1.0", "com.example.app");

/* 

@todo: find a way to fix SDL click passthrough on transparent windows + sdl3 backend. idea:

{
SDL_Window* window = SDL_GL_GetCurrentWindow(); // SDL_GetCurrentWindow();
SDL_Surface *surface = SDL_GetWindowSurface(window);
SDL_SetWindowShape(window, surface);
}

*/

#if !CODE

#define HAS_UI 1
#define BREAK DebugBreak()
#define EVAL atof
#define macro MACRO

#define ENABLE_PROFILER 1

#define app_fps() fps()
#define app_width() ((int)igGetIO()->DisplaySize.x)
#define app_height() ((int)igGetIO()->DisplaySize.y)
#define app_scale() (1.f)
#define app_time() time_ss()
#define app_path() app_recv("APPDIR")
#define app_resize(w,h) SDL_SetWindowSize(window,w,h)
#define app_frame() render_frame()
#define app_restart() app_send("restart", "")
//#define app_cursor(m) do { char x[] = "0"; x[0] += m; mouse_send("cursor", x); } while(0)
#define app_has_focus() (!!(SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS)))
#define app_focus() SDL_RaiseWindow(window)
#define app_fullscreen(x) app_send("fullscreen", va("%d", !!(x)))
#define app_has_fullscreen() atoi(app_recv("fullscreen"))
#define app_clipboard(x) app_send("clipboard", va("%d", !!(x)))
#define app_has_clipboard() app_recv("clipboard")
#define app_maximize(x) (x ? SDL_MaximizeWindow(app_handle("window")) : SDL_RestoreWindow(app_handle("window")))
#define app_has_maximize() (!!(SDL_GetWindowFlags(app_handle("window")) & SDL_WINDOW_MAXIMIZED))
#define app_title(x) app_send("title", x)

static
char *file_pathabs( const char *pathfile ) {
    char *out = va("%*.s", DIR_MAX+1, "");
#if is(win32)
    _fullpath(out, pathfile, DIR_MAX);
#else
    realpath(pathfile, out);
#endif
    return out;
}

#define input_filter_deadzone(...) vec2(0,0)
#define input_string(x) ""
#define input_use(x)

#define was_deprecated(old,new) old##_renamed_##new
#define ui_show was_deprecated(ui_show, ui_window_show)
#define ui_visible was_deprecated(ui_visible, ui_window_shown)
#define window_delta was_deprecated(window_delta, app_delta)
#define window_width was_deprecated(window_width, app_width)
#define window_height was_deprecated(window_height, app_height)
#define window_time was_deprecated(window_time, time_ss)
#define window_resize was_deprecated(window_resize, app_resize)
#define window_frame was_deprecated(window_frame, app_frame)
#define window_reload was_deprecated(window_reload, app_restart)
#define window_cursor was_deprecated(window_cursor, app_cursor)
#define window_fullscreen was_deprecated(window_fullscreen, app_fullscreen)
#define window_has_fullscreen was_deprecated(window_has_fullscreen, app_has_fullscreen)
#define window_maximize was_deprecated(window_maximize, app_maximize)
#define window_has_maximize was_deprecated(window_has_maximize, app_has_maximize)
#define window_handle was_deprecated(window_handle, app_handle)
#define window_scale was_deprecated(window_scale, app_scale)
#define window_fps was_deprecated(window_fps, render_fps)
#define window_fps_target was_deprecated(window_fps_target, fps_get_target)
#define window_has_pause was_deprecated(window_has_pause, app_has_pause)

#endif
