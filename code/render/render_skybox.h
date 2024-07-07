// -----------------------------------------------------------------------------
// skyboxes

#if !CODE

enum SKYBOX_MODE {
    SKYBOX_PANORAMA,
	SKYBOX_PROCEDURAL,
	SKYBOX_CUBEMAP,
};

enum SKYBOX_UPDATE_MODE {
    SKYBOX_UPDATE_ONCE,
    SKYBOX_UPDATE_FRAME,
};

typedef struct skybox_t {
    shader_t shader_panorama, shader_cubemap, shader_rayleigh;
    mesh_t geometry;
    cubemap_t cubemap;
    int mode;
    int update_mode;
    bool rayleigh_immediate;
    int probe_size;

    // pbr
    texture_t refl, env;
    
    // probe
    cubemap_t probe;

    // procedural coefficients
    vec3 uSunPos;                // Position of the sun
    vec3 uRayOrigin;             // Ray origin, typically the observer's position
    float uSunIntensity;         // Intensity of the sun
    float uPlanetRadius;         // Radius of the planet
    float uAtmosphereRadius;     // Radius of the atmosphere
    vec3 uRayleighScattering;    // Rayleigh scattering coefficients
    float uMieScattering;        // Mie scattering coefficient
    float uRayleighScaleHeight;  // Rayleigh scale height
    float uMieScaleHeight;       // Mie scale height
    float uMiePreferredDirection; // Mie scattering preferred direction (g)
} skybox_t;

API skybox_t skybox(const char *sky_map, const char *refl_map, const char *env_map); //< NULL for procedural
API int      skybox_render(skybox_t *sky, mat44 proj, mat44 view);
API void     skybox_destroy(skybox_t *sky);
API void     skybox_bakeprobe(skybox_t *probe, skybox_t *sky, int width);

API void ui_skybox(skybox_t *sky);

#else

// -----------------------------------------------------------------------------
// skyboxes

static inline
texture_t load_env_tex( const char *pathfile, unsigned flags ) {
    stbi_hdr_to_ldr_gamma(2.2f);
    int flags_hdr = strendi(pathfile, ".hdr") ? TEXTURE_FLOAT | TEXTURE_RGBA : 0;
    texture_t t = texture_compressed(pathfile, flags | TEXTURE_LINEAR | TEXTURE_REPEAT | TEXTURE_UNIQUE | flags_hdr);
    glBindTexture( GL_TEXTURE_2D, t.id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    return t;
}

skybox_t skybox(const char *sky_map, const char *refl_map, const char *env_map) {
    skybox_t sky = {0};
    sky.probe_size = 512;

    // sky mesh
    vec3 vertices[] = {{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},{-1,+1,-1},{+1,-1,-1},{-1,-1,-1},{-1,-1,+1},{-1,+1,+1}};
    unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };
    mesh_update(&sky.geometry, "p3", 0,countof(vertices),vertices, countof(indices),indices, MESH_TRIANGLE_STRIP);

    // sky program
    sky.shader_panorama = shader2(NULL, file_read("shaders/skybox_vs.glsl",0),
        file_read("skybox_fs.glsl",0),
        NULL, "att_position", "fragcolor");
    sky.shader_cubemap = shader2(NULL, file_read("shaders/skybox_vs.glsl",0),
        file_read("skybox_fs.glsl",0),
        NULL, "att_position", "fragcolor");
    sky.shader_rayleigh = shader2(NULL, file_read("shaders/skybox_vs.glsl",0),
        file_read("shaders/skybox_rayleigh_fs.glsl",0),
        NULL, "att_position", "fragcolor");

    shader2_setdefine(&sky.shader_panorama, "USE_POLAR", '1');
    shader2_apply(&sky.shader_panorama);

    sky.uSunPos = vec3(0, 0.1, -1);
    sky.uRayOrigin = vec3(0.0, 6372000.0, 0.0);
    sky.uSunIntensity = 22.0;
    sky.uPlanetRadius = 6371000.0;
    sky.uAtmosphereRadius = 6471000.0;
    sky.uRayleighScattering = vec3(5.5e-6, 13.0e-6, 22.4e-6);
    sky.uMieScattering = 21e-6;
    sky.uRayleighScaleHeight = 8000.0;
    sky.uMieScaleHeight = 1200.0;
    sky.uMiePreferredDirection = 0.758;

    // sky cubemap & SH
    if( sky_map ) {
        int is_panorama = file_size( sky_map );
        if( is_panorama ) { // is file
            stbi_hdr_to_ldr_gamma(2.2f);
            texture_t panorama = load_env_tex( sky_map, 0 );
            sky.cubemap = cubemap( panorama, 0 );
        } else { // is folder
            PRINTF("[warn] Folder-based skyboxes are not supported anymore!");
        }
    }
    if( refl_map ) {
        sky.refl = load_env_tex(refl_map, TEXTURE_MIPMAPS);
    }
    if( env_map ) {
        sky.env = load_env_tex(env_map, TEXTURE_MIPMAPS);
    }

    if ( !sky_map ) {
        // set up mie defaults // @fixme: use shader params instead
        sky.mode = SKYBOX_PROCEDURAL;
        skybox_bakeprobe(&sky, &sky, sky.probe_size);
    } else if ( !refl_map ) {
        skybox_bakeprobe(&sky, &sky, sky.probe_size);
    }


    return sky;
}

