#if !CODE
typedef enum OBJTYPE_game {
    /// --
    OBJTYPE_camera = 11,
    OBJTYPE_node   = 12,
    OBJTYPE_scene  = 13,
} OBJTYPE_game;
#endif

#include "game_types.h" // 1st
#include "game_math.h" // 2nd
#include "game_anim.h"
#include "game_audio2.h"
#include "game_bindings2.h"
#include "game_camera.h"
#include "game_collide.h"
#include "game_ease.h"
#include "game_fetch2.h"
#include "game_fps2.h"
#include "game_image.h"
#include "game_gamepad2.h"
#include "game_input.h"
#include "game_keyboard.h"
#include "game_level.h"
#include "game_mouse2.h"
#include "game_profiler.h"
#include "game_script_lua2.h"
#include "game_script.h"
#include "game_text.h"
#include "game_ui2.h"
#include "game_app2.h" // 3rd

#if CODE

void initG() {
    profiler_init();
    //scene_init();
    //network_init();
    //audio_init();
    //midi_init();
}
void tickG() {

#if 0
//ifdef(DEV,
    if( ui_debug ) {
        float fps = igGetIO()->Framerate;
        igText("FPS: %.2f (%.2gms)", fps, fps ? 1000.0f / fps : 0.0f);
        ui_profiler();
    }
//)
#endif

    ui_tick();
    mouse_tick();
    input_tick();
}

AUTORUN {
    hooks("init,tick",initG,tickG);
}

#endif
