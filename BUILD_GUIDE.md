# AronEngine - NDC Quad Rendering Test

## 개요
유니티 스타일 에디터를 위한 엔진 프로젝트입니다.
NDC(Normalized Device Coordinates) 좌표계와 컬러값을 갖는 버텍스를 사용하여 사각형을 그리는 기능이 구현되어 있습니다.

## 빌드 방법

### 방법 1: Visual Studio 사용 (권장)
1. `NEWARONENGINE.sln` 파일을 Visual Studio로 열기
2. 솔루션 구성을 "Debug", 플랫폼을 "x64"로 설정
3. 솔루션 빌드 (Ctrl+Shift+B)

### 방법 2: 배치 파일 사용
- `start.bat` 실행: 게임이 빌드되어 있으면 실행, 없으면 VS 열기

## 실행 및 테스트

### 조작법
- **Q 키**: NDC 좌표계 Quad 테스트 (색상 사각형)
- **T 키**: 삼각형 테스트
- **ESC 키**: 종료

### NDC Quad 테스트 내용
- 좌상단: 빨간색
- 우상단: 초록색  
- 우하단: 파란색
- 좌하단: 노란색

## 구조
```
AronEngine/
├── Engine/          # 엔진 라이브러리
│   ├── Core/        # 핵심 시스템
│   ├── Tests/       # 테스트 클래스들
│   │   ├── Test01_Triangle.h/cpp  # 삼각형 테스트
│   │   └── Test02_Quad.h/cpp      # NDC Quad 테스트
│   └── ...
├── Game/            # 게임 실행파일
├── Editor/          # 에디터 (별도)
└── start.bat        # 실행 스크립트
```

## 기술적 특징
- D3D11 기반 렌더링
- NDC 좌표계 사용 (-1 ~ 1 범위)
- 인덱스 버퍼를 통한 효율적인 사각형 렌더링
- 버텍스별 컬러 지원