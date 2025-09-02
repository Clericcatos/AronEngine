@echo off
echo AronEngine Build and Run Script
echo.

cd /d "%~dp0"

REM Find MSBuild
set MSBUILD_PATH=
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else (
    echo No MSBuild found, trying devenv...
    set "MSBUILD_PATH=devenv"
)
if "%MSBUILD_PATH%"=="" (
    echo ERROR: Visual Studio not found. Opening solution in default app...
    start "" "NEWARONENGINE.sln"
    pause
    exit /b 1
)

echo Building Engine...
if "%MSBUILD_PATH%"=="devenv" (
    devenv NEWARONENGINE.sln /build "Debug|x64" /project Engine
) else (
    "%MSBUILD_PATH%" Engine\Engine.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:q
)

if %ERRORLEVEL% NEQ 0 (
    echo Engine build failed!
    pause
    exit /b 1
)

echo Building Game...
if "%MSBUILD_PATH%"=="devenv" (
    devenv NEWARONENGINE.sln /build "Debug|x64" /project Game
) else (
    "%MSBUILD_PATH%" Game\Game.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:q
)

if %ERRORLEVEL% NEQ 0 (
    echo Game build failed!
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESS! Running game...
echo Controls: Q=Quad, T=Triangle, ESC=Exit
echo.

if exist "x64\Debug\Game.exe" (
    start "" "x64\Debug\Game.exe"
) else (
    echo Game.exe not found!
    pause
)