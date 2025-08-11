#include "framework.h"
#include "GameObject.h"
#include "../Components/Component.h"
#include "../Managers/SceneManager.h"
#include "EngineApp.h"
#include "Scene.h"

namespace AronEngine
{
    GameObject::GameObject()
        : Object("GameObject")
        , parent(nullptr)
        , scene(nullptr)
        , layer(0)
        , tag("Untagged")
    {
        transform = std::make_unique<Transform>();
        transform->SetGameObject(this);
    }

    GameObject::GameObject(const std::string& name)
        : Object(name)
        , parent(nullptr)
        , scene(nullptr)
        , layer(0)
        , tag("Untagged")
    {
        transform = std::make_unique<Transform>();
        transform->SetGameObject(this);
    }

    GameObject::~GameObject()
    {
        OnDestroy();
        
        for (auto& child : children)
        {
            if (child)
            {
                child->parent = nullptr;
            }
        }
        
        if (parent)
        {
            parent->RemoveChild(this);
        }
    }

    void GameObject::SetParent(GameObject* newParent)
    {
        if (parent == newParent)
            return;

        if (parent)
        {
            parent->RemoveChild(this);
        }

        parent = newParent;

        if (parent)
        {
            parent->AddChild(this);
            transform->SetParent(parent->transform.get());
        }
        else
        {
            transform->SetParent(nullptr);
        }
    }

    void GameObject::AddChild(GameObject* child)
    {
        if (!child || child == this)
            return;

        auto it = std::find(children.begin(), children.end(), child);
        if (it == children.end())
        {
            children.push_back(child);
        }
    }

    void GameObject::RemoveChild(GameObject* child)
    {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end()
        );
    }

    void GameObject::Start()
    {
        for (auto& component : components)
        {
            if (component->IsActive())
            {
                component->OnStart();
            }
        }
    }

    void GameObject::Update(float deltaTime)
    {
        if (!IsActive())
            return;

        for (auto& component : components)
        {
            if (component->IsActive())
            {
                component->OnUpdate(deltaTime);
            }
        }
    }

    void GameObject::LateUpdate(float deltaTime)
    {
        if (!IsActive())
            return;

        for (auto& component : components)
        {
            if (component->IsActive())
            {
                component->OnLateUpdate(deltaTime);
            }
        }
    }

    void GameObject::FixedUpdate(float fixedDeltaTime)
    {
        if (!IsActive())
            return;

        for (auto& component : components)
        {
            if (component->IsActive())
            {
                component->OnFixedUpdate(fixedDeltaTime);
            }
        }
    }

    void GameObject::OnEnable()
    {
        for (auto& component : components)
        {
            if (component->IsActive())
            {
                component->OnEnable();
            }
        }
    }

    void GameObject::OnDisable()
    {
        for (auto& component : components)
        {
            component->OnDisable();
        }
    }

    void GameObject::OnDestroy()
    {
        for (auto& component : components)
        {
            component->OnDestroy();
        }
    }

    GameObject* GameObject::Instantiate(const std::string& name)
    {
        auto* sceneManager = EngineApp::GetInstance().GetSceneManager();
        if (sceneManager && sceneManager->GetActiveScene())
        {
            return sceneManager->GetActiveScene()->CreateGameObject(name);
        }
        return nullptr;
    }

    GameObject* GameObject::Instantiate(GameObject* parent, const std::string& name)
    {
        auto* gameObject = Instantiate(name);
        if (gameObject && parent)
        {
            gameObject->SetParent(parent);
        }
        return gameObject;
    }

    void GameObject::Destroy(GameObject* gameObject)
    {
        if (!gameObject)
            return;

        auto* sceneManager = EngineApp::GetInstance().GetSceneManager();
        if (sceneManager && sceneManager->GetActiveScene())
        {
            sceneManager->GetActiveScene()->DestroyGameObject(gameObject);
        }
    }

    void GameObject::DestroyImmediate(GameObject* gameObject)
    {
        if (!gameObject)
            return;

        auto* sceneManager = EngineApp::GetInstance().GetSceneManager();
        if (sceneManager && sceneManager->GetActiveScene())
        {
            sceneManager->GetActiveScene()->DestroyGameObjectImmediate(gameObject);
        }
    }
}