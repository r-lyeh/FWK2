if "%1"=="tidy" (
	del *.lib
	del *.exp
	del *.obj
	exit /b
)

cl /c imgui.cc -I ..\..\code -I ..\ext-backend-sdl3\inc -I inc -DCODE /EHsc /O2 /MT /DNDEBUG &&^
lib /nologo /out:x64\imgui.lib imgui.obj
