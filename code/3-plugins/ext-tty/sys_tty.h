
API void        tty_attach();
API void        tty_detach();
API void        tty_color(unsigned color);
API void        tty_reset();


// ----------------------------------------------------------------------------
// tty

void tty_color(unsigned color) {
    #if is(win32)
    do_once {
        DWORD mode = 0; SetConsoleMode(GetStdHandle(-11), (GetConsoleMode(GetStdHandle(-11), &mode), mode|4));
    }
    #endif
    if( color ) {
        // if( color == RED ) alert("break on error message (RED)"), breakpoint(); // debug
        unsigned r = (color >>  0) & 255;
        unsigned g = (color >>  8) & 255;
        unsigned b = (color >> 16) & 255;
        // 24-bit console ESC[ … 38;2;<r>;<g>;<b> … m Select RGB foreground color
        // 256-color console ESC[38;5;<fgcode>m
        // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
        // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
        // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
        // 0xE8-0xFF:  grayscale from black to white in 24 steps
        r /= 51, g /= 51, b /= 51; // [0..5]
        printf("\033[38;5;%dm", r*36+g*6+b+16); // "\033[0;3%sm", color_code);
    } else {
        printf("%s", "\x1B[39;49m"); // reset
    }
}
void tty_puts(unsigned color, const char *text) {
    tty_color(color); puts(text);
}
void tty_init() {
    tty_color(0);
}
int tty_cols() {
#if is(win32)
    CONSOLE_SCREEN_BUFFER_INFO c;
    if( GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c) ) {
        int w = c.srWindow.Right-c.srWindow.Left-c.dwCursorPosition.X;
        return w > 2 ? w - 1 : w; // w-1 to allow window resizing to a larger dimension (already printed text would break otherwise)
    }
#endif
#ifdef TIOCGWINSZ
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    return ws.ws_col - 1;
#endif
#ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    return ts.ts_cols - 1;
#endif
    return 80;
}
void tty_detach() {
    ifdef(win32, FreeConsole());
}
void tty_attach() {
#if is(win32)
    // in order to have a Windows gui application with console:
    // - use WinMain() then AllocConsole(), but that may require supporintg different entry points for different platforms.
    // - /link /SUBSYSTEM:CONSOLE and then call FreeConsole() if no console is needed, but feels naive to flash the terminal for a second.
    // - /link /SUBSYSTEM:WINDOWS /entry:mainCRTStartup, then AllocConsole() as follows. Quoting @pmttavara:
    //   "following calls are the closest i'm aware you can get to /SUBSYSTEM:CONSOLE in a gui program
    //   while cleanly handling existing consoles (cmd.exe), pipes (ninja) and no console (VS/RemedyBG; double-clicking the game)"
    do_once {
        if( !AttachConsole(ATTACH_PARENT_PROCESS) && GetLastError() != ERROR_ACCESS_DENIED ) { bool ok = !!AllocConsole(); ASSERT( ok ); }
        printf("\n"); // print >= 1 byte to distinguish empty stdout from a redirected stdout (fgetpos() position <= 0)
        fpos_t pos = 0;
        if( fgetpos(stdout, &pos) != 0 || pos <= 0 ) {
            bool ok1 = !!freopen("CONIN$" , "r", stdin ); ASSERT( ok1 );
            bool ok2 = !!freopen("CONOUT$", "w", stderr); ASSERT( ok2 );
            bool ok3 = !!freopen("CONOUT$", "w", stdout); ASSERT( ok3 );
        }
    }
#endif
}

