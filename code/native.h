#pragma once

// expose native apis
#if _WIN32
#include <winsock2.h>
#endif

// expose sdl3 api
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

//#include "ext/ext-backend-imgui/api.h"

// expose 3rd party libs (stb_image, etc)
#include "3rd/3rd.h"
#include "ext/ext-backend-luajit/api.h"
