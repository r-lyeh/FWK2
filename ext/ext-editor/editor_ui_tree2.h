// ----------------------------------------------------------------------------
// editor properties

#if !CODE

API int ui_editor_tree(obj* root, const char *name);

#else

// aliases

float  WindowContentRegionAvailX;
float* FramePaddingX, FramePaddingX_Restore;
float* ButtonTextAlignX, ButtonTextAlignX_Restore;

int _ui_editor_tree(obj *o, int tabs, int xoffset) {

    // we dont display components in this view
    if( obj_flag(o, &, IS_COMPONENT) ) {
        return 0;
    }

    int num_children = array_count(o->objchildren) - 1;
    const char *icon = obj_hasmethod(o, icon) ? obj_icon(o) : UI_ICON(DEPLOYED_CODE);
    const char *open = !obj_flag(o,&,HAS_CHILDREN) ? "  " : obj_flag(o, &, IS_OPEN) ? UI_ICON(KEYBOARD_ARROW_DOWN) : UI_ICON(KEYBOARD_ARROW_RIGHT);
    char icon_name[32]; snprintf(icon_name, 32, "%s%s %s%s%s", open, icon, editor_changed(o)/*obj_flag(o,&,IS_CHANGED)*/ ? "*" : "", obj_name(o), num_children ? "/" : "");
    bool show = ui_filter && ui_filter[0] ? !!strstri(icon_name, ui_filter) : 1;

    igPushID_Ptr(o);

    if( show ) {
            bool b, child = 0;

            //child = 0; for each_objchild(o, obj*, oo) child |= !!obj_flag(oo, &, IS_SELECTED);
            if(child) igPushItemFlag(ImGuiItemFlags_MixedValue, true);
            if( (b = obj_flag(o, &, IS_SELECTED), igCheckbox("##a",&b)) ) obj_flag(o, ^=, IS_SELECTED);
            if( igIsItemHovered(0) ) igSetTooltip("Select object");
            if(child) igPopItemFlag();

            igSameLine(0,0);
                igDummy(ImVec2(1+12*tabs,0));
                igSameLine(0,0);
                if( igButton(icon_name,ImVec2(WindowContentRegionAvailX-12*tabs-xoffset,0)) ) {
                    if( input(KEY_SHIFT) )
                    obj_flag(o, |=, IS_SELECTED);
                    if( input(KEY_CTRL) )
                    obj_flag(o, ^=, IS_SELECTED);
                    else
                    if( obj_flag(o,&,HAS_CHILDREN) ) obj_flag(o, ^=, IS_OPEN);
                }
            *FramePaddingX = -2;
#if 0
                for( int i = 0; i < numicons; ++i ) {
                    igSameLine(0,0);
                    if( i == (numicons-1) ) *FramePaddingX = FramePaddingX_Restore;
                    igSmallButton(icons[i]);
                }
#else

            //igPushStyleColor_Vec4(ImGuiCol_Button, ImVec4(0,0,0,0));

                igSameLine(0,0);
                igBeginDisabled(!obj_flag(o, &, HAS_TICK));
                if( (b = !obj_flag(o, &, IS_TICKING), igSmallButton(b ? UI_ICON(STOP) : UI_ICON(PLAY_ARROW))) ) obj_flag(o, ^=, IS_TICKING);
                if(igIsItemHovered(0) ) igSetTooltip("Toggle ticking");
                igEndDisabled();

                igSameLine(0,0);
                igBeginDisabled(!obj_flag(o, &, HAS_DRAW));
                if( (b = !obj_flag(o, &, IS_DRAWING), igSmallButton(b ? UI_ICON(VISIBILITY_OFF) : UI_ICON(VISIBILITY))) ) obj_flag(o, ^=, IS_DRAWING);
                if( igIsItemHovered(0) ) igSetTooltip("Toggle drawing");
                igEndDisabled();

                igSameLine(0,0);
                *FramePaddingX = FramePaddingX_Restore;
                igSmallButton("...");

            //igPopStyleColor(1);

#endif
            *FramePaddingX = FramePaddingX_Restore;

            if( b ) {
                obj_hasmethod(o,edit) && obj_edit(o);
            }
    }

    if( obj_flag(o, &, HAS_CHILDREN) ) {
        for each_objchild(o, obj*, oo) {
            _ui_editor_tree(oo, tabs+1, xoffset);
        }
    }

    igPopID();
    return 0;
}

int ui_editor_tree(obj *root, const char *name) {
    FramePaddingX = &igGetStyle()->FramePadding.x;
    FramePaddingX_Restore = *FramePaddingX;

    ImVec2 size; igGetContentRegionAvail(&size);
    WindowContentRegionAvailX = size.x;

    ButtonTextAlignX = &igGetStyle()->ButtonTextAlign.x;
    ButtonTextAlignX_Restore = *ButtonTextAlignX;
    *ButtonTextAlignX = 0;

        _ui_editor_tree(root, 0, 2*16+32 );

    *ButtonTextAlignX = ButtonTextAlignX_Restore;

    return 0;
}

#endif
