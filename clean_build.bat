@echo off
echo Cleaning AronEngine project...

REM Clean intermediate files
echo Deleting intermediate files...
if exist "x64" rmdir /s /q "x64"
if exist "Engine\x64" rmdir /s /q "Engine\x64"
if exist "Game\x64" rmdir /s /q "Game\x64"
if exist ".vs" rmdir /s /q ".vs"

REM Clean any .obj files
echo Deleting object files...
del /s /q *.obj 2>nul
del /s /q *.pdb 2>nul
del /s /q *.ilk 2>nul
del /s /q *.idb 2>nul

echo Clean complete!
echo.
echo Please rebuild the solution in Visual Studio now.
pause