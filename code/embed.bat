@echo off
cd /d "%~dp0"

if not exist ..\bin2c.exe cl bin2c.c /Fe..\bin2c.exe /MT
if not exist ..\bin2c.exe exit /b

del ..\.embed\res.c >nul 2>nul
md ..\.embed >nul 2>nul

setlocal EnableDelayedExpansion

FOR %%A IN ("%~dp0.") DO SET parent=%%~dpA

for /R .. %%j in (*) do (
    set "str1=%%j"
    if not "x!str1:\embed\=!"=="x!str1!" for %%i in (!str1!) do (
        if exist "%%i" (
            rem normalize absolute path, by using forward slashes
            set "p=%%~fi"
            set "p=!p:%parent%=!"
            set "p=!p:\=/!"

            rem normalize id, by replacing period and slashes
            set "id=!p!"
            set "id=!id:-=_!"
            set "id=!id:/=_!"
            set "id=!id:.=_!"
         
            echo %%i
            rem echo !p!
            rem echo !id!

            ..\bin2c.exe %%i ..\.embed\!id! !id! && (
            echo #if __has_include^("!id!"^)>> ..\.embed\res.c
            echo #include "!id!">> ..\.embed\res.c
            echo #endif>> ..\.embed\res.c
            )
        )
    )
)

echo struct resource_t { const char *name, *data; unsigned size; } resources[] = {>> ..\.embed\res.c

for /R .. %%j in (*) do (
    set "str1=%%j"
    if not "x!str1:\embed\=!"=="x!str1!" for %%i in (!str1!) do (
        if exist "%%i" (
            rem normalize absolute path, by using forward slashes
            set "p=%%~fi"
            set "p=!p:%parent%=!"
            set "p=!p:\=/!"

            rem normalize id, by replacing period and slashes
            set "id=!p!"
            set "id=!id:-=_!"
            set "id=!id:/=_!"
            set "id=!id:.=_!"
         
            echo %%i
            rem echo !p!
            rem echo !id!

            echo #if __has_include^("!id!"^)>> ..\.embed\res.c
            echo { "embed!p!", !id!, ^(unsigned^)sizeof^(!id!^) },>> ..\.embed\res.c
            echo #endif>> ..\.embed\res.c
        )
    )
)

echo { NULL, NULL, 0u },>> ..\.embed\res.c
echo };>> ..\.embed\res.c
echo.>> ..\.embed\res.c
rem type ..\.embed\res.c

rem del ..\.embed\bin2c_obj >nul 2>nul
rem del ..\.embed\bin2c_exe >nul 2>nul

del  .\bin2c.obj >nul 2>nul
del ..\bin2c.exe >nul 2>nul
