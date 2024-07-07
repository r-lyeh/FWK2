#if !CODE

// lua and luaj (luajit)
API void lua_init();
API void  luaj_init();
API void   luaj_bind(const char *cffi);
API void  lua_setstring(const char *key, const char *val);
API void  lua_setstrings(const char *key, const char **val, int count);
API void  lua_runstring(const char *script);
API void  lua_runfile(const char *pathfile);
API void**lua_fn(const char *fn, const char *fmt, ...); // fmt = "ii-iif", 2 rets, 3 args
API void  lua_die(const char *err);
API void lua_quit();

#else

// ----------------------------------------------------------------------------

static lua_State *L;

#define lua_errcheck(...) \
    for( int errors = !!(__VA_ARGS__); errors && (printf("lua: %s\n", lua_tostring(L, -1)), lua_pop(L, 1), 1); errors = 0 )

#define luaL_dostringsafe(L, str) \
    luaL_dostring(L, \
        "xpcall(function()\n" \
            str \
        "end, function(err)\n" \
        "  print('Error: ' .. tostring(err))\n" \
        "  print(debug.traceback(nil, 2))\n" \
        "  --os.exit(1)\n" \
        "end)" \
    );

static int _lua_traceback(lua_State *L) {
    if (!lua_isstring(L, 1)) {
        if (lua_isnoneornil(L, 1) || !luaL_callmeta(L, 1, "__tostring")
            || !lua_isstring(L, -1))
            return 1;
        lua_remove(L, 1);
    }
    luaL_traceback(L, L, lua_tostring(L, 1), 1);
    return 1;
}

static int _pcall(lua_State *L, int nargs, int nresults) {
    int r, errfunc;

    // put _lua_traceback under function and args
    errfunc = lua_gettop(L) - nargs;
    lua_pushcfunction(L, _lua_traceback);
    lua_insert(L, errfunc);

    // call, remove _lua_traceback
    r = lua_pcall(L, nargs, nresults, errfunc);
    lua_remove(L, errfunc);
    return r;
}
static
void lua_loadstring(const char *script) {
    luaL_loadstring(L, script);
    lua_errcheck(_pcall(L, 0, LUA_MULTRET));
}
void lua_runstring(const char *script) {
    luaL_dostring(L, script);
}
void lua_runfile(const char *pathfile) {
    lua_loadstring(file_read(pathfile, 0));
}
void lua_die(const char *err) {
    luaL_error(L, err);
}

// @fixme: support string arguments/returns
// @fixme: verify multiple returns do not exceed capacity
void** lua_fn(const char *fn, const char *fmt, ...) { // fmt = "ii-iif", 2 rets, 3 args
    lua_getglobal(L, fn);

    if( !fmt ) {
        lua_errcheck(_pcall(L, 0, 0)); // 0 arguments, 0 results
        return NULL;
    }

    static __thread void *outputs[4] = {0};

    const char *ret = fmt;
    while( *fmt != '-' ) ++fmt;
    const char *retend = fmt;

    va_list v;
    va_start(v, fmt);
        while(*++fmt) {
            if( *fmt == 'i' ) lua_pushnumber(L, va_arg(v, int));
            if( *fmt == 'f' ) lua_pushnumber(L, va_arg(v, double));
        }
        int num_args = fmt - 1 - retend;
        int num_rets = retend - ret;
        lua_errcheck(_pcall(L, num_args, num_rets)); // protected call
        for( int i = 0, end = mini(countof(outputs), num_rets); i < end; ++i) {
            /**/ if( ret[i] == 'i' ) outputs[i] = (void*)(intptr_t)lua_tonumber(L, -1);
            else if( ret[i] == 'f' ) outputs[i] = (void*)(intptr_t)lua_tonumber(L, -1);
            else                     outputs[i] = NULL;
            lua_pop(L, 1);
        }
    va_end(v);

    return outputs;
}

void lua_setstring(const char *key, const char *val) {
    lua_pushstring(L, val);
    lua_setglobal(L, key);
}

