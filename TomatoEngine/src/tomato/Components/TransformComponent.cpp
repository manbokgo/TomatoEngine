#include "pch.h"
#include "TransformComponent.h"

#include "tomato/Renderer/ConstBuffer.h"
#include "tomato/Renderer/Device.h"
#include "tomato/Scene/Entity.h"

namespace tomato
{
    TransformComponent::TransformComponent()
        : m_Scale(Vec3(1.f, 1.f, 1.f))
        , m_ScaleModifier(Vec3(1.f, 1.f, 1.f))
        , m_bIgnParentScale(false)
    {
        m_vRelativeDir[(UINT)eDir::Right]	= Vec3(1.f, 0.f, 0.f);
        m_vRelativeDir[(UINT)eDir::Up]		= Vec3(0.f, 1.f, 0.f);
        m_vRelativeDir[(UINT)eDir::Front]	= Vec3(0.f, 0.f, 1.f);
    }

    TransformComponent::~TransformComponent()
    {
    }



    void TransformComponent::OnUpdate()
    {

    }

    void TransformComponent::OnLateUpdate()
    {
        // m_Translation, m_Scale, m_Rotation
        // ��=> ��������� ����� ��
        m_matWorld = Matrix::Identity;

        Vec3 modifiedScale;
        modifiedScale = m_Scale * m_ScaleModifier;

        Matrix matScale = XMMatrixScaling(modifiedScale.x, modifiedScale.y, modifiedScale.z);

        Matrix matTrans = XMMatrixTranslation(m_Translation.x, m_Translation.y, m_Translation.z);

        // Matrix matRot = XMMatrixRotationX(m_Rotation.x);
        // matRot *= XMMatrixRotationY(m_Rotation.y);
        // matRot *= XMMatrixRotationZ(m_Rotation.z);

        Matrix matRot = XMMatrixRotationRollPitchYawFromVector(m_Rotation);

        // ȸ������� �̿��ؼ� ���� ��ü�� ��, ��, �� ������ ���س��´�.
        m_vRelativeDir[(UINT)eDir::Right] = m_vWorldDir[(UINT)eDir::Right] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), matRot);
        m_vRelativeDir[(UINT)eDir::Up]	 = m_vWorldDir[(UINT)eDir::Up]	 = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), matRot);
        m_vRelativeDir[(UINT)eDir::Front] = m_vWorldDir[(UINT)eDir::Front] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), matRot);

        m_matWorld = matScale * matRot * matTrans;

        // �θ� ������Ʈ�� �ִٸ�
        Entity* pParent = GetGameObject()->GetParent();
        if (pParent)
        {
            auto parentWorldMat = pParent->Transform()->GetWorldMat();
            // �θ� ������Ʈ�� ũ�⸦ �����ϱ�� �� ���
            if (m_bIgnParentScale)
            {
                // �������� ��� �θ��� ũ�Ⱚ�� �����ؼ� ������� ����� �д�.
                Vec3 vParentScale = Vec3(1.f, 1.f, 1.f);

                while (pParent)
                {
                    vParentScale *= pParent->Transform()->GetRelativeScale();

                    // �θ� ������Ʈ�� �� ���� ũ�⸦ �����ϱ�� �� ��� ũ�⸦ ���̻� ������ �ʿ䰡 ����.
                    if (pParent->Transform()->m_bIgnParentScale)
                        pParent = nullptr;
                    else			
                        pParent = pParent->GetParent();
                }

                // �θ� ����� ũ��κ��� ������� ���ؼ� ����Ų��.
                // ����� ����ó��
                if (0.f == vParentScale.x)
                    vParentScale.x = 1.f;
                if (0.f == vParentScale.y)
                    vParentScale.y = 1.f;
                if (0.f == vParentScale.z)
                    vParentScale.z = 1.f;

                Matrix matParentScaleInv = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
                matParentScaleInv = XMMatrixInverse(nullptr, matParentScaleInv);

                // ���� ������� �̸� ���صΰ� �θ��� ��������� ���Ѵ�.
                m_matWorld *= matParentScaleInv * parentWorldMat;
            }
            else
            {
                Vec3 vParentScaleModifier = pParent->Transform()->GetScaleModifier();
                Matrix matParentScaleInv = XMMatrixScaling(vParentScaleModifier.x, vParentScaleModifier.y, vParentScaleModifier.z);
                matParentScaleInv = XMMatrixInverse(nullptr, matParentScaleInv);

                m_matWorld *= matParentScaleInv * parentWorldMat;
            }	

            // WorldDir ���ϱ�
            m_vWorldDir[(UINT)eDir::Right] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
            m_vWorldDir[(UINT)eDir::Up]	  = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
            m_vWorldDir[(UINT)eDir::Front] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);

            // ȸ��, ũ�� ��ȯ�� �̷������ ������ ����� ũ�⸦ �ʱ�ȭ �ϱ� ���� Normalize ���ش�.����
            m_vWorldDir[(UINT)eDir::Right].Normalize();
            m_vWorldDir[(UINT)eDir::Up].Normalize();
            m_vWorldDir[(UINT)eDir::Front].Normalize();
        }
    }


    void TransformComponent::UpdateData()
    {
        ConstBuffer* pCB = Device::GetInst()->GetConstBuffer(eCBType::Transform);
		
        g_transform.matWorld = m_matWorld;
        g_transform.matWV = g_transform.matWorld * g_transform.matView;
        g_transform.matWVP = g_transform.matWV * g_transform.matProj;
	
        pCB->SetData(&g_transform);
        pCB->UpdateData(ePipelineStage::AllStage);
    }



    Vec3 TransformComponent::GetWorldScale()
    {
        // �������� ��� �θ��� ũ�Ⱚ�� �����ؼ� ������� ����� �д�.	
        Vec3 vWorldScale = m_Scale;

        if (m_bIgnParentScale)
            return vWorldScale;

        Entity* pParent = GetGameObject()->GetParent();
        while (pParent)
        {		
            vWorldScale *= pParent->Transform()->GetRelativeScale();

            // �θ� ������Ʈ�� �� ���� ũ�⸦ �����ϱ�� �� ��� ũ�⸦ ���̻� ������ �ʿ䰡 ����.
            if (pParent->Transform()->m_bIgnParentScale)
                pParent = nullptr;
            else
                pParent = pParent->GetParent();
        }

        return vWorldScale;
    }

    void TransformComponent::SaveToFile(FILE* _File)
    {
        // eComponentType type = GetType();
        // fwrite(&type, sizeof(UINT), 1, _File);

        fwrite(&m_Translation, sizeof(Vec3), 1, _File);
        fwrite(&m_Scale, sizeof(Vec3), 1, _File);
        fwrite(&m_Rotation, sizeof(Vec3), 1, _File);
        fwrite(&m_bIgnParentScale, sizeof(bool), 1, _File);
    }

    void TransformComponent::LoadFromFile(FILE* _File)
    {
        fread(&m_Translation, sizeof(Vec3), 1, _File);
        fread(&m_Scale, sizeof(Vec3), 1, _File);
        fread(&m_Rotation, sizeof(Vec3), 1, _File);
        fread(&m_bIgnParentScale, sizeof(bool), 1, _File);
    }
}
