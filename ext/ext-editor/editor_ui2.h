#if !CODE

API int ui_editor();

#define UI_DETACHABLE_SECTION(VA, ...) do { \
        const char *MACRO(text) = VA; \
        static bool MACRO(collapse) = 1; \
        static int MACRO(open) = 1; \
        if( MACRO(collapse) ) { \
            MACRO(open) = 1; \
            if( igCollapsingHeader_BoolPtr(MACRO(text), &MACRO(collapse), 0) ) { \
                igIndent(12); { __VA_ARGS__; } igIndent(-12); \
            } \
        } else { \
            if( ui_window(MACRO(text), &MACRO(open)) ) { \
                static bool trans = 0, full = 0; \
                if( igCheckbox("F10",&trans) ) ( MACRO(open) &= ~UI_TRANSPARENT, MACRO(open) |= trans * UI_TRANSPARENT); igSameLine(0,0); \
                if( igCheckbox("F11",&full) ) ( MACRO(open) &= ~UI_FULLSCREEN, MACRO(open) |= full * UI_FULLSCREEN); \
                __VA_ARGS__; \
                ui_window_end(); \
            } \
            if( !MACRO(open) ) MACRO(collapse) = 1; \
        } \
    } while(0)

#else

int ui_editor() {
    UI_DETACHABLE_SECTION(va(UI_ICON(SPEED) " Profiler %5.2f/%dfps###Edit.Profiler", fps(), (int)fps_get_target()),
        ui_profiler();
    );

    UI_DETACHABLE_SECTION(UI_ICON(TERMINAL) " Terminal",
        const char *cmd = ui_console(NULL);
        if(cmd) command(cmd);
    );

    if(0) UI_DETACHABLE_SECTION(va(UI_ICON(STORAGE) " Storage %s", xstats()),
        // @todo
    );
    if(0) UI_DETACHABLE_SECTION(UI_ICON(WIFI)/*SIGNAL_CELLULAR_ALT*/ " Network 0/0 KiB",
        // @todo
        // SIGNAL_CELLULAR_1_BAR SIGNAL_CELLULAR_2_BAR
    );
    UI_DETACHABLE_SECTION(UI_ICON(STACKS) " FXs",
        ui_fxs();
    );

    if(0) UI_DETACHABLE_SECTION(UI_ICON(BUG_REPORT) " Bugs 0",
        // @todo. parse /bugs.ini, includes saved screenshots & videos.
        // @todo. screenshot include parseable level, position screen markers (same info as /bugs.ini)
    );


    // Art and bookmarks
    UI_DETACHABLE_SECTION(UI_ICON(FOLDER_SPECIAL) " Art",
        // bool inlined = true;
        // const char *file = 0;
        // if( ui_browse(&file, &inlined) ) {
        //     const char *sep = ifdef(win32, "\"", "'");
        //     app_exec(va("%s %s%s%s", ifdef(win32, "start \"\"", ifdef(osx, "open", "xdg-open")), sep, file, sep));
        igAssetsBrowser(0);
    );
    if(0) UI_DETACHABLE_SECTION(UI_ICON(BOOKMARK) " Bookmarks",0); /* @todo */


    // E,C,S,W
    if(0) UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Scene",
        UI_DETACHABLE_SECTION(UI_ICON(BUBBLE_CHART)/*SCATTER_PLOT*/ " Entities",0); /* @todo */
        UI_DETACHABLE_SECTION(UI_ICON(TUNE) " Components",0); /* @todo */
        UI_DETACHABLE_SECTION(UI_ICON(PRECISION_MANUFACTURING) " Systems",0); /* @todo */
        UI_DETACHABLE_SECTION(UI_ICON(PUBLIC) " Levels",0);
            //node_edit(editor.edit.down,&editor.edit);

        //UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Init") { /* @todo */ }
        //UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Draw") { /* @todo */ }
        //UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Tick") { /* @todo */ }
        //UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Edit") { /* @todo */ }
        //UI_DETACHABLE_SECTION(UI_ICON(ACCOUNT_TREE) " Quit") { /* @todo */ }

        // node_edit(&editor.init,&editor.init);
        // node_edit(&editor.draw,&editor.draw);
        // node_edit(&editor.tick,&editor.tick);
        // node_edit(&editor.edit,&editor.edit);
        // node_edit(&editor.quit,&editor.quit);
    );

    if(0) UI_DETACHABLE_SECTION(UI_ICON(ROCKET_LAUNCH) " AI",
        // @todo
    );
    if(0) UI_DETACHABLE_SECTION(UI_ICON(VOLUME_UP) " Audio",
        // ui_audio();
    );
    UI_DETACHABLE_SECTION(UI_ICON(PHOTO_CAMERA) " Camera",
        ui_camera( camera_get_active() );
    );
    if(0) UI_DETACHABLE_SECTION(UI_ICON(CAPTURE) " Display",0
//        // @todo: fps lock, fps target, aspect ratio, fullscreen
//        char *text = va("%s;%s;%s;%s",
//            app_has_fullscreen() ? FULLSCREEN_EXIT : FULLSCREEN,
//            PHOTO_CAMERA,
//            record_active() ? VIDEOCAM_OFF : VIDEOCAM,
//            FACE
//        );
//        int choice = ui_toolbar(text);
//        if( choice == 1 ) engine_send("key_fullscreen",0);
//        if( choice == 2 ) engine_send("key_screenshot",0);
//        if( choice == 3 ) engine_send("key_record",0);
//        if( choice == 4 ) engine_send("key_pixdebug",0);
    );
    UI_DETACHABLE_SECTION(UI_ICON(KEYBOARD) " Keyboard",
        ui_keyboard();
    );
    UI_DETACHABLE_SECTION(UI_ICON(MOUSE) " Mouse",
        ui_mouse();
    );
    UI_DETACHABLE_SECTION(UI_ICON(GAMEPAD) " Gamepads",
        ui_gamepads();
    );

    UI_DETACHABLE_SECTION(UI_ICON(TEXT_FIELDS) " Fonts",
        ui_font();
    );

    UI_DETACHABLE_SECTION(UI_ICON(TEXT_FIELDS) " UI",
        static bool show_imgui = 0;
        ui_bool("ImGui demo", &show_imgui);
        if(show_imgui) igShowDemoWindow(NULL);
    );

    if(0) UI_DETACHABLE_SECTION(UI_ICON(SCRIPT) " Scripts",
        // @todo
    );
    UI_DETACHABLE_SECTION(UI_ICON(GRADIENT) " Shaders",
        ui_shaders();
    );


    if(0) UI_DETACHABLE_SECTION(UI_ICON(SAVINGS) " Budgets",
        // @todo. // mem,fps,gfx,net,hdd,... also logging
    );

    // logic: either plug icon (power saving off) or one of the following ones (power saving on):
    //        if 0% batt (no batt): battery alert
    //        if discharging:       battery levels [alert,0..6,full]
    //        if charging:          battery charging
