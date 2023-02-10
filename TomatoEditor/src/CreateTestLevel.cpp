#include "pch.h"
#include "CreateTestLevel.h"

#include <tomato/Scene/Entity.h>
#include <tomato/Scene/Scene.h>

#include <CPlayerScript.h>


void CreateTestLevel()
{/*
    // Scene* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Test.lv");
    // CLevelMgr::GetInst()->ChangeLevel(Level);
    // return;


    Ref<Scene> pLevel = CreateRef<Scene>();

    // Layer 이름 설정
    pLevel->GetLayer(eLayer::General)->SetName(u8"Player");
    pLevel->GetLayer(eLayer::General2)->SetName(u8"PlayerProjectile");


    // Camera Object 추가
    Entity* pCamObj = new Entity;
    pCamObj->SetName(u8"MainCamera");

    pCamObj->AddComponent(new Transform);
    pCamObj->AddComponent(new Camera);

    pCamObj->Camera()->SetLayerMaskAll();
    pCamObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAHPICS);
    pCamObj->Camera()->SetOrthographicScale(3.5f);

    pLevel->AddGameObject(pCamObj, eLayer::General);


    // Directional Light 추가
    Entity* pDirLight = new Entity;
    pDirLight->SetName(u8"DirectionalLight");

    pDirLight->AddComponent(new Transform);
    pDirLight->AddComponent(new Light2D);

    pDirLight->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
    pDirLight->Light2D()->SetLightEmbient(Vec3(0.2f, 0.2f, 0.2f));
    pDirLight->Light2D()->SetLightType(eLightType::Directional);

    pLevel->AddGameObject(pDirLight, eLayer::General);


    // PointLight 추가
    Entity* pPointLight = new Entity;
    pPointLight->SetName(u8"PointLight");

    pPointLight->AddComponent(new Transform);
    pPointLight->AddComponent(new Light2D);

    pPointLight->Transform()->SetRelativePos(-500.f, 0.f, 0.f);

    pPointLight->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
    pPointLight->Light2D()->SetLightType(eLightType::Point);
    pPointLight->Light2D()->SetRadius(1000.f);


    pLevel->AddGameObject(pPointLight, eLayer::General);

    // GameObject 초기화
    Entity* pObject = nullptr;

    pObject = new Entity;
    pObject->SetName(u8"Player");

    pObject->AddComponent(new Transform);
    pObject->AddComponent(new MeshRender);
    pObject->AddComponent(new Collider2D);
    pObject->AddComponent(new Animator2D);
    pObject->AddComponent(new RigidBody2D);
    pObject->AddComponent(new CPlayerScript);

    pObject->Transform()->SetRelativePos(Vec3(0.f, 1.f, 10.f));

    pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerMtrl"));

    static auto palette = CResMgr::GetInst()->Load<Texture>(L"atlas\\beheaded_default_s.png");
    pObject->MeshRender()->GetCurMaterial()->SetTexParam(Tex2, palette);

    pObject->Collider2D()->SetCollider2DType(eCollider2DType::Rect);
    pObject->Collider2D()->SetScale(Vec2(0.25f,0.4f));
    pObject->Collider2D()->SetPivot(Vec2(0.5f, 0.f));


    pObject->Animator()->SetController(CResMgr::GetInst()->FindRes<CAnimController>(L"PlayerController"));
    
    pLevel->AddGameObject(pObject, eLayer::General);

    Entity* head = new Entity;
    head->SetName(u8"Head");

    head->AddComponent(new Transform);
    head->AddComponent(new MeshRender);

    head->Transform()->SetRelativeScale(Vec3(0.5f, 0.5f, 1.f));

    head->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    head->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"HeadMtrl"));
    Ref<Texture> pTex = CResMgr::GetInst()->FindRes<Texture>(L"atlas\\fxCommon.png");
    head->MeshRender()->GetCurMaterial()->SetTexParam(Tex0, pTex, pTex->GetFrame("fxDarknessHalo0"));

    auto color = Vec4(0.f, 0.f, 0.f, 0.9f);
    head->MeshRender()->GetCurMaterial()->SetScalarParam(Vec4_0, &color);

    pObject->AddChild(head);


    Entity* eye = new Entity;
    eye->SetName(u8"Eye");

    eye->AddComponent(new Transform);
    eye->AddComponent(new MeshRender);

    eye->Transform()->SetIgnoreParentScale(true);
    eye->Transform()->SetRelativeScale(Vec3(0.1f, 0.1f, 1.f));

    eye->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    eye->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"EyeMtrl"));
    pTex = CResMgr::GetInst()->FindRes<Texture>(L"atlas\\fxCommon.png");
    eye->MeshRender()->GetCurMaterial()->SetTexParam(Tex0, pTex, pTex->GetFrame("fxSquareYellow"));

    color = Vec4(1.f, 1.f, 1.f, 1.f);
    eye->MeshRender()->GetCurMaterial()->SetScalarParam(Vec4_0, &color);

    head->AddChild(eye);


    Entity* hand = new Entity;
    hand->SetName(u8"Hand");

    hand->AddComponent(new Transform);
    hand->AddComponent(new MeshRender);

    hand->Transform()->SetRelativeScale(Vec3(0.1f, 0.1f, 1.f));

    hand->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    hand->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"EyeMtrl"));
    pTex = CResMgr::GetInst()->FindRes<Texture>(L"atlas\\fxCommon.png");
    hand->MeshRender()->GetCurMaterial()->SetTexParam(Tex0, pTex, pTex->GetFrame("fxSquareYellow"));

    color = Vec4(1.f, 1.f, 1.f, 1.f);
    hand->MeshRender()->GetCurMaterial()->SetScalarParam(Vec4_0, &color);

    pObject->AddChild(hand);




    // TODO 에디터로
    pObject->GetScript<CPlayerScript>()->SetHead(head);
    pObject->GetScript<CPlayerScript>()->SetHand(hand);


    pObject = new Entity;
    pObject->SetName(u8"Ground");

    pObject->AddComponent(new Transform);
    pObject->AddComponent(new Collider2D);

    pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 10.f));
    pObject->Transform()->SetRelativeScale(Vec3(10.f, 1.f, 1.f));
    
    pObject->Collider2D()->SetCollider2DType(eCollider2DType::Rect);

    pLevel->AddGameObject(pObject, eLayer::General2);


    pObject = new Entity;
    pObject->SetName(u8"Ground2");

    pObject->AddComponent(new Transform);
    pObject->AddComponent(new Collider2D);

    pObject->Transform()->SetRelativePos(Vec3(-200.f, -100.f, 10.f));
    pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

    pObject->Collider2D()->SetCollider2DType(eCollider2DType::Rect);

    pLevel->AddGameObject(pObject, eLayer::General2);


    // pChild = new Entity;
    // pChild->SetName(L"Child");
    //
    // pChild->AddComponent(new Transform);
    // pChild->AddComponent(new MeshRender);
    // pChild->AddComponent(new Collider2D);
    //
    // pChild->Transform()->SetIgnoreParentScale(true);
    // pChild->Transform()->SetRelativePos(Vec3(1.f, 0.f, 100.f));
    // pChild->Transform()->SetRelativeScale(Vec3(256.f, 256.f, 1.f));
    //
    // pChild->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    // pChild->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"));
    // pChild->MeshRender()->GetCurMaterial()->SetTexParam(Tex0,
    //                                                     CResMgr::GetInst()->FindRes<Texture>(L"texture\\Player.bmp"));
    //
    // pLevel->AddGameObject(pChild, eLayer::General2);


    // Ptr<CPrefab> prefab = new CPrefab(pChild);
    // CResMgr::GetInst()->AddRes<CPrefab>(L"ChildObject", prefab.Get());
    // prefab->Save(L"prefab//ChildObject.pref");


    // Particle Object
    /*Entity* pParticle = new Entity;
    pParticle->AddComponent(new Transform);
    pParticle->AddComponent(new ParticleSystem);

    pParticle->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
    pParticle->ParticleSystem()->SetWorldSpawn(true);

    pLevel->AddGameObject(pParticle, 0);#1#


    // PostProcess Object
    /*Entity* pPostProcess = new Entity;
    pPostProcess->AddComponent(new Transform);
    pPostProcess->AddComponent(new MeshRender);

    pPostProcess->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
    pPostProcess->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));

    pPostProcess->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
    pPostProcess->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostProcessMtrl"));

    pLevel->AddGameObject(pPostProcess, 0);#1#


    // 충돌 레이어 설정
    CCollisionMgr::GetInst()->CollisionLayerCheck(0, 1);


    // Level 저장
    // CSaveLoadMgr::GetInst()->SaveLevel(pLevel, L"level\\Test.lv");


    // 레벨 설정
    CLevelMgr::GetInst()->ChangeLevel(pLevel);
    */

}
