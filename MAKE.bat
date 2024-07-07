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

export args=
export cc=cc
export o=-o

if [ "$(uname)" != "Darwin" ]; then
fi

if [ "$(uname)" = "Darwin" ]; then
fi

!cc! !o! hello.exe hello0.c -I src src/engine.c !args! || exit 1

exit


:: -----------------------------------------------------------------------------
:windows

@echo off
cd /d "%~dp0"

rem tidy environment
if "%1"=="tidy" (
    del *.mem                       > nul 2> nul
    del *.exp                       > nul 2> nul
    del *.lib                       > nul 2> nul
    del *.exe                       > nul 2> nul
    del *.obj                       > nul 2> nul
    del *.o                         > nul 2> nul
    del *.a                         > nul 2> nul
    del *.pdb                       > nul 2> nul
    del *.ilk                       > nul 2> nul
    rd /q /s .vs                    > nul 2> nul
    del cook*.csv                   > nul 2> nul
    exit /b
)

rem sync
if "%1"=="sync" (
    git reset --hard HEAD~1 && git pull
    call MAKE.bat tidy
    exit /b
)

rem Compiler detection
if "%cc%"=="" (
    echo Detecting VS 2022/2019/2017/2015/2013 x64 ...
    set "cc=cl" && where /q cl.exe || (
               if exist "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=22"
        ) else if exist "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=19"
        ) else if exist "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=17"
        ) else if exist "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" (
                  @call "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" >nul && set "vs=15"
        ) else if exist "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" (
                  @call "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" >nul && set "vs=13"
        ) else if exist "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=22"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul && set "vs=17"
        ) else (
            echo Detecting Mingw64 ...
            set "cc=gcc" && where /q gcc.exe || (
                echo Detecting TCC ... && set "cc=tcc"
            )
        )
    )
)
if "%cc%"=="cl" (
           if exist "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=22"
    ) else if exist "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=19"
    ) else if exist "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=17"
    ) else if exist "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" (set "vs=15"
    ) else if exist "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" (set "vs=13"
    ) else if exist "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=22"
    ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=19"
    ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" (set "vs=17"
    ) else set "vs=00"
    set o=/Fe:
) else (
    set o=-o
)

set args=

setlocal enableDelayedExpansion

!cc! !o! hello.exe hello0.c -I src src/engine.c !args! || set rc=1

rem PAUSE only if double-clicked from Windows explorer
(((echo.%cmdcmdline%)|%WINDIR%\system32\find.exe /I "%~0")>nul)&&pause

cmd /c exit !rc!
