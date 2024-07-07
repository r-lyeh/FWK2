#!/bin/bash 2>nul || goto :windows

# linux + osx -----------------------------------------------------------------
cd `dirname $0`
exit

: -----------------------------------------------------------------------------
:windows
@echo off

setlocal enabledelayedexpansion

pushd .
cd /d "%~dp0"

rem show ms redist dependences in any dll
rem for /R %%i in (*.exe;*.dll) do dumpbin /dependents %%i |find /i "vcr"
rem copy dlls/libs to root folder
rem for /D /R %%i in (*) do if exist %%i\x64\* xcopy /ys %%i\x64\* .. >nul 2>nul

rem generate ext.h file
echo // auto-generated file. do not edit  > ext.h
echo // list of extensions included here >> ext.h
echo. >> ext.h
for /D /R %%i in (*) do if exist %%i\api.h (
set DIR=%%i
set DIR=!DIR:%CD%\=!
set DIR=!DIR:\=/!
echo #if __has_include^("!DIR!/api.h"^) >> ext.h
echo #         include "!DIR!/api.h" >> ext.h
echo #endif >> ext.h
)

rem generate ext.hh file
echo // auto-generated file. do not edit  > ext.hh
echo // list of extensions included here >> ext.hh
echo. >> ext.hh
rem echo #ifdef __cplusplus >> ext.hh
echo #pragma push_macro("set") >> ext.hh
echo #pragma push_macro("map") >> ext.hh
echo #pragma push_macro("array") >> ext.hh
echo #undef set >> ext.hh
echo #undef map >> ext.hh
echo #undef array >> ext.hh
for /D /R %%i in (*) do if exist %%i\api.hh (
set DIR=%%i
set DIR=!DIR:%CD%\=!
set DIR=!DIR:\=/!
echo #if __has_include^("!DIR!/api.hh"^) >> ext.hh
echo #         include "!DIR!/api.hh" >> ext.hh
echo #endif >> ext.hh
)
echo #pragma pop_macro("set") >> ext.hh
echo #pragma pop_macro("map") >> ext.hh
echo #pragma pop_macro("array") >> ext.hh
rem echo #endif >> ext.hh

rem generate ext-demos.c file
echo // auto-generated file. do not edit  > ext-demos.c
echo // list of extensions included here >> ext-demos.c
echo. >> ext-demos.c
for /D /R %%i in (*) do if exist %%i\demo.c (
set DIR=%%i
set DIR=!DIR:%CD%\=!
set DIR=!DIR:\=/!
echo #if __has_include^("!DIR!/demo.c"^) >> ext-demos.c
echo #         include "!DIR!/demo.c" >> ext-demos.c
echo #endif >> ext-demos.c
)

popd

endlocal
