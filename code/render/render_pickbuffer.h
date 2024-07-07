// Screenspace object picker

#if !CODE

typedef struct pickbuffer_t {
    fbo_t fb;
} pickbuffer_t;

typedef set(unsigned) pickset_t;

API pickbuffer_t pickbuffer();
API void           pickbuffer_clear(pickbuffer_t *pick);
API void         pickbuffer_destroy(pickbuffer_t *pick);

API pickset_t pickbuffer_pick(pickbuffer_t *pick, vec2 from, vec2 to); //< locally persisting buffer

//

API void pickbuffer_rendermesh(pickbuffer_t *pick, mesh_t *m); // expects prepared shader program
API void pickbuffer_renderquad(pickbuffer_t *pick, vec2 from, vec2 to, unsigned colorid);

#else

pickbuffer_t pickbuffer() {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    pickbuffer_t pick = {0};

    pick.fb = fbo(saved_vp[2], saved_vp[3], 0, TEXTURE_RGBA);

    return pick;
}

void pickbuffer_clear(pickbuffer_t *pick) {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    fbo_resize(&pick->fb, saved_vp[2], saved_vp[3]);

    fbo_bind(pick->fb.id);
        glClearColor(0,0,0,0);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    fbo_unbind();
}

void pickbuffer_destroy(pickbuffer_t *pick) {
    fbo_destroy(pick->fb);
    pickbuffer_t _clear={0};
    *pick = _clear;
}

//

pickset_t pickbuffer_pick(pickbuffer_t *pick, vec2 from, vec2 to) {
#if 1
    vec2 aa = min2(from, to), bb = max2(from, to);
    from = aa, to = bb;
#endif
    unsigned num_pixels = (to.x-from.x)*(to.y-from.y);
    static array(unsigned) pixels; array_resize(pixels, num_pixels); //@leak
    static pickset_t selected = 0; do_once set_init_int(selected); //@leak

    set_clear(selected);

    fbo_bind(pick->fb.id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glReadPixels(from.x, pick->fb.texture_color.h - 1 - to.y, (to.x-from.x), (to.y-from.y), GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)pixels);

        for (int i = 0; i < num_pixels; ++i) {
            set_find_or_add(selected, pixels[i]);
        }
    fbo_unbind();

    return selected;
}

//

void pickbuffer_rendermesh(pickbuffer_t *pick, mesh_t *m) {
    fbo_bind(pick->fb.id); // needed?
        mesh_render(m);
    fbo_unbind(); // needed?
}

void pickbuffer_renderquad(pickbuffer_t *pick, vec2 from, vec2 to, unsigned colorid) {
    quad_render(((texture_t){0}), vec2(0,0), vec2(0,0), colorid, from, to, false);
}

#endif
