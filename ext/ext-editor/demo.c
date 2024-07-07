// -----------------------------------------------------------------------------
// in-game editor
// - rlyeh, public domain.

// ## Editor plan
// - editor = tree of nodes (objs). levels and objects are nodes, and their widgets are also nodes
// - there is a root node at top level.
// - objs can be attached to other objs.
// - some objs can be attached to other objs, and marked as components instead
// - you can perform actions on nodes, with or without descendants, top-bottom or bottom-top order.
// - these operations include load/save, undo/redo, reset, play/render, ddraw, etc
// - nodes are saved to disk as a filesystem layout: nodes are folders, and components are files
// - network replication can be done by external tools by comparing the filesystems and by sending the resulting diff
// - the api to handle the tree are modelled after shell commands: mv,cp,rm,rmrf, etc
//
// more:
// - the game tree can only display objects and children: wont display components in this view
// - after double clicking an obj, you enter the object-view: obj acts as root, and components are children
// - you have a ⬅️  icon to exit that view, and 💾 icon to save obj

#define EDITOR_VERSION "2024.12"
#include "engine.h"

#include "demo_types2.h"
#pragma push_macro("CODE")
#define CODE 1
#include "demo_types2.h"
#pragma pop_macro("CODE")

int EDITOR_AA; // 0:off,1:fxaa,2:msaa2,4:msaa4,8:msaa8

int  editor( void (*game)(unsigned frame, float dt, double t) );
int  editor_attach(void*); // returns nodeid for attached node

int main() {
    EDITOR_AA = optioni("--app.msaa", 1); // defaults to fxaa

    int APP_AA = EDITOR_AA == 2 ? APP_MSAA2 : EDITOR_AA == 4 ? APP_MSAA4 : EDITOR_AA == 8 ? APP_MSAA8 : 0;

    // create app (80% sized, no flags)
    app_create(80, APP_VIEWPORTS|APP_DOCKING|APP_AA);
    app_send("title", "Editor " EDITOR_VERSION);

    // editor loop
    while( app_swap() ) {
        editor( game_demo );
    }
}


static set(obj*) bookmarks;
void editor_bookmark(void* o, bool add) {
    if(!bookmarks) set_init_ptr(bookmarks);
    set_find_or_add(bookmarks, (obj*)o);
    if(add) obj_flag((obj*)o,|=,IS_OPEN);
    if(!add) set_erase(bookmarks, (obj*)o);
}


int editor_attach(void *o) {

    obj_setup(o);
    obj_attach(objroot, o);

    return *map_find(obj2id, o);
}



EDITOR_KEY(key_freecam, "held(RMB)");
EDITOR_KEY(key_freecam_slow, "held(SHIFT)");
//EDITOR_KEY(key_freecam_fov_in, "held(CTRL) & held(PLUS)");
//EDITOR_KEY(key_freecam_fov_out, "held(CTRL) & held(PLUS)");

EDITOR_KEY(key_select, "down(LMB)");
EDITOR_KEY(key_unselect, "held(CTRL) & down(D)");

EDITOR_FUNCTION(key_restart, "held(ALT) & down(F5)") {
    app_send("restart", "");
}
EDITOR_FUNCTION(key_screenshot, "down(F12) | down(PRINT)") {
    app_send("screenshot", __FILE__ ".png");
}
EDITOR_FUNCTION(key_record, "held(ALT) & down(Z)") {
    app_send("record", __FILE__ ".mp4");
}

EDITOR_KEY(key_freecam_focus, "down(F)");
vec3 freecam_focus; bool freecam_focus_enabled; // position in locked view


EDITOR_FUNCTION(key_select_all, "held(CTRL) & (down(A)|down(E)) ; Select all/none entities") {
    // @fixme: select only entities in view?
    if( obj_flag(objroot,&,IS_SELECTED) ) {
        obj_flagall(objroot, IS_SELECTED, 0);
    } else {
        obj_flagall(objroot, IS_SELECTED, 1);
    }
}

EDITOR_FUNCTION(key_select_invert, "held(CTRL) & down(I) ; Invert selection") {
    obj_flagall(objroot, IS_SELECTED, -1);
}

EDITOR_FUNCTION(key_select_clear, "held(CTRL) & down(D) ; Clear selection") {
    obj_flagall(objroot, IS_SELECTED, 0);
}


#if 0 // @todo
const char *key_freecam_hires = "held(RMB) & held(SHIFT)"; // x100 smaller increments
const char *key_freecam_pan = "held(ALT)";
const char *key_freecam_speed = "input(WMB)";

