#ifndef SHADOWMAP_GLSL
#define SHADOWMAP_GLSL

#ifndef NO_SHADOWS

#include "utils.glsl"

uniform bool u_shadow_receiver;
uniform float u_cascade_distances[NUM_SHADOW_CASCADES];
uniform samplerCube shadowMap[MAX_SHADOW_LIGHTS];
uniform sampler2D shadowMap2D[NUM_SHADOW_CASCADES];
uniform sampler3D shadow_offsets;
uniform int shadow_filter_size;
uniform int shadow_window_size;
uniform mat4 light_shadow_matrix_csm[NUM_SHADOW_CASCADES];
uniform float u_blend_region; /// set:0.0
uniform int u_max_shadow_cascades; /// set:0

vec2 pcf_offsets[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

//< only used for debugging, make sure NUM_SHADOW_CASCADES is set to 4
#if USE_CSM_DEBUG
const vec3 cascade_debug_colors[NUM_SHADOW_CASCADES] = vec3[NUM_SHADOW_CASCADES](
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1),
    vec3(1,1,0)
);
#endif

vec2 shadow_vsm_variance(vec3 dir, int light_index, float distance, float min_variance, float variance_transition, out float alpha) {
    // Calculate the variance
    vec3 sampledValue = texture(shadowMap[light_index], dir).rgb;
    vec2 moments = sampledValue.rg;
    alpha = 1.0;//sampledValue.b;
    float variance = max(moments.y - (moments.x * moments.x), min_variance);
    float d = distance - moments.x;
    return (vec2(linstep(variance_transition, 1.0, variance / (variance + d * d)), moments.x));
}

vec3 shadow_vsm(float distance, vec3 dir, int light_index, float min_variance, float variance_transition, float shadow_softness_raw, float penumbra_size, bool hard_shadows) {
    float clamped_distance = clamp(distance, 0.0, 200.0);
    float shadow_softness = shadow_softness_raw * 50.0;
    shadow_softness = mix(shadow_softness, distance * 10.0, penumbra_size);
    distance = distance / 200;
    
    // Get the offset coordinates
    ivec3 ofs_coord = ivec3(0);
    vec2 ofs = mod(gl_FragCoord.xy, vec2(shadow_window_size));
    ofs_coord.yz = ivec2(ofs);
    float ofs_sum = 0.0;
    int samples_div2 = int(shadow_filter_size * shadow_filter_size / 2.0);
    vec4 sc = vec4(dir, 1.0);
    sc.z = dir.z;

    vec2 tex_size = textureSize(shadowMap[light_index], 0);
    vec3 texelSize = 1.0 / vec3(tex_size.xyy);
    vec3 light_plane_normal = normalize(dir);
    vec3 up_axis = normalize(view[1].xyz);
    vec3 tangent = normalize(cross(light_plane_normal, up_axis));
    vec3 bitangent = cross(light_plane_normal, tangent);
    float alpha;

    if (hard_shadows) {
        // Sample the shadowmap directly
#if USE_SHADOW_SOFTNESS
        ofs_coord.x = 0;
        vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;
        vec3 offset_dir = tangent * offsets.r + bitangent * offsets.g;
        sc.xyz = dir.xyz + offset_dir * texelSize;
        vec2 variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance * alpha, variance.y), variance.x), 1.0);

        offset_dir = tangent * offsets.b + bitangent * offsets.a;
        sc.xyz = dir.xyz + offset_dir * texelSize;
        variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);
        return vec3(ofs_sum / 2.0);
#else
        vec2 variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance * alpha, variance.y), variance.x), 1.0);
        return vec3(ofs_sum);
#endif
    }

#if USE_SHADOW_SOFTNESS
    for (int i = 0; i < 4; i++) {
        ofs_coord.x = i;
        vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;

        vec3 offset_dir = tangent * offsets.r + bitangent * offsets.g;
        sc.xyz = dir.xyz + offset_dir * texelSize;
        vec2 variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);

        offset_dir = tangent * offsets.b + bitangent * offsets.a;
        sc.xyz = dir.xyz + offset_dir * texelSize;
        variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);
    }
