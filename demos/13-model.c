#include "engine.h"

int main() {
    // create app (80% sized, MSAA x4 flag)
    app_create(80, APP_MSAA4);

    // animated models loading
    model_t girl = model("kgirls01.iqm", 0);
    compose44( girl.pivot, vec3(0,0,0), eulerq(vec3(0,-90,0)), vec3(2,2,2)); // position, rotation, scale

    // camera
    camera_t cam = camera();

    // demo loop
    while (app_swap() && !input(KEY_ESC)) {
        // fps camera
        bool active = ui_active() || ui_hovered() ? false : input(MOUSE_L) || input(MOUSE_R);
        camera_freefly(&cam, !active);

        // debug draw
        ddraw_ground(0);
        ddraw_flush();

        // animate girl
        float delta = app_delta() * 30; // 30fps anim
        girl.curframe = model_animate(girl, girl.curframe + delta);
        model_render(&girl, cam.proj, cam.view, &girl.pivot, 1, 0);
    }
}