const char *key_toolbar1 = "down(Q)";
const char *key_toolbar2 = "down(W)";
const char *key_toolbar3 = "down(E)";
const char *key_toolbar4 = "down(R)";
const char *key_toolbar5 = "down(T)";
const char *key_toolbar6 = "down(Y)";
const char *key_focus = "down(F)";

EDITOR_FUNCTION(key_stop,       "") { app_pause(1); }
EDITOR_FUNCTION(key_mute,       "") { audio_volume_master( 1 ^ !!audio_volume_master(-1) ); }
EDITOR_FUNCTION(key_pause,      "") { app_pause( app_has_pause() ^ 1 ); }
EDITOR_FUNCTION(key_browser,    "") { ui_show("File Browser", ui_visible("File Browser") ^ true); }
EDITOR_FUNCTION(key_outliner,   "") { ui_show("Outliner", ui_visible("Outliner") ^ true); }
EDITOR_FUNCTION(key_record,     "") { if(record_active()) record_stop(), ui_notify(va("Video recorded"), date_string());
                                  else app_beep(), name = file_counter(va("%s.mp4",app_name())), app_record(name); }
EDITOR_FUNCTION(key_quit,       "") { record_stop(), exit(0); }
EDITOR_FUNCTION(key_screenshot, "") { char *name = file_counter(va("%s.png",app_name())), app_screenshot(name), ui_notify(va("Screenshot: %s", name), date_string()); }
EDITOR_FUNCTION(key_profiler,   "") { ui_show("Profiler", profiler_enable(ui_visible("Profiler") ^ true)); }
EDITOR_FUNCTION(key_fullscreen, "") { record_stop(), app_fullscreen( app_has_fullscreen() ^ 1 ); } // framebuffer resizing corrupts video stream, so stop any recording beforehand
EDITOR_FUNCTION(key_gamepad,    "") { *gamepadmask = (*gamepadmask & ~1u) | ((*gamepadmask & 1) ^ 1); }
EDITOR_FUNCTION(key_lit,        "") { *rendermask = (*rendermask & ~1u) | ((*rendermask & 1) ^ 1); }
EDITOR_FUNCTION(key_ddraw,      "") { *rendermask = (*rendermask & ~2u) | ((*rendermask & 2) ^ 2); }
#endif


typedef set(void*) list_all_t;

void list_all(list_all_t list, vec2 from, vec2 to) {
    static pickbuffer_t pick; do_once pick = pickbuffer();

    pickbuffer_clear(&pick);

    fbo_bind(pick.fb.id);
        camera_push();
        editor_event_down(objroot, HAS_DRAW|IS_DRAWING, false, obj_draw, "i", RENDER_PASS_PICK);
        sprite_flush();
        camera_pop();
    fbo_unbind();

    pickset_t sel = pickbuffer_pick(&pick, from, to);

    for each_set(sel, unsigned, value) {

        void **found = map_find(id2obj,value);
        if(found) {
            obj *o = *(obj**)found;
            set_find_or_add(list, o);
        }

    }
}


void editor_edit1(obj *o) {
    if( obj_name(o) )
    ui_separator(va("%s (%s)", obj_name(o), obj_type(o)));
    else
    ui_separator(va("%p (%s)", o, obj_type(o)));

    if( obj_hasmethod(o, edit) ) obj_edit(o);
    if( obj_hasmethod(o, menu) ) obj_menu(o);

    for each_objmember(o,TYPE,NAME,PTR) {
        igPushID_Ptr(PTR);

        if( !editor_changed(PTR) ) {
            obj_push(o);
        }

        int undo_clicked = 0;
        if( editor_changed(PTR) ) {
            undo_clicked = igSmallButton(UI_ICON(UNDO) " ");
            igSameLine(0,0);
        }

        char *label = va("%s", NAME);
        int changed = 0;
        /**/ if( !strcmp(TYPE,"float") )   changed = ui_float(label, PTR);
        else if( !strcmp(TYPE,"int") )     changed = ui_int(label, PTR);
        else if( !strcmp(TYPE,"vec2") )    changed = ui_float2(label, PTR);
        else if( !strcmp(TYPE,"vec3") )    changed = ui_float3(label, PTR);
        else if( !strcmp(TYPE,"vec4") )    changed = ui_float4(label, PTR);
        else if( !strcmp(TYPE,"rgb") )     changed = ui_color3(label, PTR);
        else if( !strcmp(TYPE,"rgba") )    changed = ui_color4(label, PTR);
        else if( !strcmp(TYPE,"color") )   changed = ui_color4f(label, PTR);
        else if( !strcmp(TYPE,"color3f") ) changed = ui_color3f(label, PTR);
        else if( !strcmp(TYPE,"color4f") ) changed = ui_color4f(label, PTR);
        else if( !strcmp(TYPE,"char*") )   changed = ui_string(label, PTR);
        else ui_label2(label, va("(%s)", TYPE)); // INFO instead of (TYPE)?
        if( changed ) {
            editor_setchanged(PTR, 1);
        }
        if( undo_clicked ) {
            editor_setchanged(PTR, 0);
        }
        if( !editor_changed(PTR) ) {
            obj_pop(o);
        }

        igPopID();
    }

    for each_objchild(o,obj*,oo) {
        if( obj_flag(oo,&,IS_COMPONENT) ) {
            editor_edit1(oo);
        }
    }
}


