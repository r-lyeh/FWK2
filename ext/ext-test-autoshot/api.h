#if CODE

int last_w, last_h;
void *last;

void pic_save(void) {
    if( last )
    for( int i = 0; i < 1000; ++i ) {
        char fname[128];
        snprintf(fname, 128, "auto(%04d).png", i);
        if( !is_file(fname) ) {
            stbi_flip_vertically_on_write(1);
            stbi_write_png(fname, last_w, last_h, 3, last, last_w * 3);
            return;
        }
    }
}

void pic_tick() {
    last_w = app_width();
    last_h = app_height();
    last = screenshot_async(3); // 3 RGB, 4 RGBA, -3 BGR, -4 BGRA
}

AUTORUN {
    if( getenv("AUTOSCREENSHOT") ifdef(win32, || GetAsyncKeyState(VK_CONTROL) & 0x8000) ) {
        puts("[x] " __FILE__ " enabled: will be taking picture at end of program.");
        hooks("tick",pic_tick);
        atexit(pic_save);
    }
}

#endif
