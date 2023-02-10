#include "pch.h"
#include "Entity.h"

#include "tomato/Components/TransformComponent.h"

namespace tomato
{
    Entity::Entity() {}
    Entity::~Entity() {}
    void Entity::OnUpdate() {}
    void Entity::OnLateUpdate() {}

    void Entity::AddChild(Entity* child)
    {
        if (const Entity* childParent = child->GetParent())
        {
            if (childParent == this)
            {
                return;
            }

            child->Deparent();
        }

        m_Children.push_back(child);
        child->m_Parent = this;
    }

    void Entity::RemoveChild(Entity* child)
    {
        const auto iter = std::find(m_Children.begin(), m_Children.end(), child);
        ASSERT(iter != m_Children.end());

        m_Children.erase(iter);
        child->m_Parent = nullptr;
    }

    void Entity::SetParent(Entity* parent)
    {
        parent->AddChild(this);
    }

    void Entity::Deparent()
    {
        m_Parent->RemoveChild(this);
    }

    Matrix Entity::GetWorldTransform() const
    {
        return Transform()->GetWorldMat();
    }

    void Entity::OnSensorExit2D(const Collision2DData& collisionData) {}
    void Entity::OnSensorEnter2D(const Collision2DData& collisionData) {}
    void Entity::OnCollisionExit2D(const Collision2DData& collisionData) {}
    void Entity::OnCollisionEnter2D(const Collision2DData& collisionData) {}
    
}
