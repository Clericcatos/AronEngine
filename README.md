# NEWARONENGINE

AronEngine을 기반으로 FMOD 오디오 시스템을 통합한 새로운 게임 엔진입니다.

## 주요 변경사항

### 오디오 시스템
- DirectSound 제거
- FMOD Core API 통합
- 2D/3D 오디오 지원
- 실시간 볼륨/피치 조절
- 스트리밍 및 샘플 재생 지원
- 다양한 오디오 포맷 지원 (WAV, MP3, OGG, FLAC)

### 개선된 컴포넌트
- **AudioSource**: FMOD 기반 오디오 재생 컴포넌트
- **AudioClip**: 개선된 오디오 리소스 관리
- **AudioManager**: FMOD 시스템 관리

## FMOD 설치 방법

1. FMOD 다운로드 (https://www.fmod.com/download)
2. FMOD SDK 압축 해제
3. 다음 파일들을 복사:

### 헤더 파일 (ThirdParty/FMOD/inc/):
- fmod.hpp
- fmod.h
- fmod_codec.h
- fmod_common.h
- fmod_dsp.h
- fmod_errors.h
- fmod_output.h

### 라이브러리 파일 (ThirdParty/FMOD/lib/):
- fmod_vc.lib (Release용)
- fmodL_vc.lib (Debug용)
- fmod.dll
- fmodL.dll

## 빌드 방법

1. Visual Studio 2022로 NEWARONENGINE.sln 열기
2. Debug/Release x64 구성 선택
3. 솔루션 빌드

## 실행 방법

컴파일 후 x64/Debug/ 또는 x64/Release/ 폴더에서:
- Game.exe 실행
- fmod.dll (또는 fmodL.dll) 파일이 같은 폴더에 있어야 함

## 조작법

- **WASD/방향키**: 플레이어 이동 및 점프
- **T**: 오디오 재생 테스트
- **M**: 오디오 정지
- **L**: 루프 모드 토글
- **E**: 레벨 에디터 토글
- **ESC**: 종료

## 구조

```
NEWARONENGINE/
├── Engine/              # 엔진 코어 라이브러리
│   ├── Core/           # 기본 시스템
│   ├── Components/     # 게임 컴포넌트
│   ├── Managers/       # 시스템 매니저
│   ├── Resources/      # 리소스 관리
│   ├── Systems/        # 렌더링/스크립트 시스템
│   ├── Editor/         # 레벨 에디터
│   └── Utils/          # 유틸리티
├── Game/               # 테스트 애플리케이션
├── Assets/             # 게임 에셋
│   ├── Audio/
│   ├── Textures/
│   └── Scenes/
└── ThirdParty/         # 외부 라이브러리
    ├── FMOD/          # FMOD 오디오 라이브러리
    └── ImGui/         # ImGui UI 라이브러리
```

## 문제 해결

- FMOD 관련 링크 오류: FMOD SDK가 올바르게 설치되었는지 확인
- 오디오 재생 안됨: 오디오 파일 경로 및 포맷 확인
- 에디터 실행 안됨: ImGui 라이브러리 빌드 확인