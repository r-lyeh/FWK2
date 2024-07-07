// @fixme: single gizmo to handle multiple items. imGuizmo does not support this yet
//
// @todo: shift -> high precision mode (100x smaller value changes)
// @todo: ctrl -> snap mode (Blender uses 5.0 as a default step increment)
// @todo: shift+ctrl -> high precision snap (basically still does 100x smaller movement but rounds the number, so as you rotate it changes like 23, 24, 25, ...)

// transform gizmos

//API int  gizmo(vec3 *pos, vec3 *rot, vec3 *sca); // @fixme
API int  gizmo44(int count, float **transform);
API bool gizmo_active();
API bool gizmo_hovered();
API int  gizmo_mode();

#if CODE

EDITOR_KEY(key_gizmo_precision, "held(SHIFT)"); // x100 smaller increments
EDITOR_KEY(key_gizmo_next_mode, "down(SPACE)");
EDITOR_KEY(key_gizmo_prev_mode, "down(SPACE) & held(SHIFT)");
EDITOR_KEY(key_gizmo_translate, "down(Q)");
EDITOR_KEY(key_gizmo_rotate, "down(W)");
EDITOR_KEY(key_gizmo_scale, "down(E)");
EDITOR_KEY(key_gizmo_bounds, "down(R)");

static int _gizmo_pass;
static int _gizmo_view_hovered;
static int _gizmo_view_active;

bool gizmo_active() {
    return ImGuizmo_IsUsingAny() || ImGuizmo_IsUsingViewManipulate() || _gizmo_view_active;
}
bool gizmo_hovered() {
    return ImGuizmo_IsOver_Nil() || _gizmo_view_hovered;
}

struct gizmo_t {
    int manipulator; // 0:translate, 1:rotate, 2:scale, 3:bounds
    bool useCubeView;
    bool world; // world or local mode
    bool useSnap;
    vec3 snap; // units for translation, rotation, scale
    bool fullscreen;
    float camDistance;
} gizmo_settings = { 0, true, false, true, {1.0, 5.0, 0.1}, true, 8.f };

void TransformEdit(float* cameraView, const float* cameraProjection, float* matrix) {
#if 0
    ImVec2 sz = igGetIO()->DisplaySize;
    ImGuizmo_SetRect(0, 0, sz.x, sz.y);
#else
    ImVec2 pos = igGetMainViewport()->Pos, sz = igGetMainViewport()->Size; 
    ImGuizmo_SetRect(pos.x, pos.y, sz.x, sz.y);
#endif

    struct gizmo_t *g = &gizmo_settings;

    vec3 snap = g->snap;
    if( g->manipulator == 0 ) snap.y = snap.z = snap.x;
    if( g->manipulator == 1 ) snap.x = snap.y;
    if( g->manipulator == 0 ) snap.x = snap.z;
    vec3 precision = scale3(snap, 0.01f);
    const float *snap_ptr = binding(key_gizmo_precision) ? &precision.x : g->useSnap ? &snap.x : NULL;

    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;
    static float *deltaMatrix = NULL;
    float *bounds_ptr = g->manipulator == 3 ? bounds : NULL;
    float *boundsSnap_ptr = g->manipulator == 3 ? boundsSnap : NULL;

    int wmodes[] = { ImGuizmo_LOCAL, ImGuizmo_WORLD };
    int mmodes[] = { ImGuizmo_TRANSLATE, ImGuizmo_ROTATE, ImGuizmo_SCALE, ImGuizmo_BOUNDS };

    ImGuizmo_Manipulate(cameraView, cameraProjection, 
        mmodes[g->manipulator], wmodes[g->world], matrix, 
        deltaMatrix, snap_ptr, bounds_ptr, boundsSnap_ptr);
}

