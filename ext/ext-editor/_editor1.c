// -----------------------------------------------------------------------------
// in-game editor
// - rlyeh, public domain.

// ## Editor roadmap
// - Gizmos✱, scene tree, property editor✱, load/save✱, undo/redo✱, copy/paste, on/off (vis,tick,ddraw,log), vcs.
// - Scenenode pass: node singleton display, node console, node labels, node outlines✱.<!-- node == gameobj ? -->
// - Render pass: billboards✱, materials, un/lit, cast shadows, wireframe, skybox✱/mie✱, fog/atmosphere
// - Level pass: volumes, triggers, platforms, level streaming, collide✱, physics
// - Edit pass: Procedural content, brushes, noise and CSG.
// - GUI pass: timeline and data tracks, node graphs. <!-- worthy: will be reused into materials, animgraphs and blueprints -->

// ## Alt plan
// editor is a database + window/tile manager + ui toolkit; all network driven.
// to be precise, editor is a dumb app and ...
// - does not know a thing about what it stores.
// - does not know how to render the game graphics.
// - does not know how to run the game logic.
//
// the editor will create a canvas for your game to render.
// your game will be responsible to tick the logic and render the window inside the editor.
//
// that being said, editor...
// - can store datas hierarchically.
// - can perform diffs and merges, and version the datas into repositories.
// - can be instructed to render UI on top of game and window views.
// - can download new .natvis and plugins quickly.
// - can dump whole project in a filesystem form (zip).

// - editor reflects database contents up-to-date.
// - database can be queried and modified via OSC(UDP) commands.

// editor database uses one table, and stores two kind of payload types:
// - classes: defines typename and dna. class names are prefixed by '@'
// - instances: defines typename and datas. instance names are as-is, not prefixed.
//
// every save contains 5Ws: what, who, when, where, how,
// every save can be diffed/merged.

// ----------------------------------------------------------------------------

// TODO
// good UI/UX blender ideas
// ui density is perfect: font face, font size and icon sizes are perfect even in a low res 1366x768 scenario
// viewport: not cluttered at all. vertical floating toolbar on the left, floating icons on the right
// left icons are tools related to current edition mode, right icons are camera only.
// left icons can be long pressed for a hovered submenu, right icons can be clicked or dragged.
// non-matching text filtering works by disabling widgets, not by hiding them
// window contents can be promoted to any display any other window content by clicking their [icon V]
// there are no windows or minimize/maximize buttons
// windows are grouped with a vertical toolbar inside the right bar docked section
// you can only display 1 window at time.
// panels can be dragged within a window and make much more sense than our approach
// scene tree: can make collections/groups
// widget tree drawing is much simpler than us, just a vertical parent line per collection and indented arrows per child
// context menus are always like: optional icon (white), text (white), either optional input binding (gray) or submenu arrow
// input boxes are super reactive
// < > are hidden unless hovered
// < > can be clicked to de/increment (release)
// < > can be horizontally dragged to de/increment larger quantities (hold)
// < > can be vertically dragged to include other Y,Z properties in the selection (hold)
// 0.53m can be clicked (release) or dragged (<-> hovered icon) (hold).
// 0.53m whole text is automatically selected when clicked.
// you can enter units when entering values 350m, 350km, 12mi
// you can enter math values: 30+20+sin(1)
// numeric: units always specified: 0 m, 90º, 1.25x
// numeric: epsilons (non-zero) displayed as -0.00000 even if digits cannot fit widget rect// 
//
// operation context is split into different sections in menu bar: modeling, sculpting, uv editing, shading, anim, etc
// cam 3d orientation gizmo top right: can be dragged (orbit cam) or clicked (reposition)
// rotation gizmo: anchor can be positioned within world
// gizmo: will display always initial position while dragging, and tape distance so far
// right floating icons can be dragged: cam orientation, cam panning, cam zooming, cam iso/dimetric
// ctrl-z undo, ctrl-shift-z redo


TODO("COOK_ON_DEMAND 1: this directive should be on client, not within ENGINE.DLL");
TODO("ENABLE_AUTOTESTS 1: this directive should be on client, not within ENGINE.DLL");

// ----------------------------------------------------------------------------

TODO("reflect: iterate components+metas on REFLECT too, so they're properly saved/loaded");

