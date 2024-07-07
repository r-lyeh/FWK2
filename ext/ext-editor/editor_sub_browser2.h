#if CODE

#define BROWSER_ICON  UI_ICON(FOLDER_SPECIAL)
#define BROWSER_TITLE "Browser " BROWSER_ICON

EDITOR_FUNCTION(key_browser, "held(CTRL)&down(2)") {
    ui_window_show(BROWSER_TITLE, ui_window_shown(BROWSER_TITLE) ^ true);
}

int subeditor_browser(int window_mode) {
    static int open;

    if( subeditor_begin(window_mode, BROWSER_TITLE, &open) ) {

        if( ui_browse("editor.browser", "open file", "*.*", BROWSER_OPEN1) ) {
            // browser results (if any)
            int count;
            for( char **list = 0; (list = ui_browse_results("editor.browser", &count)); list = 0 ) {
                while(count--) {
                    const char *filename = list[count];
                    const char *sep = ifdef(win32, "\"", "'");
                    system(va("%s %s%s%s", ifdef(win32, "start \"\"", ifdef(osx, "open", "xdg-open")), sep, filename, sep));
                }

                open = 0;
            }
        }

        subeditor_end(window_mode);
    }
    return 0;
}

AUTORUN {
    array_push(subeditors, subeditor_browser);
}

#endif
