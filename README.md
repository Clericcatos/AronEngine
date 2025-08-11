# AronEngine

Direct2D 기반 2D 게임 엔진

## 주요 특징

- **Direct2D 렌더링**: 하드웨어 가속 2D 그래픽 렌더링
- **컴포넌트 시스템**: Unity 스타일의 GameObject-Component 구조
- **씬 관리**: 다중 씬 지원 및 씬 전환
- **통합 에디터**: 게임 제작을 위한 에디터 시스템 (기본 구조만 구현)
- **입력 처리**: 키보드 및 마우스 입력 시스템
- **리소스 관리**: 텍스처 등 리소스 로딩 및 관리

## 프로젝트 구조

```
AronEngine/
├── Engine/              # 엔진 라이브러리
│   ├── Core/           # 핵심 시스템 (EngineApp, GameObject, Scene 등)
│   ├── Components/     # 컴포넌트들 (Transform, Component 등)
│   ├── Systems/        # 시스템들 (RenderSystem, TransformSystem 등)
│   ├── Managers/       # 관리자들 (SceneManager, ResourceManager)
│   ├── Editor/         # 에디터 시스템
│   ├── Resources/      # 리소스 관련 (Texture2D)
│   └── Utils/          # 유틸리티 (Vector2, Color, Matrix3x2)
├── Game/               # 게임 실행 프로젝트
├── External/           # 외부 라이브러리 (ImGui)
└── README.md
```

## 빌드 요구사항

- Windows 10/11
- Visual Studio 2019 이상
- Windows SDK 10.0
- C++17 지원

## 사용법

### 1. 기본 애플리케이션 생성

```cpp
#include "Core/EngineApp.h"

class MyApp : public AronEngine::EngineApp
{
public:
    virtual void OnInit() override
    {
        // 초기화 코드
    }
    
    virtual void OnUpdate(float deltaTime) override
    {
        // 업데이트 코드
    }
    
    virtual void OnRender() override
    {
        // 렌더링 코드
        auto renderer = GetRenderer();
        renderer->DrawText(L"Hello, AronEngine!", Vector2(100, 100), Color::White);
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    MyApp app;
    app.Initialize(hInstance, 1600, 900, L"My Game", true);
    app.Run();
    app.Shutdown();
    return 0;
}
```

### 2. GameObject와 Component 사용

```cpp
// GameObject 생성
auto gameObject = GameObject::Instantiate("MyObject");

// Transform 사용
gameObject->GetTransform()->SetPosition(Vector2(100, 100));
gameObject->GetTransform()->SetRotation(45.0f);

// 커스텀 컴포넌트 추가
gameObject->AddComponent<MyCustomComponent>();
```

### 3. 씬 관리

```cpp
// 씬 생성
auto sceneManager = GetSceneManager();
auto scene = sceneManager->CreateScene("Level1");

// 씬 로드
sceneManager->LoadScene("Level1");
```

### 4. 입력 처리

```cpp
auto input = GetInput();

if (input->GetKeyDown(KeyCode::Space))
{
    // 스페이스 키를 눌렀을 때
}

if (input->GetMouseButtonDown(0))
{
    Vector2 mousePos = input->GetMousePosition();
    // 마우스 왼쪽 버튼을 눌렀을 때
}
```

### 5. 렌더링

```cpp
auto renderer = GetRenderer();

// 기본 도형 그리기
renderer->DrawRectangle(Vector2(100, 100), Vector2(200, 100), Color::Red);
renderer->FillCircle(Vector2(300, 300), 50, Color::Blue);

// 텍스트 그리기
renderer->DrawText(L"Score: 100", Vector2(10, 10), Color::White, L"Arial", 16.0f);
```

## 확장 가능한 기능

현재 기본 구조가 완성되어 있어 다음 기능들을 쉽게 추가할 수 있습니다:

- **스프라이트 렌더링**: SpriteRenderer 컴포넌트
- **애니메이션**: Animator 및 AnimationClip 시스템
- **물리**: Rigidbody 및 Collider 컴포넌트
- **오디오**: AudioSource 및 AudioClip
- **UI 시스템**: UI Canvas 및 UI 컴포넌트들
- **파티클 시스템**: ParticleSystem 컴포넌트

## 교수님 코드 대비 개선사항

1. **모던 C++ 활용**: 스마트 포인터, auto 키워드, 범위 기반 for 등
2. **더 나은 구조**: 명확한 폴더 구조와 네임스페이스 분리
3. **확장성**: 컴포넌트 시스템으로 기능 확장 용이
4. **에디터 지원**: 기본적인 에디터 시스템 구조 제공
5. **타입 안전성**: 템플릿과 타입 체크 강화

## Eclipse 엔진 대비 개선사항

1. **깔끔한 코드**: 더 읽기 쉽고 유지보수하기 좋은 구조
2. **명확한 책임 분리**: 각 클래스와 시스템의 역할이 명확
3. **확장 가능한 아키텍처**: 새로운 기능 추가가 용이
4. **현대적 설계**: 싱글톤 패턴의 올바른 사용, RAII 등

이 엔진은 기본적인 2D 게임 개발에 필요한 모든 구조를 제공하며, 필요에 따라 기능을 확장해 나갈 수 있도록 설계되었습니다.