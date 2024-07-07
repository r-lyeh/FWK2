// -----------------------------------------------------------------------------
// immediate ui framework
// - rlyeh, public domain
//
// @fixme: tooltips + left color tabs
// @fixme: my float
// @fixme: ID collisions bitmask+buttons
// @fixme: docking colors
// @fixme: auto-relocate panels
// @fixme: PANEL_OPEN
// @fixme: panel inside window
// @fixme: test&bugfix ui_dialog()
//
// @todo: menus
// @todo: ui_alert()
// @todo: ui_filter()
// @todo: ui_browse()
// @todo: bold font + icons
// @todo: logger/console
// @todo: imguizmo
// @todo: ui_gallery()

#ifndef PANEL_FLAGS_
#define PANEL_FLAGS_
enum PANEL_FLAGS {
    PANEL_OPEN = 1,
};
#endif

API int ui_notify(const char *label, const char *body);
API int ui_window(const char *label, int *open);
API int  ui_panel(const char *label, int flags); // may be embedded inside a window, or standalone
API int    ui_section(const char *label);
API int    ui_separator();
API int    ui_int(const char *label, int *value);
API int    ui_bool(const char *label, bool *value);
API int    ui_float(const char *label, float value[1]);
API int    ui_float2(const char *label, float value[2]);
API int    ui_float3(const char *label, float value[3]);
API int    ui_float4(const char *label, float value[4]);
API int    ui_buffer(const char *label, char *buffer, int buflen);
API int    ui_color3(const char *label, unsigned *color); //[0..255]
API int    ui_color3f(const char *label, float color[3]); //[0..1]
API int    ui_color4(const char *label, unsigned *color); //[0..255]
API int    ui_color4f(const char *label, float color[4]); //[0..1]
API int    ui_button(const char *label);
API int    ui_buttons(int buttons, /*labels*/...);
API int    ui_toolbar(int icons, /*icon labels*/...); // int choice = ui_toolbar(4,"A","B","C","D");
API int    ui_browse(const char **outfile, bool *inlined); // may be embedded inside a window or inside a panel
API int    ui_toggle(const char *label, bool *value);
API int    ui_alert(const char *message);
API int    ui_dialog(const char *label, const char *text, int choices, bool *show); // @fixme: return
API int    ui_list(const char *label, const char **items, int num_items, int *selector);
API int    ui_bitmask(const char *label, int bits, void *ptr);
API int    ui_label(const char *label);
API int    ui_label2(const char *label, const char *caption);
API int    ui_slider(const char *label, float *value);
API int    ui_slider2(const char *label, float *value, const char *caption);
API int  ui_panel_end();
API int ui_window_end();

API int ui_enable();
API int ui_disable();
API int ui_enabled();

API int ui_hide(bool);
API int ui_hidden();

API int ui_demo(int do_windows);
//API void *ui_handle();




#if CODE
#include <float.h> // FLT_MIN,FLT_MAX

int ui_is_hidden = 0;
int ui_is_enabled = 1;

const
int ui_slider_flags = 0;
// ImGuiSliderFlags_Logarithmic = 1 << 5,
// ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
// ImGuiSliderFlags_NoInput = 1 << 7,
// ImGuiSliderFlags_WrapAround = 1 << 8,
// ImGuiSliderFlags_ClampOnInput = 1 << 9,
// ImGuiSliderFlags_ClampZeroRange = 1 << 10,
// ImGuiSliderFlags_AlwaysClamp = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ClampZeroRange,

int ui_window(const char *label, int *open) {
    bool wopen = *open;
    if( wopen ) {
        igSetNextWindowSize(ImVec2(200,1.5*200), ImGuiCond_FirstUseEver);
        igBegin(label, &wopen, ImGuiWindowFlags_NoCollapse/*|ImGuiWindowFlags_AlwaysAutoResize*/);
        igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
        *open = wopen;
        return 1;
    }
    return wopen;
}
int ui_window_end() {
    igEnd();
    return 1;
}

int ui_panel(const char *label, int flags) {// may be embedded inside a window, or standalone
    igSetNextWindowSize(ImVec2(200,1.5*200), ImGuiCond_FirstUseEver);
    igBegin(label, NULL, 0|ImGuiWindowFlags_NoDocking);
    igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
    return 1;
}
int ui_panel_end() {
    igEnd();
    return 1;
}

int ui_separator() {
    igSeparator();
    return 1;
}


int ui_notify(const char *label, const char *body) {
    return igInsertNotification('w', label, body, 3.0f), 1;
}

