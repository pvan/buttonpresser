@echo off

IF NOT EXIST build mkdir build


if not defined DevEnvDir (
    call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)



set WarningFlags= -WX -W4 -wd4201 -wd4100

REM  -WX     (treat warnings as errors)
REM  -wd---- (disable ---- warnings)
REM  -we---- (treat ---- warnings as errors)
REM  -wd4201 (nameless struct/union)
REM  -wd4100 (unused parameters)
REM  -wd4189 (unused local var)
REM  -wd4244 (implicit cast)
REM  -wd4505 (unused local functions)
REM  -wd4389 (unsigned/sign comparisons)
REM  -we4302 (treat truncation warnings as errors)
REM  -Wv:18  (disable warnings introduced after compiler v18)


REM  -Zi  enable debugging info (.pdb)


pushd build
cl -nologo -Zi %WarningFlags% ..\buttonpresser.cpp %LinkerFlags% user32.lib Winmm.lib
popd