TODO("edit: tree nav");
TODO("edit:   keys up,down,left,right -> move selection");
TODO("edit:   reordering/dragging items on a list. ctrl+cursors");
TODO("edit:   tab -> cycle next node of matching highlighted type");
TODO("edit: ^C^V^X thru clipboard. ^C to serialize to clipboard.");
TODO("edit: ^Z^Y cursor too. also fix undo ordering");
TODO("edit: ^S^L^N. load/save as filesystems");
TODO("edit: ^B(toolbar)");
TODO("edit: ^E prop single-view for multiple selections: should inspect common fields only");
TODO("edit:    two-column way (or Nth) for multiple selections");
TODO("edit: tab/caps view, world + entity only, obj printf");
TODO("edit: obj bounds, obj picking, obj collisions");
TODO("edit:   LMB object picking, RMB object serialization + log, floating ICONS bulb light");
TODO("edit:   worldtraveller component");
TODO("edit:   levelstreamer component");
TODO("edit: OSC server/client port 2023");
TODO("edit: add/rem entities, add/rem components, add/rem/pause/resume systems");
TODO("edit: event loop: tick,draw*,spawn,delete,un/load from bvh stream,");

TODO("edit: overlay scene editor");
TODO("edit:   overlay0 artwork");
TODO("edit:   overlay1 world editor: gizmo, grid, snap, splats (@todo: fixed screen size gizmos)");
TODO("edit:   overlay2 script editor");
TODO("edit:   overlay3 track, spline, keys editor");
TODO("edit:   overlay4 node editor (shader/anim/bt/hfsm/material/audio/blueprints)");
TODO("edit:   overlay5 csv editor");
TODO("edit:   overlay6 bug/task editor");

TODO("gfx: tree traversal from game");
TODO("gfx:   bvh and collision queries");
TODO("gfx:   visibility and pvs queries");

TODO("obj: finish SYSTEMS and join queries");
TODO("obj: make it work with /GL flag (VC)");
TODO("obj: impl obj_mutate() ... deprecate?");
TODO("obj: make() from mpack(m) + native(n)");
TODO("obj: make() should save schema `[{mn`+version. and (m)pack and (n)ative should start with objtype");
TODO("obj: super()");
TODO("obj: lock()/trylock()/unlock()/barrier(N)");
TODO("obj: diff()/patch()");
TODO("obj: free obj_children()/payload");
TODO("obj: free obj_components()/payload2");

TODO("pack: mp2json, json2mp");
TODO("pack: simplify msgpack API, make it growth similar to va()");

TODO("serialize array(types)");
TODO("serialize map(char*,types)");
TODO("serialize map(int,types)");
TODO("sprite: solid platforms, one-way platforms");
TODO("sprite: shake left-right, up-down");
TODO("sprite: coyote time");
TODO("sprite: jump buffering before grounded");
TODO("sprite: double jump, wall sliding, wall climbing");
TODO("sprite: hitbox for enemies -> wall detection");

TODO("new: integrate with Art/ browser");
TODO("bug: lite key bindings are being sent to editor");
TODO("bug: not sending quit signal to lite neither at window close nor editor close (see: temporary files)");
TODO("bug: missing search results window");
TODO("bug: missing code completions popup");

// TODO("eval:  https://github.com/drmargarido/linters")
// TODO("eval:  https://github.com/monolifed/theme16")
// TODO("eval:  https://github.com/vincens2005/lite-formatters")
// https://github.com/takase1121/lite-xl-img
// https://github.com/takase1121/lite-xl-finder
// https://github.com/rxi/lite/commit/236a585756cb9fa70130eee6c9a604780aced424 > suru.png
// https://github.com/rxi/lite/commit/f90b00748e1fe1cd2340aaa06d2526a1b2ea54ec

// ----------------------------------------------------------------------------------------
// editor loop

API void editor_pump();
API void editor_frame( void (*game)(unsigned, float, double) );

// ----------------------------------------------------------------------------------------
// engine section: @todo: expand me

API float* engine_getf(const char *key);
API int*   engine_geti(const char *key);
API char** engine_gets(const char *key);
API int    engine_send(const char *cmd, const char *optional_value);


