#if !CODE

typedef struct gamepad_icon_t {
    const char *name;
    float value;
    float polarity;
    int texture_id;
    image_t i;
} gamepad_icon_t;

typedef struct gamepad_t {
    unsigned index;
    const char *name;
    float deadzone;
    array(gamepad_icon_t) axis;
    array(gamepad_icon_t) button;
    void *handle;

    bool plugged;
    float battery;
    // @todo: vec4 led;
    // @todo: vec2 touch[SDL_GetNumGamepadTouchpadFingers];
    // @todo: char text[];
    // @todo: rumble
    // @todo: sensors[];

    // quick access:

    float *a, *b, *x, *y;
    float *up, *down, *left, *right;
    float *menu, *start;

    float *lpadx, *lpady; // *lpad_raw2;
    float *lb, *lthumb;

    float *rpadx, *rpady; // *rpad_raw2;
    float *rb, *rthumb;

    float *lt, *rt;

} gamepad_t;

API gamepad_t gamepad(unsigned gamepad_id, float deadzone); // id:[0..4], deadzone<=0.15f

API int ui_gamepad(unsigned gamepad_id);
API int ui_gamepads();

#else

static
gamepad_icon_t from_surface(SDL_Surface *s) {
    gamepad_icon_t p = {0};
    if( s ) {
        assert(s->pixels);
        assert(s->pitch / s->w == 4); // s->format, SDL_PIXELFORMAT_RGB24 SDL_PIXELFORMAT_RGBX8888

        p.i = image_create(s->w, s->h, IMAGE_RGBA);
        memcpy(p.i.pixels, s->pixels, s->h * s->pitch);

        p.texture_id = texture_create(s->w, s->h, 4, s->pixels, TEXTURE_RGBA).id;
        SDL_DestroySurface(s);
    }
    return p;
}

gamepad_t gamepad_create(unsigned player_index) {
    do_once {

        // [ref] https://github.com/mdqinc/SDL_GameControllerDB
        const char *mappings = file_read("gamecontrollerdb.txt", 0);
        if( mappings ) {
            for each_substring(mappings, "\r\n", line) {
                SDL_AddGamepadMapping(line);
            }
        }

        // In your app near startup (preferably after SDL_Init)...
        if (!ControllerImage_Init()) {
            alert(va("ControllerImage_Init() failed. Reason: '%s'\n", SDL_GetError()));
        }

        // Load in the controller and image data...
        // fallback
        if(1)
        {
            int blen; char *bin = file_read("controllerimage-standard.bin", &blen);
            if (!ControllerImage_AddData(bin, blen)) {
                alert(va("ControllerImage_AddData() failed. Reason: '%s'\n", SDL_GetError()));
            }
        }

        // main skin
        if(0)
        {
            int blen; char *bin = file_read("controllerimage-kenney.bin", &blen);
            if (!ControllerImage_AddData(bin, blen)) {
                alert(va("ControllerImage_AddData() failed. Reason: '%s'\n", SDL_GetError()));
            }
        }
    }

#if 0
    int count = 0;
    SDL_JoystickID *list = SDL_GetGamepads(&count);
    if(!count) exit((alert("No gamepad found"), 0));
    SDL_Gamepad *handle = SDL_OpenGamepad(list[0]);
#else
    SDL_Gamepad *handle = SDL_GetGamepadFromPlayerIndex(player_index);
#endif

    // Have a controller? Prepare to get images for it, probably when you open the gamepad:
    // There's a version that accepts a device instance ID and not an `SDL_Gamepad *`, too.
    ControllerImage_Device *imgdev = ControllerImage_CreateGamepadDevice(handle); // @leak
    if(!imgdev) return ((gamepad_t){0});
    // if (!imgdev) PRINTF("ControllerImage_CreateGamepadDevice() failed. Reason: '%s'\n", SDL_GetError());

    // Get image data for specific buttons or axes on the gamepad.
    enum { ICON_WIDTH = 128 };

    array(gamepad_icon_t) axis = 0;
    array_resize(axis, SDL_GAMEPAD_AXIS_COUNT);

    for( int i = 0; i < array_count(axis); ++i ) {
        SDL_Surface *surf = ControllerImage_CreateSurfaceForAxis(imgdev, i, ICON_WIDTH);
        // if( !surf ) PRINTF("ControllerImage_CreateSurfaceForAxis() failed. Reason: '%s'\n", SDL_GetError());
        axis[i] = from_surface(surf);
        axis[i].name = SDL_GetGamepadStringForAxis(i);
        axis[i].polarity = strendi(axis[i].name, "y") ? -1 : +1;
    }

    array(gamepad_icon_t) button = 0;
    array_resize(button, SDL_GAMEPAD_BUTTON_COUNT);

    for( int i = 0; i < array_count(button); ++i ) {
        SDL_Surface *surf = ControllerImage_CreateSurfaceForButton(imgdev, i, ICON_WIDTH);
        // if( !surf ) PRINTF("ControllerImage_CreateSurfaceForButton() failed. Reason: '%s'\n", SDL_GetError());
        button[i] = from_surface(surf);
        button[i].name = SDL_GetGamepadStringForButton(i);
        button[i].polarity = +1;
    }

    gamepad_t u = {0};
    u.index = player_index;
    u.axis = axis;
    u.button = button;
    u.handle = handle;
    u.name = SDL_GetGamepadName(handle);

    u.a = &u.button[SDL_GAMEPAD_BUTTON_SOUTH].value;
    u.b = &u.button[SDL_GAMEPAD_BUTTON_EAST].value;
    u.x = &u.button[SDL_GAMEPAD_BUTTON_WEST].value;
    u.y = &u.button[SDL_GAMEPAD_BUTTON_NORTH].value;
    u.up = &u.button[SDL_GAMEPAD_BUTTON_DPAD_UP].value;
    u.down = &u.button[SDL_GAMEPAD_BUTTON_DPAD_DOWN].value;
    u.left = &u.button[SDL_GAMEPAD_BUTTON_DPAD_LEFT].value;
    u.right = &u.button[SDL_GAMEPAD_BUTTON_DPAD_RIGHT].value;
    u.menu = &u.button[SDL_GAMEPAD_BUTTON_BACK].value; // _GUIDE?
    u.start = &u.button[SDL_GAMEPAD_BUTTON_START].value;

    u.lb = &u.button[SDL_GAMEPAD_BUTTON_LEFT_SHOULDER].value;
    u.lthumb = &u.button[SDL_GAMEPAD_BUTTON_LEFT_STICK].value;

    u.rb = &u.button[SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER].value;
    u.rthumb = &u.button[SDL_GAMEPAD_BUTTON_RIGHT_STICK].value;

    u.lt = &u.axis[SDL_GAMEPAD_AXIS_LEFT_TRIGGER].value;
    u.rt = &u.axis[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER].value;
    u.lpadx = &u.axis[SDL_GAMEPAD_AXIS_LEFTX].value;
    u.lpady = &u.axis[SDL_GAMEPAD_AXIS_LEFTY].value;
    u.rpadx = &u.axis[SDL_GAMEPAD_AXIS_RIGHTX].value;
    u.rpady = &u.axis[SDL_GAMEPAD_AXIS_RIGHTY].value;

    return u;

    // @fixme: Done with this controller? Free up resources...
    // ControllerImage_DestroyDevice(imgdev);

    // @fixme: At app shutdown...
    // ControllerImage_Quit();  // safe even if ControllerImage_Init() failed!
}

