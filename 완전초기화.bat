@echo off
echo ========================================
echo AronEngine 완전 초기화 및 리빌드
echo ========================================
echo.

echo 1단계: Visual Studio 완전 종료 확인
echo 중요: 모든 Visual Studio 인스턴스를 닫아주세요!
pause
echo.

echo 2단계: 모든 빌드 아티팩트 삭제
echo 빌드 디렉토리 삭제 중...
if exist "x64" rmdir /s /q "x64"
if exist "Engine\x64" rmdir /s /q "Engine\x64"  
if exist "Game\x64" rmdir /s /q "Game\x64"
if exist "External\x64" rmdir /s /q "External\x64"
if exist "ThirdParty\x64" rmdir /s /q "ThirdParty\x64"

echo Visual Studio 캐시 삭제 중...
if exist ".vs" rmdir /s /q ".vs"

echo 임시 파일들 삭제 중...
del /s /q *.pdb 2>nul
del /s /q *.ilk 2>nul
del /s /q *.tlog 2>nul
del /s /q *.idb 2>nul
del /s /q *.pch 2>nul
del /s /q *.ipch 2>nul
del /s /q *.obj 2>nul
del /s /q *.lib 2>nul
del /s /q *.exp 2>nul
del /s /q *.suo 2>nul
del /s /q *.user 2>nul
del /s /q *.ncb 2>nul
del /s /q *.sdf 2>nul
del /s /q *.opensdf 2>nul

echo.
echo 3단계: IntelliSense 데이터베이스 완전 삭제
for /f "delims=" %%d in ('dir /b /ad /s Browse.VC.db* 2^>nul') do rmdir /s /q "%%d" 2>nul
for /f "delims=" %%f in ('dir /b /s Browse.VC.db* 2^>nul') do del /f /q "%%f" 2>nul

echo.
echo 4단계: 소스 파일 타임스탬프 강제 업데이트
echo 모든 헤더 파일 업데이트...
for /r %%f in (*.h) do (
    echo. >> "%%f"
)

echo 모든 소스 파일 업데이트...
for /r %%f in (*.cpp) do (
    echo. >> "%%f"
)

echo.
echo 5단계: 프로젝트 파일 타임스탬프 업데이트
if exist "AronEngine.sln" (
    echo. >> "AronEngine.sln"
)
for /r %%f in (*.vcxproj) do (
    echo. >> "%%f"
)

echo.
echo ========================================
echo 완전 초기화 완료!
echo ========================================
echo.
echo 이제 다음 단계를 수행하세요:
echo.
echo 1. Visual Studio 2022 실행
echo 2. AronEngine.sln 열기
echo 3. 도구 메뉴 → 옵션 → 텍스트 편집기 → C/C++ → 고급
echo 4. "IntelliSense 사용 안 함"을 "False"로 설정
echo 5. "정보 데이터베이스 재검색" 클릭
echo 6. Visual Studio 재시작
echo 7. 솔루션 탐색기에서 솔루션 우클릭
echo 8. "솔루션 정리" 선택
echo 9. "솔루션 다시 빌드" 선택
echo 10. Game 프로젝트를 시작 프로젝트로 설정
echo 11. F5로 실행
echo.
pause