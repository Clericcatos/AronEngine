@echo off
echo Testing AronEngine Build...

REM Try to find MSBuild
set "MSBUILD_PATH="
for /f "tokens=*" %%i in ('where msbuild 2^>nul') do set "MSBUILD_PATH=%%i"

if "%MSBUILD_PATH%"=="" (
    echo MSBuild not found in PATH. Trying to find VS installation...
    
    REM Try VS 2022
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
        set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
        set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
        set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    )
    
    REM Try VS 2019
    if "%MSBUILD_PATH%"=="" (
        if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
            set "MSBUILD_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
        ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" (
            set "MSBUILD_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
        )
    )
)

if "%MSBUILD_PATH%"=="" (
    echo MSBuild not found! Please install Visual Studio or Build Tools.
    pause
    exit /b 1
)

echo Found MSBuild at: %MSBUILD_PATH%
echo Building AronEngine solution...

"%MSBUILD_PATH%" AronEngine.sln /p:Configuration=Debug /p:Platform=x64 /v:minimal

if %ERRORLEVEL%==0 (
    echo.
    echo ==========================================
    echo BUILD SUCCESSFUL!
    echo Korean localization reversion completed.
    echo Project now compiles without errors.
    echo ==========================================
) else (
    echo.
    echo ==========================================
    echo BUILD FAILED!
    echo Check the output above for errors.
    echo ==========================================
)

pause