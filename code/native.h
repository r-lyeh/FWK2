#pragma once

// expose sdl3 api
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

// expose cimgui api
#ifdef __cplusplus
#include <cimgui/imgui/imgui.h>
#include <cimgui/imgui/imgui_internal.h>
#else
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 0
#endif
#include <cimgui/cimgui.h>

#ifndef __cplusplus
#define ImVec2(...) ( (struct ImVec2) { __VA_ARGS__ } )
#define ImVec4(...) ( (struct ImVec4) { __VA_ARGS__ } )
#endif

// 
EXTERN_C
void igInsertNotification(char type, const char *title, const char *message, float timeout);

EXTERN_C
void igShowImPlotDemoWindow(bool* p_open);

EXTERN_C
bool igKnob(const char *label, float *value, int steps);

// expose native apis
#if _WIN32
#   include <winsock2.h>
#endif
