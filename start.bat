@echo off
cd /d "%~dp0"

echo AronEngine - NDC Quad Test
echo.

if exist "x64\Debug\Game.exe" (
    echo Running game...
    echo Controls: Q = Quad Test, T = Triangle Test, ESC = Exit
    echo.
    start "" "x64\Debug\Game.exe"
) else (
    echo Game.exe not found! Please build the project first.
    echo Opening solution file...
    start "" "NEWARONENGINE.sln"
)

pause