#pragma once
#include "../Core/framework.h"
#include "../Core/Scene.h"
#include <memory>
#include <vector>

namespace AronEngine
{

    class SceneManager
    {
    private:
        std::vector<std::unique_ptr<Scene>> scenes;
        Scene* activeScene;
        Scene* nextScene;

    public:
        SceneManager();
        ~SceneManager() = default;

        Scene* CreateScene(const std::string& name);
        void LoadScene(const std::string& name);
        void LoadScene(int index);
        void LoadNextScene();

        Scene* GetActiveScene() const { return activeScene; }
        Scene* GetScene(const std::string& name) const;
        Scene* GetScene(int index) const;
        
        int GetSceneCount() const { return static_cast<int>(scenes.size()); }

        void Update(float deltaTime);
        void LateUpdate(float deltaTime);
        void FixedUpdate(float fixedDeltaTime);

    private:
        void SwitchScene();
    };
}