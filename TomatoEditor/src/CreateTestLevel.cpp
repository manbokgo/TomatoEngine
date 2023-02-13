#include "pch.h"
#include "CreateTestLevel.h"

#include "tomato/Components/Light3DComponent.h"
#include "tomato/Core/CResMgr.h"
#include "tomato/Resources/Material.h"
#include "tomato/Scene/Entity.h"
#include "tomato/Scene/Scene.h"

namespace tomato {
    void CreateTestLevel(const Ref<Scene>& scene)
    {
		{
			Entity* pObject = scene->CreateEntity("Sphere");

			pObject->AddComponent<TransformComponent>();
			pObject->AddComponent<MeshRenderComponent>();

			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"SphereMesh"));
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<Material>(L"Std3DMtrl"));
		}

		{
			Entity* pDirLight = scene->CreateEntity("DirectionalLight");

			pDirLight->AddComponent<TransformComponent>();
			pDirLight->AddComponent<Light3DComponent>();

			pDirLight->Transform()->SetRelativeRotation({ XM_PI / 2.f, 0.f, 0.f });

			pDirLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
			pDirLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
			pDirLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
			pDirLight->Light3D()->SetLightType(eLightType::Directional);
		}
    }
}
