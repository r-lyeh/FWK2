#ifndef WINDOW_H
#define WINDOW_H

enum { WINDOW_TRANSPARENT = 1, WINDOW_SQUARE = 2 };

#endif

API void window_create(float scale, unsigned flags);
API int  window_swap();

API void window_debug(int enabled);

API void window_override_icons();

#if CODE
extern int keep;
extern SDL_Window *window;

int  sdl3win_init(float scale, unsigned flags);
void sdl3win_tick();
void sdl3win_swap();
void sdl3win_quit(void);

void window_debug(int enabled) {}

void window_create(float scale, unsigned flags) {
    if( !window ) {
        sdl3win_init(scale, flags);
        atexit(sdl3win_quit);
    }
}

int window_swap() {
    do_once {
        sdl3win_tick();
        return keep;
    }
    sdl3win_swap();
    sdl3win_tick();
    return keep;
}

#if is(win32)
#include <shellapi.h>

// Set the window icon for every window in your app (including MessageBox() calls and assertion failures) instead of just your primary window.
static HICON appIcon; // = (HICON)GetClassLong(hWnd, GCL_HICON);
static LRESULT window_create_callback(int type, WPARAM wparam, LPARAM lparam) {
    if (type == HCBT_CREATEWND) {
        SendMessage((HWND)wparam, WM_SETICON, ICON_SMALL, (LPARAM)appIcon);
        SendMessage((HWND)wparam, WM_SETICON, ICON_BIG, (LPARAM)appIcon);
    }
    return CallNextHookEx(NULL, type, wparam, lparam);
}
void window_override_icons() {
    ONCE {
        HANDLE hInstance = GetModuleHandleA(NULL);
        appIcon = ExtractIconA(hInstance, __argv[0], 0 );
        if(!appIcon) appIcon = ExtractIconA(hInstance, va("%s.exe", __argv[0]), 0 );
    }
    SetWindowsHookEx(WH_CBT, window_create_callback, NULL, GetCurrentThreadId());
}

#else

void window_override_icons() {}

#endif

#endif
