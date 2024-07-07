copy /y ..\SDL3\x64\SDL3.dll

cl /Feglfw3.exe imgui.cpp ^
imgui_demo.cpp ^
imgui_draw.cpp ^
imgui_tables.cpp ^
imgui_widgets.cpp ^
imgui_impl_glfw.cpp imgui_impl_opengl3.cpp glfw3.cpp ^
-D_GLFW_WIN32 -I..\glfw ..\glfw\unity.c opengl32.lib gdi32.lib

cl /Fesdl3.exe imgui.cpp ^
imgui_demo.cpp ^
imgui_draw.cpp ^
imgui_tables.cpp ^
imgui_widgets.cpp ^
imgui_impl_sdl3.cpp imgui_impl_opengl3.cpp sdl3.cpp ^
opengl32.lib -I.. ..\SDL3\x64\SDL3.lib
