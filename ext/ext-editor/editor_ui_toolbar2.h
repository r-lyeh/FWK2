API int ui_editor_toolbar_begin();
API int ui_editor_toolbar_end();

#if CODE

int ui_editor_toolbar_begin() {
    enum locations {
        top_left = 0,
        top_right = 0<<1|1<<0,
        bottom_left = 1<<1|1<<1,
        bottom_right = 1<<1|1<<0,
        centered = -1,
    };

    static int location = top_right;

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

//  igSetNextWindowSize(ImVec2(268, 24), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings /*| ImGuiWindowFlags_NoFocusOnAppearing*/ | ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBackground;

    static int guid = 0;
    igPushID_Int(++guid);

        // create a window, if possible
        if( !igBegin("##Editor.toolbar", NULL, window_flags) ) {
            igEnd();
            igPopID();
            return 0;
        }

    return 1;
}

int ui_editor_toolbar_end() {
    igEnd();
    igPopID();
    return 1;
}

#endif
