// 컴파일 테스트용 간단한 프로그램
#include "Engine/Core/framework.h"
#include "Engine/Core/TextureManager.h"
#include "Engine/Components/SpriteRenderer.h"

using namespace AronEngine;

int main()
{
    // TextureManager 인스턴스 테스트
    TextureManager& tm = TextureManager::GetInstance();
    
    // 메서드 존재 확인
    bool supported = tm.IsSupportedFormat("test.png");
    
    // Sprite 구조체 테스트
    std::shared_ptr<Texture> testTexture = std::make_shared<Texture>();
    std::shared_ptr<Sprite> testSprite = std::make_shared<Sprite>(testTexture, "test");
    
    std::cout << "컴파일 테스트 성공!" << std::endl;
    return 0;
}