void editor_save1(const char *zippath, void *o) {
    zippath = stringf( "%s/%s", zippath, obj_name(o) );

    char *out = obj_saveini(o);
    zip(zippath, out, strlen(out));

    for each_objchild(o,obj*,oo) {
        if( obj_flag(oo,&,IS_COMPONENT) ) {
            editor_save1(zippath, oo);
        }
    }

    for each_objchild(o,obj*,oo) {
        if( !obj_flag(oo,&,IS_COMPONENT) ) {
            editor_save1(zippath, oo);
        }
    }

    FREE((char*)zippath);
}

EDITOR_FUNCTION(key_save, "held(CTRL) & down(S) ; save contents") {
    // editor_event_down(objroot, HAS_TICK|IS_TICKING, true, obj_tick, "d", dt);
    unlink(".state.zip");
    editor_save1(".state.zip", objroot);
}






int editor( void (*game)(unsigned frame, float dt, double t) ) {

    do_once {
        // enable outlines
        fx_load("**/fx/editor*.glsl");
        editor_bookmark(objroot, 1);
    }

    // camera
    static camera_t cam;
    do_once { cam = camera(); cam.damping = 1; }

    // fps camera
    bool active = ui_active() || ui_hovered() || gizmo_active() ? false : binding(key_freecam);
    mouse_show( !active );

    // @fixme: freestanding widgets should be embedded in viewport, top-right
    static int camera_mode;
    if( ui_editor_toolbar_begin() ) {
        if( igSmallButton(cam.orthographic ? 
            "2d":"3d" 
            // UI_ICON(2D) : UI_ICON(3D) 
            )) cam.orthographic ^= 1, camera_fps(&cam, 0, 0);
        igSameLine(0,0);
        ui_radio("cam", &camera_mode, 4, 
            "fps","orbit","pan","fov"
            // UI_ICON(MULTIMODAL_HAND_EYE),UI_ICON(ORBIT),UI_ICON(PAN_TOOL),UI_ICON(VRPANO)
        );
        ui_editor_toolbar_end();
    }

    do_once active = 1;
    if( active ) { // clicked[>0] dragged[<0]
        vec2 mouse_sensitivity = vec2(0.1, -0.1); // sensitivity + polarity
        vec2 drag = mul2( vec2(input_diff(MOUSE_X), input_diff(MOUSE_Y)), mouse_sensitivity );
        do_once drag.x = 0, drag.y = 0;

        if( camera_mode == 0 ) camera_fps(&cam, drag.x, drag.y); // camera_freefly(&cam, !active);
        if( camera_mode == 1 ) camera_orbit(&cam, drag.x, drag.y, 0); //len3(cam->position) );
        if( camera_mode == 2 ) camera_moveby(&cam, scale3(vec3(drag.x, drag.y, 0), 10)) ;
        if( camera_mode == 3 ) camera_fov(&cam, cam.fov += drag.y - drag.x);

        int div_speed = binding(key_freecam_slow) ? 0.1f : 1.0f;
        cam.speed = clampf(cam.speed + input_diff(MOUSE_W) / 10, 0.05f, 5.0f);
        vec3 wasdecq = scale3(vec3(input(KEY_D)-input(KEY_A),input(KEY_E)-(input(KEY_C)||input(KEY_Q)),input(KEY_W)-input(KEY_S)), cam.speed);
        cam.speed_buildup = !len3sq(wasdecq) ? 1.f : cam.speed_buildup + (cam.speed * cam.accel * div_speed * app_delta());
        camera_moveby(&cam, scale3(wasdecq, app_delta() * 60 * cam.speed_buildup * div_speed));
    }

#if 0
    // gamepad, fps camera
    if( input(GAMEPAD_CONNECTED) ) {
        vec2 filtered_lpad = input_filter_deadzone(input2(GAMEPAD_LPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
        vec2 filtered_rpad = input_filter_deadzone(input2(GAMEPAD_RPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
        vec2 mouse = scale2(vec2(filtered_rpad.x, filtered_rpad.y), 1.0f);
        vec3 wasdec = scale3(vec3(filtered_lpad.x, input(GAMEPAD_LT) - input(GAMEPAD_RT), filtered_lpad.y), 1.0f);
        camera_moveby(&cam, scale3(wasdec, app_delta() * 60));
        camera_fps(&cam, mouse.x,mouse.y);
        mouse_show( true );
        return;
    }
#endif

#if 1

    // time
    static double t;
    static float dt;
    static float slomo = 1;
    dt = clampf(app_delta(), 0, 1) * !app_has_pause() * slomo;
    t += dt;

    // frame and tick
    static unsigned frame;
    if( game ) game(frame, dt, t);
    frame += !app_has_pause();
    frame += !frame;

    // process inputs & messages
    //editor_pump();

    static int ingame = 0;

    #if 0
    // draw menubar
    if( ui_tray() ) {
        ui_editor_bar(frame, t, slomo);
        ui_tray_end();
    }
    #endif

    if( ingame ) return 0;

#endif

    // tick new demo objects
    camera_enable(&cam);

    // @todo do fixed event slices instead: while( dt > 1/60. ) event(1/60.), dt -= 1/60.;
    editor_event_down(objroot, HAS_TICK|IS_TICKING, false, obj_tick, "d", dt);

    // draw editor grid
    fx_enable_all(0);
    fx_enable(fx_find("editorFXAA.glsl"), EDITOR_AA == 1);
    fx_begin();
    {
        ddraw_ground(0);
        ddraw_flush();
    }
    fx_end(0,0);

    // user object selection: performs a whole scene render pass when mouse is clicked
    static list_all_t list; do_once set_init_ptr(list); set_clear(list);
    // check that we're not moving a window + not in fps cam
    if( gizmo_hovered() || gizmo_active() || ui_active() ? 0 : !!mouse().cursor ) {
        // click + click rect
        if( 1 ) {
            static vec2 from = {0}, to = {0};
            /**/ if( input_idle(MOUSE_L) || input_down(MOUSE_L) ) {
                from = to = vec2(input(MOUSE_X), input(MOUSE_Y));
            }
            else if( input(MOUSE_L) ) {
                to = vec2(input(MOUSE_X), input(MOUSE_Y));
                if( len2sq(sub2(from,to)) > 0 ) {
                    vec2 a = min2(from, to), b = max2(from, to);
                    ddraw_push_2d();
                    ddraw_color_push(YELLOW);
                    ddraw_line( vec3(a.x,a.y,0),vec3(b.x,a.y,0) );
                    ddraw_line( vec3(b.x,a.y,0),vec3(b.x,b.y,0) );
                    ddraw_line( vec3(b.x,b.y,0),vec3(a.x,b.y,0) );
                    ddraw_line( vec3(a.x,b.y,0),vec3(a.x,a.y,0) );
                    ddraw_color_pop();
                    ddraw_pop_2d();
                }
            }
            else if( input_up(MOUSE_L) ) {
                // single click
                mouse_t m = mouse();
                list_all(list, vec2(m.x,m.y), vec2(m.x+1,m.y+1));
                // rect click
                vec2 aa = min2(from, to), bb = max2(from, to);
                list_all(list, aa, bb);
                // count items in list
                int count = set_count(list);
                // single selection
                if( count == 1 ) {
                    for each_set(list,obj*,o) {
                        if( !o ) 
                            obj_flagall(objroot,IS_SELECTED,0);
                        else {
                            if( !input(KEY_CTRL) && !input(KEY_SHIFT) ) {
                                obj_flagall(objroot,IS_SELECTED,0);
                                obj_flag(o,|=,IS_SELECTED);
                            }
                            else if( input(KEY_SHIFT) ) {
                                obj_flag(o,|=,IS_SELECTED);
                            }
                            else {
                                if( obj_flag(o,&,IS_SELECTED) )
                                    obj_flag(o,&=,~IS_SELECTED);
                                else
                                    obj_flag(o,|=,IS_SELECTED);
                            }
                        }
                    }
                }
                // multi selection
                if( count > 1 ) {
                    if( !input(KEY_CTRL) && !input(KEY_SHIFT) ) {
                        obj_flagall(objroot,IS_SELECTED,0);
                    }

                    for each_set(list,obj*,o) {
                        if( o ) {
                            if( !input(KEY_CTRL) ) {
                                obj_flag(o,|=,IS_SELECTED);
                            }
                            else {
                                if( obj_flag(o,&,IS_SELECTED) )
                                    obj_flag(o,&=,~IS_SELECTED);
                                else
                                    obj_flag(o,|=,IS_SELECTED);
                            }
                        }
                    }
                }
            }
        }
    }

    // draw game scene
    fx_enable_all(0);
    fx_enable(fx_find("editorFXAA.glsl"), 0); // EDITOR_AA == 1);
    fx_begin();
    {
        camera_push();
        int default_render_pass = -1;
        editor_event_down(objroot, HAS_DRAW|IS_DRAWING, false, obj_draw, "i", default_render_pass);
        sprite_flush();
        camera_pop();
    }
    fx_end(0,0);

    // draw object outlines + gizmo
    array(obj*) selection = editor_group_down(objroot,IS_SELECTED,IS_SELECTED);

    // draw object outlines
    if( 1 ) {
        // draw outline
        fx_enable_all(0);
        fx_enable(fx_find("editorOutline.glsl"), 1);
        fx_begin();

            camera_push();

                for each_array(selection,obj*,o)
                    while( (o = obj_parent(o)) != NULL )
                        if( obj_flag(o,&,HAS_VIEW) )
                            obj_flag(o,|=,IS_SELECTED);

                int default_render_pass = -1;
                editor_event_down(objroot, HAS_DRAW|IS_SELECTED, true, obj_draw, "i", default_render_pass);

                obj_flagall(objroot,IS_SELECTED,0);
                for each_array(selection,obj*,o)
                    obj_flag(o,|=,IS_SELECTED);

            sprite_flush();
            camera_pop();

        fx_end(0,0);
    }

    // find gizmo matrices + centroid of selection
    static array(float*) matrices; array_resize(matrices, 0);

            // centroid of selection
            vec3 centroid = {0};
            int num_points = 0;

            for each_array(selection, obj*, o) {
                if( obj_hasmethod(o, xform) ) {
                    // collect matrices
                    float *pivot = obj_xform(o);
                    array_push(matrices, pivot);
                    // eval centroid
                    centroid = add3(centroid, ptr3(&pivot[12]));
                    ++num_points;
                }
            }

            centroid = scale3(centroid, 1. / (num_points+!num_points));

    // draw gizmos
    gizmo44( array_count(matrices), matrices );

    // focus camera, lock on. @todo: reposition camera and enable orbit mode
    freecam_focus = centroid;
    if( gizmo_hovered() || gizmo_active() || binding(key_freecam) || !array_count(selection) || mouse().l ) freecam_focus_enabled = 0;
    if( binding(key_freecam_focus) ) freecam_focus_enabled = !freecam_focus_enabled;
    if( freecam_focus_enabled ) camera_lookat(&cam, freecam_focus);

    // inspect UI
#if DOCKING
    static int wopen; wopen = 1;
    if( ui_window("Inspector", &wopen) ) {
#else
    if( ui_panel("Inspector", UI_OPEN) ) {
#endif
        ui_editor_controlbar(frame, t, slomo);

        for each_set(bookmarks,obj*,o) {
            ui_separator(obj_name(o));
            igPushID_Ptr(o);
            ui_editor_tree(o, obj_name(o));
            igPopID();
        }
        if( ui_section("Dev Utils") ) {
            ui_label2("Gizmo hovered", va("%d", gizmo_hovered()));
            ui_label2("Gizmo active", va("%d", gizmo_active()));
            ui_label2("Selected objects", va("%d", array_count(selection)));
            for each_array(selection, obj*, o) {
                igIndent(12);
                ui_label2("Object", obj_name(o));
                igIndent(-12);
            }
        }
#if DOCKING
        ui_window_end();
    }
    static int wopen2; wopen2 = 1;
    if( ui_window("Object", &wopen2) ) {
#endif
        for each_array(selection,obj*,o) {
            editor_edit1(o);
        }
#if DOCKING
        ui_window_end();
#else
        ui_panel_end();
#endif
    }

    return 0;
}

