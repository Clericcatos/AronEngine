/*
=== DirectX 11 Vertex Buffer & Index Buffer 사각형 렌더링 ===

1. 데이터 구조 정의
*/
struct Vertex3D
{
    DirectX::XMFLOAT3 position; // NDC 좌표 (-1~1 범위)
    DirectX::XMFLOAT4 color;    // RGBA 색상값 (0~1 범위)
};

/*
2. Vertex Buffer 생성 과정
*/
bool CreateVertexBuffer()
{
    // NDC 좌표계에서 사각형의 4개 꼭짓점 정의
    Vertex3D vertices[] = {
        { XMFLOAT3(-0.3f,  0.4f, 0.0f), XMFLOAT4(0.9f, 0.4f, 0.7f, 1.0f) }, // 왼쪽 위
        { XMFLOAT3( 0.3f,  0.4f, 0.0f), XMFLOAT4(0.4f, 0.8f, 0.9f, 1.0f) }, // 오른쪽 위  
        { XMFLOAT3( 0.3f, -0.4f, 0.0f), XMFLOAT4(0.6f, 0.9f, 0.5f, 1.0f) }, // 오른쪽 아래
        { XMFLOAT3(-0.3f, -0.4f, 0.0f), XMFLOAT4(1.0f, 0.7f, 0.3f, 1.0f) }  // 왼쪽 아래
    };

    // GPU 메모리에 버텍스 데이터 업로드
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;        // GPU가 읽기 전용으로 사용
    bufferDesc.ByteWidth = sizeof(vertices);       // 전체 데이터 크기
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 버텍스 버퍼로 사용

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices; // CPU 메모리의 데이터 포인터

    return SUCCEEDED(device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer));
}

/*
3. Index Buffer 생성 과정
*/
bool CreateIndexBuffer()
{
    // 사각형을 2개의 삼각형으로 분할하는 인덱스
    UINT indices[] = {
        0, 1, 2,  // 첫 번째 삼각형 (시계방향)
        0, 2, 3   // 두 번째 삼각형 (시계방향)
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(indices);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼로 사용

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = indices;

    return SUCCEEDED(device->CreateBuffer(&bufferDesc, &initData, &indexBuffer));
}

/*
4. Shader 생성 과정
*/
bool CreateShaders()
{
    // Vertex Shader: 각 정점의 위치와 색상을 처리
    const char* vertexShaderCode = R"(
        struct VS_INPUT
        {
            float3 pos : POSITION;  // 입력: 3D 위치
            float4 color : COLOR;   // 입력: 색상
        };
        
        struct VS_OUTPUT
        {
            float4 pos : SV_POSITION; // 출력: 화면 좌표
            float4 color : COLOR;     // 출력: 색상 (그대로 전달)
        };
        
        VS_OUTPUT main(VS_INPUT input)
        {
            VS_OUTPUT output;
            output.pos = float4(input.pos, 1.0f); // 3D를 4D로 변환
            output.color = input.color;
            return output;
        }
    )";

    // Pixel Shader: 각 픽셀의 최종 색상 결정
    const char* pixelShaderCode = R"(
        float4 main(float4 pos : SV_POSITION, float4 color : COLOR) : SV_TARGET
        {
            return color; // 버텍스에서 보간된 색상을 그대로 사용
        }
    )";

    // 셰이더 컴파일 및 생성
    // D3DCompile()로 HLSL 코드를 바이트코드로 변환
    // CreateVertexShader(), CreatePixelShader()로 GPU에 업로드
}

/*
5. Input Layout 생성
*/
bool CreateInputLayout()
{
    // GPU에게 버텍스 데이터 구조를 알려줌
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        //         오프셋 12 = position(12바이트) 다음부터 color 시작
    };

    return SUCCEEDED(device->CreateInputLayout(inputDesc, 2, vsBlob->GetBufferPointer(), 
                                              vsBlob->GetBufferSize(), &inputLayout));
}

/*
6. 렌더링 과정
*/
void Render()
{
    // 1. 렌더 파이프라인 설정
    context->IASetInputLayout(inputLayout);           // 입력 데이터 구조 설정
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 삼각형 리스트로 그리기
    
    // 2. 버퍼 바인딩
    UINT stride = sizeof(Vertex3D);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset); // 버텍스 버퍼 설정
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);    // 인덱스 버퍼 설정
    
    // 3. 셰이더 설정
    context->VSSetShader(vertexShader, nullptr, 0); // 버텍스 셰이더 활성화
    context->PSSetShader(pixelShader, nullptr, 0);  // 픽셀 셰이더 활성화
    
    // 4. 그리기 실행
    context->DrawIndexed(6, 0, 0); // 6개 인덱스로 2개 삼각형 = 1개 사각형
}

/*
=== 핵심 개념 ===

NDC (Normalized Device Coordinates):
- 화면을 -1~1 범위로 정규화한 좌표계
- 중앙이 (0,0), 왼쪽 위가 (-1,1), 오른쪽 아래가 (1,-1)

Vertex Buffer:
- GPU 메모리에 저장된 정점 데이터 배열
- 각 정점은 위치, 색상, 텍스처 좌표 등을 포함

Index Buffer:
- 정점들을 어떤 순서로 연결할지 지정하는 인덱스 배열
- 메모리 절약: 4개 정점으로 사각형 표현 (8개 정점 불필요)

Shader:
- GPU에서 실행되는 작은 프로그램
- Vertex Shader: 각 정점 처리
- Pixel Shader: 각 픽셀 색상 계산

이 방식으로 하드웨어 가속을 통해 빠른 렌더링이 가능합니다.
*/