#define API extern "C" __declspec(dllexport)
#include "3rd/3rd_icon_ms.h"
#define UI_ICON(GLYPH) ICON_MS_##GLYPH

#include <SDL3/SDL.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USE_WCHAR32

#include "cimgui/imgui/imgui_impl_opengl3.cpp"
#include "cimgui/imgui/imgui_impl_sdl3.cpp"

#include "cimgui/cimgui.cpp"
#include "cimgui/imgui/imgui_impl_sdl3.h"
#include "cimgui/imgui/imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "cimgui/imgui/imgui.cpp"
#include "cimgui/imgui/imgui_demo.cpp"
#include "cimgui/imgui/imgui_draw.cpp"
#include "cimgui/imgui/imgui_tables.cpp"
#include "cimgui/imgui/imgui_widgets.cpp"

//#include "inc/3rd_imLegitProfilerTask.h"
//#include "inc/3rd_imLegitProfiler.h"
#include "inc/3rd_imNodeFlow.hh"
#include "inc/3rd_imKnobs.hh"
#include "inc/3rd_imNotify.hh"
#include "inc/3rd_imPlot.hh"
#include "inc/3rd_imSequencer.hh"
#include "3rd/3rd_stb_image.h"
#include "inc/3rd_imFileDialog.hh"

extern "C" API void *app_handle(const char *);
#include "cimplot/cimplot.cpp"
#include "cimguizmo/cimguizmo.cpp"
#include "cimguizmo/ImGuizmo/ImGuizmo.cpp"
//#include "inc/3rd_imGuizmo.hh"
#pragma comment(lib, "opengl32") // needed?

#if (RLYEH_MOD1+RLYEH_MOD2+RLYEH_MOD3+RLYEH_MOD4) != 4
#error DearImgui integrator, did you forget to integrate any of our custom modifications?
#endif
