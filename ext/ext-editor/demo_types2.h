// ----------------------------------------------------------------------------
// demo

// @fixme: deprecate use of 'menu' to handle UI
// @fixme: deprecate use of 'edit' to manipulate as physical object
// @fixme: deprecate use of 'aabb' to measure bounding boxes
// @fixme: deprecate use of 'icon' to retrieve class icon
// @fixme: allow annotations in STRUCT() members, like shaders: /// set:1,1,0,1 max:1,1,1,1 min:-1,-1,-1,-1 tip:"blabla"

#if !CODE

typedef struct lit { OBJ
    vec3 pos;
    vec3 dir;
    int type;
} lit;

OBJTYPEDEF(lit,201);

typedef struct kid { OBJ
    int kid;
    vec2 pos;
    vec2 vel;
    float angle;
    unsigned color;
    int controllerid;

    // --- private
    char *filename;
    texture_t texture_;
} kid;

OBJTYPEDEF(kid,200);

typedef struct model2 { OBJ
    char *filename_;
    model_t mdl_;
} model2;

OBJTYPEDEF(model2,199);

typedef struct cam2 { OBJ
    int dummy;
    camera_t cam_;
} cam2;

OBJTYPEDEF(cam2,198);

void game_demo(unsigned frame, float dt, double t);

#else

void cam2_ctor(cam2* obj) {
    // init camera (x,y) (z = zoom)
    obj->cam_ = camera();
    obj->cam_.position = vec3(app_width()/2,app_height()/2,1);
}
void cam2_tick(cam2 *obj) {
}
void cam2_view(cam2 *obj) {
    camera_enable(&obj->cam_);
    camera_fps(&obj->cam_,0,0);
}
void cam2_draw(cam2 *obj) {
    cam2_view(obj);
}
float *cam2_xform(cam2 *obj) {
    return obj->cam_.view;
}
const char *cam2_icon(cam2 *obj) {
    return UI_ICON(PHOTO_CAMERA);
}
AUTORUN {
    STRUCT(cam2, int, dummy); // @fixme: remove me
    EXTEND(cam2, ctor,tick,draw,xform,icon,view);
}



void model2_ctor(model2 *obj) {
    if( !obj->filename_ ) obj->filename_ = "cube.iqm";
    obj->filename_ = STRDUP(obj->filename_);
    obj->mdl_ = model(obj->filename_, 0);
    compose44(obj->mdl_.pivot, vec3(0,0,0), eulerq(vec3(0,0,0)), vec3(2,2,2)); // position, rotation, scale
}
const char *model2_icon(model2 *obj) {
    return UI_ICON(GAMEPAD);
}
int model2_edit(model2 *obj) {
    //if( obj_flag(obj,&,IS_SELECTED) )
    return 1;
}
int model2_menu(model2 *obj) {
    //if( obj_flag(obj,&,IS_SELECTED) )
    {
        ui_mat44("xform", obj->mdl_.pivot);
        editor_label3(ptr3(obj->mdl_.pivot+12), obj_name(obj));
        float *list[] = { &obj->mdl_.pivot[0] };
        gizmo44(1, list); //ui_label2("obj2", obj_name(obj));
    }
    return 1;
}
void model2_tick(model2 *obj, double delta) {
    obj->mdl_.curframe = model_animate(obj->mdl_, obj->mdl_.curframe + delta * 30);
}
void model2_draw(model2 *obj, int render_pass) {
    {
        unsigned id = obj->objid;
        unsigned r = (id >> 0) & 255;
        unsigned g = (id >> 8) & 255;
        unsigned b = (id >>16) & 255;
        unsigned a = (id >>24) & 255;
        obj->mdl_.object_id = vec4(r/255.,g/255.,b/255.,a/255.);
    }
    camera_t *cam = camera_get_active(); // @fixme: supply this via proj[16],view[16] args instead
    model_render(&obj->mdl_, cam->proj, cam->view, &obj->mdl_.pivot, 1, render_pass);
}
float *model2_xform(model2 *obj) {
    return obj->mdl_.pivot;
}
AUTORUN {
    STRUCT(model2, char*, filename_);
    STRUCT(model2, mat44, mdl_.pivot);
    EXTEND(model2, ctor,icon,edit,draw,tick,menu,xform);
}