#else
    for (int i = 0; i < 4; i++) {
        vec3 pos = sc.xyz;
        pos += vec3(pcf_offsets[i] * texelSize.xy, 0.0);
        vec2 variance = shadow_vsm_variance(pos, light_index, distance, min_variance, variance_transition, alpha);
        ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);
    }
#endif

#if USE_SHADOW_SOFTNESS
    float shadow_sum = ofs_sum / 8.0;

    if (shadow_sum != 0.0 && shadow_sum != 1.0) {
        for (int i = 4; i < samples_div2; i++) {
            ofs_coord.x = i;
            vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;

            vec3 offset_dir = tangent * offsets.r + bitangent * offsets.g;
            sc.xyz = dir.xyz + offset_dir * texelSize;
            vec2 variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
            ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);

            offset_dir = tangent * offsets.b + bitangent * offsets.a;
            sc.xyz = dir.xyz + offset_dir * texelSize;
            variance = shadow_vsm_variance(sc.xyz, light_index, distance, min_variance, variance_transition, alpha);
            ofs_sum += min(max(step(distance*alpha, variance.y), variance.x), 1.0);
        }

        shadow_sum = ofs_sum / (samples_div2 * 2.0);
    }
#else
    float shadow_sum = ofs_sum / 4.0;
#endif

    return vec3(shadow_sum);
}

vec3 shadow_csm(float distance, vec3 lightDir, int light_index, float shadow_bias, float normal_bias, float shadow_softness, bool hard_shadows) {
    // Determine which cascade to use
    int cascade_index = -1;
    int max_cascades = min(NUM_SHADOW_CASCADES, u_max_shadow_cascades);
    if (u_max_shadow_cascades == 0) {
        max_cascades = NUM_SHADOW_CASCADES;
    }
    for (int i = 0; i < max_cascades; i++) {
        if (distance < u_cascade_distances[i]) {
            cascade_index = i;
            break;
        }
    }
    if (cascade_index == -1) {
        cascade_index = max_cascades - 1;
    }
    
    int matrix_offset = 0;
    float blend_factor = 0.0;

#if USE_BLEND_REGIONS
    // Blend cascades using a blend region value
    if (u_blend_region > 0.0)
    {
        float cascade_distance = u_cascade_distances[cascade_index];
        if (distance > cascade_distance - u_blend_region) {
            blend_factor = (distance - (cascade_distance - u_blend_region)) / u_blend_region;
            blend_factor = clamp(blend_factor, 0.0, 1.0);
            matrix_offset = 1;
        }
    }
#endif

#if USE_CSM_DEBUG
    return mix(cascade_debug_colors[cascade_index], cascade_debug_colors[cascade_index + matrix_offset], blend_factor);
#endif

    light_t light = lightBuffer.lights[light_index];

    vec4 fragPosLightSpace = light_shadow_matrix_csm[cascade_index] * vec4(v_position_ws, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
#if USE_BLEND_REGIONS
    vec4 fragPosLightSpace2 = light_shadow_matrix_csm[cascade_index+matrix_offset] * vec4(v_position_ws, 1.0);
    vec3 projCoords2 = fragPosLightSpace2.xyz / fragPosLightSpace2.w;
#endif

    projCoords = projCoords * 0.5 + 0.5;
    vec4 sc = vec4(projCoords, 1.0);
    float currentDepth = projCoords.z;
#if USE_BLEND_REGIONS
    projCoords2 = projCoords2 * 0.5 + 0.5;
    vec4 sc2 = vec4(projCoords2, 1.0);
    float currentDepth2 = projCoords2.z;
#endif

    if (currentDepth > 1.0) {
        return vec3(1.0);
    }

    // Calculate bias
    vec3 normal = normalize(v_normal_ws);
    float slopedBias = normal_bias * max(0.0, dot(normal, -lightDir));
    float bias = slopedBias + shadow_bias;
    bias *= 1.0f / (u_cascade_distances[cascade_index] * 0.01f);
    bias = min(bias, shadow_bias);
    sc.xyz += normal * bias;
#if USE_BLEND_REGIONS
    float bias2 = bias;
    bias2 *= 1.0f / (u_cascade_distances[cascade_index+matrix_offset] * 0.01f);
    bias2 = min(bias2, shadow_bias);
    sc2.xyz += normal * bias2;
#endif
 
    // CSM
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap2D[cascade_index], 0);
#if USE_BLEND_REGIONS
    vec2 texelSize2 = vec2(1,1);
    if (cascade_index < max_cascades - 1) {
        texelSize2 = 1.0 / textureSize(shadowMap2D[cascade_index + matrix_offset], 0);
    }
#endif

    // Get the offset coordinates
    ivec3 ofs_coord = ivec3(0);
    vec2 ofs = mod(gl_FragCoord.xy, vec2(shadow_window_size));
    ofs_coord.yz = ivec2(ofs);
    float ofs_sum = 0.0;
    int samples_div2 = int(shadow_filter_size * shadow_filter_size / 2.0);

    if (hard_shadows) {
#if USE_SHADOW_SOFTNESS
        ofs_coord.x = 0;
        vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;
#else
        vec4 offsets = vec4(0.0);
#endif

        sc.xy = projCoords.xy + offsets.rg * texelSize;
        float csmDepth = texture(shadowMap2D[cascade_index], sc.xy).r;
#if USE_BLEND_REGIONS
        float csmDepth2 = 1;
        sc2.xy = projCoords2.xy + offsets.rg * texelSize2;
        if (cascade_index != max_cascades - 1) {
            csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], sc2.xy).r;
        }
        return vec3(mix(currentDepth - bias > csmDepth ? 0.0 : 1.0, currentDepth2 - bias2 > csmDepth2 ? 0.0 : 1.0, blend_factor));
