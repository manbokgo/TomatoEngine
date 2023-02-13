#include "pch.h"
#include "Light3DComponent.h"

#include "TransformComponent.h"
#include "tomato/Renderer/Renderer3D.h"

namespace tomato {
    const tLightInfo& Light3DComponent::GetInfo()
    {
        m_Info.vWorldPos = Vec4(Transform()->GetWorldPos(), 0.f);
        m_Info.vWorldDir = Vec4(Transform()->GetWorldDir(eDir::Forward), 0.f);
        return m_Info;
    }
}
