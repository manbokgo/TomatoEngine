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
			Entity* sphere = scene->CreateEntity("Sphere");

			sphere->AddComponent<TransformComponent>();
			sphere->AddComponent<MeshRenderComponent>();

			sphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"SphereMesh"));
			sphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<Material>(L"Std3DMtrl"));
		}

		{
			Entity* sphere = scene->CreateEntity("Sphere (2)");

			sphere->AddComponent<TransformComponent>();
			sphere->AddComponent<MeshRenderComponent>();

			sphere->Transform()->SetRelativePos({ 2, -1, 0 });

			sphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"SphereMesh"));
			sphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<Material>(L"Std3DMtrl"));
		}

		{
			Entity* plane = scene->CreateEntity("Plane");

			plane->AddComponent<TransformComponent>();
			plane->AddComponent<MeshRenderComponent>();

			plane->Transform()->SetRelativePos({0, -2, 0});
			plane->Transform()->SetRelativeScale({10, 10, 1});
			plane->Transform()->SetRelativeRotation({ XM_PI / 2.f, 0, 0});

			plane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"RectMesh"));
			plane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<Material>(L"Std3DMtrl"));
		}

		/*{
			Entity* directionalLight = scene->CreateEntity("DirectionalLight");

			directionalLight->AddComponent<TransformComponent>();
			directionalLight->AddComponent<Light3DComponent>();

			directionalLight->Transform()->SetRelativeRotation({ XM_PI / 2.f, 0.f, 0.f });
			directionalLight->Transform()->SetRelativePos({ 0, 5, 0 });

			directionalLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
			directionalLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
			directionalLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
			directionalLight->Light3D()->SetLightType(eLightType::Directional);
		}*/

		{
			Entity* pointLight = scene->CreateEntity("PointLight");

			pointLight->AddComponent<TransformComponent>();
			pointLight->AddComponent<Light3DComponent>();
			
			pointLight->Transform()->SetRelativePos({ 0, -1, 0 });

			pointLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
			pointLight->Light3D()->SetLightType(eLightType::Point);
			pointLight->Light3D()->SetRadius(3);
		}

		{
			Entity* pointLight = scene->CreateEntity("PointLight (2)");

			pointLight->AddComponent<TransformComponent>();
			pointLight->AddComponent<Light3DComponent>();

			pointLight->Transform()->SetRelativePos({ 3, -0.5f, 0 });

			pointLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
			pointLight->Light3D()->SetLightType(eLightType::Point);
			pointLight->Light3D()->SetRadius(3);
		}

		{
			Entity* spotLight = scene->CreateEntity("SpotLight");

			spotLight->AddComponent<TransformComponent>();
			spotLight->AddComponent<Light3DComponent>();

			spotLight->Transform()->SetRelativePos({ 0, 3.f, 0 });
			spotLight->Transform()->SetRelativeRotation({ XM_PI / 2.f, 0, 0 });

			spotLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
			spotLight->Light3D()->SetLightType(eLightType::Spot);
			spotLight->Light3D()->SetRadius(5);
			spotLight->Light3D()->SetAngleInner(XMConvertToRadians(20.f));
			spotLight->Light3D()->SetAngleOuter(XMConvertToRadians(60.f));
		}
    }
}
