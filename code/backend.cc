#include <map>
#include <set>
#include <string>
#include <cmath>

#include "engine.h"
#include "native.hh"

//#include "../ext/ext-backend-imgui/api.hh"
#include "../ext/ext-backend-imgui/inc/cimgui/imgui/imgui_impl_sdl3.h"
#include "../ext/ext-backend-imgui/inc/cimgui/imgui/imgui_impl_opengl3.h"

#include <stdio.h>
#include <stdlib.h>

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#define EVENTS_A            array(SDL_Event) sdl3_events;
#define EVENTS_B            array_clear(sdl3_events);
#define EVENTS_C            array_push(sdl3_events, event);

#define MOUSE_MOTION_A      //extern float mouse_dx, mouse_dy;
#define MOUSE_MOTION_B      //mouse_dx = mouse_dy = 0;
#define MOUSE_MOTION_C      //if (event.type == SDL_EVENT_MOUSE_MOTION) mouse_dx = event.motion.xrel, mouse_dy = event.motion.yrel; //< @r-lyeh

#define GL_DEBUG_CONTEXT_A  if( optioni("--gl.debug",flag("--gl.debug")) ) SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#define GL_DEBUG_CONTEXT_B  if( optioni("--gl.debug",flag("--gl.debug")) ) glDebugEnable();
#define GL_PRINT_INFOS      PRINTF("Build version: %d.%d.%d\n", BUILD_VERSION / 10000, (BUILD_VERSION / 100) % 100, BUILD_VERSION % 100), \
                            PRINTF("Monitor: %s (vsync=%5.2fHz, requested=%f)\n", SDL_GetDisplayName(monitor), monitor_fps, asked), \
                            PRINTF("GPU device: %s\n", glGetString(GL_RENDERER)), \
                            PRINTF("GPU driver: %s\n", glGetString(GL_VERSION)), \
                            /*PRINTF("GPU OpenGL: %d.%d\n", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version))*/ 1;

#define IMGUI_IF_FRAME3    if( ui_frame > 3 ) // we consolidate UI widgets during first frames. do not draw those temporary frames.
#define IMGUI_VIEWPORTS    if( (flags & APP_VIEWPORTS) || optioni("--ui.viewports",flag("--ui.viewports")) ) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
#define IMGUI_DOCKING      if( (flags & APP_DOCKING) || optioni("--ui.docking",flag("--ui.docking")) ) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Multi-Viewport / Platform Windows
#define IMGUI_THEME        igCherryTheme(), ui_loadfonts();
#define IMGUI_SETTINGS     io.IniFilename = option("--ui.ini", ".settings.ini");
#define IMGUI_DEPS_A       igFileDialogInit(), ImPlot_CreateContext(), ImPlot_SetImGuiContext(ctx);
#define IMGUI_DEPS_B       /* Notifications: style setup + background color */ \
                           ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f); /* Disable round borders */ \
                           ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); /* Disable borders */ \
                               ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));  \
                                   igRenderNotifications(); \
                               ImGui::PopStyleColor(1); \
                           ImGui::PopStyleVar(2);

#define APP_UNICODE        SDL_StartTextInput(window); // @fixme beware of mobiles OSK
#define APP_IF_UI_VISIBLE  if( !ui_hidden() )
#define APP_MSAA_A         int app_msaa = 0;
#define APP_MSAA_B         SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, app_msaa = optioni("--app.msaa", (flags & APP_MSAA8 ? 8 : flags & APP_MSAA4 ? 4 : flags & APP_MSAA2 ? 2 : 0 )));
#define APP_DELAY          SDL_Delay(optioni("--app.delay", 10)); // delay when minimized
#define APP_SDL3_AUTORUN   AUTORUN { if (!SDL_Init(~0u)) die(SDL_GetError()); }
#define APP_BOUNDS         /* position & square */ \
                            if(flags & APP_TRANSPARENT) window_flags |= SDL_WINDOW_TRANSPARENT; \
                            int numdp; SDL_DisplayID *dps = SDL_GetDisplays(&numdp); \
                            int monitor = dps[0]; \
                            if(scale <= 1) scale *= 100; \
                            bool fullscreen = scale >= 100; \
                            if(scale >= 100) scale = 100; \
                            scale /= 100; \
                            SDL_Rect bounds; \
                            if( !SDL_GetDisplayUsableBounds(monitor, &bounds) ) die( SDL_GetError() ); \
                            int ww = bounds.w * scale, hh = bounds.h * scale; \
                            int orientation = SDL_GetCurrentDisplayOrientation(monitor); \
                            if( orientation == SDL_ORIENTATION_PORTRAIT || orientation == SDL_ORIENTATION_PORTRAIT_FLIPPED ) { \
                                int swap = ww; ww = hh; hh = swap; \
                            } \
                            if( flags & APP_SQUARE ) ww = hh = min(ww,hh);
#define APP_MONITOR_VSYNC   monitor_fps = SDL_GetCurrentDisplayMode(monitor)->refresh_rate; \
                            float asked = optioni("--fps", (int)(monitor_fps+0.5)+2); /* 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync, N to any given fps number. defaults to monitor Hz */ \
                            /**/ if( asked ==  0 ) target_fps = 0, SDL_GL_SetSwapInterval( 0); /* disable our fps limiter + vsync off: max speed */ \
                            else if( asked ==  1 ) target_fps = 0, SDL_GL_SetSwapInterval( 1); /* disable our fps limiter + vsync on : vsync caps fps to monitor hz */ \
                            else if( asked  <  0 ) target_fps = -monitor_fps, SDL_GL_SetSwapInterval(-1); /* disable our fps limiter + adaptive : vsync when above monitor hz and off when it's below */ \
                            else                   target_fps = asked; /* target specific framerate */ \
                            /* SDL_ShowWindow(window); */

extern "C" {
    /*static*/ SDL_Window* window;
    /*static*/ SDL_Renderer* renderer;
    /*static*/ SDL_GLContext glContext;
    /*static*/ int app_keep = 1;

    APP_MSAA_A

    float monitor_fps = 0;
    extern uint64_t ui_frame;
    volatile float target_fps;

    MOUSE_MOTION_A

    EVENTS_A
}

extern "C"
int sdl3_init(float scale, unsigned flags) {

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

    GL_DEBUG_CONTEXT_A

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

    APP_MSAA_B
    APP_BOUNDS

    window = SDL_CreateWindow("", ww, hh, window_flags);
    if (!window) die(SDL_GetError());
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);

    APP_MONITOR_VSYNC

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto *ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    IMGUI_DOCKING
    IMGUI_VIEWPORTS
    IMGUI_THEME
    IMGUI_SETTINGS

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GL_DEBUG_CONTEXT_B

    GL_PRINT_INFOS

    IMGUI_DEPS_A

    APP_UNICODE

    return 0;
}


extern "C"
void sdl3_tick() {
    MOUSE_MOTION_B

    EVENTS_B

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            app_keep = 0;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            app_keep = 0;

        EVENTS_C

        MOUSE_MOTION_C
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    {
        //SDL_Delay(10);

        APP_DELAY
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    IMGUI_DEPS_B
}

extern "C"
void sdl3_swap() {
    ImGuiIO& io = ImGui::GetIO();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    //glClearColor(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
    //glClear(GL_COLOR_BUFFER_BIT);

    APP_IF_UI_VISIBLE

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, glContext) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    IMGUI_IF_FRAME3

    SDL_GL_SwapWindow(window);
}

extern "C"
void sdl3_quit(void) {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

APP_SDL3_AUTORUN
