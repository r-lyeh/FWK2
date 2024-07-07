#if !CODE

API int ui_editor_controlbar(unsigned frame, double t, float slomo);

#else

EDITOR_FUNCTION(key_play, "down(F1)|down(F5)") {
    app_pause(0);
    // app_slomo(1);
}
EDITOR_FUNCTION(key_pause, "down(F2)") {
    app_pause(1);
}
EDITOR_FUNCTION(key_stop, "down(ESC)") {
    app_pause(1);
    // app_time(0);
}
EDITOR_FUNCTION(key_frame, "down(F4)") {
}
EDITOR_FUNCTION(key_slomo, "0") {
}
EDITOR_FUNCTION(key_eject, "held(SHIFT)&down(F1)") {
    // in_editor ^= 1;
}

int ui_editor_controlbar(unsigned frame, double t, float slomo) {
    if( !ui_debug ) return 0;

        static int stats_mode = 1;
        const char *stats = va("x%4.3f %03d.%03dss %02dF %s",
            slomo, (int)t, (int)(1000 * (t - (int)t)),
            (frame-1) % ((int)fps_get_target() + !(int)fps_get_target()),
            stats_mode == 1 ? va("%5.2f/%dfps", fps() * !app_has_pause(), (int)fps_get_target()) : stats_mode == 0 ? "0/0 KiB" : xstats());
        const char *ICON_PLAY = app_has_pause() ? UI_ICON(PLAY_ARROW) : UI_ICON(PAUSE);
        const char *ICON_SKIP = app_has_pause() ? UI_ICON(SKIP_NEXT) : UI_ICON(FAST_FORWARD);
        igPushStyleColor_Vec4(ImGuiCol_Button, ImVec4(0,0,0,0));

#if 0 // if main menubar
        if( igSmallButton(UI_ICON(SETTINGS)) ) editor_key("settings");
        igSameLine(0,0);
#endif

        if( igSmallButton(ICON_PLAY)) { if(t == 0) editor_key("eject"); editor_key(app_has_pause() ? "play" : "pause"); }
        igSameLine(0,0);
        if( igSmallButton(ICON_SKIP)) editor_key(app_has_pause() ? "frame" : "slomo");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(STOP))) editor_key("stop");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(EJECT))) editor_key("eject");
        igSameLine(0,0);
        if( igSmallButton(stats)) stats_mode = (stats_mode+1) % 3;

#if 0 // sub-editors
        igSameLine(0,0);
        //UI_MENU_ALIGN_RIGHT(32+32+32+32+32+32+32 + 32*2*is_borderless + 10, clicked_titlebar = time_ms())
        if( igSmallButton(UI_ICON(FOLDER_SPECIAL))) editor_key("browser");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(ACCOUNT_TREE))) editor_key("scene");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(SCRIPT))) editor_key("script");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(TIMELINE))) editor_key("timeline");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(TERMINAL))) editor_key("console");
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(POLYLINE))) editor_key("nodes");
#endif

#if 0 // if titlebar-less
        igSameLine(0,0);
        if( igSmallButton(app_has_maximize() ? UI_ICON(MINIMIZE) : UI_ICON(MAXIMIZE))) app_maximize(1 ^ app_has_maximize());
        igSameLine(0,0);
        if( igSmallButton(UI_ICON(CLOSE))) editor_key("quit");
#endif

        igPopStyleColor(1);

    return 0;
}

#endif