#else
        return vec3(currentDepth - bias > csmDepth ? 0.0 : 1.0);
#endif
    }

#if USE_SHADOW_SOFTNESS
    for (int i = 0; i < 4; i++) {
        ofs_coord.x = i;
        vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;

        sc.xy = projCoords.xy + offsets.rg * texelSize;
        float alpha=1.0;
        float csmDepth = texture(shadowMap2D[cascade_index], sc.xy).r;
#if USE_BLEND_REGIONS
        float csmDepth2 = 1;
        sc2.xy = projCoords2.xy + offsets.rg * texelSize2;
        if (cascade_index != max_cascades - 1) {
            csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], sc2.xy).r;
        }
        ofs_sum += mix(currentDepth - bias > csmDepth ? alpha : 0.0, currentDepth2 - bias2 > csmDepth2 ? alpha : 0.0, blend_factor);
#else
        ofs_sum += currentDepth - bias > csmDepth ? alpha : 0.0;
#endif

        sc.xy = projCoords.xy + offsets.ba * texelSize;
        csmDepth = texture(shadowMap2D[cascade_index], sc.xy).r;
#if USE_BLEND_REGIONS
        csmDepth2 = 1;
        sc2.xy = projCoords2.xy + offsets.ba * texelSize2;
        if (cascade_index != max_cascades - 1) {
            csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], sc2.xy).r;
        }
        ofs_sum += mix(currentDepth - bias > csmDepth ? alpha : 0.0, currentDepth2 - bias2 > csmDepth2 ? alpha : 0.0, blend_factor);
#else
        ofs_sum += currentDepth - bias > csmDepth ? alpha : 0.0;
#endif
    }
