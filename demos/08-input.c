#include "engine.h"

int main() {
    app_create(0.75, 0);

    while (app_swap()) {
        if( ui_panel("Input", UI_OPEN) ) {
            ui_separator("Mouse");
            ui_mouse();
            ui_separator("Keyboard");
            ui_keyboard();
            ui_separator("Gamepads");
            ui_gamepads();
            ui_panel_end();
        }
    }
}
