API int subeditor_textedit(int submode);

#if CODE

#include "_3rd_lite_sys.h"
#include "_3rd_lite.h"

#define TEXTEDIT_ICON  UI_ICON(CONTENT_PASTE)
#define TEXTEDIT_TITLE "Script " TEXTEDIT_ICON

EDITOR_FUNCTION(script, "held(CTRL)&down(3)") { 
    ui_window_show(TEXTEDIT_TITLE, ui_window_shown(TEXTEDIT_TITLE) ^ true);
}

int subeditor_textedit(int submode) {
    static lua_State *L = 0;
    do_once {
        L = script_init_env(SCRIPT_LUA|SCRIPT_DEBUGGER);

        const char *platform = "" // "Android" "FreeBSD" "OpenBSD" "NetBSD"
            ifdef(ems, "Emscripten")
            ifdef(linux, "Linux")
            ifdef(osx, "macOS")
            ifdef(win32, "Windows")
        ;
        char *pathexe = va("%s%s%s", app_path(), app_name(), ifdef(win32, ".exe", ""));
        lt_init(L, app_handle("window"), LT_DATAPATH, __argc, __argv, app_scale(), platform, pathexe);
    }

    unsigned lt_none = 0u;
    unsigned lt_all = ~0u; // & ~(GLEQ_WINDOW_MOVED/*|GLEQ_WINDOW_RESIZED|GLEQ_WINDOW_REFRESH*/);
    lt_events = lt_none;

    // int mouse_in_rect = 0;
    submode = SUBEDITOR_WINDOW; // force submode

    static int open;
    if( subeditor_begin(submode, TEXTEDIT_TITLE, &open) ) {

        ImVec2 m; igGetMousePos(&m);

        lt_events = lt_all;
        if( !igIsWindowFocused(0) ) lt_events = lt_none;

        ImRect bounds = igGetCurrentWindow()->ContentRegionRect;

        lt_mx = m.x - bounds.Min.x;
        lt_my = m.y - bounds.Min.y;
        lt_wx = 0;//bounds.Min.x;
        lt_wy = 0;//bounds.Min.y;
        lt_ww = bounds.Max.x - bounds.Min.x;
        lt_wh = bounds.Max.y - bounds.Min.y;

        if( lt_resizesurface(lt_getsurface(0), lt_ww, lt_wh) ) {
            //gleq_window_refresh_callback(window_handle());
        }

#if 0
        blit_rgb( lt_getsurface(0)->t );
#else
        ui_texture(NULL, lt_getsurface(0)->t);
#endif

        if( igIsWindowHovered(0) ) { // &ui_ctx->input, ((struct nk_rect){lt_wx+5,lt_wy+5,lt_ww-10,lt_wh-10}))
            lt_events &= ~(1<<31); // dont cursor shape
        }

        subeditor_end(submode);
    }

    lt_tick(L);
    return 0;
}

AUTORUN {
    // array_push(subeditors, subeditor_textedit);
}

#endif