#else
    float alpha=1.0;
    for (int i = 0; i < 4; i++) {
        vec3 pos = sc.xyz;
        pos += vec3(pcf_offsets[i] * texelSize.xy, 0.0);
        float csmDepth = texture(shadowMap2D[cascade_index], pos.xy).r;
#if USE_BLEND_REGIONS
        float csmDepth2 = 1;
        vec3 pos2 = sc2.xyz;
        pos2 += vec3(pcf_offsets[i] * texelSize2.xy, 0.0);
        if (cascade_index != max_cascades - 1) {
            csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], pos2.xy).r;
        }
        ofs_sum += mix(currentDepth - bias > csmDepth ? alpha : 0.0, currentDepth2 - bias2 > csmDepth2 ? alpha : 0.0, blend_factor);
#else
        ofs_sum += currentDepth - bias > csmDepth ? alpha : 0.0;
#endif
    }
#endif

#if USE_SHADOW_SOFTNESS
    float shadow_sum = ofs_sum / 8.0;

    if (shadow_sum != 0.0 && shadow_sum != 1.0) {
        for (int i = 4; i < samples_div2; i++) {
            ofs_coord.x = i;
            vec4 offsets = texelFetch(shadow_offsets, ofs_coord, 0) * shadow_softness;

            sc.xy = projCoords.xy + offsets.rg * texelSize;
            float alpha=1.0;
            float csmDepth = texture(shadowMap2D[cascade_index], sc.xy).r;
#if USE_BLEND_REGIONS
            float csmDepth2 = 1;
            sc2.xy = projCoords2.xy + offsets.rg * texelSize2;
            if (cascade_index != max_cascades - 1) {
                csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], sc2.xy).r;
            }
            ofs_sum += mix(currentDepth - bias > csmDepth ? alpha : 0.0, currentDepth2 - bias2 > csmDepth2 ? alpha : 0.0, blend_factor);
#else
            ofs_sum += currentDepth - bias > csmDepth ? alpha : 0.0;
#endif

            sc.xy = projCoords.xy + offsets.ba * texelSize;
            csmDepth = texture(shadowMap2D[cascade_index], sc.xy).r;
#if USE_BLEND_REGIONS
            csmDepth2 = 1;
            sc2.xy = projCoords2.xy + offsets.ba * texelSize2;
            if (cascade_index != max_cascades - 1) {
                csmDepth2 = texture(shadowMap2D[cascade_index + matrix_offset], sc2.xy).r;
            }
            ofs_sum += mix(currentDepth - bias > csmDepth ? alpha : 0.0, currentDepth2 - bias2 > csmDepth2 ? alpha : 0.0, blend_factor);
#else
            ofs_sum += currentDepth - bias > csmDepth ? alpha : 0.0;
#endif
        }

        shadow_sum = ofs_sum / (samples_div2 * 2.0);
    }
#else
    float shadow_sum = ofs_sum / 4.0;
#endif

    return vec3(1.0 - shadow_sum);
}

vec4 shadowmap(int idx, in vec4 peye, in vec4 neye) {
    vec3 fragment = vec3(peye);
    vec3 shadowFactor = vec3(1.0);
    light_t light = lightBuffer.lights[idx];

    if (light.processed_shadows) {
        if (light.type == LIGHT_DIRECTIONAL) {
            shadowFactor = shadow_csm(-peye.z, light.dir.xyz, idx, light.shadow_bias, light.normal_bias, light.shadow_softness, light.hard_shadows);
        } else if (light.type == LIGHT_POINT || light.type == LIGHT_SPOT) {
            vec3 light_pos = (view * vec4(light.pos.xyz, 1.0)).xyz;
            vec3 dir = light_pos - fragment;
            vec4 sc = inv_view * vec4(dir, 0.0);
            shadowFactor = shadow_vsm(length(dir), -sc.xyz, idx, light.min_variance, light.variance_transition, light.shadow_softness, light.penumbra_size, light.hard_shadows);
        }
    }

    return vec4(shadowFactor, 1.0);
}

vec4 shadowing(int idx) {
    if (u_shadow_receiver) {
        return shadowmap(idx, vpeye, vneye);
    } else {
        return vec4(1.0);
    }
}

#else

vec4 shadowing(int idx) {
    return vec4(1.0);
}

#endif

#endif

