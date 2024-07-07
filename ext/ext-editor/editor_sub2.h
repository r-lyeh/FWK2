#if !CODE

enum SUBEDITOR_MODES {
    SUBEDITOR_WINDOW,
    SUBEDITOR_PANEL,
    // SUBEDITOR_WINDOW_NK_SMALL
};

API int subeditor_begin(int submode, const char *title, int *ui_flags);
API int subeditor_end(int submode);

typedef int(*subeditor)(int submode);

API extern array(subeditor) subeditors;

#else

int subeditor_begin(int submode, const char *title, int *ui_flags) {
    if( submode == SUBEDITOR_WINDOW ) {
        return ui_window(title, ui_flags);
    } else {
        return ui_panel(title, *ui_flags);
    }
}
int subeditor_end(int submode) {
    if( submode == SUBEDITOR_WINDOW ) {
        ui_window_end();
    } else {
        ui_panel_end();
    }
    return 0;
}

array(subeditor) subeditors;

void subeditors_tick() {
    for each_array(subeditors, subeditor, fn) {
        const int preferred_window_mode = SUBEDITOR_WINDOW;
        fn(preferred_window_mode);
    }
}

AUTORUN {
    if( HAS_EDITOR )
    hooks("tick", subeditors_tick);
}

#endif
