@echo off
rem Usage: build.bat [debug|release] [app|sim|all]
setlocal enabledelayedexpansion
cd /d "%~dp0"
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=debug
set TARGET=%2
if "%TARGET%"=="" set TARGET=all

if not defined VCINSTALLDIR (
  call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
)

if not exist bin mkdir bin
set COMMON=/nologo /utf-8 /std:c++20 /EHsc /W3 /wd4828 /fp:precise /external:W0 /external:Ithird_party\dxlib /MP /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /Ithird_party\dxlib /Isrc
if "%CONFIG%"=="release" (
  set FLAGS=%COMMON% /O2 /MT /DNDEBUG
  set SUFFIX=
) else (
  set FLAGS=%COMMON% /Od /Zi /MTd /D_DEBUG
  set SUFFIX=_d
)

set GAMESRC=
for %%f in (src\game\*.cpp src\core\*.cpp) do set GAMESRC=!GAMESRC! %%f
set AISRC=src\sim\Policy.cpp src\sim\RunPolicy.cpp

set FAIL=0
if "%TARGET%"=="sim" goto sim
if "%TARGET%"=="all" goto app
if "%TARGET%"=="app" goto app
goto end

:app
set OBJ=obj\%CONFIG%_app
if not exist %OBJ% mkdir %OBJ%
set APPSRC=
for %%f in (src\app\*.cpp) do set APPSRC=!APPSRC! %%f
rc /nologo /fo %OBJ%\app.res res\app.rc
cl %FLAGS% /Fo%OBJ%\ /Fd%OBJ%\ %APPSRC% %GAMESRC% %AISRC% %OBJ%\app.res /Febin\CardForge%SUFFIX%.exe /link /LIBPATH:third_party\dxlib /SUBSYSTEM:WINDOWS /IGNORE:4099
if errorlevel 1 (set FAIL=1& echo BUILD FAILED: app) else (echo BUILD OK: bin\CardForge%SUFFIX%.exe)
if "%TARGET%"=="app" goto end

:sim
set OBJ=obj\%CONFIG%_sim
if not exist %OBJ% mkdir %OBJ%
cl %FLAGS% /Fo%OBJ%\ /Fd%OBJ%\ %GAMESRC% %AISRC% src\sim\SimMain.cpp /Febin\CardForgeSim%SUFFIX%.exe /link /SUBSYSTEM:CONSOLE
if errorlevel 1 (set FAIL=1& echo BUILD FAILED: sim) else (echo BUILD OK: bin\CardForgeSim%SUFFIX%.exe)

:end
exit /b %FAIL%
