extern "C"
void igCherryTheme() {
    ImGui::StyleColorsDark();    // Reset styles

    // cherry colors, 3 intensities
    #define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
    #define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
    #define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
    #define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
    // text
    #define TXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = TXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TXT(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = HI( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
//  style.Colors[ImGuiCol_Column]                = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
//  style.Colors[ImGuiCol_ColumnHovered]         = MED( 0.78f);
//  style.Colors[ImGuiCol_ColumnActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);
    // [...]
    style.Colors[ImGuiCol_ModalWindowDimBg]      = BG( 0.73f);
    style.Colors[ImGuiCol_Header]                 = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_ButtonActive]           = style.Colors[ImGuiCol_TitleBgActive];
    style.Colors[ImGuiCol_ResizeGripActive]       = style.Colors[ImGuiCol_TitleBgActive];

    style.Colors[ImGuiCol_Tab]                    = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.20f, 0.38f, 0.62f, 0.96f);
    style.Colors[ImGuiCol_TabSelected]            = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
    style.Colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.00f, 0.33f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
#if 0
    style.colors[ImGuiCol_TabHovered]             = ImVec4(0.39f, 0.00f, 0.17f, 1.00f);
    style.colors[ImGuiCol_Tab]                    = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    style.colors[ImGuiCol_TabSelected]            = ImVec4(0.25f, 0.03f, 0.12f, 1.00f);
    style.colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.39f, 0.00f, 0.17f, 0.51f);
#endif



    style.FrameRounding            = 3.0f;
    style.GrabRounding             = 2.0f;
    style.GrabMinSize              = 20.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;

    style.ItemSpacing.x            = 4;
    style.FramePadding.y = 0;
    style.ItemSpacing.y = 2;
#if 0
    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5, 2);
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
#endif

    style.WindowTitleAlign.x = 0.50f;
    style.SelectableTextAlign.x = 0.50f;
    style.SeparatorTextAlign.x = 0.04f;
    style.SeparatorTextBorderSize = 1;
    style.SeparatorTextPadding = ImVec2(0,0);

    //style.Colors[ImGuiCol_Header].w = 0/255.f; // collapsable headers
    style.Colors[ImGuiCol_TableBorderLight].w = 80/255.f; // column resizing grips

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;

    #undef HI
    #undef MED
    #undef LOW
    #undef BG
    #undef TXT
}

extern "C"
void igTextWithHoverColor(ImU32 col, ImVec2 indents_offon, const char* text_begin) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const char *text_end = text_begin + strlen(text_begin);

    // Layout
    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const ImVec2 text_size = CalcTextSize(text_begin, text_end);
    ImRect bb(text_pos.x, text_pos.y, text_pos.x + text_size.x, text_pos.y + text_size.y);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    bool hovered = IsItemHovered();
    if (hovered) PushStyleColor(ImGuiCol_Text, col);
    RenderText(bb.Min + ImVec2(hovered ? indents_offon.y : indents_offon.x,0), text_begin, text_end, false);
    if (hovered) PopStyleColor();
}

extern "C"
int igCurrentWindowStackSize(void) {
    using namespace ImGui;
    return GImGui->CurrentWindowStack.Size;
}

extern "C"
int igIsAnyWindowHovered(void) {
    return igIsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

extern "C"
float igGetWindowPosX(void) { ImVec2 v; return igGetWindowPos(&v), v.x; }

extern "C"
float igGetWindowPosY(void) { ImVec2 v; return igGetWindowPos(&v), v.y; }

extern "C"
int igRightAlign(const char* str_id) {
    if(igBeginTable(str_id, 2, ImGuiTableFlags_SizingFixedFit, ImVec2(-1,0), 0)) {
        igTableSetupColumn("a", ImGuiTableColumnFlags_WidthStretch, 0,0);

        igTableNextColumn();
        igTableNextColumn();
        return true;
    }
    return false;
}
extern "C"
int igRightAlignEnd() {
    return igEndTable(), false;
}


