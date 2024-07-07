if "%1"=="tidy" (
	del *.ilk
	del *.pdb
	del *.obj
	del *.exe
	exit /b
)

for %%i in (*.c) do cl %%i -I ..\include ..\src\glfm_desktop.c -I deps -D_USE_MATH_DEFINES /Zi /nologo %*

cl test*.c -I ..\include ..\src\glfm_desktop.c -I deps -D_USE_MATH_DEFINES /Zi /nologo %*
