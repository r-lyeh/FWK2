 #include "engine.h"

int MODEL17 = 0;
const char *MODELS17[] = {
#if __has_include("demos/meshes/sphere.iqm")
    "meshes/sphere.iqm",
#endif
#if __has_include("demos/meshes/suzanne.iqm")
    "meshes/suzanne.iqm",
#endif
#if __has_include("demos/meshes/gazebo.iqm")
    "meshes/gazebo.iqm",
#endif
};

int SKYBOX17 = 0;
const char *SKYBOXES17[][3] = { // reflection, rad, env
#if __has_include("demos/hdr/Tokyo_BigSight_1k.hdr")
    {"hdr/Tokyo_BigSight_1k.hdr","hdr/Tokyo_BigSight_1k.hdr","hdr/Tokyo_BigSight_Env.hdr"},
#endif
#if __has_include("demos/hdr/GCanyon_C_YumaPoint_1k.hdr")
    {"hdr/GCanyon_C_YumaPoint_1k.hdr","hdr/GCanyon_C_YumaPoint_1k.hdr","hdr/GCanyon_C_YumaPoint_Env.hdr"},
#endif
#if __has_include("demos/hdr/Factory_Catwalk_1k.hdr")
    {"hdr/Factory_Catwalk_1k.hdr","hdr/Factory_Catwalk_Rad.hdr","hdr/Factory_Catwalk_Env.hdr"},
#endif
#if __has_include("demos/hdr/graffiti_shelter_4k.hdr")
    {"hdr/graffiti_shelter_4k.hdr","hdr/graffiti_shelter_Rad.hdr","hdr/graffiti_shelter_Env.hdr"},
#endif
#if __has_include("demos/hdr/music_hall_01_4k.hdr")
    {"hdr/music_hall_01_4k.hdr","hdr/music_hall_01_4k.hdr","hdr/music_hall_01_Env.hdr"},
#endif
#if __has_include("demos/hdr/the_sky_is_on_fire_2k.hdr")
    {"hdr/the_sky_is_on_fire_2k.hdr","hdr/the_sky_is_on_fire_2k.hdr","hdr/the_sky_is_on_fire_Env.hdr"},
#endif
#if __has_include("demos/hdr/MonValley_G_DirtRoad_1k.hdr")
    {"hdr/MonValley_G_DirtRoad_1k.hdr","hdr/MonValley_G_DirtRoad_1k.hdr","hdr/MonValley_G_DirtRoad_Env.hdr"},
#endif
#if __has_include("demos/hdr/Shiodome_Stairs_1k.hdr")
    {"hdr/Shiodome_Stairs_1k.hdr","hdr/Shiodome_Stairs_1k.hdr","hdr/Shiodome_Stairs_Env.hdr"},
#endif
#if __has_include("demos/hdr/mesto.hdr")
    {"hdr/mesto.hdr","hdr/mesto.hdr","hdr/mesto_Env.hdr"},
#endif
};

int main() {
    app_create(80, 0);

    camera_t cam = camera(); {
        cam.damping = true;
        cam.position = vec3(0, 0.8, 3);
        cam.pitch = -15;
        cam.yaw = -90;
        camera_fps(&cam, 0, 0);
    }
    skybox_t sky = {0};
    model_t mdl = {0};

    bool skybox_initialized = 0;
    bool model_initialized = 0;
    bool model_must_reload = 0;

    while( app_swap()) {
        if (input_down(KEY_ESC)) break;
        // reloading
        if (model_must_reload) {
            model_must_reload = 0;
            model_initialized = 0;
        }
        if( !skybox_initialized ) {
            skybox_initialized = 1;
            sky = skybox(SKYBOXES17[0][0], SKYBOXES17[0][1], SKYBOXES17[0][2]);
        }
        if (!model_initialized) {
            mdl = model(MODELS17[MODEL17], 0);
            {
                mdl.materials[0].layer[MATERIAL_CHANNEL_ROUGHNESS].map.color = vec4(0,0,0,0);
                mdl.materials[0].layer[MATERIAL_CHANNEL_METALLIC].map.color = vec4(1,1,1,0);
            }
            model_skybox(&mdl, sky);
            model_initialized = 1;
        }

        // fps camera
        bool active = ui_active() || ui_hovered() || gizmo_active() ? false : input(MOUSE_L) || input(MOUSE_M) || input(MOUSE_R);
        mouse_cursor( !active );

        camera_freefly(&cam, 0);

        // render
        mat44 mvp; multiply44x2(mvp, cam.proj, cam.view);
        skybox_render(&sky, cam.proj, cam.view);
        model_render(&mdl, cam.proj, cam.view, &mdl.pivot, 1, -1);

        if( ui_panel("Scene", UI_OPEN)) {
            if( ui_list("Model", &MODEL17, countof(MODELS17), MODELS17) ) {
                model_must_reload = 1;
            }
            static array(char*) list; do_once for( int i = 0; i < countof(SKYBOXES17); ++i ) array_push(list, (char*)SKYBOXES17[i][0]);
            if( ui_list("Skybox", &SKYBOX17, array_count(list), list) ) {
                skybox_destroy(&sky);
                sky = skybox(SKYBOXES17[SKYBOX17][0], SKYBOXES17[SKYBOX17][1], SKYBOXES17[SKYBOX17][2]);
                model_skybox(&mdl, sky);
            }
            ui_separator("Materials");
            ui_materials(&mdl);
            ui_panel_end();
        }
    }
}