typedef union engine_var {
    int i;
    float f;
    char *s;
} engine_var;
static map(char*,engine_var) engine_vars;
float *engine_getf(const char *key) {
    if(!engine_vars) map_init_str(engine_vars);
    engine_var *found = map_find_or_add(engine_vars, (char*)key, ((engine_var){0}) );
    return &found->f;
}
int *engine_geti(const char *key) {
    if(!engine_vars) map_init_str(engine_vars);
    engine_var *found = map_find_or_add(engine_vars, (char*)key, ((engine_var){0}) );
    return &found->i;
}
char **engine_gets(const char *key) {
    if(!engine_vars) map_init_str(engine_vars);
    engine_var *found = map_find_or_add(engine_vars, (char*)key, ((engine_var){0}) );
    if(!found->s) found->s = stringf("%s","");
    return &found->s;
}

// editing:
// nope > functions: add/rem property

UI_ICON(PLAY_ARROW) // PLAY
UI_ICON(PAUSE) // PAUSE
UI_ICON(STOP) // STOP
UI_ICON(CLOSE) // CANCEL

UI_ICON(WARNING) // WARNING
UI_ICON(FOLDER_SPECIAL) // BROWSER
UI_ICON(VIEW_IN_AR) // OUTLINER
UI_ICON(BUILD) // BUILD
UI_ICON(PHOTO_CAMERA) // SCREENSHOT
UI_ICON(VIDEOCAM) // CAMERA_ON
UI_ICON(VIDEOCAM_OFF) // CAMERA_OFF
UI_ICON(VIDEOGAME_ASSET) // GAMEPAD_ON
UI_ICON(VIDEOGAME_ASSET_OFF) // GAMEPAD_OFF
UI_ICON(VOLUME_UP) // AUDIO_ON
UI_ICON(VOLUME_OFF) // AUDIO_OFF
UI_ICON(FULLSCREEN_EXIT) // WINDOWED
UI_ICON(FULLSCREEN) // FULLSCREEN
UI_ICON(LIGHTBULB) // LIGHTS_ON
UI_ICON(LIGHTBULB_OUTLINE) // LIGHTS_OFF
UI_ICON(IMAGE_SEARCH) // RENDER_BASIC
UI_ICON(INSERT_PHOTO) // RENDER_FULL

UI_ICON(SIGNAL_CELLULAR_ALT) // SIGNAL
UI_ICON(STORAGE) // DISK
UI_ICON(SPEED) // RATE

UI_ICON(TODAY) // CLOCK
UI_ICON(TIMELAPSE) // CHRONO

UI_ICON(SETTINGS) // SETTINGS
UI_ICON(G_TRANSLATE) // LANGUAGE
UI_ICON(FACE) // PERSONA
UI_ICON(MESSAGE) // SOCIAL
UI_ICON(CREATE_NEW_FOLDER) // NEW_FOLDER
UI_ICON(REPLAY) // RESTART
UI_ICON(CLOSE) // QUIT

ICON_POWER            UI_ICON(BOLT) // UI_ICON(POWER)
ICON_BATTERY_CHARGING UI_ICON(BATTERY_CHARGING_90)
ICON_BATTERY_LEVELS \
        UI_ICON(BATTERY_ALERT), \
        UI_ICON(BATTERY_0_BAR),UI_ICON(BATTERY_1_BAR), \
        UI_ICON(BATTERY_2_BAR),UI_ICON(BATTERY_3_BAR), \
        UI_ICON(BATTERY_4_BAR),UI_ICON(BATTERY_5_BAR), \
        UI_ICON(BATTERY_6_BAR),UI_ICON(BATTERY_FULL)

struct editor_t {
    // time
    unsigned   frame;
    double     t, dt, slomo;
    // controls
    int        attached;
    int        active; // focus? does_grabinput instead?
    int        key;
    bool       gamepad; // mask instead? |1|2|4|8
    bool       battery; // battery mode: low fps
    bool       unlit;
    bool       ddraw;
} editor = {
    .active = 1,
    .gamepad = 1,
};

// ----------------------------------------------------------------------------------------
// tty

