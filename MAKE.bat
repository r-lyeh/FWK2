#!/bin/bash 2>nul || goto :windows

# linux + osx -----------------------------------------------------------------
cd `dirname $0`

# tidy environment
if [ "$1" = "tidy" ]; then
    rm a.out 2> /dev/null
    rm engine.o 2> /dev/null
    exit
fi

# sync
if [ "$1" = "sync" ]; then
    git reset --hard HEAD^1 && git pull
    sh MAKE.bat tidy
    exit
fi

# fuse
## cat hello cook.zip > fused
## chmod +x fused

export args=
export cc=cc
export o=-o

if [ "$(uname)" != "Darwin" ]; then
export exe=.linux
fi

if [ "$(uname)" = "Darwin" ]; then
export exe=.osx
fi

echo !cc! !o! hello!exe! hello.c -I code code/engine.c !args! || exit 1
echo Compared to v1, osx/linux support is dropped till further notice.

exit


:: -----------------------------------------------------------------------------
:windows

@echo off
cd /d "%~dp0"

set "help="
if "%1"=="help" set help=1
if "%1"=="-h"   set help=1
if "%1"=="-?"   set help=1
if "%1"=="/?"   set help=1
if defined help (
    echo %0 [tidy]
    echo %0 [sync]
    echo %0 [plug] [dir^|add^|del^|syn] [*^|owner/repo^|@filelist]
    echo %0 [asan] [debug^|devel^|release^|retail] [embed] [file1.c file2.cc ...]
    exit /b
)

rem tidy environment
if "%1"=="tidy" (
    del *.def                       > nul 2> nul
    del *.mem                       > nul 2> nul
    del *.exp                       > nul 2> nul
    del *.lib                       > nul 2> nul
    del *.exe                       > nul 2> nul
    del *.obj                       > nul 2> nul
    del *.o                         > nul 2> nul
    del *.a                         > nul 2> nul
    del *.pdb                       > nul 2> nul
    del *.ilk                       > nul 2> nul
    del *.log                       > nul 2> nul
    rd /q /s .vs                    > nul 2> nul
    del cook*.csv                   > nul 2> nul
    del cook.zip                    > nul 2> nul
    del *.dll                       > nul 2> nul
    del .settings.ini               > nul 2> nul
    del .log.txt                    > nul 2> nul
    del ";*"                        > nul 2> nul
    del code\game\embed\engine.ffi  > nul 2> nul
    rd /q /s include                > nul 2> nul
    rd /q /s lib                    > nul 2> nul
    rd /q /s .embed                 > nul 2> nul

    for /R art %%i in (.*) do del "%%i" > nul 2> nul
    for /R lab %%i in (.*) do del "%%i" > nul 2> nul
    for /R lab0 %%i in (.*) do del "%%i" > nul 2> nul
    for /R demos %%i in (.*) do del "%%i" > nul 2> nul
    for /R code %%i in (.*) do del "%%i" > nul 2> nul
    for /R ext %%i in (.*) do del "%%i" > nul 2> nul
    exit /b
)

rem sync
if "%1"=="sync" (
    echo Updating all folders to latest commit
    git reset --hard HEAD~1 && git pull
    call MAKE.bat tidy
    exit /b
)

rem compiler detection
if "%cc%"=="" (
    echo Detecting VS 2022/2019/2017/2015/2013 x64 ...
    (set "cc=cl" && where /q cl.exe) || (

               if exist "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=15"
        ) else if exist "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=13"
        ) else (
            echo Detecting Mingw64 ...
            (set "cc=gcc" && where /q gcc.exe) || (
                echo Detecting TCC ... && set "cc=tcc"
            )
        )
    )
)

rem Determinism notes (see: Jolt)
rem - Compile your application mode in Precise mode (clang: -ffp-model=precise, MSVC: /fp:precise)
rem - Turn off floating point contract operations (clang: -ffp-contract=off). See also: -ffloat-store and -fexcess-precision

