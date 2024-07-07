#include "engine.h"

API void hello_from_C() {
    puts("hello from C");
}

int main() {
    app_create(0.75, 0);

    lua_init();
    luaj_init();
    luaj_bind("API void hello_from_C();");

    while( app_swap() ) {
        if( ui_panel("Script", UI_OPEN) ) {
            if( ui_button("Test ffi call") ) {
                lua_runstring("c.hello_from_C()");
                lua_runstring("c.ui_notify(_VERSION, 'Hello from Lua!')");
            }
            
            ui_separator(NULL);

            static char *source; do_once {
                lua_runstring( source = STRDUP(
                    "function func(x)\n"
                    "    return 10+80*math.abs(math.sin(x))\n"
                    "end\n") );
            }
            void **results = lua_fn("func","f-f", app_time());
            ui_text("Script", 5, &source);
            ui_percent("Value", (float)(uintptr_t)(results[0]));
            if( ui_button("Compile script") ) {
                lua_runstring(source);
            }

            ui_panel_end();
        }
    }
}
