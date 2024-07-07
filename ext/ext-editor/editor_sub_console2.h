#if CODE

#define CONSOLE_ICON  UI_ICON(TERMINAL)
#define CONSOLE_TITLE "Console " CONSOLE_ICON

EDITOR_FUNCTION(key_console, "held(CTRL)&down(4)") {
    ui_window_show(CONSOLE_TITLE, ui_window_shown(CONSOLE_TITLE) ^ true);
}

int editor_console(int window_mode) {
    static int open;
    if( subeditor_begin(window_mode, CONSOLE_TITLE, &open) ) {

        ui_console("editor.console");

        subeditor_end(window_mode);
    }
    return 0;
}

AUTORUN {
    array_push(subeditors, editor_console);
}

#endif