void TransformView(float* cameraView, float *extra, const float* cameraProjection, int count, float *matrix)
{
    struct gizmo_t *g = &gizmo_settings;

    const char *tooltip_0 = "Enable snap";
    const char *tooltip_1 = g->manipulator == 0 ? "Distance units" : g->manipulator == 1 ? "Degrees" : "Scale units";
    const char *tooltip_2 = "Cube view or scalars";
    const char *tooltip_3 = "Local or world mode";
    const char *tooltip_4 = va("Translation mode (%s)", key_gizmo_translate);
    const char *tooltip_5 = va("Rotation mode (%s)", key_gizmo_rotate);
    const char *tooltip_6 = va("Scale mode (%s)", key_gizmo_scale);
    const char *tooltip_7 = va("Bounds mode (%s)", key_gizmo_bounds);

    ImVec4 on  = *igGetStyleColorVec4(ImGuiCol_ButtonActive);
    ImVec4 off = *igGetStyleColorVec4(ImGuiCol_Button);

    if( g->useCubeView ) {
        ImGuizmo_SetRect(0, 0, igGetIO()->DisplaySize.x, igGetIO()->DisplaySize.y);

        #if 1
        static int location = 1<<1|1<<0; // bottom-right
        ImGuiIO* io = igGetIO();
        ImGuiViewport* viewport = igGetMainViewport();
        const float PAD = 0.0f;
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_area = viewport->WorkSize;
        ImVec2 window_pos;
        window_pos.x = (location & 1) ? (work_pos.x + work_area.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_area.y - PAD) : (work_pos.y + PAD);
        #endif

        enum { _128 = 96 };
        float viewManipulateX = window_pos.x + 10;
        float viewManipulateY = window_pos.y - 10 - _128;

        float length = g->camDistance;
        ImVec2 size = ImVec2(_128, _128);
        ImVec2 position = ImVec2(viewManipulateX - _128, viewManipulateY);
        unsigned bgcolor = 0x10101010;

        ImGuizmo_ViewManipulate_Float(cameraView, length, position, size, bgcolor, extra);

        mouse_t m = mouse();
        if( m.x >= position.x && m.y >= position.y )
            if( m.x < (position.x+size.x) && m.y < (position.y+size.y) )
                _gizmo_view_hovered |= 1, _gizmo_view_active |= m.l;

    } else {

#if 1

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo_DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);

        igSetNextItemWidth(-60);
        igDragFloat3("Position", matrixTranslation, 0.005f, 0, 0, "%.3f", 0);
        igSetNextItemWidth(-60);
        igDragFloat3("Rotation", matrixRotation, 0.005f, 0, 0, "%.3f", 0);
        //igSliderFloat3("Rotation", matrixRotation, -360, 360, "%.3f", 0);
        igSetNextItemWidth(-60);
        igDragFloat3("Scale", matrixScale, 0.005f, 0, 0, "%.3f", 0);

        ImGuizmo_RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

#else
        static quat rot; do_once rot = eulerq(vec3(0,0,0));
        //quat rot = mat44q(matrix);
        vec3 deg = euler(rot);

        // extract position: ok
        vec3 pos = ptr3(matrix + 3*4);
        // extract scaling: ok
        vec3 col1 = vec3(matrix[0*4+0], matrix[0*4+1], matrix[0*4+2]);
        vec3 col2 = vec3(matrix[1*4+0], matrix[1*4+1], matrix[1*4+2]);
        vec3 col3 = vec3(matrix[2*4+0], matrix[2*4+1], matrix[2*4+2]);
        vec3 sca = vec3( len3(col1), len3(col2), len3(col3) );

        igSetNextItemWidth(-60);
        igInputFloat3("Position", &pos.x, NULL,0);
        igSetNextItemWidth(-60);
        igInputFloat3("Rotation", &deg.x, NULL,0);
        igSetNextItemWidth(-60);
        igInputFloat3("Scale", &sca.x, NULL,0);

        compose44(matrix, pos, rot = eulerq(deg), sca);
#endif

    }

    igCheckbox("Snap ", &g->useSnap);
    if(igIsItemHovered(0)) igSetTooltip(tooltip_0);
    igSameLine(0,0);
    if(!g->useSnap) ui_enable(0);
        igPushItemWidth(50);
        igInputFloat(" ", g->snap.v3 + g->manipulator, 0,0,NULL,0);
        if(igIsItemHovered(0)) igSetTooltip(tooltip_1);
    if(!g->useSnap) ui_enable(1);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->useCubeView ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->useCubeView ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->useCubeView ? on : off);
        if(igSmallButton(UI_ICON(DEPLOYED_CODE))) g->useCubeView ^= 1;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_2);
        igPopStyleColor(3);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->world ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->world ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->world ? on : off);
        if(igSmallButton(UI_ICON(PUBLIC))) g->world ^= 1;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_3);
        igPopStyleColor(3);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->manipulator == 0 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->manipulator == 0 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->manipulator == 0 ? on : off);
        if(igSmallButton(UI_ICON(DRAG_PAN))) g->manipulator = 0;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_4);
        igPopStyleColor(3);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->manipulator == 1 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->manipulator == 1 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->manipulator == 1 ? on : off);
        if(igSmallButton(UI_ICON(REFRESH))) g->manipulator = 1;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_5);
        igPopStyleColor(3);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->manipulator == 2 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->manipulator == 2 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->manipulator == 2 ? on : off);
        if(igSmallButton(UI_ICON(OPEN_IN_FULL))) g->manipulator = 2;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_6);
        igPopStyleColor(3);
    igSameLine(0,0);
        igPushStyleColor_Vec4(ImGuiCol_Button, g->manipulator == 3 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, g->manipulator == 3 ? on : off);
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, g->manipulator == 3 ? on : off);
        if(igSmallButton(UI_ICON(RESIZE))) g->manipulator = 3;
        if(igIsItemHovered(0)) igSetTooltip(tooltip_7);
        igPopStyleColor(3);
}

