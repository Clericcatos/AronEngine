#include "framework.h"
#include "SceneManager.h"
#include "../Core/Scene.h"

namespace AronEngine
{
    SceneManager::SceneManager()
        : activeScene(nullptr)
        , nextScene(nullptr)
    {
    }

    Scene* SceneManager::CreateScene(const std::string& name)
    {
        auto scene = std::make_unique<Scene>(name);
        Scene* ptr = scene.get();
        scenes.push_back(std::move(scene));
        
        if (!activeScene)
        {
            activeScene = ptr;
            activeScene->SetLoaded(true);
        }
        
        return ptr;
    }

    void SceneManager::LoadScene(const std::string& name)
    {
        Scene* scene = GetScene(name);
        if (scene)
        {
            nextScene = scene;
        }
    }

    void SceneManager::LoadScene(int index)
    {
        Scene* scene = GetScene(index);
        if (scene)
        {
            nextScene = scene;
        }
    }

    void SceneManager::LoadNextScene()
    {
        if (nextScene)
        {
            SwitchScene();
        }
    }

    Scene* SceneManager::GetScene(const std::string& name) const
    {
        for (const auto& scene : scenes)
        {
            if (scene->GetName() == name)
                return scene.get();
        }
        return nullptr;
    }

    Scene* SceneManager::GetScene(int index) const
    {
        if (index >= 0 && index < static_cast<int>(scenes.size()))
        {
            return scenes[index].get();
        }
        return nullptr;
    }

    void SceneManager::Update(float deltaTime)
    {
        if (nextScene)
        {
            SwitchScene();
        }

        if (activeScene)
        {
            activeScene->Update(deltaTime);
        }
    }

    void SceneManager::LateUpdate(float deltaTime)
    {
        if (activeScene)
        {
            activeScene->LateUpdate(deltaTime);
        }
    }

    void SceneManager::FixedUpdate(float fixedDeltaTime)
    {
        if (activeScene)
        {
            activeScene->FixedUpdate(fixedDeltaTime);
        }
    }

    void SceneManager::SwitchScene()
    {
        if (nextScene && nextScene != activeScene)
        {
            activeScene = nextScene;
            activeScene->SetLoaded(true);
            activeScene->Start();
            nextScene = nullptr;
        }
    }
}