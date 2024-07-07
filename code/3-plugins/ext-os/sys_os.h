
API bool        os_open(const char *folder_file_or_url);
API const char* os_exec(const char *command); // returns ("%15d %s", retcode, output_last_line)
API int         os_spawn(const char *command); // @fixme: add const char *logfile optional arg
API void        os_singleton(const char *guid);

API const char* os_cmdline();

const char *os_cmdline() {
    static char *cmdline = 0;
    if( !cmdline ) {
        if( argc() <= 1 ) strcatf(&cmdline, "%s", " ");
        for( int i = 1; i < argc(); ++i ) strcatf(&cmdline, " %s", argv(i));
    }
    return cmdline+1;
}


const char * os_exec( const char *cmd ) {
    static __thread char output[4096+16] = {0};
    char *buf = output + 16; buf[0] = 0; // memset(buf, 0, 4096);

    if( !cmd[0] ) return "0               ";
    cmd = file_normalize(cmd);

    int rc = -1;

    // pick the fastest code path per platform
#if is(osx)
    rc = system(cmd);
#elif is(win32)
    STARTUPINFOA si = {0}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    snprintf(output+16, 4096, "cmd /c \"%s\"", cmd);

    int prio = //strstr(cmd, "ffmpeg") || strstr(cmd, "furnace") || strstr(cmd, "ass2iqe") ?
    REALTIME_PRIORITY_CLASS; //: 0;

//prio |= DETACHED_PROCESS;
//si.dwFlags = STARTF_USESTDHANDLES;

    if( CreateProcessA(
        NULL, output+16, // cmdline
        NULL,
        NULL,
        FALSE, // FALSE: dont inherit handles
        prio /*CREATE_DEFAULT_ERROR_MODE|CREATE_NO_WINDOW*/, // 0|HIGH_PRIORITY_CLASS
        NULL, // "", // NULL would inherit env
        NULL, // current dir
        &si, &pi) )
    {
        // Wait for process
        DWORD dwExitCode2 = WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD dwExitCode; GetExitCodeProcess(pi.hProcess, &dwExitCode);
        rc = dwExitCode;
    }
    else
    {
        // CreateProcess() failed
        rc = GetLastError();
    }
#else
    rc = system(cmd);
#endif

    return snprintf(output, 16, "%-15d", rc), buf[-1] = ' ', output;
}

int os_spawn( const char *cmd ) {
    if( !cmd[0] ) return false;
    cmd = file_normalize(cmd);

    char buffer[1024];
    if( snprintf(buffer, 1024, ifdef(win32, "cmd /c \"%s\"", "%s &"), cmd) < 1024 ) 
        return ifdef(win32, WinExec(buffer, SW_HIDE) > 31, system(buffer) == 0);

    die("os_spawn(): truncated string");
    return false;
}



void os_singleton(const char *guid) {
    #ifdef _WIN32
    do_once {
        char buffer[128];
        snprintf(buffer, 128, "Global\\{%s}", guid);
        static HANDLE app_mutex = 0;
        app_mutex = CreateMutexA(NULL, FALSE, buffer);
        if( ERROR_ALREADY_EXISTS == GetLastError() ) {
            exit(-1);
        }
    }
    #endif
}

#ifdef OS_SINGLETON_GUID
AUTORUN { os_singleton(OS_SINGLETON_GUID); }
#endif

static
bool os_open_folder(const char *file) {
    char buf[1024];
#ifdef _WIN32
    snprintf(buf, sizeof(buf), "start \"\" \"%s\"", file);
#elif __APPLE__
    snprintf(buf, sizeof(buf), "%s \"%s\"", file_directory(file) ? "open" : "open --reveal", file);
#else
    snprintf(buf, sizeof(buf), "xdg-open \"%s\"", file);
#endif
    return os_spawn(buf);
}

static
bool os_open_file(const char *file) {
    char buf[1024];
#ifdef _WIN32
    snprintf(buf, sizeof(buf), "start \"\" \"%s\"", file);
#elif __APPLE__
    snprintf(buf, sizeof(buf), "open \"%s\"", file);
#else
    snprintf(buf, sizeof(buf), "xdg-open \"%s\"", file);
#endif
    return os_spawn(buf);
}

static
bool os_open_url(const char *url) {
    return os_open_file(url);
}

bool os_open(const char *link) {
    if( file_directory(link) ) return os_open_folder(link);
    if( file_exist(link) ) return os_open_file(link);
    return os_open_url(link);
}