if "%cc%"=="cl" (
    set o=/Fe:
    set args=/Zi /MT /EHsc /MP /nologo /fp:precise
) else (
    set o=-o
    set args=-ffp-model=precise -ffp-contract=off
)

setlocal enableDelayedExpansion

rem PAUSE at EOF only if double-clicked from Windows explorer
set "needs_pause="
(((echo .%cmdcmdline% | find /i "/c")>nul) && set "needs_pause=1")
(((echo .%cmdcmdline% | find /i "VsDevCmd")>nul) && set "needs_pause=")

set "tier=devel"
set "ffis=code\engine.h"

:parse_args
if not "%1"=="" (
    if "%1"=="embed" (
        pushd code
        call embed
        popd
    ) else if "%1"=="plug" (
        pushd ext
        call %*
        popd
        exit /b
    ) else if "%1"=="docs" (
        rem set symbols...
            (git describe --tags --abbrev=0 > info.obj) || (echo.0>info.obj)
            set /p VERSION=<info.obj
            git rev-list --count --first-parent HEAD > info.obj
            set /p GIT_REVISION=<info.obj
            git rev-parse --abbrev-ref HEAD > info.obj
            set /p GIT_BRANCH=<info.obj
            date /t > info.obj
            set /p LAST_MODIFIED=<info.obj
        rem ...and generate docs
            call make .github\docs.c
            docs code\engine.h --excluded=3rd_glad.h,engine.h,engine_compat.h, 2> .github\docs.html
        exit /b
    ) else if "%1"=="asan" (
        set args=!args! /fsanitize=address
    ) else if "%1"=="debug" (
        set "tier=debug"
    ) else if "%1"=="devel" (
        set "tier=devel"
    ) else if "%1"=="release" (
        set "tier=release"
    ) else if "%1"=="retail" (
        set "tier=retail"
    ) else if "%1"=="o0" (
        set "tier=debug"
    ) else if "%1"=="O0" (
        set "tier=debug"
    ) else if "%1"=="o1" (
        set "tier=devel"
    ) else if "%1"=="O1" (
        set "tier=devel"
    ) else if "%1"=="o2" (
        set "tier=release"
    ) else if "%1"=="O2" (
        set "tier=release"
    ) else if "%1"=="o3" (
        set "tier=retail"
    ) else if "%1"=="O3" (
        set "tier=retail"
    ) else if exist "%1" (
        rem set objs=!objs! %1
        for %%i in ("%1") do (
            set "is_h="
            if "%%~xi"==".h"   set is_h=1
            if "%%~xi"==".H"   set is_h=1
            if "%%~xi"==".hh"  set is_h=1
            if "%%~xi"==".HH"  set is_h=1
            if "%%~xi"==".hpp" set is_h=1
            if "%%~xi"==".HPP" set is_h=1
            if defined is_h (set ffis=!ffis! %1) else (set objs=!objs! %1)
        )
    ) else (
        set args=!args! %1
    )
    shift
    goto parse_args
)
if "!objs!"=="" (
    set objs=!o! hello.exe hello.c
)

if "!tier!" == "debug" (
    set args=!args! /DEBUG /Od
)
if "!tier!" == "devel" (
    set args=!args! /DNDEBUG=1
)
if "!tier!" == "release" (
    set args=!args! /DNDEBUG=2 /Os /Ox /O2 /Oy /GL /GF /Gs
    set libs=!libs! /OPT:ICF,3
)
if "!tier!" == "retail" (
    set args=!args! /DNDEBUG=3 /Os /Ox /O2 /Oy /GL /GF /Gs /Gw /arch:AVX2
    set libs=!libs! /OPT:ICF,3 /LTCG
)

rem generate ext/ext.h file
pushd ext & call make & popd
rem copy dlls/libs to root folder
for /D %%i in (ext\*) do if exist %%i\x64\* xcopy /ys %%i\x64\* >nul 2>nul