// plain and ctrl keys
EDITOR_FUNCTION(play, "down(F5)",                              ) {
    if(editor.t > 0) { window_pause(1), editor.frame = 0, editor.t = 0, editor.dt = 0, editor.slomo = 0, editor.active = 1; window_cursor(1); editor_select("**"); editor_destroy_selected(); }
    window_pause(0); /* if(!editor.slomo) editor.active = 0; */ editor.slomo = 1; 
}
EDITOR_FUNCTION(stop, "down(ESC)",                             ) { if(editor.t > 0) { window_pause(1), editor.frame = 0, editor.t = 0, editor.dt = 0, editor.slomo = 0, editor.active = 1; window_cursor(1); editor_select("**"); editor_destroy_selected(); }}
EDITOR_FUNCTION(eject, "down(F1)",                             ) { /*window_pause(!editor.active); editor.slomo = !!editor.active;*/ editor.active ^= 1; }
EDITOR_FUNCTION(pause, "(held(CTRL) & down(P)) | down(PAUSE) | down(F6)", ) { window_pause( window_has_pause() ^ 1 ); }
EDITOR_FUNCTION(frame, "held(CTRL) & down(LEFT)",              ) { window_pause(1); editor.frame++, editor.t += (editor.dt = 1/60.f); }
EDITOR_FUNCTION(slomo, "held(CTRL) & down(RIGHT)",             ) { window_pause(0); editor.slomo = maxf(fmod(editor.slomo * 2, 16), 0.125); }