gamepad_t gamepad(unsigned player, float deadzone) {
    static map(unsigned,gamepad_t) map;
    if(!map) map_init_int(map);

    gamepad_t *found = map_find(map, player);
    if(!found) {
        gamepad_t g = gamepad_create(player);
        if(!g.name) return g;
        found = map_find_or_add(map, player, g);
    }

    found->plugged = SDL_GamepadConnected(found->handle);
    if( found->plugged ) {

        int battery;
        SDL_PowerState power = SDL_GetGamepadPowerInfo(found->handle, &battery);
        /**/ if( battery < 0 ) found->battery = 0;
        else if( power == SDL_POWERSTATE_NO_BATTERY ) found->battery = 0;
        else if( power == SDL_POWERSTATE_CHARGED ) found->battery = +100;
        else if( power == SDL_POWERSTATE_CHARGING ) found->battery = +battery;
        else if( power == SDL_POWERSTATE_ON_BATTERY) found->battery = -battery;

        deadzone = clampf(deadzone > 0 ? deadzone : 0.15f, 0, 0.85f);
        
        for( int i = 0; i < array_count(found->axis); ++i ) {
            Sint16 v = SDL_GetGamepadAxis(found->handle, (SDL_GamepadAxis)i);
            // For thumbsticks, the state is a value ranging from -32768 (up/left) to 32767 (down/right).
            // Triggers range from 0 when released to 32767 when fully pressed, and never return a negative value.
            if( abs(v) > (32768 * deadzone) ) {
                found->axis[i].value = clampf(mapf(abs(v) / 32767., deadzone, 1 - deadzone, 0, 1), 0, 1) * ((v < 0) ? -found->axis[i].polarity : found->axis[i].polarity);
            } else {
                found->axis[i].value = 0;
            }
        }
        for( int i = 0; i < array_count(found->button); ++i ) {
            found->button[i].value = !!SDL_GetGamepadButton(found->handle, (SDL_GamepadButton)i);
        }

    }

    return *found;
}

int ui_gamepad(unsigned gamepad_id) {
    gamepad_t u = gamepad(gamepad_id, 0);
    if( ui_section(va("Gamepad #%d", gamepad_id)) && u.plugged ) {
        ui_label2("Model", u.name);
        ui_label2("Battery", va("%.2f%% %s", absf(u.battery), u.battery < 0 ? "Draining" : u.battery > 0 ? "Charging" : ""));
        for( int i = 0; i < array_count(u.button); ++i ) {
            if( !u.button[i].texture_id ) continue;

            bool click = u.button[i].value;
            if( !click ) ui_enable(0);
                ui_image(u.button[i].name, u.button[i].texture_id, 32, 32);
            if( !click ) ui_enable(1);
            else {
                texture_t t;
                t.id = u.button[i].texture_id;
                t.w = u.button[i].i.w;
                t.h = u.button[i].i.h;
                blit(&t, 0, BLIT_RGB);
            }
        }
        for( int i = 0; i < array_count(u.axis); ++i ) {
            if( !u.axis[i].texture_id ) continue;

            bool click = u.axis[i].value;
            if( !click ) ui_enable(0);
                ui_image(u.axis[i].name, u.axis[i].texture_id, 32, 32);
                ui_clampf(u.axis[i].name, &u.axis[i].value, -1, 1);
            if( !click ) ui_enable(1);
            else {
                texture_t t;
                t.id = u.axis[i].texture_id;
                t.w = u.axis[i].i.w;
                t.h = u.axis[i].i.h;
                blit(&t, 0, BLIT_RGB);
            }
        }
    }
    return 0;
}

int ui_gamepads() {
    for( int i = 0; i < 4; ++i ) {
        ui_gamepad(i);
    }
    return 0;
}

#endif
