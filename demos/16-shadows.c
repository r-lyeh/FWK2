 #include "engine.h"

int MODEL = 0;
const char *MODELS[] = {
#if __has_include("demos/meshes/ShadowsTest.iqm")
    "meshes/ShadowsTest.iqm",
#endif
#if __has_include("demos/meshes/suzanne.iqm")
    "meshes/suzanne.iqm",
#endif
#if __has_include("demos/meshes/sphere.iqm")
    "meshes/sphere.iqm",
#endif
#if __has_include("demos/meshes/gazebo.iqm")
    "meshes/gazebo.iqm",
#endif
#if __has_include("lab/art/models/machina/Type93_02.iqm")
    "models/machina/Type93_02.iqm",
#endif
#if __has_include("lab/art/models/street/Highway_Interchange.iqm")
    "models/street/Highway_Interchange.iqm",
#endif
};

int SKYBOX = 0;
const char *SKYBOXES[][3] = { // reflection, rad, env
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

int main(int argc, char** argv) {
    app_create(80, 0);

    fx_load("fx/**.glsl");
    fx_enable(fx_find("fxTonemapUchimura.glsl"), 1);

    camera_t cam = camera(); {
        cam.position = vec3(0, 7.5, 15);
        cam.pitch = -15;
        cam.yaw = -90;
        camera_fps(&cam, 0, 0);
    }
    skybox_t sky = {0};
    model_t mdl = {0};
    shadowmap_t sm = {0};

    light_t lit = light(); {
        lit.type = LIGHT_POINT;
        lit.cast_shadows = true;
    }
    light_t lit2 = light(); {
        lit2.type = LIGHT_POINT;
        lit2.cast_shadows = true;
        lit2.diffuse = scale3(vec3(1, 0.7, 0.8), 1.0f);
    }
    light_t lit3 = light(); {
        lit3.type = LIGHT_SPOT;
        lit3.cast_shadows = true;
        lit3.diffuse = scale3(vec3(1, 0.7, 0.8), 1.0f);
        lit3.power *= 2.0f;
    }
    light_t lit4 = light(); {
        lit4.type = LIGHT_DIRECTIONAL;
        lit4.cast_shadows = true;
        lit4.diffuse = scale3(vec3(1, 0.7, 0.8), 1.0f);
    }
    light_t lit5 = light(); {
        lit5.type = LIGHT_POINT;
        lit5.cast_shadows = true;
        lit5.diffuse = scale3(vec3(1, 0, 0), 1.0f);
    }
    light_t lit6 = light(); {
        lit6.type = LIGHT_POINT;
        lit6.cast_shadows = true;
        lit6.diffuse = scale3(vec3(0, 1, 0), 1.0f);
    }
    light_t lit7 = light(); {
        lit7.type = LIGHT_POINT;
        lit7.cast_shadows = true;
    }

    array(light_t) point_lights = 0;
    array_push(point_lights, lit);
    array_push(point_lights, lit2);

    array(light_t) spot_lights = 0;
    array_push(spot_lights, lit3);

    array(light_t) directional_lights = 0;
    array_push(directional_lights, lit4);

    array(light_t) all_lights = 0;
    array_push(all_lights, lit);
    array_push(all_lights, lit2);
    array_push(all_lights, lit4);
    array_push(all_lights, lit5);
    array_push(all_lights, lit6);

    bool skybox_initialized = 0;
    bool model_initialized = 0;
    bool model_must_reload = 0;
    bool rotate_sun = 1;

    while( app_swap()) {
        if (input_down(KEY_ESC)) break;
        // reloading
        if (model_must_reload) {
            model_must_reload = 0;
            model_initialized = 0;
        }
        if( !skybox_initialized ) {
            skybox_initialized = 1;
            sky = skybox(0,0,0);
            sky.mode = SKYBOX_CUBEMAP;
            sky.cubemap = sky.probe;
            sm = shadowmap();
        }
        if (!model_initialized) {
            mdl = model(MODELS[MODEL], 0);
            model_skybox(&mdl, sky);

            // disable IBL for all materials
            // for each_material(mdl, material) {
            //     material->enable_ibl = false;
            // }
            model_initialized = 1;
        }

        // fps camera
        bool active = ui_active() || ui_hovered() || gizmo_active() ? false : input(MOUSE_L) || input(MOUSE_M) || input(MOUSE_R);
        mouse_cursor( !active );

        camera_freefly(&cam, 0);

        enum {
            POINT, SPOT, DIR, ALL, NONE
        };
        static unsigned mode = DIR;

        if (!ui_active()) {
            if (input_down(KEY_1)) mode = POINT;
            if (input_down(KEY_2)) mode = SPOT;
            if (input_down(KEY_3)) mode = DIR;
            if (input_down(KEY_4)) mode = ALL;
            if (input_down(KEY_5)) mode = NONE;
            if (input_down(KEY_R)) rotate_sun = !rotate_sun;
        }
        light_t *lights = 0;
        switch (mode) {
            case POINT: lights = point_lights; break;
            case SPOT: lights = spot_lights; break;
            case DIR: lights = directional_lights; break;
            case ALL: lights = all_lights; break;
            case NONE: lights = 0; break;
        }

        // Animate light
        if (mode == POINT) {
            lights[0].pos = vec3(0, 5.5, 1);
            lights[0].pos.x += sinf(app_time()*2)*4.5f;
            lights[0].pos.y += cosf(app_time()*2)*1.0;
            lights[0].pos.z += cosf(app_time()*2)*6.0;

            lights[1].pos = vec3(0, 7.5, 1);
            lights[1].pos.x += sinf(app_time()*4)*4.5f;
            lights[1].pos.y += cosf(app_time()*4)*1.0;
            lights[1].pos.z += cosf(app_time()*4)*6.0;
        }

        static bool camera_spot = true;
        if (input_down(KEY_SPACE)) camera_spot = !camera_spot;
        if (mode == SPOT && camera_spot) {
            lights[0].pos = cam.position;
            lights[0].pos = add3(lights[0].pos, scale3(cam.lookdir, 1.5f));
            vec3 rhs = cross3(cam.lookdir, vec3(0,1,0));
            lights[0].pos = add3(lights[0].pos, scale3(rhs, 0.5));
            lights[0].dir = cam.lookdir;
        }

        if (mode == DIR) {
            lights[0].dir = vec3(1,-1,-1);

            if (rotate_sun) {
                lights[0].dir.x = cosf(app_time());
                lights[0].dir.z = sinf(app_time());
            }
        }

        if (mode == ALL) {
            lights[0].pos = vec3(0, 5.5, 1);
            lights[0].pos.x += sinf(app_time()*2)*4.5f;
            lights[0].pos.y += cosf(app_time()*2)*1.0;
            lights[0].pos.z += cosf(app_time()*2)*6.0;

            lights[1].pos = vec3(0, 7.5, 1);
            lights[1].pos.x += sinf(app_time()*4)*4.5f;
            lights[1].pos.y += cosf(app_time()*4)*1.0;
            lights[1].pos.z += cosf(app_time()*4)*6.0;

            lights[2].pos = cam.position;
            lights[2].pos = add3(lights[2].pos, scale3(cam.lookdir, 1.5f));
            vec3 rhs = cross3(cam.lookdir, vec3(0,1,0));
            lights[2].pos = add3(lights[2].pos, scale3(rhs, 0.5));
            lights[2].dir = cam.lookdir;

            lights[3].dir = vec3(1,-1,-1);

            lights[4].pos = vec3(-3, 3, 0);
            lights[5].pos = vec3(3, 3, 0);
        }

        // Render shadowmap
        if (lights) {
            shadowmap_begin(&sm);
            {
                for (int i = 0; i < array_count(lights); i++) {
                while (shadowmap_step(&sm)) {
                    shadowmap_light(&sm, &lights[i], cam.proj, cam.view);
                    model_render(&mdl, cam.proj, cam.view, &mdl.pivot, 1, RENDER_PASS_SHADOW);
                }
            }
        }
            shadowmap_end(&sm);
        }

        // render
        mat44 mvp; multiply44x2(mvp, cam.proj, cam.view);
        fx_begin();
        {
            skybox_render(&sky, cam.proj, cam.view);

            model_shadow(&mdl, mode == NONE ? 0 : &sm);
            model_light(&mdl, array_count(lights), lights);
            model_render(&mdl, cam.proj, cam.view, &mdl.pivot, 1, -1);
        }
        fx_end(0,0);

        if( ui_panel("Scene", UI_OPEN)) {
            if( ui_list("Model", &MODEL, countof(MODELS), MODELS) ) {
                model_must_reload = 1;
            }
            static array(char*) list; do_once for( int i = 0; i < countof(SKYBOXES); ++i ) array_push(list, (char*)SKYBOXES[i][0]);
            if( ui_list("Skybox", &SKYBOX, array_count(list), list) ) {
                skybox_destroy(&sky);
                sky = skybox(SKYBOXES[SKYBOX][0], SKYBOXES[SKYBOX][1], SKYBOXES[SKYBOX][2]);
                model_skybox(&mdl, sky);
            }
            ui_separator("Skybox");
            ui_skybox(&sky);
            ui_separator("Materials");
            ui_materials(&mdl);
            ui_separator("Lights");
            ui_lights(array_count(lights), lights);
            ui_separator("Shadowmap");
            ui_shadowmap(&sm);
            ui_panel_end();
        }
    }
}
