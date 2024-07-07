// -----------------------------------------------------------------------------
// cubemaps

#if !CODE

typedef struct cubemap_t {
    unsigned id;    // texture id

    // bake data
    int framebuffer;
    int depth_texture;
    unsigned width, height;
    int step;
    vec3 pos;
} cubemap_t;

API cubemap_t  cubemap( texture_t texture, int flags ); // 1 equirectangular panorama
API void       cubemap_destroy(cubemap_t *c);
API cubemap_t* cubemap_get_active();
API void       cubemap_beginbake(cubemap_t *c, vec3 pos, unsigned width, unsigned height);
API bool       cubemap_stepbake(cubemap_t *c, mat44 proj /* out */, mat44 view /* out */);
API void       cubemap_endbake(cubemap_t *c, int step /* = 16 */);
#else

cubemap_t cubemap( texture_t texture, int flags ) {
    cubemap_t c = {0};
    c.id = texture.id;
    return c;
}

void cubemap_destroy(cubemap_t *c) {
    glDeleteTextures(1, &c->id);
    c->id = 0; // do not destroy SH coefficients still. they might be useful in the future.

    if (c->framebuffer) {
        glDeleteFramebuffers(1, &c->framebuffer);
        glDeleteTextures(1, &c->id);
        glDeleteTextures(1, &c->depth_texture);
        c->framebuffer = 0;
    }
}

static cubemap_t *last_cubemap;

cubemap_t* cubemap_get_active() {
    return last_cubemap;
}

// cubemap baker

static int sky_last_fb;
static int sky_last_vp[4];
void cubemap_beginbake(cubemap_t *c, vec3 pos, unsigned width, unsigned height) {
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &sky_last_fb);
    glGetIntegerv(GL_VIEWPORT, sky_last_vp);
    c->step = 0;
    c->pos = pos;

    if (c->width != width || c->height != height) {
        c->width = width;
        c->height = height;

        if (c->framebuffer) {
            glDeleteFramebuffers(1, &c->framebuffer);
            glDeleteTextures(1, &c->id);
            glDeleteTextures(1, &c->depth_texture);
            c->framebuffer = 0;
        }
    }

    if (!c->framebuffer) {
        glGenFramebuffers(1, &c->framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, c->framebuffer);
        
        glGenTextures(1, &c->id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, c->id);

        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glGenTextures(1, &c->depth_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, c->depth_texture);
        for (int i = 0; i < 6; i++) {
            // Create a 16-bit depth component texture for each face of the cubemap
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}

bool cubemap_stepbake(cubemap_t *c, mat44 proj /* out */, mat44 view /* out */) {
    if (c->step >= 6) return false;

    static vec3 directions[6] = {{ 1, 0, 0},{-1, 0, 0},{ 0, 1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1}};
    static vec3 up_vectors[6] = {{ 0,-1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1},{ 0,-1, 0},{ 0,-1, 0}};

    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, c->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + c->step, c->id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + c->step, c->depth_texture, 0);
    glClearColor(0, 0, 0, 1);
    glClearDepth(gl_reversez ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, c->width, c->height);

    perspective44(proj, 90.0f, c->width / (float)c->height, 0.1f, 1000.f);
    lookat44(view, c->pos, add3(c->pos, directions[c->step]), up_vectors[c->step]);
    ++c->step;

    return true;
}

void cubemap_endbake(cubemap_t *c, int step) {
    if (!step) {
        step = 16;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, c->id); 
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, sky_last_fb);
    glViewport(sky_last_vp[0], sky_last_vp[1], sky_last_vp[2], sky_last_vp[3]);
}


#endif
