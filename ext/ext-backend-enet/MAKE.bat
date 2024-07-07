@echo off
cd "%~dp0"

pushd x64
cl ..\lib.c /c /O2 /Oy /MT /DNDEBUG
lib /out:enet.lib lib.obj
del *.obj
popd
