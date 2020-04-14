@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

REM https://blog.spreendigital.de/2019/06/25/how-to-compile-lua-5-3-5-for-windows/
REM build lua from sources

cd lua
cl /MD /O2 /c /DLUA_BUILD_AS_DLL *.c
ren lua.obj lua.o
ren luac.obj luac.o
link /DLL /IMPLIB:lua.lib /OUT:lua.dll *.obj
link /OUT:lua.exe lua.o lua.lib
del /S *.obj
del /S *.o
cd ..

REM run lua script to continue making projects

call "lua\lua" "utils\build.lua"