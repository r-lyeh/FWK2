#ifndef MODEL_FS_GLSL
#define MODEL_FS_GLSL
#define FS_PASS

struct ColorMap
{
    bool has_tex;
    vec4 color;
};

#ifdef FS_PASS
uniform ColorMap map_albedo;    uniform sampler2D map_albedo_tex;
uniform ColorMap map_normals;   uniform sampler2D map_normals_tex;
uniform ColorMap map_roughness; uniform sampler2D map_roughness_tex;
uniform ColorMap map_metallic;  uniform sampler2D map_metallic_tex;
uniform ColorMap map_ao;        uniform sampler2D map_ao_tex;
uniform ColorMap map_ambient;   uniform sampler2D map_ambient_tex;
uniform ColorMap map_emissive;  uniform sampler2D map_emissive_tex;
uniform ColorMap map_parallax;  uniform sampler2D map_parallax_tex;
#endif

#define sample_colormap(ColorMap_, uv_) \
    (ColorMap_.has_tex ? texture( ColorMap_##_tex, uv_ ) : ColorMap_.color)

// Material data
uniform float u_ssr_strength;
uniform vec3 u_base_reflectivity;
uniform float u_cutout_alpha; /// set:0.75
uniform float u_emissive_value; /// set:1.0
uniform bool u_parallax_clip; /// set:0
uniform bool u_enable_shading; /// set:1
uniform bool u_enable_ibl; /// set:1
uniform float u_global_alpha; /// set:1.0
uniform float u_global_opacity; /// set:1.0
uniform vec4 object_id;

// Input Attributes
in vec3 v_position;
in vec3 v_position_ws;
in vec4 vpeye;
in vec4 vneye;

in vec3 v_normal, v_normal_ws;
in vec2 v_texcoord, v_texcoord2;
in vec4 v_color;
in vec3 v_tangent, v_tangent_ws;
in vec3 v_tangent_view, v_tangent_world;
in vec3 v_binormal;
in vec3 v_to_camera;
in vec3 v_vertcolor;
in float v_depth;
in mat3 v_tbn;
out vec4 fragcolor;

// Global uniforms
uniform vec3 u_cam_pos;
uniform vec3 u_cam_dir;
uniform float frame_time;
uniform uint frame_count;
uniform mat4 view, inv_view;

#endif
