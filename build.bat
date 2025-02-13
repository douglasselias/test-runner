@echo off

cl

if %ERRORLEVEL% neq 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

cls

rmdir /S /Q .\build
mkdir build
pushd .\build

set ignored_flags=/wd4189 /wd4996 /wd4100 /wd4244 /wd4255 /wd5045 /wd4710 /wd4711
cl /nologo /diagnostics:caret /WX /W4 %ignored_flags%  ..\embed.c

embed

cl /nologo /diagnostics:caret /Wall /WX /W4 %ignored_flags% /Gw /GL /O2 ..\main.c /link /out:test_runner.exe

test_runner

popd