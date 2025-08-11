@echo off
echo Fixing Visual Studio IntelliSense cache...

REM Close Visual Studio first!
echo IMPORTANT: Make sure Visual Studio is closed before continuing!
pause

REM Delete IntelliSense database
echo Deleting IntelliSense cache...
if exist ".vs\AronEngine\v17\.suo" del /f /q ".vs\AronEngine\v17\.suo"
if exist ".vs\AronEngine\v17\Browse.VC.db" del /f /q ".vs\AronEngine\v17\Browse.VC.db"
if exist ".vs\AronEngine\v17\ipch" rmdir /s /q ".vs\AronEngine\v17\ipch"

REM Delete all precompiled headers
echo Deleting precompiled headers...
del /s /q *.pch 2>nul
del /s /q *.ipch 2>nul

REM Touch all header files to force rebuild
echo Updating header file timestamps...
for /r %%f in (*.h) do (
    copy /b "%%f"+,, "%%f" >nul
)

echo.
echo IntelliSense cache cleared!
echo.
echo Now:
echo 1. Open Visual Studio
echo 2. Open AronEngine.sln
echo 3. Build -> Clean Solution
echo 4. Build -> Rebuild Solution
echo.
pause