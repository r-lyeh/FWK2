#if CODE

int  editor_fps;
bool editor_powersave;

EDITOR_FUNCTION(key_powersave, "held(ALT) & down(P)") {
    editor_powersave ^= 1;
    ui_notify("Setting changed", va("editor_powersave = %d", editor_powersave));
}

void editor_tick_fps() {
    do_once {
        extern float monitor_fps;
        editor_fps = optioni("--editor.fps", monitor_fps + 0.5); // defaults to monitor_rate

        // [0]no batt found, [+100]charged, [+X]charging, [-X]draining
        int battery = atoi(app_recv("battery"));
        if( battery < 0 ) key_powersave(); // editor_powersave = 1;
    }

    SDL_WindowFlags slow_window_flags =
        SDL_GetWindowFlags(app_handle("window")) & (SDL_WINDOW_OCCLUDED|SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED);

    // adaptive framerate based on battery charge / foreground app status. typical case:
    //       fg bg app
    // batt+ 60 10
    // batt- 30  5
    int target = slow_window_flags ? editor_fps/6 : editor_fps;
    if( editor_powersave ) target /= 2;
    command2("app.fps", va("%d", target));
}

AUTORUN {
    if( HAS_EDITOR )
    hooks("tick", editor_tick_fps);
}

#endif
