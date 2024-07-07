API int editor_label2(vec2 pos, const char *icon);
API int editor_label3(vec3 pos, const char *icon);

#if CODE
int editor_label2(vec2 pos, const char *icon) {
    ImGuiViewport *vp = igGetMainViewport();
    ImDrawList* dl = igGetBackgroundDrawList(vp); // igGetWindowDrawList();
//  ImDrawList_PushClipRectFullScreen(dl);
    ImDrawList_AddText_Vec2(dl,ImVec2(pos.x+vp->WorkPos.x,pos.y+vp->WorkPos.y),~0u,icon,icon+strlen(icon));
//  ImDrawList_PopClipRect(dl);
    return 0; // @fixme: return 1 if clicked
}
int editor_label3(vec3 pos, const char *icon) {
    return editor_label2(editor_xy(pos), icon);
}
#endif
