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

        // 위치정보 업데이트
        Transform()->UpdateData();
		
        // 사용할 재질 업데이트
        GetCurMaterial()->Bind();
	
        // 사용할 메쉬 업데이트 및 렌더링
        GetMesh()->Render();

        // 사용한 텍스쳐 레지스터 비우기
        Material::Clear();
    }

    
}
