// ----------------------------------------------------------------------------
// editor bindings

#if !CODE

#define EDITOR_KEY(CMD,KEYS) \
    const char *CMD; AUTORUN { CMD = editor_keyfind(#CMD, KEYS); }

#define EDITOR_FUNCTION(CMD,KEYS) \
    void CMD(); AUTORUN { array_push(editor_bindings, ((editor_binding_t){#CMD,editor_keyfind(#CMD,KEYS),CMD}) ); } void CMD()

// impl details

typedef struct editor_binding_t {
    const char *command;
    const char *keys;
    void (*fn)();
} editor_binding_t;

API int         editor_key(const char *cmd);
API const char* editor_keyfind(const char *cmd, const char *defaults); // find a binding key

API extern array(editor_binding_t) editor_bindings;

#else

#ifndef EDITOR_INI
#define EDITOR_INI ".editor.ini"
#endif

array(editor_binding_t) editor_bindings;

const char* editor_keyfind(const char *key, const char *defaults) {
    if( HAS_EDITOR ) {
        static ini_t inis; do_once inis = ini(EDITOR_INI);
        if( !inis ) map_init_str(inis);

        char longkey[128]; snprintf(longkey, 128, "editor.%s", key);
        char** found = map_find(inis, (char*)longkey);
        if(found) return *found;

        ini_write(EDITOR_INI, "editor", key, defaults);
    }

    // remove comments from key=val
    const char *has_comments = strchr(defaults, ';');
    defaults = has_comments ? stringf("%.*s", (int)(has_comments - defaults), defaults) : defaults; // @leak

    return defaults;
}

int editor_exec(const char *cmd, const char *val) {
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        if( it->command && !strcmp(it->command, cmd) ) {
            return it->fn(val), 1;
        }
    }
    return 0;
}

int editor_key(const char *key) {
   return editor_exec(va("key_%s", key), "1"); 
}

void editor_bindings_tick() {
    do_once
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        igAddCommandPalette(it->command, it->fn);
    }
    for each_array_ptr(editor_bindings,editor_binding_t,it) {
        if( it->keys && binding(it->keys) ) {
            it->fn();
        }
    }
}

AUTORUN {
    hooks("tick,exec", editor_bindings_tick,editor_exec);
}

#endif
