#if !CODE

#define IMGUI_USE_WCHAR32

#ifdef __cplusplus

#include <cimgui/imgui/imgui.h>
#include <cimgui/imgui/imgui_internal.h>

#include <cimgui/cimgui.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#include <cimplot/cimplot.h>
#include <cimguizmo/cimguizmo.h>

#else

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#include <cimgui/cimgui.h>
#define ImVec2(...) ( (struct ImVec2) { __VA_ARGS__ } )
#define ImVec4(...) ( (struct ImVec4) { __VA_ARGS__ } )
#include <cimplot/cimplot.h>
#include <cimguizmo/cimguizmo.h>

#endif

// expose cimgui api

bool igKnob(const char *label, float *value, int steps);

void igTextWithHoverColor(ImU32 col, ImVec2 indents, const char* text);

void igFileDialogInit();
 int igFileDialogOpen(const char *window_name, const char *info, const char *exts);
 int igFileDialogOpen1(const char *window_name, const char *info, const char *exts);
 int igFileDialogSave(const char *window_name, const char *info, const char *exts);
char **igFileDialogResults(const char *window_name, int *count);
void igFileDialogDemo();

void igNodeFlowDemo();

void igSequencerDemo();

void igGuizmoDemo();

void igInsertNotification(char type, const char *title, const char *message, float timeout);
void igRenderNotifications();

void igShowImPlotDemoWindow(bool* p_open);

int igCurrentWindowStackSize(void);

float igGetWindowPosX(void);
float igGetWindowPosY(void);

void igSpinner(unsigned mode);

void igAddCommandPalette(const char *text, void (*func)());

#endif