int lit_aabb(lit *obj, aabb *box) {
    *box = aabb( vec3(obj->pos.x-16,obj->pos.y-16,0), vec3(obj->pos.x+16,obj->pos.y+16,1) );
    return 1;
}
const char *lit_icon(lit *obj) {
    const char *icon =
        obj->type == 0 ? UI_ICON(WB_IRIDESCENT) :
        obj->type == 1 ? UI_ICON(WB_INCANDESCENT) :
        obj->type == 2 ? UI_ICON(FLARE) :
        obj->type == 3 ? UI_ICON(WB_SUNNY) : "";
    return icon;
}
int lit_edit(lit *obj) {
    const char *all_icons =
        UI_ICON(WB_IRIDESCENT)
        UI_ICON(WB_INCANDESCENT)
        UI_ICON(FLARE)
        UI_ICON(WB_SUNNY)

        UI_ICON(LIGHT_MODE)
        UI_ICON(LIGHT)

        UI_ICON(FLASHLIGHT_OFF)
        UI_ICON(FLASHLIGHT_ON)
        UI_ICON(HIGHLIGHT)
        UI_ICON(HIGHLIGHT_ALT)
        UI_ICON(LIGHTBULB)
        UI_ICON(LIGHTBULB_OUTLINE)
        UI_ICON(NIGHTLIGHT)
        UI_ICON(NIGHTLIGHT_ROUND)

        UI_ICON(WALL_LAMP)           //
        UI_ICON(SUNNY)               // directional
    ;
    // editor_label2(vec2(obj->pos.x+16,obj->pos.y-32),all_icons);
    if( obj_flag(obj,&,IS_SELECTED) ) {
    obj->pos.x += input(KEY_RIGHT) - input(KEY_LEFT);
    obj->pos.y += input(KEY_DOWN) - input(KEY_UP);
    obj->type = (obj->type + !!input_down(KEY_SPACE)) % 4;
    }
    editor_label2(vec2(obj->pos.x,obj->pos.y),lit_icon(obj));

    return 1;
}

AUTORUN {
    STRUCT(lit, vec3, pos);
    STRUCT(lit, vec3, dir);
    STRUCT(lit, int, type);
    EXTEND(lit, edit,icon,aabb);
}






void kid_ctor(kid *obj) {
    obj->kid = randi(0,3);
    obj->pos = vec2(randi(0, app_width()), randi(0, app_height()));
    obj->vel.x = obj->vel.y = 100 + 200 * randf();
    obj->controllerid = randi(0,3);

    obj->texture_ = texture(obj->filename, TEXTURE_RGBA|TEXTURE_LINEAR);
}
void kid_tick(kid *obj, double delta) {
    // add velocity to position
    vec2 off = vec2( input(KEY_RIGHT)-input(KEY_LEFT), input(KEY_DOWN)-input(KEY_UP) );
    obj->pos = add2(obj->pos, scale2(mul2(obj->vel, off), delta * (obj->controllerid == 0)));

    // wrap at screen boundaries
    const int appw = app_width(), apph = app_height();
    if( obj->pos.x < 0 ) obj->pos.x += appw; else if( obj->pos.x > appw ) obj->pos.x -= appw;
    if( obj->pos.y < 0 ) obj->pos.y += apph; else if( obj->pos.y > apph ) obj->pos.y -= apph;
}
void kid_draw(kid *obj, int render_pass) {
    // 4x4 tilesheet
    int col = (((int)obj->kid) % 4);
    int row = (((int)obj->pos.x / 10 ^ (int)obj->pos.y / 10) % 4);
    float position[3] = {obj->pos.x,obj->pos.y,obj->pos.y}; // position(x,y,depth: sort by Y)
    float offset[2]={0,0}, scale[2]={1,1};
    float coords[3]={col * 4 + row,4,4}; // num_frame(x) in a 4x4(y,z) spritesheet
    unsigned flags = 0;
    if( render_pass == RENDER_PASS_PICK ) {
        aabb box;
        kid_aabb(obj, &box);
        pickbuffer_renderquad(NULL, ptr2(&box.min.x), ptr2(&box.max.x), obj->objid);
    }
    else {
        sprite_sheet(obj->texture_, coords, position, obj->angle*TO_DEG, offset, scale, obj->color, flags);
    }
}
int kid_aabb(kid *obj, aabb *box) {
    *box = aabb( vec3(obj->pos.x-16,obj->pos.y-16,0), vec3(obj->pos.x+16,obj->pos.y+16,1) );
    return 1;
}
int kid_edit(kid *obj) {
    aabb box;
    if( kid_aabb(obj, &box) ) {
        ddraw_color_push(YELLOW);
        ddraw_push_2d();
        ddraw_aabb(box.min, box.max);
        ddraw_pop_2d();
        ddraw_color_pop();
    }
    if( obj_flag(obj,&,IS_SELECTED) ) {
        obj->pos.x += input(KEY_RIGHT) - input(KEY_LEFT);
        obj->pos.y += input(KEY_DOWN) - input(KEY_UP);

        editor_label2(vec2(obj->pos.x+16,obj->pos.y-16),UI_ICON(VIDEOGAME_ASSET));
    }
    return 1;
}
void kid_menu(kid *obj, const char *argv) {
    if( ui_section("Private section") ) {
        ui_color4("Color_", &obj->color);
        ui_texture("Texture_", obj->texture_);
    }
}
const char* kid_icon(kid *obj) {
    return UI_ICON(IMAGE); // ACCOUNT_BOX);
}

