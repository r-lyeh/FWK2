// lite editor, platform details
// - rlyeh, public domain

#define LT_DATAPATH          "/lite"

#define lt_assert(x)         ASSERT(x)

#define lt_realpath(p, q)    file_pathabs(p)
#define lt_realpath_free(p)

#define lt_malloc(n)         MALLOC(n)
#define lt_calloc(n,m)       CALLOC(n,m)
#define lt_free(p)           FREE(p)
#define lt_memcpy(d,s,c)     memcpy(d,s,c)
#define lt_memset(p,ch,c)    memset(p,ch,c)

#define lt_time_ms()         time_ms()
#define lt_sleep_ms(ms)      sleep_ms(ms)

#define lt_getclipboard(w)   app_has_clipboard()
#define lt_setclipboard(w,s) app_clipboard(s)

#define lt_window()          app_handle("window")
#define lt_setwindowmode(m)  app_fullscreen(m == 2), (m < 2 && (app_maximize(m),1)) // 0:normal,1:maximized,2:fullscreen
#define lt_setwindowtitle(t) //app_settitle(t)
#define lt_haswindowfocus()  app_has_focus()
#define lt_setcursor(shape)  mouse_cursor(lt_events & (1<<31) ? 1/*auto*/ : shape==1?4:1/*specific*/) // 0:arrow(1),1:ibeam(4),2:sizeh(5),3:sizev(5),4:hand(3)

#define lt_prompt(message,title) (dialog(title, message, 2) == 1)

unsigned lt_events = ~0u;
int lt_mx = 0, lt_my = 0, lt_wx = 0, lt_wy = 0, lt_ww = 0, lt_wh = 0;

typedef struct lt_surface {
    unsigned w, h;
    void *pixels;
    texture_t t;
} lt_surface;

typedef struct lt_rect {
    int x, y, width, height;
} lt_rect;

lt_surface *lt_getsurface(void *window) {
    static lt_surface s = {0};
    return &s;
}
void lt_updatesurfacerects(lt_surface *s, lt_rect* rects, unsigned count) {
    if(0)
    for( int i = 0; i < count; ++i ) {
        memset( (unsigned*)s->pixels + (rects[i].x + rects[i].y * s->w), 0xFF, rects[i].width*4 );
        memset( (unsigned*)s->pixels + (rects[i].x + (rects[i].y + (rects[i].height-1)) * s->w), 0xFF, rects[i].width*4 );
        for( int y = 1; y < (rects[i].height-1); ++y ) {
            ((unsigned*)s->pixels)[ rects[i].x + y * s->w ] =
            ((unsigned*)s->pixels)[ rects[i].x + (rects[i].width-1) + y * s->w ] = 0xFFFFFFFF;
        }
    }

    // update contents
    texture_update(&s->t, s->w, s->h, 4, s->pixels, TEXTURE_LINEAR|TEXTURE_BGRA);
}

void ren_set_clip_rect(struct lt_rect rect);
void rencache_invalidate(void);
int lt_resizesurface(lt_surface *s, int ww, int wh) {
    s->w = ww, s->h = wh;
    if( s->t.id == 0 || s->w != s->t.w || s->h != s->t.h ) {
        // invalidate tiles
        ren_set_clip_rect( (lt_rect) { 0, 0, s->w, s->h } );
        rencache_invalidate();

        // texture clear
        if( !s->t.id ) s->t = texture_create(1, 1, 4, "    ", TEXTURE_LINEAR|TEXTURE_RGBA|TEXTURE_BYTE );
        s->pixels = REALLOC(s->pixels, s->w * s->h * 4);
        memset(s->pixels, 0, s->w * s->h * 4);

        // texture update
        lt_updatesurfacerects(s,0,0);
        return 1; // resized
    }
    return 0; // unchanged
}

void *lt_load_file(const char *filename, int *size) {
    int datalen; char *data = file_read(filename, &datalen);
    if( !data || !datalen ) {
        filename = (const char *)file_norm(filename);
        if( strbegi(filename, app_path()) ) filename += strlen(app_path());
        data = file_read(filename, &datalen);
    }
    if (size) *size = 0;
    if (!data) { return NULL; }
    if (size) *size = datalen;
    // return permanent buffers here, since file_read() does return temporaries
    data = memcpy(MALLOC(datalen+1), data, datalen);
    data[datalen] = 0;
    return data;
}

