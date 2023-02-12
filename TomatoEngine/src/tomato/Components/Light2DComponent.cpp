#include "pch.h"
#include "Light2DComponent.h"

#include "TransformComponent.h"
#include "tomato/Core/CKeyMgr.h"
#include "tomato/Core/CTimeMgr.h"


namespace tomato
{
    Light2DComponent::Light2DComponent()
    {
        m_Info.iLightType = eLightType::Point;
    }

    Light2DComponent::~Light2DComponent()
    {
    }

    void Light2DComponent::OnLateUpdate()
    {
        if (KEY_PRESSED(KEY::L))
        {
            Vec3 vRot = Transform()->GetRelativeRotation();
            vRot.z += DT * (XM_PI / 2.f);
            Transform()->SetRelativeRotation(vRot);
        }


        m_Info.vWorldPos = { Transform()->GetWorldPos(), 0.f };
        m_Info.vWorldDir = { Transform()->GetWorldDir(eDir::Forward), 0.f };

        // TODO 카메라에 레지스터
        // CRenderMgr::GetInst()->RegisterLight2D(this);
    }
    
}
