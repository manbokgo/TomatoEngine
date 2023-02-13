#include "pch.h"
#include "TransformComponent.h"

#include "tomato/Renderer/ConstBuffer.h"
#include "tomato/Renderer/Device.h"
#include "tomato/Scene/Entity.h"

namespace tomato
{
    TransformComponent::TransformComponent()
    {
        m_RelativeDir[(UINT)eDir::Right] = Vec3(1.f, 0.f, 0.f);
        m_RelativeDir[(UINT)eDir::Up] = Vec3(0.f, 1.f, 0.f);
        m_RelativeDir[(UINT)eDir::Forward] = Vec3(0.f, 0.f, 1.f);
    }

    TransformComponent::~TransformComponent() { }

    void TransformComponent::OnUpdate() { }

    void TransformComponent::OnLateUpdate()
    {
        Vec3 modifiedScale = m_Scale * m_ScaleModifier;

        Matrix matScale = Matrix::CreateScale(modifiedScale);
        Matrix matRot = Matrix::CreateFromYawPitchRoll(m_Rotation);
        Matrix matTrans = Matrix::CreateTranslation(m_Position);

        m_WorldMat = matScale * matRot * matTrans;

        m_RelativeDir[(UINT)eDir::Right] = matRot.Right();
        m_RelativeDir[(UINT)eDir::Up] = matRot.Up();
        m_RelativeDir[(UINT)eDir::Forward] = matRot.Backward();


        Entity* parent = GetGameObject()->GetParent();
        if (parent)
        {
            Matrix parentWorldMat = parent->Transform()->GetWorldMat();

            if (m_bIgnoreParentScale)
            {
                // 위쪽으로 모든 부모의 크기값을 누적해서 역행렬을 만들어 둔다.
                Vec3 parentScale = Vec3::One;
                while (parent)
                {
                    parentScale *= parent->Transform()->GetRelativeScale();

                    // 부모 오브젝트도 그 위로 크기를 무시하기로 한 경우 크기를 더이상 누적할 필요가 없다.
                    if (parent->Transform()->m_bIgnoreParentScale)
                    {
                        parent = nullptr;
                    }
                    else
                    {
                        parent = parent->GetParent();
                    }
                }

                // 부모 행렬의 크기부분을 역행렬을 구해서 상쇄시킨다.
                // 역행렬 예외처리
                if (parentScale.x == 0.f) { parentScale.x = 1.f; }
                if (parentScale.y == 0.f) { parentScale.y = 1.f; }
                if (parentScale.z == 0.f) { parentScale.z = 1.f; }

                Matrix matParentScaleInv = Matrix::CreateScale(parentScale).Invert();

                m_WorldMat *= matParentScaleInv * parentWorldMat;
            }
            else
            {
                Vec3   parentScaleModifier = parent->Transform()->GetScaleModifier();
                Matrix parentScaleModifierInv = Matrix::CreateScale(parentScaleModifier).Invert();

                m_WorldMat *= parentScaleModifierInv * parentWorldMat;
            }


            // WorldDir 구하기
            m_WorldDir[(UINT)eDir::Right] = m_WorldMat.Right();
            m_WorldDir[(UINT)eDir::Right].Normalize();

            m_WorldDir[(UINT)eDir::Up] = m_WorldMat.Up();
            m_WorldDir[(UINT)eDir::Up].Normalize();

            m_WorldDir[(UINT)eDir::Forward] = m_WorldMat.Backward();
            m_WorldDir[(UINT)eDir::Forward].Normalize();
        }
        else
        {
            m_WorldDir = m_RelativeDir;
        }
    }


    void TransformComponent::UpdateData()
    {
        ConstBuffer* pCB = Device::GetInst()->GetConstBuffer(eCBType::Transform);

        g_transform.matWorld = m_WorldMat;
        g_transform.matWV = g_transform.matWorld * g_transform.matView;
        g_transform.matWVP = g_transform.matWV * g_transform.matProj;

        pCB->SetData(&g_transform);
        pCB->UpdateData(ePipelineStage::AllStage);
    }


    Vec3 TransformComponent::GetWorldScale() const
    {
        Vec3 worldScale = m_Scale;
        if (m_bIgnoreParentScale)
        {
            return worldScale;
        }

        const Entity* parent = GetGameObject()->GetParent();
        while (parent)
        {
            worldScale *= parent->Transform()->GetRelativeScale();

            // 부모 오브젝트도 그 위로 크기를 무시하기로 한 경우 크기를 더이상 누적할 필요가 없다.
            if (parent->Transform()->m_bIgnoreParentScale)
            {
                parent = nullptr;
            }
            else
            {
                parent = parent->GetParent();
            }
        }

        return worldScale;
    }
}