#if 0
    int battery_read = app_battery();
    int battery_level = abs(battery_read);
    int battery_discharging = battery_read < 0 && battery_level < 100;
    if( battery_level ) {
        const char *battery_levels[9] = { // @todo: remap [7%..100%] -> [0..1] ?
            BATTERY_ALERT,BATTERY_0_BAR,BATTERY_1_BAR,
            BATTERY_2_BAR,BATTERY_3_BAR,BATTERY_4_BAR,
            BATTERY_5_BAR,BATTERY_6_BAR,BATTERY_FULL,
        };
        power_icon_label = (const char*)va("%s Power %d%%",
            battery_discharging ? battery_levels[(int)((9-1)*clampf(battery_level/100.f,0,1))] : BATTERY_CHARGING_FULL,
            battery_level);
#endif

    if(0) UI_DETACHABLE_SECTION(UI_ICON(POWER) " Power",0
//      int choice = ui_toolbar( UI_ICON(POWER) ";" UI_ICON(BOLT) );
//      if( choice == 1 ) engine_send("key_battery","0");
//      if( choice == 2 ) engine_send("key_battery","1");
    );

    UI_DETACHABLE_SECTION(UI_ICON(WATER) " Reflection",
        ui_reflect("*");
    );

    if(0) UI_DETACHABLE_SECTION(UI_ICON(EXTENSION) " Plugins",
        // @todo. include VCS
        UI_DETACHABLE_SECTION(UI_ICON(BUILD) " Cook",0
            // @todo
        );
    );

    return 0;
}

static
void editor_ui_tick() {
    if( !ui_debug ) return;

    ui_editor();
}

AUTORUN {
//  if( HAS_EDITOR )
    hooks("tick", editor_ui_tick);
}

#endif
