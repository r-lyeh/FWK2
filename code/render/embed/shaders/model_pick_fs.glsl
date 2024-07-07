#include "model_fs.glsl"
#include "surface.glsl"

void main() {
    vec4 albedo = get_diffuse_map();
    if (albedo.a < u_cutout_alpha) discard;

    fragcolor = object_id;
}