// alt keys
EDITOR_FUNCTION(quit, "held(ALT) & down(F4)",                  ) { record_stop(), exit(0); }
EDITOR_FUNCTION(mute, "held(ALT) & down(M)",                   ) { audio_volume_master( 1 ^ !!audio_volume_master(-1) ); }
EDITOR_FUNCTION(gamepad, "held(ALT) & down(G)",                ) { editor.gamepad ^= 1; }
EDITOR_FUNCTION(record, "held(ALT) & down(Z)",                 ) { if(record_active()) record_stop(), ui_notify(va("Video recorded"), date_string()); else { char *name = file_counter(va("%s.mp4",app_name())); app_beep(), window_record(name); } }
EDITOR_FUNCTION(screenshot, "held(ALT) & down(S)",             ) { char *name = file_counter(va("%s.png",app_name())); window_screenshot(name), ui_notify(va("Screenshot: %s", name), date_string()); }
EDITOR_FUNCTION(battery, "held(ALT) & down(B)",                ) { editor.battery ^= 1; }
EDITOR_FUNCTION(outliner, "held(ALT) & down(O)",               ) { ui_show("Outliner", ui_visible("Outliner") ^ true); }
EDITOR_FUNCTION(profiler, "held(ALT) & down(P)",               ) { ui_show("Profiler", profiler_enable(ui_visible("Profiler") ^ true)); }
EDITOR_FUNCTION(fullscreen, "(held(ALT)&down(ENTER))|down(F11)",{ record_stop(), window_fullscreen( app_has_fullscreen() ^ 1 ); } // close any recording before framebuffer resizing, which would corrupt video stream
EDITOR_FUNCTION(unlit, "held(ALT) & down(U)",                  ) { editor.unlit ^= 1; }
EDITOR_FUNCTION(ddraw, "held(ALT) & down(D)",                  ) { editor.ddraw ^= 1; }

// ----------------------------------------------------------------------------------------

void editor_frame( void (*game)(unsigned, float, double) ) {
    do_once {
        set_init_ptr(editor.world);
        //set_init_ptr(editor.selection);
        profiler_enable( false );

        window_pause( true );
        window_cursor_shape(CURSOR_SW_AUTO);

        // fx_load("**/editorOutline.glsl");
        // fx_enable(0, 1);

        editor_seticoninstance(editor.root, ICON_MDI_SIGNAL_VARIANT);
        editor_seticoninstance(editor.on_init, ICON_MDI_SIGNAL_VARIANT);
        editor_seticoninstance(editor.on_tick, ICON_MDI_SIGNAL_VARIANT);
        editor_seticoninstance(editor.on_draw, ICON_MDI_SIGNAL_VARIANT);
        editor_seticoninstance(editor.on_edit, ICON_MDI_SIGNAL_VARIANT);
        editor_seticoninstance(editor.on_quit, ICON_MDI_SIGNAL_VARIANT);

        editor_seticonclass(obj_type(editor.root), ICON_MDI_CUBE_OUTLINE);
    }


    // draw edit view (gizmos, position markers, etc).
    for each_set_ptr(editor.world,obj*,o) {
        if( obj_hasmethod(*o,edit) ) {
            obj_edit(*o);
        }
    }

    // draw silhouettes
    sprite_flush();
    fx_begin();
    for each_map_ptr(*editor_selected_map(),void*,o,int,selected) {
        if( !*selected ) continue;
        if( obj_hasmethod(*o,draw) ) {
            obj_draw(*o);
        }
        if( obj_hasmethod(*o,edit) ) {
            obj_edit(*o);
        }
    }
    sprite_flush();
    fx_end(0,0);

    // draw mouse aabb
    aabb mouse = { vec3(input(MOUSE_X),input(MOUSE_Y),0), vec3(input(MOUSE_X),input(MOUSE_Y),1)};
    if( 1 ) {
        ddraw_color_push(YELLOW);
        ddraw_push_2d();
        ddraw_aabb(mouse.min, mouse.max);
        ddraw_pop_2d();
        ddraw_color_pop();
    }

    // tick mouse aabb selection and contextual tab (RMB)
    aabb box = {0};
    for each_set(editor.world,obj*,o) {
        if( !obj_hasmethod(o, aabb) ) continue;
        if( !obj_aabb(o, &box) ) continue;

        // trigger contextual inspector
        if( input_down(MOUSE_R) ) {
            int is_selected = editor_selected(o);
            editor_setpopup(o, is_selected);
        }

        // draw contextual inspector
        if( editor_popup(o) ) {
            if( editor_begin(va("%s (%s)", obj_name(o), obj_type(o)),EDITOR_WINDOW_NK_SMALL) ) {
                ui_label2(obj_name(o), obj_type(o));
                editor_inspect(o);
                editor_end(EDITOR_WINDOW_NK_SMALL);
            } else {
                editor_setpopup(o, 0);
            }
        }
    }
}


void editor_gizmos(int dim) {
    // debugdraw
    if(dim == 2) ddraw_push_2d();
    ddraw_ontop_push(0);

    // draw gizmos, aabbs, markers, etc
    for each_map_ptr(*editor_selected_map(),void*,o,int,selected) {
        if( !*selected ) continue;

        void *obj = *o;

        // get transform
        vec3 *p = NULL;
        vec3 *r = NULL;
        vec3 *s = NULL;
        aabb *a = NULL;

        for each_objmember(obj,TYPE,NAME,PTR) {
            /**/ if( !strcmp(NAME, "position") ) p = PTR;
            else if( !strcmp(NAME, "pos") ) p = PTR;
            else if( !strcmp(NAME, "rotation") ) r = PTR;
            else if( !strcmp(NAME, "rot") ) r = PTR;
            else if( !strcmp(NAME, "scale") ) s = PTR;
            else if( !strcmp(NAME, "sca") ) s = PTR;
            else if( !strcmp(NAME, "aabb") ) a = PTR;
        }

        ddraw_ontop(0);

        // bounding box 3d
        if( 0 ) {
            aabb box;
            if( obj_hasmethod(*o, aabb) && obj_aabb(*o, &box) ) {
                ddraw_color_push(YELLOW);
                ddraw_aabb(box.min, box.max);
                ddraw_color_pop();
            }
        }

        // position marker
        if( p ) {
            static map(void*, vec3) prev_dir = 0;
            do_once map_init_ptr(prev_dir);
            vec3* dir = map_find_or_add(prev_dir, obj, vec3(1,0,0));

            static map(void*, vec3) prev_pos = 0;
            do_once map_init_ptr(prev_pos);
            vec3* found = map_find_or_add(prev_pos, obj, *p), fwd = sub3(*p, *found);
            if( (fwd.y = 0, len3sq(fwd)) ) {
                *found = *p;
                *dir = norm3(fwd);
            }

            // float diameter = len2( sub2(vec2(box->max.x,box->max.z), vec2(box->min.x,box->min.z) ));
            // float radius = diameter * 0.5;
            ddraw_position_dir(*p, *dir, 1);
        }

        ddraw_ontop(1);

        // transform gizmo
        if( p && r && s ) {
            gizmo(p,r,s);
        }
    }

    ddraw_ontop_pop();
    if(dim == 2) ddraw_pop_2d();
}