int gizmo2(bool is_ortho, float cameraView[16], float extra[6], const float cameraProjection[16], int count, float objectMatrix[16]) {

    ImGuizmo_SetOrthographic(is_ortho);

    {
        enum locations {
            top_left = 0,
            top_right = 0<<1|1<<0,
            bottom_left = 1<<1|1<<1,
            bottom_right = 1<<1|1<<0,
            centered = -1,
        };

        static int location = bottom_right;

        if( location == centered ) {
            ImVec2 pos;
            ImGuiViewport_GetCenter(&pos, igGetMainViewport());
            igSetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        } else {
            const float PAD = 0.0f;
            ImGuiIO* io = igGetIO();
            ImGuiViewport* viewport = igGetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
            igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }
//        igSetNextWindowSize(ImVec2(268, 24), ImGuiCond_Always);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings /*| ImGuiWindowFlags_NoFocusOnAppearing*/ | ImGuiWindowFlags_NoNav;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBackground;

        static int guid = 0;
        igPushID_Int(++guid);

            // create a window and insert the inspector
            if( igBegin("Editor", NULL, window_flags) ) {

                int lastUsing = 0;
                for (int matId = 0; matId < count; matId++)
                {
                    ImGuizmo_PushID_Int(matId);

                    if( objectMatrix ) {
                        TransformEdit(cameraView, cameraProjection, &objectMatrix[16*matId]);

                        if( ImGuizmo_IsUsing() ) {
                            lastUsing = matId;
                        }
                    }

                    ImGuizmo_PopID();
                }

                if( _gizmo_pass == 0 ) {
                    float id[16] = {1,0,0,0, 0,1,0,0, 0,0,0,1, 0,0,0,1 };
                    TransformView(cameraView, extra, cameraProjection, count, objectMatrix ? &objectMatrix[0] : id);
                    if( igIsWindowHovered(0/*ImGuiHoveredFlags_AnyWindow*/) )
                        _gizmo_view_hovered |= 1, _gizmo_view_active |= igIsAnyItemActive();
                }
            }

            igEnd();

            _gizmo_view_hovered |= igIsWindowHovered(0/*ImGuiHoveredFlags_AnyWindow*/);

        igPopID();
    }

    // process key shortcuts if user is in edit mode
    if( !mouse().hidden && !ui_active() ) {

        struct gizmo_t *g = &gizmo_settings;
        if (binding(key_gizmo_translate)) g->manipulator = 0;
        if (binding(key_gizmo_rotate)) g->manipulator = 1;
        if (binding(key_gizmo_scale)) g->manipulator = 2;
        if (binding(key_gizmo_bounds)) g->manipulator = 3;
        int next = binding(key_gizmo_next_mode);
        int prev = binding(key_gizmo_prev_mode);
        if( next || prev ) {
            /**/ if( g->manipulator == 0 ) g->manipulator = prev ? 3 : 1;
            else if( g->manipulator == 1 ) g->manipulator = prev ? 0 : 2;
            else if( g->manipulator == 2 ) g->manipulator = prev ? 1 : 3;
            else                           g->manipulator = prev ? 2 : 0;
        }
    }

    return ImGuizmo_IsUsing();
}

