#define IMGUI_DEFINE_MATH_OPERATORS

#include <cimgui/imgui/imgui_impl_opengl3.cpp>
#include <cimgui/imgui/imgui_impl_sdl3.cpp>

#include <cimgui/cimgui.cpp>
#include <cimgui/imgui/imgui_impl_sdl3.h>
#include <cimgui/imgui/imgui_impl_opengl3.h>

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <cimgui/imgui/imgui.cpp>
#include <cimgui/imgui/imgui_demo.cpp>
#include <cimgui/imgui/imgui_draw.cpp>
#include <cimgui/imgui/imgui_tables.cpp>
#include <cimgui/imgui/imgui_widgets.cpp>

#pragma comment(lib, "code/3rd/SDL3/x64/SDL3")
#pragma comment(lib, "opengl32") // needed?

//#include "3rd_imLegitProfilerTask.h"
//#include "3rd_imLegitProfiler.h"
#include "3rd_imNodeFlow.hh"
#include "3rd_imKnobs.hh"
#include "3rd_imNotify.hh"
#include "3rd_imPlot.hh"
#include "3rd_stb_image.h"
#ifndef GL_NEAREST // @fixme
#define GL_NEAREST 0x2600
#endif
#ifndef GL_BGRA // @fixme
#define GL_BGRA 0x80E1
#endif
#ifndef glGenerateMipmap // @fixme
#define glGenerateMipmap(target)
#endif
#include "3rd_imFileDialog.hh"
