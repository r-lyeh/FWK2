API int ui_keyboard();

#if CODE

int ui_keyboard() {
    int keys[] = {
        KEY_ESC, KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12,-1,              KEY_PRINT,KEY_PAUSE,KEY_SCROLL,-1,     -1,
        KEY_TICK, KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,  KEY_MINUS,KEY_EQUAL,KEY_BS,-1,             KEY_INS,KEY_HOME,KEY_PGUP,-1,        KEY_NUMLOCK,KEY_KPDIV,KEY_KPMUL,KEY_KPSUB,-1,
        KEY_TAB,   KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,  KEY_LSQUARE,KEY_RSQUARE,KEY_HASH,KEY_ENTER,-1,KEY_DEL,KEY_END,KEY_PGDN,-1,     KEY_KP7,KEY_KP8,KEY_KP9,KEY_KPADD,-1,
        KEY_CAPS,     KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L,  KEY_SEMICOLON,KEY_QUOTE,  KEY_ENTER,-1,-1,                                        KEY_KP4,KEY_KP5,KEY_KP6,KEY_KPADD,-1,
        KEY_LSHIFT,KEY_ISO,  KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,  KEY_COMMA,KEY_DOT,KEY_SLASH,  KEY_RSHIFT,-1,                KEY_UP, -1,                KEY_KP1,KEY_KP2,KEY_KP3,KEY_KPENTER,-1,
        KEY_LCTRL,KEY_LMETA,KEY_LALT,         KEY_SPACE,     KEY_RALT,KEY_RMETA,KEY_MENU,KEY_RCTRL,-1,                     KEY_LEFT,KEY_DOWN,KEY_RIGHT, -1,     KEY_KP0,KEY_KPDOT,KEY_KPENTER,-1,
    };

    int sizes[INPUT_MAX] = {
        [KEY_F12] = -1, [KEY_SCROLL] = -1,
        [KEY_TAB] = 2, [KEY_BS] = -1, [KEY_PGUP] = -1, 
        [KEY_CAPS] = 3, [KEY_ENTER] = -1, [KEY_PGDN] = -1,
        [KEY_LSHIFT] = 3, [KEY_RSHIFT] = -1, [KEY_UP] = -1,
        [KEY_SPACE] = 6, [KEY_RCTRL] = -1, [KEY_RIGHT] = -1, [KEY_KP0] = 2,
    };

    // igPushStyleVar_Vec2(ImGuiStyleVar_ItemSpacing, ImVec2(1,1));

    ImVec2 area;
    igGetContentRegionAvail(&area);

    igColumns(3,"",false);
    igSetColumnWidth(0,area.x*0.60);
    igSetColumnWidth(1,area.x*0.20);
    igSetColumnWidth(2,area.x*0.20);

    for( int col = 0, k = 0; k < countof(keys); ++col, ++k ) {
        int vk = keys[k];
        if(vk == -1) { igDummy(ImVec2(0,0)); igNextColumn(); continue; }
        const char *name = input_name(vk);
        if(input(vk)) igPushStyleColor_Vec4(ImGuiCol_Button, *igGetStyleColorVec4(ImGuiCol_ButtonHovered));
        igPushID_Int(k);
        igButton(strchr(name, ' ') ? strchr(name, ' ') + 1 : name, ImVec2(20 * (sizes[vk] ? sizes[vk] : 1),0)); if(igIsItemHovered(0)) igSetTooltip(name);
        igPopID();
        if(input(vk)) igPopStyleColor(1);
        igSameLine(0,0);
     }

    igColumns(1,"",false);

    // igPopStyleVar(1);

    return 0;
}

#endif
