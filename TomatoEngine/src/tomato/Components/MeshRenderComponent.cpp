#include "pch.h"
#include "MeshRenderComponent.h"

#include "tomato/Components/TransformComponent.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Mesh.h"

namespace tomato
{
    MeshRenderComponent::MeshRenderComponent()
    {
    }

    MeshRenderComponent::~MeshRenderComponent()
    {
    }


    void MeshRenderComponent::OnUpdate()
    {
    }

    void MeshRenderComponent::OnLateUpdate()
    {
    }

    void MeshRenderComponent::render()
    {
        if (!IsActive())
        {
            return;
        }

        if (!GetMesh() || !GetCurMaterial())
        {
            return;
        }

        // ��ġ���� ������Ʈ
        Transform()->UpdateData();
		
        // ����� ���� ������Ʈ
        GetCurMaterial()->Bind();
	
        // ����� �޽� ������Ʈ �� ������
        GetMesh()->Render();

        // ����� �ؽ��� �������� ����
        Material::Clear();
    }

    
}
