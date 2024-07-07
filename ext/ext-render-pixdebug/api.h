#if !CODE

#else

int debug_tool_pass = -1;
int postfx_debug_tool_enabled = 0;

int fxdebugtool_exec(const char *cmd, const char *val) {
    if( !strcmp(cmd, "key_pixdebug" )) {
        do_once {
            fx_load("**/fxDebugTool.glsl");
	        debug_tool_pass = fx_find("fxDebugTool.glsl");
            fx_order(debug_tool_pass, 1000);
        }
        postfx_debug_tool_enabled = (val ? val[0] - '0' : !postfx_debug_tool_enabled);
        input_blocked += postfx_debug_tool_enabled ? +1 : -1;
	    return 1;
    }
    return 0;
}

void fxdebugtool_tick() {
    int restore = input_blocked;
    input_blocked = 0;
    if (ifdef(retail, 0, 1) && input(KEY_ALT) && input_down(KEY_T)) {
        fxdebugtool_exec("key_pixdebug", 0);
    }
    input_blocked = restore;

    if (postfx_debug_tool_enabled) {
        fx_enable(debug_tool_pass, true);
    } else {
        fx_enable(debug_tool_pass, false);
    }
}

AUTORUN {
	puts("pixdebug runtime installed [ALT+T] during a fx_begin()/fx_end() section: " __FILE__);
	hooks("tick,exec", fxdebugtool_tick,fxdebugtool_exec);
}

#endif
