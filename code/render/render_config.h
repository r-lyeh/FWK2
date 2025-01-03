// -----------------------------------------------------------------------------
// render config directives

#ifndef GLOBAL_FRUSTUM_ENABLED
#define GLOBAL_FRUSTUM_ENABLED 1 ///+
#endif

#ifndef GLOBAL_FRUSTUM_DEBUG_DRAW
#define GLOBAL_FRUSTUM_DEBUG_DRAW 0 ///+
#endif

#ifndef GLOBAL_FRUSTUM_FOV_MULTIPLIER
#define GLOBAL_FRUSTUM_FOV_MULTIPLIER 1.0f ///+
#endif

#ifndef GLOBAL_FX_PASS_ENABLED
#define GLOBAL_FX_PASS_ENABLED 0 ///+
#endif

#ifndef ENABLE_REVERSE_Z
#define ENABLE_REVERSE_Z 0 //ifdef(ems, 0, 1) ///+
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 96
#endif

#ifndef MAX_SHADOW_LIGHTS
#define MAX_SHADOW_LIGHTS 8
#endif

#ifndef NUM_SHADOW_CASCADES
#define NUM_SHADOW_CASCADES 4
#endif

#ifndef SHADOW_CASCADE_BLEND_REGION_OFFSET
#define SHADOW_CASCADE_BLEND_REGION_OFFSET 0.2f
#endif

#ifndef SHADOW_CASCADE_BLEND_REGION
#define SHADOW_CASCADE_BLEND_REGION 0.0f
#endif

#ifndef DEFAULT_SHADOW_VSM_RESOLUTION
#define DEFAULT_SHADOW_VSM_RESOLUTION 256
#endif

#ifndef DEFAULT_SHADOW_CSM_RESOLUTION
#define DEFAULT_SHADOW_CSM_RESOLUTION 2048
#endif

#ifndef ENABLE_CSM_DOWNSCALING
#define ENABLE_CSM_DOWNSCALING 0 ///+
#endif

#ifndef ENABLE_CSM_HALF_PRECISION
#define ENABLE_CSM_HALF_PRECISION 0 ///+
#endif

#ifndef ENABLE_SHADOW_BILINEAR_FILTERING
#define ENABLE_SHADOW_BILINEAR_FILTERING 0 ///+
#endif

#ifndef ENABLE_CSM_LAST_CASCADE_USES_FAR_PLANE
#define ENABLE_CSM_LAST_CASCADE_USES_FAR_PLANE 1 ///+
#endif