static renderstate_t skybox_rs;
API vec4 window_getcolor_(); // internal use, not public

static inline
void skybox_render_rayleigh(skybox_t *sky, mat44 proj, mat44 view) {
    last_cubemap = &sky->cubemap;
    shader2_adduniform(&sky->shader_rayleigh, uniform("vec3 uSunPos", .v3 = sky->uSunPos));
    shader2_adduniform(&sky->shader_rayleigh, uniform("vec3 uRayOrigin", .v3 = sky->uRayOrigin));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uSunIntensity", .f = sky->uSunIntensity));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uPlanetRadius", .f = sky->uPlanetRadius));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uAtmosphereRadius", .f = sky->uAtmosphereRadius));
    shader2_adduniform(&sky->shader_rayleigh, uniform("vec3 uRayleighScattering", .v3 = sky->uRayleighScattering));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uMieScattering", .f = sky->uMieScattering));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uRayleighScaleHeight", .f = sky->uRayleighScaleHeight));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uMieScaleHeight", .f = sky->uMieScaleHeight));
    shader2_adduniform(&sky->shader_rayleigh, uniform("float uMiePreferredDirection", .f = sky->uMiePreferredDirection));
    shader2_apply(&sky->shader_rayleigh);

    do_once {
        skybox_rs = renderstate();
        skybox_rs.depth_test_enabled = 1;
        skybox_rs.cull_face_enabled = 0;
        skybox_rs.front_face = GL_CCW;
    }

    mat44 mvp; multiply44x2(mvp, proj, view);

    shader_bind(sky->shader_rayleigh.program);
    shader_mat44("u_mvp", mvp);

    renderstate_apply(&skybox_rs);
    mesh_render(&sky->geometry);
}

void skybox_bakeprobe(skybox_t *probe, skybox_t *sky, int width) {
    cubemap_beginbake(&probe->probe, vec3(0, 0, 0), width, width);
    mat44 proj, view;
    while (cubemap_stepbake(&probe->probe, proj, view)) {
        skybox_render(sky, proj, view);
    }
    cubemap_endbake(&probe->probe, 0);
}

int skybox_render(skybox_t *sky, mat44 proj, mat44 view) {
    if (sky->mode == SKYBOX_PROCEDURAL) {
        skybox_render_rayleigh(sky, proj, view);
        return 0;
    }
    last_cubemap = &sky->cubemap;

    do_once {
        skybox_rs = renderstate();
        skybox_rs.depth_test_enabled = 1;
        skybox_rs.cull_face_enabled = 0;
        skybox_rs.front_face = GL_CCW;
        skybox_rs.depth_func = GL_LEQUAL;
        skybox_rs.reverse_z = 0;
    }

    mat44 mvp;
    multiply44x2(mvp, proj, view);

    shader_bind(sky->mode == SKYBOX_CUBEMAP ? sky->shader_cubemap.program : sky->shader_panorama.program);
    shader_mat44("u_mvp", mvp);

    if (sky->mode == SKYBOX_CUBEMAP)
       shader_cubemap("u_skybox", sky->cubemap.id, 0);
    else
       shader_texture("u_skybox", sky->cubemap.id, 0);

    renderstate_apply(&skybox_rs);
    mesh_render(&sky->geometry);
    return 0;
}
void skybox_destroy(skybox_t *sky) {
    shader2_destroy(&sky->shader_panorama);
    shader2_destroy(&sky->shader_cubemap);
    shader2_destroy(&sky->shader_rayleigh);
    cubemap_destroy(&sky->cubemap);
    cubemap_destroy(&sky->probe);
    mesh_destroy(&sky->geometry);
}

void ui_skybox(skybox_t *sky) {
    const char *modes[] = {
        "Panorama", "Procedural", "Cubemap"
    };

    const char *rates[] = {
        "Manual", "Every Frame"
    };

    ui_list("Mode", &sky->mode, countof(modes), modes);
    ui_list("Update Rate", &sky->update_mode, countof(rates), rates);
    
    ui_enable(sky->mode != SKYBOX_CUBEMAP);
    if (ui_button("Bake Cubemap")) {
        skybox_bakeprobe(sky, sky, sky->probe_size);
    }
    ui_enable(1);

    ui_separator("Procedural coefficients");
    ui_enable(sky->mode == SKYBOX_PROCEDURAL);
    ui_float3("Sun Position", &sky->uSunPos.x);
    ui_float3("Ray Origin", &sky->uRayOrigin.x);
    ui_float("Sun Intensity", &sky->uSunIntensity);
    ui_float("Planet Radius", &sky->uPlanetRadius);
    ui_float("Atmosphere Radius", &sky->uAtmosphereRadius);
    ui_float3("Rayleigh Scattering", &sky->uRayleighScattering.x);
    ui_float("Mie Scattering", &sky->uMieScattering);
    ui_float("Rayleigh Scale Height", &sky->uRayleighScaleHeight);
    ui_float("Mie Scale Height", &sky->uMieScaleHeight);
    ui_float("Mie Preferred Direction", &sky->uMiePreferredDirection);
    ui_enable(1);
}

#endif