void lua_setstrings(const char *key, const char **val, int count) {
    lua_createtable(L, count, 0);
    for (int i = 0; i < count; ++i) {
        lua_pushstring(L, val[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setglobal(L, key);
}

// equivalent to:
//     ffi = require 'ffi'
//     ffi.cdef(...)

void luaj_bind(const char *cffi) {
    luaL_Buffer buf; // will accumulate ffi cdefs onto here

    // get ffi.cdef
    lua_getglobal(L, "require");
    lua_pushstring(L, "ffi");
    lua_errcheck(_pcall(L, 1, 1));
    lua_getfield(L, lua_gettop(L), "cdef");

    // accumulate ffi cdefs
    luaL_buffinit(L, &buf);
        char *clean = STRDUP(cffi);
        for( char *s = clean; (s = strstr(s, "API ")); memset( ( s += 4 ) - 4, ' ', 4) ) {
        }
        luaL_addstring(&buf, clean);
        FREE(clean);
    luaL_pushresult(&buf);

    // call it
    lua_errcheck(_pcall(L, 1, 0));
}

static
int __file_read(lua_State *L) {
    char *file = va("%s", file_norm(luaL_checkstring(L, 1)));
    if( strbegi(file, app_path()) ) file += strlen(app_path());
    strswap(file+1, ".", "/");
    strswap(file+1, "/lua", ".lua");
    int len; char *data = file_read(file, &len);
    if( len ) {
        data = memcpy(MALLOC(len+1), data, len), data[len] = 0;
        //tty_color(data ? GREEN : RED);
        //printf("%s (%s)\n%s", file, data ? "ok" : "failed", data);
        //tty_color(0);
    }
    return lua_pushstring(L, data), 1; // "\n\tcannot find `%s` within mounted zipfiles", file), 1;
}

static
void lua_add_custompackageloader(lua_State* L) {
    // expose our API file_read() function
    lua_pushcfunction( L, __file_read );
    lua_setglobal( L, "__file_read" );

#if 1 // 5.1 to 5.3 (bc of LUAJIT)
    luaL_dostringsafe(L,
        "-- make package.searchers available as an alias for package.loaders\n"
        "local p_index = { searchers = package.loaders }\n"
        "setmetatable(package, {\n"
        "   __index = p_index,\n"
        "   __newindex = function(p, k, v)\n"
        "      if k == \"searchers\" then\n"
        "         rawset(p, \"loaders\", v)\n"
        "         p_index.searchers = v\n"
        "      else\n"
        "         rawset(p, k, v)\n"
        "      end\n"
        "   end\n"
        "})\n"
    );
    luaL_dostringsafe(L,
//        "local unpack = lua_version == \"5.1\" and unpack or table.unpack\n"
    "if not table.unpack then\n"
    "   table.unpack = unpack\n"
    "end\n"
    );
#endif

    // install our zip loader at the end of package.loaders
    luaL_dostringsafe(L,
//    "package.path = [[;<?>;<<?.lua>>;]]\n" // .. package.path\n"
    "package.searchers[#package.searchers + 1] = function(libraryname)\n"
    "    for pattern in string.gmatch( package.path, '[^;]+' ) do\n"
    "        local proper_path = string.gsub(pattern, '?', libraryname)\n"
    "        local f = __file_read(proper_path, nil)\n"
    "        if f ~= nil then\n"
    "           return load(f, proper_path)\n"
    "        end\n"
    "    end\n"
    "    return nil\n"
    "end\n"
    );
}

void lua_init() {
    L = lua_open();
    // load various Lua libraries
    luaL_openlibs(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);
    // add our custom package loader (embed,zip,cook,etc)
    lua_add_custompackageloader(L);
    // start lua VM with GC stopped
    luaL_dostringsafe(L,"collectgarbage(\"stop\")");
}
void luaj_init() {
    lua_loadstring("C = (require 'ffi').C");
    lua_loadstring("c = (require 'ffi').C");

    char *bindings = file_read("engine.ffi", 0);
    luaj_bind(bindings);
}
void lua_quit() {
    lua_close(L);
}

#endif