AUTORUN {
    // reflect
    STRUCT(kid, int, kid);
    STRUCT(kid, vec2, pos);
    STRUCT(kid, vec2, vel);
    STRUCT(kid, float, angle, "Tilt degrees");
    STRUCT(kid, rgba,  color, "Tint color");
    STRUCT(kid, char*, filename, "Filename" );
    EXTEND(kid, ctor,tick,draw,aabb,edit,menu,icon);
}




void game_demo(unsigned frame, float dt, double t) {
    do_once {
        // animated models loading
        model2 *witch = obj_new_name(model2, "witch", "witch.iqm");
        // editor_bookmark(witch);
        editor_attach(witch);

        // animated models loading
        model2 *kgirl = obj_new_name(model2, "kgirl", "kgirls01.iqm");
        // editor_bookmark(kgirl);
        editor_attach(kgirl);
    }

    static kid *kid0;
    static kid *kid1;
    static kid *kid2;
    static cam2 *cam;
    if( !frame ) {
        cam = obj_new_name(cam2, "cam2d");

        kid0 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=5,2\n"
            "angle=pi/12\n"
            "color=#ffcf\n"
        );
        kid1 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=1,100\n"
            "angle=pi/12\n"
            "color=#fcc\n"
        );
        kid2 = obj_make(
            "[kid]\n"
            "filename=spriteSheetExample.png\n"
            "pos=50,200\n"
            "angle=pi/12\n"
            "color=#ccccffff\n"
        );

        obj_setname(kid0, "kid0");
        obj_setname(kid1, "kid1");
        obj_setname(kid2, "kid2");

        obj*lit3 = obj_make(
            "[lit]\n"
            "pos=300,300,0\n"
            "type=1"
        );
        obj*obj4 = obj_new_name(obj, "obj4");
        obj*obj5 = obj_new_name(obj, "obj5");

        obj_attach(cam, kid0);
        obj_attach(cam, kid1);
        obj_attach(cam, kid2);
        obj_attach_component(kid1, lit3);
        obj_attach_component(kid1, obj4);
        obj_attach_component(kid0, obj5);

        editor_attach(cam);
    }


#if 0
    // camera panning (x,y) & zooming (z)
    if(0)
    if( !ui_hovered() && !ui_active() ) {
        if( input(MOUSE_L) ) cam.position.x -= input_diff(MOUSE_X);
        if( input(MOUSE_L) ) cam.position.y -= input_diff(MOUSE_Y);
        cam.position.z += input_diff(MOUSE_W) * 0.1; // cam.p.z += 0.001f; for tests
    }
#endif

    // tick game
    if( dt ) {
        kid0->angle = 5 * sin(t+dt);
    }
}


#endif
