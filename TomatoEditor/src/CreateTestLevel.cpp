#include "pch.h"
#include "CreateTestLevel.h"

#include "tomato/Core/CResMgr.h"
#include "tomato/Resources/Material.h"
#include "tomato/Scene/Entity.h"
#include "tomato/Scene/Scene.h"

namespace tomato {
    void CreateTestLevel(const Ref<Scene>& scene)
    {
		// GameObject ÃÊ±âÈ­
		Entity* pObject = nullptr;

		pObject = scene->CreateEntity("Player");
		
		pObject->AddComponent<TransformComponent>();
		pObject->AddComponent<MeshRenderComponent>();

		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"SphereMesh"));
		pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<Material>(L"Std3DMtrl"));
    }
}