const char* lt_button_name(int button) {
    if(button == SDL_BUTTON_LEFT) return "left";
    if(button == SDL_BUTTON_RIGHT) return "right";
    if(button == SDL_BUTTON_MIDDLE) return "middle";
    return "?";
}

char* lt_key_name(char *dst, int key, int vk, int mods) {
    // @todo: "altgr" -> left ctrl + right alt

    if( key == SDL_SCANCODE_UP ) return "up";
    if( key == SDL_SCANCODE_DOWN ) return "down";
    if( key == SDL_SCANCODE_LEFT ) return "left";
    if( key == SDL_SCANCODE_RIGHT ) return "right";
    if( key == SDL_SCANCODE_LALT ) return "left alt";
    if( key == SDL_SCANCODE_RALT ) return "right alt";
    if( key == SDL_SCANCODE_LSHIFT ) return "left shift";
    if( key == SDL_SCANCODE_RSHIFT ) return "right shift";
    if( key == SDL_SCANCODE_LCTRL ) return "left ctrl";
    if( key == SDL_SCANCODE_RCTRL ) return "right ctrl";
    //if( key == SDL_SCANCODE_LSUPER ) return "left windows";
    //if( key == SDL_SCANCODE_RSUPER ) return "left windows";
    if( key == SDL_SCANCODE_MENU ) return "menu";

    if( key == SDL_SCANCODE_ESCAPE ) return "escape";
    if( key == SDL_SCANCODE_BACKSPACE ) return "backspace";
    if( key == SDL_SCANCODE_RETURN ) return "return";
    if( key == SDL_SCANCODE_KP_ENTER ) return "keypad enter";
    if( key == SDL_SCANCODE_TAB ) return "tab";
    if( key == SDL_SCANCODE_CAPSLOCK ) return "capslock";

    if( key == SDL_SCANCODE_HOME ) return "home";
    if( key == SDL_SCANCODE_END ) return "end";
    if( key == SDL_SCANCODE_INSERT ) return "insert";
    if( key == SDL_SCANCODE_DELETE ) return "delete";
    if( key == SDL_SCANCODE_PAGEUP ) return "pageup";
    if( key == SDL_SCANCODE_PAGEDOWN ) return "pagedown";

    if( key == SDL_SCANCODE_F1 ) return "f1";
    if( key == SDL_SCANCODE_F2 ) return "f2";
    if( key == SDL_SCANCODE_F3 ) return "f3";
    if( key == SDL_SCANCODE_F4 ) return "f4";
    if( key == SDL_SCANCODE_F5 ) return "f5";
    if( key == SDL_SCANCODE_F6 ) return "f6";
    if( key == SDL_SCANCODE_F7 ) return "f7";
    if( key == SDL_SCANCODE_F8 ) return "f8";
    if( key == SDL_SCANCODE_F9 ) return "f9";
    if( key == SDL_SCANCODE_F10 ) return "f10";
    if( key == SDL_SCANCODE_F11 ) return "f11";
    if( key == SDL_SCANCODE_F12 ) return "f12";

    char *p = strcpy(dst, SDL_GetKeyName(key));
    while (*p) {
        *p = tolower(*p);
        p++;
    }
    return dst;
}

void lt_globpath(struct lua_State*L, const char *path) {
    unsigned j = 0;

    if(!strend(path, "/")) path = (const char *)va("%s/", path);
    for( dir_t *d = dir_open(path, ""); d; dir_close(d), d = 0 ) {
        for( unsigned i = 0, end = dir_count(d); i < end; ++i ) {
            char *name = dir_name(d,i);
            char *last = name + strlen(name) - 1;
            if( *last == '/' ) *last = '\0';
            name = (char*)file_name(name);
            lua_pushstring(L, name);
            lua_rawseti(L, -2, ++j);
        }
    }

    // optimize this block
    for( const char *section = strstri(path, LT_DATAPATH); section && section[sizeof(LT_DATAPATH)-1] == '/'; section = 0) {
        int count; char** list = file_list("**", &count);
        for( unsigned i = 0; i < count; ++i ) {
            char *name = list[i];
            if( !strstri(name, section+1) ) continue;
            lua_pushstring(L, file_name(name));
            lua_rawseti(L, -2, ++j);
        }
    }
}

