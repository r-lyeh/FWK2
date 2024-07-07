if "%1"=="tidy" (
	del *.obj
	del *.exe
	del *.pdb
	del *.ilk
	del *.dll
	exit /b
)

copy /y ..\..\..\ext\ext-backend-sdl3\x64\SDL3.dll

if "%1"=="c" (set glm=glmC) else (set glm=glmCPP)

cl *.cpp -I 3rd -I 3rd\%glm% /std:c++20 /EHsc -I ..\..\..\ext\ext-backend-sdl3\inc\ /link /LIBPATH:..\..\..\ext\ext-backend-sdl3\x64 %*
