#include "pch.h"
#include "RenderComponent.h"

#include "tomato/Resources/Material.h"

namespace tomato
{
    RenderComponent::RenderComponent()
    {
    }

    RenderComponent::RenderComponent(const RenderComponent& _origin)
        : Component(_origin)
        , m_pMesh(_origin.m_pMesh)
        , m_pSharedMtrl(_origin.m_pSharedMtrl)
        , m_pDynamicMtrl(nullptr)
        , m_pCurMtrl(nullptr)
    {
        if (_origin.m_pCurMtrl == _origin.m_pSharedMtrl)
        {
            m_pCurMtrl = m_pSharedMtrl;
        }
        else if (_origin.m_pCurMtrl == _origin.m_pDynamicMtrl)
        {
            GetDynamicMaterial();
        }
    }

    RenderComponent::~RenderComponent()
    {	
        m_pDynamicMtrl = nullptr;
    }


    Ref<Material> RenderComponent::GetSharedMaterial()
    {
        m_pCurMtrl = m_pSharedMtrl;

        if (nullptr != m_pDynamicMtrl)
        {
            m_pDynamicMtrl = nullptr;
        }

        return m_pSharedMtrl;
    }

    Ref<Material> RenderComponent::GetDynamicMaterial()
    {
        if (m_pDynamicMtrl)
            return m_pDynamicMtrl;

        // TODO 다이나믹 머티리얼?
        // m_pDynamicMtrl = CreateRef<Material>(m_pSharedMtrl->Clone());
        m_pDynamicMtrl->SetName(m_pSharedMtrl->GetName() + u8"_Clone");
        m_pCurMtrl = m_pDynamicMtrl;

        return m_pCurMtrl;
    }
    
}
