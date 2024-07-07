// find 2D coordinate <-> 3D point in current camera view
API vec2 editor_xy(vec3 xyz);
API vec3 editor_xyz(vec2 xy); // formerly editor_pick()

#if CODE

vec2 editor_xy(vec3 p) {
    // clip_position = Projection_Matrix * View_Matrix * Model_Matrix * vertex_position
    // viewport_position.{x,y} = viewport.{x,y} + viewport.{width,height} * (0.5 + clip_position.{x,y} / 2)

    camera_t *cam = camera_get_active();
    p = transform344(cam->view, p);
    p = transform344(cam->proj, p);
    p = add3(scale3(p, 0.5), vec3(0.5,0.5,0.5));
    return vec2( p.x * app_width(), (1-p.y) * app_height());
}

vec3 editor_xyz(vec2 mouse) {
    // unproject 2d coord as 3d coord
    vec2 dpi = ifdef(osx, window_dpi(), vec2(1,1));
    camera_t *camera = camera_get_active();
    float x = (2.0f * mouse.x) / (dpi.x * app_width()) - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / (dpi.y * app_height());
    float z = 1.0f;
    vec3 ray_nds = vec3(x, y, z);
    vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    mat44 inv_proj; invert44(inv_proj, camera->proj);
    mat44 inv_view; invert44(inv_view, camera->view);
    vec4 p = transform444(inv_proj, ray_clip);
    vec4 eye = vec4(p.x, p.y, -1.0, 0.0);
    vec4 wld = norm4(transform444(inv_view, eye));
    return vec3(wld.x, wld.y, wld.z);
}

#endif