int ui_int(const char *label, int *value) {
    return igInputInt(label, value, 1, 10, 0);
}
int ui_bool(const char *label, bool *value) {
    return igCheckbox(label, value);
}
int ui_toggle(const char *label, bool *value) {
    return igCheckbox(label, value);
}
int ui_float(const char *label, float *v) {
    return igSliderFloat(label, v, -FLT_MAX/2, FLT_MAX/2, "%f", ui_slider_flags);
}
int ui_float2(const char *label, float v[2]) {
    return igInputFloat2(label, v, "%f", ui_slider_flags);
}
int ui_float3(const char *label, float v[3]) {
    return igInputFloat3(label, v, "%f", ui_slider_flags);
}
int ui_float4(const char *label, float v[4]) {
    return igInputFloat4(label, v, "%f", ui_slider_flags);
}
int ui_slider(const char *label, float *value) {
    return igSliderFloat(label, value, 0, 1, "%f", ui_slider_flags);
}
int ui_slider2(const char *label, float *value, const char *caption) {
    return igSliderFloat(label, value, 0, 1, caption, ui_slider_flags);
}
int ui_buffer(const char *label, char *buffer, int buflen) {
    const int flags = 0; // ImGuiInputTextFlags
    return igInputText(label, buffer, buflen, flags, NULL, NULL);
}

