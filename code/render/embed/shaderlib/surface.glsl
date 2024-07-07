#ifndef SURFACE_GLSL
#define SURFACE_GLSL

#include "light.glsl"
#include "parallax.glsl"

struct surface_t {
    vec3 normal;
    vec4 albedo;
    vec4 fragcolor;
    vec3 light_direct;
    vec3 light_indirect;
    vec3 emissive;
    float roughness;
    float metallic;
    float ao;
    float alpha;
    material_t material;
};

vec4 get_diffuse_map() {
    vec4 result;
    result = sample_colormap(map_albedo, v_texcoord);
    result.a *= u_global_alpha*u_global_opacity;
    return result;
}

surface_t surface() {
    surface_t s;

    s.normal = normalize(v_normal_ws);
    s.light_direct = vec3(0.0, 0.0, 0.0);
    s.light_indirect = vec3(0.0, 0.0, 0.0);
    s.albedo = vec4(0.5, 0.5, 0.5, 1.0);
    s.emissive = vec3(0.0, 0.0, 0.0);
    s.roughness = 1.0;
    s.metallic = 0.0;
    s.ao = 1.0;
    s.alpha = 1.0;

    vec2 uvs = v_texcoord;
    
    vec3 tangent_view_dir = normalize(v_tangent_view - v_tangent_world);
#if USE_PARALLAX
    if (map_parallax.has_tex) {
        uvs = parallax(uvs, tangent_view_dir);
        if(u_parallax_clip)
            if(uvs.x > 1.0 || uvs.y > 1.0 || uvs.x < 0.0 || uvs.y < 0.0)
                discard;
    }
#endif

    vec4 baseColor_alpha = map_albedo.color;
#if USE_ALBEDO
    baseColor_alpha = sample_colormap( map_albedo, uvs );
#endif
    s.albedo = baseColor_alpha;

    if (!map_albedo.has_tex) {
        s.albedo.rgb = pow(s.albedo.rgb, vec3(2.2));
    }


    if( map_metallic.has_tex && map_roughness.has_tex ) {
#if USE_METALLIC
        s.metallic = sample_colormap( map_metallic, uvs ).x;
#endif
#if USE_ROUGHNESS
        s.roughness = sample_colormap( map_roughness, uvs ).x;
#endif
    }
    else if( map_roughness.has_tex ) {
#if USE_ROUGHNESS
        s.metallic = sample_colormap( map_roughness, uvs ).b;
        s.roughness = sample_colormap( map_roughness, uvs ).g;
#endif
    } else {
        // rely on color specifically
        s.metallic = map_metallic.color.x;
        s.roughness = map_roughness.color.x;
    }

    s.ao = map_ao.color.x;
#if USE_AO
    s.ao = sample_colormap( map_ao, uvs ).x;
#endif

    s.emissive = map_emissive.color.rgb;
#if USE_EMISSIVE
    s.emissive = sample_colormap( map_emissive, uvs ).rgb;
#endif
    if (!map_emissive.has_tex) {
        s.emissive.rgb = pow(s.emissive.rgb, vec3(2.2));
    }
    s.emissive *= u_emissive_value;

    s.normal = v_normal_ws;

#if USE_NORMALS
    if ( map_normals.has_tex )
    {
        // Mikkelsen's tangent space normal map decoding. See http://mikktspace.com/ for rationale.
        vec3 normalmap = texture( map_normals_tex, uvs ).xyz * vec3(2.0) - vec3(1.0);
        float normalmap_mip = textureQueryLod( map_normals_tex, uvs ).x;
        float normalmap_length = length(normalmap);
        normalmap /= normalmap_length;

        vec3 bi = cross( v_normal_ws, v_tangent );
        vec3 nmap = normalmap.xyz;
        s.normal = normalize( nmap.x * v_tangent + nmap.y * bi + nmap.z * v_normal_ws );

        if (USE_NORMAL_VARIATION_TO_ROUGHNESS)
        {
            // Try to reduce specular aliasing by increasing roughness when minified normal maps have high variation.
            float variation = 1. - pow( normalmap_length, 8. );
            float minification = clamp( normalmap_mip - 2., 0., 1. );
            s.roughness = mix( s.roughness, 1.0, variation * minification );
        }
    }
#endif

    vec3 N = s.normal;
    vec3 V = normalize( v_to_camera );

    vec3 Lo = vec3(0.);
    vec3 F0 = u_base_reflectivity;
    F0 = mix( F0, s.albedo.rgb, s.metallic );

    material_t pbr_mat;
    pbr_mat.albedo = s.albedo.rgb;
    pbr_mat.normal = N;
    pbr_mat.F0 = F0;
    pbr_mat.roughness = s.roughness;
    pbr_mat.metallic = s.metallic;
    pbr_mat.alpha = s.alpha;
    pbr_mat.tangent_view_dir = tangent_view_dir;
    pbr_mat.texcoords = uvs;

    s.material = pbr_mat;
    Lo += lighting(pbr_mat);
    s.light_direct = Lo;

#if USE_AMBIENT
    if ( map_ambient.has_tex )
        s.light_indirect = sample_colormap( map_ambient, uvs ).xyz;
#endif

    vec3 diffuse_ambient;
    vec3 specular_ambient;

#if USE_IBL
    if (u_enable_ibl) {
        vec3 irradiance = sample_irradiance_fast( N, v_tangent );
        vec3 F = fresnel_schlick_roughness( N, V, F0, s.roughness );
        vec3 kS = F;
        vec3 kD = vec3(1.) - kS;
        kD *= 1.0 - s.metallic;
        kD *= s.alpha;
        diffuse_ambient = irradiance * s.albedo.rgb;
        specular_ambient = specular_ibl( V, N, s.roughness, F, s.metallic );
#if _USE_SPECULAR_AO_ATTENUATION
        s.light_indirect = s.ao * (kD * diffuse_ambient + specular_ambient);
#else
        s.light_indirect = s.ao * (kD * diffuse_ambient) + specular_ambient;
#endif
    }
#endif

    s.albedo *= v_color;

    s.fragcolor = s.albedo;
    s.fragcolor.rgb *= s.light_direct + s.light_indirect;
    s.fragcolor.rgb += s.emissive;
    s.fragcolor.a *= u_global_alpha;
    s.fragcolor *= vec4(u_global_opacity);

    // gamma correction
    vec3 color = s.fragcolor.rgb;
    color = pow( color, vec3(1. / 2.2) );
    s.fragcolor.rgb = color;
    
    return s;
}

#endif
