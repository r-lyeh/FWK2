#include "model_fs.glsl"
#include "surface.glsl"
#include "fog.glsl"

void do_fullbright_pass(vec4 albedo) {
    fragcolor = albedo;
    fragcolor.rgb = pow(fragcolor.rgb, vec3(1.0/2.2));
}

void main() {
    vec4 albedo = get_diffuse_map();
    if (albedo.a < u_cutout_alpha) discard;
#if USE_SHADING
    if (u_enable_shading) {
        surface_t surf = surface();
        fragcolor = surf.fragcolor;
    } else {
        do_fullbright_pass(albedo);
    }
#else
    do_fullbright_pass(albedo);
#endif

    fragcolor.rgb = do_fog(fragcolor.rgb);
}