int gizmo44b(float transform[16]) {
    if( _gizmo_pass == 0 ) {
        ImGuizmo_BeginFrame();
    }

    if( _gizmo_pass == 0 )
    {
        static camera_t copy = {0};
        do_once copy = *camera_get_active();

        if( !ImGuizmo_IsUsingViewManipulate() ) {
            copy = *camera_get_active();
        }

        float extra[9];
        gizmo2(!!copy.orthographic, copy.view, extra, copy.proj, 1, transform);

        if( ImGuizmo_IsUsingViewManipulate() ) {

            camera_t *cam = camera_get_active(); {
                bool damping = cam->damping;
                cam->damping = 0;
                cam->last_move = vec3(0,0,0);
                cam->position = ptr3(extra+0);
                camera_lookat(cam, ptr3(extra+3));
                cam->damping = damping;
            }

        }
    }

    _gizmo_pass++;

    return gizmo_active();
}

int gizmo44(int count, float **matrices) {
    if( count <= 0 ) {
        // draw dummy gizmo so it renders viewcube too during first pass
        return gizmo44b(NULL);
    }

    // [single-gizmo-multiple-objects]
    // imguizmo does not support single-gizmo-multiple-objects.
    // to address this limitation, we calc dragged distance in object [0]...
    vec3 src = ptr3(&matrices[0][12]);
    gizmo44b(matrices[0]);
    vec3 dst = ptr3(&matrices[0][12]);
    vec3 inc = sub3(dst, src);
    // ... and add that exact distance to each remaining object in list
    if( gizmo_mode() == 0 && gizmo_active() ) {
        for( int id = 1; id < count; ++id) {
            float *matrix = matrices[id];
            matrix[12] += inc.x;
            matrix[13] += inc.y;
            matrix[14] += inc.z;
        }
    }

    return gizmo_active();
}

#if 0 // @fixme: 
// try: https://theorangeduck.com/page/variations-muller
// try: https://theorangeduck.com/page/closed-form-matrix-decompositions
int gizmo(vec3 *pos, vec3 *rot, vec3 *sca) {
    mat44 matrix;
    compose44(matrix, *pos, eulerq(*rot), *sca);
    int changed = gizmo44(matrix);
    ImGuizmo_DecomposeMatrixToComponents(matrix, &pos->x, &rot->x, &sca->x);
    return changed;
}
#endif

int gizmo_mode() {
    return gizmo_settings.manipulator;
}

void gizmo_tick() {
    do_once {
    ImGuizmo_GetStyle()->Colors[ImGuizmo_INACTIVE];
    ImGuizmo_GetStyle()->Colors[ImGuizmo_TRANSLATION_LINE];
    ImGuizmo_GetStyle()->Colors[ImGuizmo_SCALE_LINE];
    ImGuizmo_GetStyle()->Colors[ImGuizmo_TEXT];
    ImGuizmo_GetStyle()->Colors[ImGuizmo_TEXT_SHADOW];
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_X] =
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_X] = ImVec4(1,0,0,1);
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_Y] =
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_Y] = ImVec4(0,1,0,1);
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_Z] =
    ImGuizmo_GetStyle()->Colors[ImGuizmo_DIRECTION_Z] = ImVec4(0,0,1,1);
    ImGuizmo_GetStyle()->Colors[ImGuizmo_SELECTION  ] = ImVec4(1,1,0,1);
    ImGuizmo_GetStyle()->Colors[ImGuizmo_HATCHED_AXIS_LINES] = ImVec4(0,0,0,0.75);
    }

    _gizmo_pass = 0;
    _gizmo_view_active = 0;
    _gizmo_view_hovered = 0;
}

AUTORUN {
    hooks("tick", gizmo_tick);
}

#endif
