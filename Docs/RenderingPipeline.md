# D3D11 렌더링 파이프라인 스테이지

## 개요
Direct3D 11 렌더링 파이프라인은 3D 그래픽스를 화면에 표시하기 위한 일련의 처리 단계들로 구성됩니다. 각 스테이지는 특정한 역할을 담당하며, GPU에서 병렬로 처리됩니다.

## 파이프라인 스테이지

### 1. Input Assembler (IA) 단계
**역할**: 버텍스 데이터와 인덱스 데이터를 읽어 프리미티브(삼각형, 선 등)를 조립

**주요 기능**:
- 버텍스 버퍼에서 버텍스 데이터 읽기
- 인덱스 버퍼를 사용한 효율적인 버텍스 재사용
- 프리미티브 토폴로지 설정 (삼각형, 선, 점 등)

**AronEngine 예제**:
```cpp
// Input Assembler 설정
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
context->IASetInputLayout(inputLayout.Get());
```

### 2. Vertex Shader (VS) 단계
**역할**: 각 버텍스를 개별적으로 처리하여 3D 좌표를 화면 좌표로 변환

**주요 기능**:
- 3D 월드 좌표를 NDC(Normalized Device Coordinates)로 변환
- 버텍스별 라이팅 계산
- 텍스처 좌표 전달

**AronEngine 예제**:
```hlsl
// BasicVertexShader.hlsl
struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;  // NDC 좌표
    float4 Color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f);  // NDC 변환
    output.Color = input.Color;
    return output;
}
```

### 3. Hull Shader (HS) 단계 (선택적)
**역할**: 테셀레이션을 위한 제어점 생성

**주요 기능**:
- 패치의 테셀레이션 인자 결정
- 제어점 변환
- 고해상도 지형, 곡면 생성

**사용 사례**: 지형 렌더링, 곡면 모델링 (AronEngine에서는 미구현)

### 4. Tessellator 단계 (선택적)
**역할**: Hull Shader 출력을 기반으로 새로운 버텍스 생성

**주요 기능**:
- 패치를 작은 삼각형들로 분할
- LOD(Level of Detail) 지원
- 동적 지오메트리 생성

### 5. Domain Shader (DS) 단계 (선택적)
**역할**: 테셀레이션된 버텍스의 최종 위치 계산

**주요 기능**:
- 테셀레이션 좌표를 3D 위치로 변환
- 디스플레이스먼트 매핑
- 노멀 계산

### 6. Geometry Shader (GS) 단계 (선택적)
**역할**: 프리미티브 단위로 지오메트리 생성/수정/삭제

**주요 기능**:
- 점에서 삼각형 생성 (파티클 시스템)
- 지오메트리 복제 (그림자 볼륨)
- 프리미티브 타입 변환

**예제**: 빌보드 쿼드 생성, 파티클 렌더링

### 7. Rasterizer (RS) 단계
**역할**: 3D 프리미티브를 2D 픽셀로 변환

**주요 기능**:
- 삼각형을 픽셀로 분해
- 백페이스 컬링 (뒷면 제거)
- 뷰포트 변환
- 클리핑 (화면 밖 제거)

**AronEngine 설정**:
```cpp
// 뷰포트 설정
D3D11_VIEWPORT viewport = {};
viewport.Width = (float)width;
viewport.Height = (float)height;
viewport.MinDepth = 0.0f;
viewport.MaxDepth = 1.0f;
context->RSSetViewports(1, &viewport);
```

### 8. Pixel Shader (PS) 단계
**역할**: 각 픽셀의 최종 색상 계산

**주요 기능**:
- 텍스처 샘플링
- 라이팅 계산
- 최종 색상 출력

**AronEngine 예제**:
```hlsl
// BasicPixelShader.hlsl
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return input.Color;  // 버텍스 색상을 그대로 출력
}
```

### 9. Output Merger (OM) 단계
**역할**: 픽셀 셰이더 출력을 최종 렌더 타겟에 합성

**주요 기능**:
- 깊이/스텐실 테스트
- 알파 블렌딩
- 멀티 렌더 타겟 처리

**AronEngine 설정**:
```cpp
// 렌더 타겟 설정
context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
```

## 데이터 흐름

```
버텍스 데이터 → IA → VS → [HS → Tessellator → DS] → [GS] → RS → PS → OM → 화면
     ↓           ↓    ↓                                    ↓    ↓    ↓
   인덱스      프리미티브  NDC좌표                         픽셀  색상  최종이미지
```

## NDC 좌표계
**Normalized Device Coordinates (-1 ~ 1 범위)**:
- X축: -1(왼쪽) ~ +1(오른쪽)
- Y축: -1(아래쪽) ~ +1(위쪽)  
- Z축: 0(가까움) ~ 1(멀음)

## AronEngine에서의 구현

### Test02_Quad 클래스
```cpp
// NDC 좌표계 사각형 정점
vertices[0] = { (-0.5f,  0.5f, 0.0f), (1,0,0,1) };  // 빨강 좌상단
vertices[1] = { ( 0.5f,  0.5f, 0.0f), (0,1,0,1) };  // 초록 우상단
vertices[2] = { ( 0.5f, -0.5f, 0.0f), (0,0,1,1) };  // 파랑 우하단
vertices[3] = { (-0.5f, -0.5f, 0.0f), (1,1,0,1) };  // 노랑 좌하단

// 인덱스 버퍼로 2개 삼각형 구성
indices = { 0,1,2, 0,2,3 };
```

### 파이프라인 설정 예제
```cpp
// 1. Input Assembler
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

// 2. Shader 설정
context->VSSetShader(vertexShader, nullptr, 0);
context->PSSetShader(pixelShader, nullptr, 0);

// 3. 그리기
context->DrawIndexed(6, 0, 0);  // 6개 인덱스로 2개 삼각형
```

## 최적화 팁

1. **배칭**: 동일한 셰이더/텍스처를 사용하는 오브젝트들을 함께 렌더링
2. **인덱스 버퍼**: 버텍스 재사용으로 메모리 절약
3. **상태 변경 최소화**: 셰이더나 텍스처 변경을 줄임
4. **Z-버퍼 최적화**: 가까운 오브젝트부터 그려서 픽셀 셰이더 실행 줄임

## 참고 자료
- Microsoft DirectX 11 문서
- AronEngine/Engine/Tests/Test01_Triangle.cpp (삼각형 예제)
- AronEngine/Engine/Tests/Test02_Quad.cpp (사각형 예제)