int ui_color3f(const char *label, float color[3]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return igColorEdit3(label, color, ImGuiColorEditFlags_Float);
}
int ui_color3(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color3f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_color4f(const char *label, float color[4]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return igColorEdit4(label, color, ImGuiColorEditFlags_Float|ImGuiColorEditFlags_AlphaPreviewHalf|ImGuiColorEditFlags_AlphaBar);
}
int ui_color4(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color4f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_button(const char *label) {
    return igButton(label, ImVec2(-1,0));
}
int ui_buttons(int buttons, /*labels*/...) {
    const char *labels[16] = {0};
    if( buttons >= COUNTOF(labels) ) die("ui_buttons() capacity exceeded");
        va_list list;
        va_start(list, buttons);
        for( int i = 0; i < buttons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    int touched = 0;

    igColumns(buttons, "##", false);
    for( int i = 0; i < buttons; ++i ) {
        if( igButton(labels[i], ImVec2(-1,0)) )
            touched = i + 1;
        igNextColumn();
    }

    return touched;
}
int ui_toolbar(int icons, ...) { // int icon = ui_toolbar(4,"A",B",C",D") {
    const char *labels[16] = {0};
    if( icons >= COUNTOF(labels) ) die("ui_toolbar() capacity exceeded");
        va_list list;
        va_start(list, icons);
        for( int i = 0; i < icons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    int icon = 0;
    for( int i = 0; i < icons ; ++i ) {
        if( igSmallButton(labels[i])) {
            icon = i+1;
        }
        if( i < (icons-1) ) igSameLine(0,0);
    }
    return icon;
}

int ui_browse(const char **outfile, bool *inlined) {// may be embedded inside a window or inside a panel
    return ui_label("UI_BROWSE() @todo"), 0;
}

// @fixme: show arg not needed?
int ui_dialog(const char *label, const char *text, int choices, bool *show) {// @fixme: return
    int selected = 0;

        if (igButton(label,ImVec2(0,0)))
            *show = 1, igOpenPopup_Str(label,0);

        if( *show )
        if (igBeginPopupModal(label, NULL, 0/*ImGuiWindowFlags_MenuBar*/))
        {
            igText("%s", text);

            if( choices > 0 ) {
                selected = ui_buttons(choices > 3 ? 3 : choices, "OK", "Cancel", "Retry");
                if( selected ) {
                    *show = 0, igCloseCurrentPopup();
                }
            }
            else {
                if (igButton("Close",ImVec2(0,0)))
                    selected = 1, *show = 0, igCloseCurrentPopup();
            }

            igEndPopup();
        }
        return selected;

    return ui_label("UI_DIALOG() @todo"), 0;
}
int ui_alert(const char *message) {
    bool dummy = 0;
    return ui_dialog("Alert", message, 1, &dummy);
}
int ui_bitmask(const char *label, int numbits, void *ptr) {
    igText("%s", label); igSameLine(0,0);

    uintptr_t addr = (uintptr_t)ptr;

    int touched = 0;
    for( int i = 0; i < numbits ; ++i ) {
        if( igSmallButton(addr & (1ull<<i) ? "1" : "0") ) {
            touched = i+1;
            addr ^= (1ull<<i);
            memcpy(ptr, &addr, numbits/8);
        }
        if( i < (numbits-1) ) igSameLine(0,0);
    }

    return touched;
}

int ui_list(const char *label, const char **items, int num_items, int *selector) {
    const int height_in_items = num_items > 4 ? 4 : num_items;
    return igListBox_Str_arr(label,selector,items,num_items,height_in_items);
}
int ui_label(const char *label) {
    return igText("%s", label), 1;
}
int ui_label2(const char *label, const char *text) {
    return igLabelText(label, "%s", text), 1;
}
int ui_section(const char *label) { // @fixme: push different font face
    igPushStyleColor_Vec4(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
    igText("%s", label);
    igPopStyleColor(1);
    return 1;
}

int ui_enable() {
    if( !ui_is_enabled ) {
        igEndDisabled();
    }
    ui_is_enabled = 1;
    return 1;
}
int ui_disable() {
    if( ui_is_enabled ) {
        igBeginDisabled(1);
    }
    ui_is_enabled = 0;
    return 1;
}
int ui_enabled() {
    return ui_is_enabled;
}

int ui_hide(bool hidden) {
    ui_is_hidden = hidden;
    return 1;
}
int ui_hidden() {
    return ui_is_hidden;
}

int ui_demo(int do_windows) {
    static int integer = 42;
    static bool toggle = true;
    static bool boolean = true;
    static float floating = 3.14159;
    static float float2[2] = {1,2};
    static float float3[3] = {1,2,3};
    static float float4[4] = {1,2,3,4};
    static float rgbf[3] = {0.84,0.67,0.17};
    static float rgbaf[4] = {0.67,0.90,0.12,0.5};
    static unsigned rgb = CYAN;
    static unsigned rgba = PINK;
    static float slider = 0.5f;
    static float slider2 = 0.5f;
    static char string[64] = "hello world 123";
    static int item = 0; const char *list[] = {"one","two","three"};
    static bool show_dialog = false;
    static bool show_browser = false;
    static const char* browsed_file = "";
    static uint8_t bitmask = 0x55;
    static int hits = 0;
    static int window1 = 0, window2 = 0, window3 = 0;
    static int disable_all = 0;

    if( ui_panel("UI", 0) ) {
        int choice = ui_toolbar(4,"Browser","Toast@Notify","Toggle on/off","Alert");
            if(choice == 1) show_browser = true;
            if(choice == 2) ui_notify(va("My random toast (%d)", rand()), va("This is notification #%d", ++hits));
            if(choice == 3) disable_all ^= 1;
            if(choice == 4) ui_alert("Button was pressed");

        if( disable_all ) ui_disable();

        if( ui_browse(&browsed_file, &show_browser) ) puts(browsed_file);

        if( ui_section("Labels")) {}
        if( ui_label("my label")) {}
        if( ui_label("my label with tooltip@built on " __DATE__ " " __TIME__)) {}
//        if( ui_label2_toolbar("my toolbar", ICON_MD_STAR ICON_MD_STAR_OUTLINE ICON_MD_BOOKMARK ICON_MD_BOOKMARK_BORDER) ) {}
        //if( ui_label2_wrap("my long label", "and some long long long long text wrapped")) {}

        if( ui_section("Types")) {}
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my string", string, 64) ) puts("string changed");

        if( ui_section("Vectors") ) {}
        if( ui_float2("my float2", float2) ) puts("float2 changed");
        if( ui_float3("my float3", float3) ) puts("float3 changed");
        if( ui_float4("my float4", float4) ) puts("float4 changed");

        if( ui_section("Lists")) {}
        if( ui_list("my list", list, 3, &item ) ) puts("list changed");

        if( ui_section("Colors")) {}
        if( ui_color3("my color3", &rgb) ) puts("color3 changed");
        if( ui_color4("my color4@this is a tooltip", &rgba) ) puts("color4 changed");
        if( ui_color3f("my color3f", rgbf) ) puts("color3f changed");
        if( ui_color4f("my color4f@this is a tooltip", rgbaf) ) puts("color4f changed");

        if( ui_section("Sliders")) {}
        if( ui_slider("my slider", &slider)) puts("slider changed");
        if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");

        if( do_windows ) {
        if( ui_section("Windows")) {}
        int show = ui_buttons(3, "Container", "SubPanel", "SubRender");
        if( show == 1 ) window1 = 1;
        if( show == 2 ) window2 = 1;
        if( show == 3 ) window3 = 1;
        }

        if( ui_section("Others")) {}
        if( ui_bitmask("my bitmask", 8, &bitmask) ) printf("bitmask changed %x\n", bitmask);
        if( ui_toggle("my toggle", &toggle) ) printf("toggle %s\n", toggle ? "on":"off");
#if HAS_IMAGE
        if( ui_image("my image", texture_checker().id, 0, 0) ) { puts("image clicked"); }
#endif

        if( ui_separator() ) {}
        if( ui_button("my button") ) { puts("button clicked"); show_dialog = true; }
        if( ui_buttons(2, "yes", "no") ) { puts("button clicked"); }
        if( ui_buttons(3, "yes", "no", "maybe") ) { puts("button clicked"); }
        if( ui_dialog("my dialog", __FILE__ "\n" __DATE__ "\n" "Public Domain.", 2/*two buttons*/, &show_dialog) ) {}

        if( disable_all ) ui_enable(); // restore enabled state

        ui_panel_end();
    }

    if( !do_windows ) return 0;

    // window api showcasing
    if( ui_window("Container demo", &window1) ) {
        ui_label("label #1");
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my string", string, 64) ) puts("string changed");
        ui_window_end();
    }

    if( ui_window("SubPanel demo", &window2) ) {
        if( ui_panel("panel #2", 0) ) {
            ui_label("label #2");
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            ui_panel_end();
        }
        ui_window_end();
    }

    if( ui_window("SubRender demo", &window3) ) {
        if( ui_panel("panel #3A", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }
        if( ui_panel("panel #3B", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }

#if HAS_IMAGE
        const char *title = "SubRender demo";
        struct nk_window *win = nk_window_find(ui_ctx, title);
        if( win ) {
            enum { menubar_height = 65 }; // title bar (~32) + menu bounds (~25)
            struct nk_rect bounds = win->bounds; bounds.y += menubar_height; bounds.h -= menubar_height;
#if 1
            ddraw_flush();

            // @fixme: this is breaking rendering when post-fxs are in use. edit: cannot reproduce
            static texture_t tx = {0};
            if( texture_rec_begin(&tx, bounds.w, bounds.h )) {
                glClearColor(0.15,0.15,0.15,1);
                glClear(GL_COLOR_BUFFER_BIT);
                ddraw_grid(10);
                ddraw_flush();
                texture_rec_end(&tx);
            }
            struct nk_image image = nk_image_id((int)tx.id);
            nk_draw_image_flipped(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#else
            static video_t *v = NULL;
            do_once v = video( "bjork-all-is-full-of-love.mp4", VIDEO_RGB );

            texture_t *textures = video_decode( v );

            struct nk_image image = nk_image_id((int)textures[0].id);
            nk_draw_image(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#endif
        }
#endif

        ui_window_end();
    }
    return 0;
}

#endif

#if 0

API int   ui_collapse(const char *label, const char *id);
API int   ui_collapseo(const char *label, const char *id);
API int   ui_contextual();
API int    ui_string(const char *label, char **string);
API int    ui_short(const char *label, short *value);
API int    ui_unsigned(const char *label, unsigned *value);
API int    ui_unsigned2(const char *label, unsigned *value);
API int    ui_unsigned3(const char *label, unsigned *value);
API int    ui_float_(const char *label, float *value, float step);
API int    ui_float2_(const char *label, float value[2], float step);
API int    ui_float3_(const char *label, float value[3], float step);
API int    ui_float4_(const char *label, float value[4], float step);
API int    ui_mat33(const char *label, float mat33[9]);
API int    ui_mat34(const char *label, float mat34[12]);
API int    ui_mat44(const char *label, float mat44[16]);
API int    ui_double(const char *label, double *value);
API int    ui_button_transparent(const char *label);
API int    ui_submenu(const char *options); // int choice = ui_submenu("A;B;C;D");
API int    ui_radio(const char *label, const char **items, int num_items, int *selector);
#if HAS_IMAGE
API int    ui_texture(const char *label, texture_t t);
API int    ui_subtexture(const char *label, texture_t t, unsigned x, unsigned y, unsigned w, unsigned h);
API int    ui_image(const char *label, handle id, unsigned w, unsigned h); //(w,h) can be 0
API int    ui_subimage(const char *label, handle id, unsigned iw, unsigned ih, unsigned sx, unsigned sy, unsigned sw, unsigned sh);
API int    ui_colormap(const char *label, colormap_t *cm); // returns num member changed: 1 for color, 2 for texture map
#endif
API int    ui_console(const char *optional_cmd);
API int    ui_clampf_(const char *label, float *value, float minf, float maxf, float step);
API int    ui_clampf(const char *label, float *value, float minf, float maxf);
API int    ui_label2_bool(const char *label, bool enabled);
API int    ui_label2_float(const char *label, float value);
API int    ui_label2_toolbar(const char *label, const char *icons);
API int   ui_contextual_end(int close);
API int   ui_collapse_clicked();
API int   ui_collapse_end();

API int  ui_show(const char *panel_or_window_title, int enabled);
API int  ui_dims(const char *panel_or_window_title, float width, float height);
API int  ui_visible(const char *panel_or_window_title); // @todo: include ui_collapse() items that are open as well?
API vec2 ui_get_dims();

API int ui_has_menubar();
API int ui_menu(const char *items); // semicolon-separated or comma-separated items
API int ui_menu_editbox(char *buf, int bufcap);
API int ui_item();

API int ui_popups(); // ui_any_popup()? ui_has_popups()?
API int ui_hover(); // ui_is_hover()?
API int ui_active(); // ui_is_active()?

#endif