rem collect include folders iteratively
for /D %%i in (ext\*) do if exist "%%i\inc"     set exts=!exts! -I%%i\inc
for /D %%i in (ext\*) do if exist "%%i\include" set exts=!exts! -I%%i\include
for /D %%i in (ext\*) do if exist "%%i\x64"     set libs=!libs! /LIBPATH:%%i\x64
set args=!exts! !args!

rem same than above, but for private extensions now
if exist ext-private (
    rem generate ext-private/ext.h file
    pushd ext-private & call make & popd
    rem copy dlls/libs to root folder
    for /D %%i in (ext-private\*) do if exist %%i\x64\* xcopy /ys %%i\x64\* >nul 2>nul

    rem collect include folders iteratively
    for /D %%i in (ext-private\*) do if exist "%%i\inc"     set exts=!exts! -I%%i\inc
    for /D %%i in (ext-private\*) do if exist "%%i\include" set exts=!exts! -I%%i\include
    for /D %%i in (ext-private\*) do if exist "%%i\x64"     set libs=!libs! /LIBPATH:%%i\x64
    set args=!exts! !args!
)

rem since it takes a lot of time, compile dependencies at least once a day. hopefully ok for most users.
rem get system date and remove '/' chars on it. also remove spaces present on some configs.
set datestr=%date%
set datestr=%datestr:/=%
set datestr=%datestr:-=%
set datestr=%datestr:.=%
set datestr=%datestr: =%

set old=%time%

set "rebuild="
if not exist  native-!datestr!.obj set rebuild=1
if not exist backend-!datestr!.obj set rebuild=1

rem generate FFI bindings
if defined rebuild (
echo !ffis!
set defs=-D__forceinline= -DSDL_DISABLE_ANALYZE_MACROS -DINT_MAX=2147483647 -DAPI= -DHAS_BITFIELDS=0 
echo /* !datestr!: Auto-generated file. Do not edit */ > code\game\embed\engine.ffi
echo vs%vs%: !cc! -EP -Icode -I. -Icode\3rd\nil !ffis! !defs! !args!
             !cc! -EP -Icode -I. -Icode\3rd\nil !ffis! !defs! !args! |^
findstr /R /C:"[^ ]" |^
find /V "static " |^
find /V "__pragma(" |^
find /V "#pragma once" |^
find /V "#pragma warning" |^
find /V "#pragma comment" |^
find /V "#pragma region" |^
find /V "#pragma endregion" >> code\game\embed\engine.ffi
)

if defined rebuild (
echo vs%vs%: !cc! -Icode -I. /c code\native.c code\backend.cc !args!
             !cc! -Icode -I. /c code\native.c code\backend.cc !args! || set rc=1
move /y native.obj   native-!datestr!.obj >nul && rem >2 nul
move /y backend.obj backend-!datestr!.obj >nul && rem >2 nul
)

echo vs%vs%: !cc! -Icode -I. !objs! native-!datestr!.obj backend-!datestr!.obj code\engine.c !args! /link !libs!
             !cc! -Icode -I. !objs! native-!datestr!.obj backend-!datestr!.obj code\engine.c !args! /link !libs! || set rc=1

set now=%time%
set "old=!old::0=: !"
set "old=!old:,0=: !"
set "old=!old:.0=: !"
set "now=!now::0=: !"
set "now=!now:,0=: !"
set "now=!now:.0=: !"
set cs0=!old:~0,2!*3600*100+!old:~3,2!*60*100+!old:~6,2!*100+!old:~9,2!
set cs1=!now:~0,2!*3600*100+!now:~3,2!*60*100+!now:~6,2!*100+!now:~9,2!
set /A diff=!cs1!-^(!cs0!^)
set diff=!diff:~0,-2!.!diff:~-2!
if defined rebuild (echo !diff!s ^(cold build^) ^(next build will be faster^)) else (echo !diff!s ^(warm build^))

:eof
if defined needs_pause ( pause )

cmd /c exit !rc!
