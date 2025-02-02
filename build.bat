@echo off

cl

if %ERRORLEVEL% neq 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

cls

rmdir /S /Q .\build
mkdir build
pushd .\build

cl /nologo /diagnostics:caret /WX /W4 /wd4189 /wd4996 /wd4100 /wd4244 ..\embed.c

embed.exe

@REM cl /nologo /diagnostics:caret /WX /W4 /wd4189 /wd4996 /wd4100 /wd4244 ..\main.c
cl /nologo /Gw /GL /O2 ..\main.c

main.exe

popd