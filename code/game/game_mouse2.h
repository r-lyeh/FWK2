// mouse ----------------------------------------------------------------------
// - rlyeh, public domain

#if !CODE

typedef enum CURSORS {
    CURSOR_HIDE, // @deprecated
    CURSOR_AUTO,
    CURSOR_ARROW,
    CURSOR_HAND,
    CURSOR_TEXT,
    CURSOR_CROSS,
    CURSOR_MAX
} CURSORS;

typedef struct mouse_t {
    int x, y, any, cursor, hidden;
    union { int l, left; };
    union { int m, middle; };
    union { int r, right; };
    union { int w, wheel; };
    // hovering, dragging, uiactive, vec2 clip
} mouse_t;

API mouse_t mouse();
API void    mouse_show(int on);
API void    mouse_cursor(int shape); // 0(hide),1(auto),2(arrow),3(hand),4(ibeam),5(cross)
API int     mouse_send(const char *cmd, const char *val);

API int     ui_mouse();

API int     id_cursor(int shape);

#else

#define ENABLE_MOUSEMOTION 1

float mouse_wheel, mouse_dx, mouse_dy, mouse_rel;
int g_mousecursor = 1;
int g_mouseshown = 1;

void mouse_cursor(int shape) { // 0(hide),1(auto),2(arrow),3(hand),4(ibeam),5(cross)
    g_mousecursor = shape;
}
void mouse_show(int shape) {
    g_mouseshown = shape;
}
void mouse_clip(bool enabled) {
    // @todo: via cimgui
}

mouse_t mouse() { // x,y,wheelx,wheely
    float mx, my;
    int buttons = SDL_GetMouseState(&mx, &my);

#if ENABLE_MOUSEMOTION
    extern SDL_Window * window;
    if(SDL_GetWindowRelativeMouseMode(window)) mx = mouse_dx, my = mouse_dy;
#endif

    mouse_t m = {0};
    m.x = mx;
    m.y = my;
    m.l = !!(buttons & SDL_BUTTON_LMASK);
    m.m = !!(buttons & SDL_BUTTON_MMASK);
    m.r = !!(buttons & SDL_BUTTON_RMASK);
    m.any = !!buttons;
    m.wheel = mouse_wheel;
    m.cursor = igGetMouseCursor();
    m.hidden = !g_mouseshown;
    /**/ if( m.cursor == ImGuiMouseCursor_None ) m.cursor = 0;
    else if( m.cursor == ImGuiMouseCursor_Arrow ) m.cursor = 1;
    else if( m.cursor == ImGuiMouseCursor_Hand ) m.cursor = 2;
    else if( m.cursor == ImGuiMouseCursor_TextInput ) m.cursor = 3;
    else if( m.cursor == ImGuiMouseCursor_ResizeAll ) m.cursor = 4;

    // @todo: buttons 4,5 SDL_BUTTON_X1MASK SDL_BUTTON_X2MASK
    // @todo: adjust mouse coords when shader for CRT distortion is applied

    return m;
}

int mouse_send(const char *cmd, const char *val) {
    if( !strcmp(cmd, "cursor") ) return mouse_cursor(atoi(val)), 1;
    return 0;
}

int id_cursor(int shape) {
    /**/ if( shape == 0 ) return ImGuiMouseCursor_None;
    else if( shape == 1 ) return ImGuiMouseCursor_Arrow;
    else if( shape == 2 ) return ImGuiMouseCursor_Arrow;
    else if( shape == 3 ) return ImGuiMouseCursor_Hand;
    else if( shape == 4 ) return ImGuiMouseCursor_TextInput;
    else if( shape == 5 ) return ImGuiMouseCursor_ResizeAll;
//  else if( shape == 0 ) return ImGuiMouseCursor_ResizeNS;
//  else if( shape == 0 ) return ImGuiMouseCursor_ResizeEW;
//  else if( shape == 0 ) return ImGuiMouseCursor_ResizeNESW;
//  else if( shape == 0 ) return ImGuiMouseCursor_ResizeNWSE;
    return ImGuiMouseCursor_Arrow;
}

void mouse_tick() {
    mouse_wheel += igGetIO()->MouseWheel; // MouseWheelH

#if ENABLE_MOUSEMOTION
    extern SDL_Window *window;
    static vec2 diff;
    // relative cursor handler when cursor is hidden
    if( !g_mouseshown && SDL_GetWindowRelativeMouseMode(window) ) {
        SDL_GetRelativeMouseState(&diff.x, &diff.y);
        mouse_dx += diff.x;
        mouse_dy += diff.y;
    }
    // show/hide cursor
    static vec2 coord = {0};
    if( !g_mouseshown && !SDL_GetWindowRelativeMouseMode(window) ) {
        SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1");

        // save x/y position and hide cursor
        SDL_GetGlobalMouseState(&coord.x, &coord.y);
        SDL_SetWindowRelativeMouseMode(window, 1);

        // now in relative mode, ensure next mouse_tick() starts with zero diffs + mouse_dx/dy pair close to current mx/my pos
        SDL_GetRelativeMouseState(&diff.x, &diff.y);
        SDL_GetRelativeMouseState(&diff.x, &diff.y);
        SDL_GetMouseState(&mouse_dx, &mouse_dy);
    }
    if( g_mouseshown && SDL_GetWindowRelativeMouseMode(window) ) {
        // show cursor and teleport cursor back to x/y
        SDL_SetWindowRelativeMouseMode(window, 0);
        SDL_WarpMouseGlobal(coord.x, coord.y);
    }
#endif

    // imgui_sdl3 backend does reset mouse cursor every frame
    int is_auto = g_mousecursor == 1;
    if( is_auto ) return;
    // else manual cursor
    igSetMouseCursor( id_cursor(g_mousecursor) );
}

int ui_mouse() {
    mouse_t m = mouse();

    ui_enable(0);
    ui_int("X", &m.x);
    ui_int("Y", &m.y);
    ui_int("Wheel", &m.w);
    ui_separator(NULL);
    ui_int("Left", &m.l);
    ui_int("Middle", &m.m);
    ui_int("Right", &m.r);
    ui_separator(NULL);
    ui_enable(1);

    int shape = g_mousecursor-1;
    int choice = ui_radio("Cursor",&shape,5,"Auto","Arrow","Hand","Text","Resize");
    if(choice) mouse_send("cursor", va("%d",CURSOR_AUTO+choice-1));

    bool visible = g_mouseshown;
    ui_bool("Shown", &visible);

    return 0;
}

#endif
