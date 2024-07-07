typedef unsigned handle; // GLuint

// -----------------------------------------------------------------------------
// renderstate
typedef struct renderstate_t {
    // Clear color
    float clear_color[4];

    // Color mask
    bool color_mask[4];

    // Clear depth
    double clear_depth;

    // Depth test
    bool depth_test_enabled;
    bool depth_write_enabled;
    unsigned depth_func;
    bool reverse_z;
    
    // Polygon offset
    bool polygon_offset_enabled;
    float polygon_offset;
    float polygon_offset_factor;

    // Blending
    bool blend_enabled;
    unsigned blend_func;
    unsigned blend_src;
    unsigned blend_dst;

    // Culling
    bool cull_face_enabled;
    unsigned cull_face_mode;

    // Stencil test
    bool stencil_test_enabled;
    unsigned stencil_func;
    unsigned stencil_op_fail, stencil_op_zfail, stencil_op_zpass;
    int stencil_ref;
    unsigned stencil_read_mask;
    unsigned stencil_write_mask;

    // Face culling direction
    unsigned front_face; // GL_CW or GL_CCW

    // Line width
    bool line_smooth_enabled;
    float line_width;

    // Point size
    bool point_size_enabled;
    float point_size;

    // Polygon mode
    unsigned polygon_mode_face;
    unsigned polygon_mode_draw;

    // Scissor test
    bool scissor_test_enabled;

    // Seamless cubemap
    bool seamless_cubemap;

    // Depth clamp
    bool depth_clamp_enabled;
} renderstate_t;

API renderstate_t renderstate();
API bool            renderstate_compare(const renderstate_t *stateA, const renderstate_t *stateB);
API uint32_t        renderstate_checksum(const renderstate_t *state);
API void            renderstate_apply(const renderstate_t *state);