int lt_emit_event(lua_State *L, const char *event_name, const char *event_fmt, ...) {
    int count = 0;
    lua_pushstring(L, event_name);
    if( event_fmt ) {
        va_list va;
        va_start(va, event_fmt);
        for( ; event_fmt[count]; ++count ) {
            /**/ if( event_fmt[count] == 'd' ) { int d = va_arg(va, int); lua_pushnumber(L, d); }
            else if( event_fmt[count] == 'f' ) { double f = va_arg(va, double); lua_pushnumber(L, f); }
            else if( event_fmt[count] == 's' ) { const char *s = va_arg(va, const char *); lua_pushstring(L, s); }
        }
        va_end(va);
    }
    return 1+count;
}

int lt_poll_event(lua_State *L) { // init.lua > core.step() wakes on mousemoved || inputtext
    int rc = 0;
    char buf[32];

    static unsigned clicks_time = 0, clicks = 0;
    if( (lt_time_ms() - clicks_time) > 400 ) clicks = 0;

    //
    extern array(SDL_Event) sdl3_events;
    if( lt_events ) 
    for( int i = 0, count = array_count(sdl3_events); i < count; ++i ) {
        SDL_Event e = sdl3_events[i];

        switch (e.type) {
        default:
#if 0
            break; case SDL_EVENT_QUIT: // it used to be ok. depends on window_swap() flow
            rc += lt_emit_event(L, "quit", NULL);
            return rc;

            break; case SDL_EVENT_WINDOW_MOVED:
            lt_wx = e.window.data1;
            lt_wy = e.window.data2;

            break; case SDL_EVENT_WINDOW_RESIZED:
            rc += lt_emit_event(L, "resized", "dd", lt_ww = e.window.data1, lt_wh = e.window.data2);
            lt_resizesurface(lt_getsurface(lt_window()), lt_ww, lt_wh);

            break; case SDL_EVENT_WINDOW_EXPOSED:
            rc += lt_emit_event(L, "exposed", NULL);
            rencache_invalidate();
#endif

            break; case SDL_EVENT_DROP_FILE:
            rc += lt_emit_event(L, "filedropped", "sdd", e.drop.data, lt_mx, lt_my);

            break; case SDL_EVENT_KEY_DOWN:
            rc += lt_emit_event(L, "keypressed", "s", lt_key_name(buf, e.key.key, e.key.scancode, e.key.mod));

            break; case SDL_EVENT_KEY_UP:
            rc += lt_emit_event(L, "keyreleased", "s", lt_key_name(buf, e.key.key, e.key.scancode, e.key.mod));

            break; case SDL_EVENT_TEXT_INPUT:
            rc += lt_emit_event(L, "textinput", "s", e.text.text);
            return 2; // do not process queue further. next key_down event of this keypress would invalidate this event otherwise

            break; case SDL_EVENT_MOUSE_BUTTON_DOWN:
            rc += lt_emit_event(L, "mousepressed", "sddd", lt_button_name(e.button.button), lt_mx, lt_my, 1 + clicks);

            break; case SDL_EVENT_MOUSE_BUTTON_UP:
            clicks += e.button.button == SDL_BUTTON_LEFT;
            clicks_time = lt_time_ms();
            rc += lt_emit_event(L, "mousereleased", "sdd", lt_button_name(e.button.button), lt_mx, lt_my);

            break; case SDL_EVENT_MOUSE_MOTION:;
#if 0
            static int prevx = 0, prevy = 0;
            lt_mx = e.motion.x /*- lt_wx*/, lt_my = e.motion.y /*- lt_wy*/;
            rc += lt_emit_event(L, "mousemoved", "dddd", lt_mx, lt_my, lt_mx - prevx, lt_my - prevy);
            prevx = lt_mx, prevy = lt_my;
#else
            static int prevx = 0, prevy = 0;
            rc += lt_emit_event(L, "mousemoved", "dddd", lt_mx, lt_my, lt_mx - prevx, lt_my - prevy);
            prevx = lt_mx, prevy = lt_my;
#endif

            break; case SDL_EVENT_MOUSE_WHEEL:
            rc += lt_emit_event(L, "mousewheel", "f", e.wheel.y);
        }
    }

    return rc;
}
