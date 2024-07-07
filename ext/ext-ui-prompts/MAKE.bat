    cl src/make-controllerimage-data.c                  %* -I src -I ..\ext-backend-sdl3\inc ..\ext-backend-sdl3\x64\SDL3.lib 
rem cl src/demo-controllerimage.c src/controllerimage.c %* -I src -I ..\ext-backend-sdl3\inc ..\ext-backend-sdl3\x64\SDL3.lib 
rem cl src/test-controllerimage.c src/controllerimage.c %* -I src -I ..\ext-backend-sdl3\inc ..\ext-backend-sdl3\x64\SDL3.lib 
rem copy /y ..\ext-backend-sdl3\x64\SDL3.dll

make-controllerimage-data src\art
move /y *.bin embed
