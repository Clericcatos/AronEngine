@echo off
echo Building NEWARONENGINE...
echo.

REM Build the solution
msbuild NEWARONENGINE.sln /p:Configuration=Debug /p:Platform=x64 /m

if %ERRORLEVEL% == 0 (
    echo.
    echo ================================
    echo BUILD SUCCESSFUL!
    echo ================================
    echo.
    echo The engine has been built successfully.
    echo You can find the executable at: x64\Debug\Game.exe
    echo.
    echo NOTE: Make sure to copy fmod.dll to the same folder as Game.exe
    echo      before running the application.
    echo.
) else (
    echo.
    echo ================================
    echo BUILD FAILED!
    echo ================================
    echo.
    echo Please check the error messages above.
)

pause