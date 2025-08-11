@echo off
echo ===============================================
echo AronEngine 강제 리빌드 스크립트
echo ===============================================
echo.
echo STEP 1: Visual Studio 종료 확인
echo IMPORTANT: Visual Studio를 모두 닫은 후 계속하세요!
pause
echo.

echo STEP 2: 모든 중간 파일 및 캐시 삭제
echo Deleting build artifacts...

REM 빌드 디렉토리 삭제
if exist "x64" rmdir /s /q "x64"
if exist "Engine\x64" rmdir /s /q "Engine\x64"  
if exist "Game\x64" rmdir /s /q "Game\x64"
if exist "External\x64" rmdir /s /q "External\x64"

REM Visual Studio 캐시 삭제
if exist ".vs" rmdir /s /q ".vs"

REM 중간 파일들 삭제
del /s /q *.pdb 2>nul
del /s /q *.ilk 2>nul
del /s /q *.tlog 2>nul
del /s /q *.idb 2>nul
del /s /q *.pch 2>nul
del /s /q *.ipch 2>nul
del /s /q *.obj 2>nul
del /s /q *.lib 2>nul
del /s /q *.exp 2>nul

echo 중간 파일들이 삭제되었습니다.
echo.

echo STEP 3: 헤더 파일 타임스탬프 업데이트
echo Updating header file timestamps...
for /r %%f in (*.h) do (
    copy /b "%%f"+,, "%%f" >nul 2>nul
)
for /r %%f in (*.cpp) do (
    copy /b "%%f"+,, "%%f" >nul 2>nul
)

echo 모든 파일 타임스탬프가 업데이트되었습니다.
echo.

echo STEP 4: 프로젝트 의존성 재설정
echo Regenerating project dependencies...

REM 솔루션 파일 타임스탬프 업데이트
if exist "AronEngine.sln" (
    copy /b "AronEngine.sln"+,, "AronEngine.sln" >nul
)

echo.
echo ===============================================
echo 강제 리빌드 준비 완료!
echo ===============================================
echo.
echo 이제 다음 단계를 수행하세요:
echo 1. Visual Studio 2022 열기
echo 2. AronEngine.sln 열기
echo 3. 솔루션 탐색기에서 솔루션 우클릭
echo 4. "솔루션 정리" 선택
echo 5. "솔루션 다시 빌드" 선택
echo.
echo 빌드가 완료되면 Game 프로젝트를 실행하세요.
echo.
pause