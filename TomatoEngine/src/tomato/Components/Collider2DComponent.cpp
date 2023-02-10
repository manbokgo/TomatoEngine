#include "pch.h"
#include "Collider2DComponent.h"

#include "TransformComponent.h"

namespace tomato
{
    Collider2DComponent::Collider2DComponent()
        :  m_Scale(Vec2(1.f, 1.f))
        , m_Pivot(Vec2(0.5f, 0.5f))
        , m_eType(eCollider2DType::Rect)
        , m_OverlapCount(0)
        , m_bIgnoreObjectScale(false)
    {
    }

    Collider2DComponent::~Collider2DComponent()
    {
    }

    void Collider2DComponent::OnLateUpdate()
    {
        // 행렬 연산
        // 크 회 이
        m_FinalScale = m_Scale;
        if (!m_bIgnoreObjectScale)
        {
            Vec3 vWorldScale = Transform()->GetWorldScale();
            m_FinalScale *= Vec2(vWorldScale.x, vWorldScale.y);
        }
		
        Matrix matScale = XMMatrixScaling(m_FinalScale.x, m_FinalScale.y, 1.f);

        Matrix matRot = XMMatrixRotationX(m_Rot.x);
        matRot *= XMMatrixRotationY(m_Rot.y);
        matRot *= XMMatrixRotationZ(m_Rot.z);

        // FinalPos 계산
        Vec3 vObjectPos = Transform()->GetWorldPos();
        m_FinalPos = m_OffsetPos + Vec2(vObjectPos.x, vObjectPos.y) + m_FinalScale * (Vec2(0.5f, 0.5f) - m_Pivot);

        Matrix matTrans = XMMatrixTranslation(m_FinalPos.x, m_FinalPos.y, 0.f);

        // 충돌체 월드 행렬
        m_matWorld = matScale * matRot * matTrans;

        // DebugDraw 요청
        #ifdef _DEBUG
        // 오버랩이 1개 이상이면 빨간색으로 그린다.
        Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        if(0 < m_OverlapCount)
            vColor = Vec4(1.f, 0.f, 0.f, 1.f);

        if (eCollider2DType::Rect == m_eType)
        {
            // DebugDrawRect(vColor, Vec3(m_FinalPos.x, m_FinalPos.y, 0.f), Vec3(m_FinalScale.x, m_FinalScale.y, 1.f), m_Rot);
        }
        else
        {
            // DebugDrawCircle(vColor, Vec3(m_FinalPos.x, m_FinalPos.y, 0.f), m_FinalScale.x);
        }
        #endif
    }

}
