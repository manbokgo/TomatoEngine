#include "pch.h"

#include <set>

#pragma warning(push)
#pragma warning(disable : 4819 6285 26437 26450 26451 26498 4251 26800 26495 4275 26813)
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#pragma warning(pop) 

#include "CEventMgr.h"
#include "CResMgr.h"
#include "tomato/Resources/PaintShader.h"
#include "tomato/Resources/ParticleUpdateShader.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    void CResMgr::CreateAnimClipFromTex(wstring atlasName, Ref<Texture> texture, Ref<Texture> normalTexture)
    {
        wstring atlasNamePrefix = atlasName + L"/";

        const std::unordered_map<string, SpriteFrm>& frames = texture->GetFrames();
        std::set<Ref<AnimClip>>                     updatedClips;

        for (const auto& [name, frame] : frames)
        {
            static const std::regex re(R"((\w*)_(\d{2,})$)");
            std::smatch             match;

            if (std::regex_match(name, match, re))
            {
                wstring animName = atlasNamePrefix + StringToWstring(match[1].str());
                int     index    = std::stoi(match[2].str());

                Ref<AnimClip> animationClip = FindRes<AnimClip>(animName);
                if (!animationClip)
                {
                    animationClip = CreateRef<AnimClip>();
                    AddRes<AnimClip>(animName, animationClip);
                }

                updatedClips.insert(animationClip);

                AnimationFrm animFrm;
                animFrm.SpriteName    = animName;
                animFrm.Sprite        = frame;
                animFrm.Duration      = 0.0333f;
                animFrm.SpriteTexture       = texture;
                animFrm.NormalTexture = normalTexture;
                animFrm.Index         = index;

                animationClip->AddFrame(animFrm);
            }
        }

        for (const auto& updatedClip : updatedClips)
        {
            updatedClip->SortFrameByIndex();
        }
    }

    void CResMgr::CreateDefaultAnimController()
    {
        Ref<AnimController> playerController = CreateRef<AnimController>();
        AddRes<AnimController>(L"PlayerController", playerController);

        AnimState* idle = playerController->AddState("idle");
        idle->SetClip(FindRes<AnimClip>(L"beheaded/idle"));
        idle->SetRepeat(true);

        AnimState* walk = playerController->AddState("walk");
        walk->SetClip(FindRes<AnimClip>(L"beheaded/walk"));
        walk->SetRepeat(true);

        AnimState* run = playerController->AddState("run");
        run->SetClip(FindRes<AnimClip>(L"beheaded/run"));
        run->SetRepeat(true);

        AnimState* jumpUp = playerController->AddState("jumpUp");
        jumpUp->SetClip(FindRes<AnimClip>(L"beheaded/jumpUp"));

        AnimState* jumpTransition = playerController->AddState("jumpTransition");
        jumpTransition->SetClip(FindRes<AnimClip>(L"beheaded/jumpTransition"));

        AnimState* jumpDown = playerController->AddState("jumpDown");
        jumpDown->SetClip(FindRes<AnimClip>(L"beheaded/jumpDown"));

        AnimState* land = playerController->AddState("land");
        land->SetClip(FindRes<AnimClip>(L"beheaded/land"));

        AnimState* rise = playerController->AddState("rise");
        rise->SetClip(FindRes<AnimClip>(L"beheaded/rise"));

        AnimState* idleRun = playerController->AddState("idleRun");
        idleRun->SetClip(FindRes<AnimClip>(L"beheaded/idleRun"));

        AnimState* runIdle = playerController->AddState("runIdle");
        runIdle->SetClip(FindRes<AnimClip>(L"beheaded/runIdle"));

        AnimState* volte = playerController->AddState("volte");
        volte->SetClip(FindRes<AnimClip>(L"beheaded/volte"));

        AnimState* atkBackStabberA = playerController->AddState("atkBackStabberA");
        atkBackStabberA->SetClip(FindRes<AnimClip>(L"beheaded/atkBackStabberA"));

        playerController->SetDefaultState(idle);
        playerController->AddParameter("velocityX");
        playerController->AddParameter("velocityY");
        playerController->AddParameter("grounded", true);
        playerController->AddParameter("volte", true);

        playerController->AddParameter("attack", true);
        playerController->AddParameter("StartSword");
        playerController->AddParameter("StartShield");

        {
            AnimCondition condition;

            AnimTransition idleToIdleRun;
            idleToIdleRun.DestState = idleRun;

            condition.Mode      = eAnimConditionMode::GreaterOrEqual;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.7f;
            idleToIdleRun.Conditions.push_back(condition);

            idle->AddTransition(idleToIdleRun);

            AnimTransition idleToWalk;
            idleToWalk.DestState = walk;

            condition.Mode      = eAnimConditionMode::GreaterOrEqual;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.3f;
            idleToWalk.Conditions.push_back(condition);

            idle->AddTransition(idleToWalk);


            AnimTransition idleToJumpUp;
            idleToJumpUp.DestState = jumpUp;

            condition.Mode      = eAnimConditionMode::GreaterOrEqual;
            condition.Parameter = "velocityY";
            condition.Threshold = 0.01f;
            idleToJumpUp.Conditions.push_back(condition);

            idle->AddTransition(idleToJumpUp);

            AnimTransition idleToJumpDown;
            idleToJumpDown.DestState = jumpDown;

            condition.Mode      = eAnimConditionMode::Less;
            condition.Parameter = "velocityY";
            condition.Threshold = 0.f;
            idleToJumpDown.Conditions.push_back(condition);

            idle->AddTransition(idleToJumpDown);


            AnimTransition idleToAttack;
            idleToAttack.DestState = atkBackStabberA;

            condition.Mode      = eAnimConditionMode::If;
            condition.Parameter = "attack";
            idleToAttack.Conditions.push_back(condition);

            idle->AddTransition(idleToAttack);
        }

        {
            AnimCondition condition;

            AnimTransition atkBackStabberAToIdle;
            atkBackStabberAToIdle.DestState = idle;
            atkBackStabberAToIdle.IsExit    = true;

            atkBackStabberA->AddTransition(atkBackStabberAToIdle);
        }

        {
            AnimCondition condition;

            AnimTransition walkToRun;
            walkToRun.DestState = run;

            condition.Mode      = eAnimConditionMode::GreaterOrEqual;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.7f;
            walkToRun.Conditions.push_back(condition);

            walk->AddTransition(walkToRun);

            AnimTransition walkToIdle;
            walkToIdle.DestState = idle;

            condition.Mode      = eAnimConditionMode::Less;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.3f;
            walkToIdle.Conditions.push_back(condition);

            walk->AddTransition(walkToIdle);
        }

        {
            AnimCondition condition;

            AnimTransition idleRunToRun;
            idleRunToRun.DestState = run;
            idleRunToRun.IsExit    = true;

            idleRun->AddTransition(idleRunToRun);
        }

        {
            AnimCondition condition;

            AnimTransition runIdleToIdle;
            runIdleToIdle.DestState = idle;
            runIdleToIdle.IsExit    = true;

            runIdle->AddTransition(runIdleToIdle);
        }

        {
            AnimCondition condition;

            AnimTransition volteToRun;
            volteToRun.DestState = run;
            volteToRun.IsExit    = true;

            volte->AddTransition(volteToRun);
        }

        {
            AnimCondition condition;

            AnimTransition runToRunIdle;
            runToRunIdle.DestState = runIdle;

            condition.Mode      = eAnimConditionMode::Less;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.3f;
            runToRunIdle.Conditions.push_back(condition);

            run->AddTransition(runToRunIdle);

            AnimTransition runToWalk;
            runToWalk.DestState = walk;

            condition.Mode      = eAnimConditionMode::Less;
            condition.Parameter = "velocityX";
            condition.Threshold = 0.7f;
            runToWalk.Conditions.push_back(condition);

            run->AddTransition(runToWalk);

            AnimTransition runToJumpUp;
            runToJumpUp.DestState = jumpUp;

            condition.Mode      = eAnimConditionMode::GreaterOrEqual;
            condition.Parameter = "velocityY";
            condition.Threshold = 0.01f;
            runToJumpUp.Conditions.push_back(condition);

            run->AddTransition(runToJumpUp);


            AnimTransition runToVolte;
            runToVolte.DestState = volte;

            condition.Mode      = eAnimConditionMode::If;
            condition.Parameter = "volte";
            runToVolte.Conditions.push_back(condition);

            run->AddTransition(runToVolte);
        }


        {
            AnimCondition condition;

            AnimTransition jumpUpToJumpTransition;
            jumpUpToJumpTransition.DestState = jumpTransition;

            condition.Mode      = eAnimConditionMode::Less;
            condition.Parameter = "velocityY";
            condition.Threshold = 0.f;
            jumpUpToJumpTransition.Conditions.push_back(condition);

            jumpUp->AddTransition(jumpUpToJumpTransition);
        }

        {
            AnimCondition condition;

            AnimTransition jumpTransitionToJumpDown;
            jumpTransitionToJumpDown.DestState = jumpDown;
            jumpTransitionToJumpDown.IsExit    = true;

            jumpTransition->AddTransition(jumpTransitionToJumpDown);
        }


        {
            AnimCondition condition;

            AnimTransition jumpDownToLand;
            jumpDownToLand.DestState = land;

            condition.Mode      = eAnimConditionMode::If;
            condition.Parameter = "grounded";
            jumpDownToLand.Conditions.push_back(condition);

            jumpDown->AddTransition(jumpDownToLand);
        }


        {
            AnimCondition condition;

            AnimTransition landToRise;
            landToRise.DestState = rise;
            landToRise.IsExit    = true;

            land->AddTransition(landToRise);
        }


        {
            AnimCondition condition;

            AnimTransition riseToIdle;
            riseToIdle.DestState = idle;
            riseToIdle.IsExit    = true;

            rise->AddTransition(riseToIdle);
        }
    }

    void CResMgr::Init()
    {
        InitSound();

        CreateDefaultMesh();

        CreateDefaultTexture();

        CreateDefaultGraphicsShader();

        CreateDefaultComputeShader();

        CreateDefaultMaterial();

        CreateFromAtlasFile("atlas\\zombie.atlas");
        CreateFromAtlasFile("atlas\\beheaded.atlas");
        CreateFromAtlasFile("atlas\\prison.atlas");
        CreateFromAtlasFile("atlas\\fxCommon.atlas");

        CreateDefaultAnimController();

        FindRes<AnimClip>(L"beheaded/atkBackStabberA")->AddEventToFrame(5,
            []
            {
                /*Entity* hand = CLevelMgr::GetInst()
                               ->GetCurLevel()->
                               FindObjectByName(u8"Hand");

                auto hitBox = new Entity;
                hitBox->SetName(u8"EnemyHitBox");

                hitBox->AddComponent(new Transform);
                hitBox->AddComponent(new Collider2D);

                hitBox->Transform()->SetRelativePos(
                    hand->Transform()->GetWorldPos());
                hitBox->Transform()->SetRelativeScale(
                    Vec3(2.7f, 2.7f, 1.f));

                hitBox->Collider2D()->SetCollider2DType(
                    eCollider2DType::Circle);

                hitBox->SetTag(eTag::EnemyHitBox);

                tEvent event;
                event.eType  = eEventType::CreateObject;
                event.wParam = (DWORD_PTR)hitBox;
                event.lParam = (DWORD_PTR)eLayer::General;

                CEventMgr::GetInst()->AddEvent(event);*/
            });
    }

    void CResMgr::InitSound()
    {
        FMOD::System_Create(&Sound::g_pFMOD);

        if (nullptr == Sound::g_pFMOD)
        {
            assert(nullptr);
        }

        // 32개 채널 생성
        Sound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
    }

    void CResMgr::CreateDefaultMesh()
    {
        vector<Vtx>  vecVtx;
        vector<UINT> vecIdx;
        Vtx          v;
        Ref<Mesh>   pMesh = nullptr;

        // Point Mesh
        v.vPos   = Vec3(0.f, 0.f, 0.f);
        v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        v.vUV    = Vec2(0.f, 0.f);

        UINT idx = 0;

        pMesh = CreateRef<Mesh>();
        pMesh->Create(&v, 1, &idx, 1);
        AddRes<Mesh>(L"PointMesh", pMesh);


        // 사각형 메쉬 만들기
        v.vPos = Vec3(-0.5f, 0.5f, 0.f);
        v.vNormal = Vec3(0.f, 0.f, -1.f);
        v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        v.vUV = Vec2(0.f, 0.f);
        vecVtx.push_back(v);

        v.vPos = Vec3(0.5f, 0.5f, 0.f);
        v.vNormal = Vec3(0.f, 0.f, -1.f);
        v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        v.vUV = Vec2(1.f, 0.f);
        vecVtx.push_back(v);

        v.vPos = Vec3(0.5f, -0.5f, 0.f);
        v.vNormal = Vec3(0.f, 0.f, -1.f);
        v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        v.vUV = Vec2(1.f, 1.f);
        vecVtx.push_back(v);

        v.vPos = Vec3(-0.5f, -0.5f, 0.f);
        v.vNormal = Vec3(0.f, 0.f, -1.f);
        v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        v.vUV = Vec2(0.f, 1.f);
        vecVtx.push_back(v);

        // Index
        vecIdx.push_back(0);
        vecIdx.push_back(2);
        vecIdx.push_back(3);

        vecIdx.push_back(0);
        vecIdx.push_back(1);
        vecIdx.push_back(2);

        pMesh = CreateRef<Mesh>();
        pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"RectMesh", pMesh);
        vecIdx.clear();


        // Index
        vecIdx.push_back(0);
        vecIdx.push_back(1);
        vecIdx.push_back(2);
        vecIdx.push_back(3);
        vecIdx.push_back(0);

        pMesh = CreateRef<Mesh>();
        pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"RectMesh_Debug", pMesh);
        vecVtx.clear();
        vecIdx.clear();


        // 원형메쉬 만들기
        // 중심점	
        v.vPos   = Vec3(0.f, 0.f, 0.f);
        v.vNormal = Vec3(0.f, 0.f, -1.f);
        v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        v.vUV    = Vec2(0.5f, 0.5f);

        vecVtx.push_back(v);

        int   iSlice  = 40;
        float fRadius = 0.5f;
        float fTheta  = XM_2PI / (float)iSlice;

        for (int i = 0; i < iSlice; ++i)
        {
            v.vPos   = Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f);
            v.vNormal = Vec3(0.f, 0.f, -1.f);
            v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
            v.vUV    = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));

            vecVtx.push_back(v);
        }

        for (UINT i = 0; i < (UINT)iSlice; ++i)
        {
            vecIdx.push_back(0);
            if (i == iSlice - 1)
            {
                vecIdx.push_back(1);
            }
            else
            {
                vecIdx.push_back(i + 2);
            }
            vecIdx.push_back(i + 1);
        }

        pMesh = CreateRef<Mesh>();
        pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"CircleMesh", pMesh);
        vecIdx.clear();


        // Cicle_debug
        for (int i = 0; i < vecVtx.size() - 2; ++i)
        {
            vecIdx.push_back(i + 1);
        }
        vecIdx.push_back(1);
    
        pMesh = CreateRef<Mesh>();
        pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"CircleMesh_Debug", pMesh);
        vecVtx.clear();
        vecIdx.clear();



        // Cube Mesh
        Vtx arrCube[24] = {};
        // 윗면
        arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
        arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        arrCube[0].vUV = Vec2(0.f, 0.f);
        arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

        arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
        arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        arrCube[1].vUV = Vec2(0.f, 0.f);
        arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

        arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
        arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        arrCube[2].vUV = Vec2(0.f, 0.f);
        arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

        arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
        arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        arrCube[3].vUV = Vec2(0.f, 0.f);
        arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


        // 아랫 면	
        arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
        arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        arrCube[4].vUV = Vec2(0.f, 0.f);
        arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

        arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
        arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        arrCube[5].vUV = Vec2(0.f, 0.f);
        arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

        arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
        arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        arrCube[6].vUV = Vec2(0.f, 0.f);
        arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

        arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
        arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
        arrCube[7].vUV = Vec2(0.f, 0.f);
        arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

        // 왼쪽 면
        arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
        arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        arrCube[8].vUV = Vec2(0.f, 0.f);
        arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

        arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
        arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        arrCube[9].vUV = Vec2(0.f, 0.f);
        arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

        arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
        arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        arrCube[10].vUV = Vec2(0.f, 0.f);
        arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

        arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
        arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
        arrCube[11].vUV = Vec2(0.f, 0.f);
        arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

        // 오른쪽 면
        arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
        arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        arrCube[12].vUV = Vec2(0.f, 0.f);
        arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

        arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
        arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        arrCube[13].vUV = Vec2(0.f, 0.f);
        arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

        arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
        arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        arrCube[14].vUV = Vec2(0.f, 0.f);
        arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

        arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
        arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
        arrCube[15].vUV = Vec2(0.f, 0.f);
        arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

        // 뒷 면
        arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
        arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
        arrCube[16].vUV = Vec2(0.f, 0.f);
        arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

        arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
        arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
        arrCube[17].vUV = Vec2(0.f, 0.f);
        arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

        arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
        arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
        arrCube[18].vUV = Vec2(0.f, 0.f);
        arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

        arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
        arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
        arrCube[19].vUV = Vec2(0.f, 0.f);
        arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

        // 앞 면
        arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
        arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        arrCube[20].vUV = Vec2(0.f, 0.f);
        arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

        arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
        arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        arrCube[21].vUV = Vec2(0.f, 0.f);
        arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

        arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
        arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        arrCube[22].vUV = Vec2(0.f, 0.f);
        arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

        arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
        arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        arrCube[23].vUV = Vec2(0.f, 0.f);
        arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

        // 인덱스
        for (int i = 0; i < 12; i += 2)
        {
            vecIdx.push_back(i * 2);
            vecIdx.push_back(i * 2 + 1);
            vecIdx.push_back(i * 2 + 2);

            vecIdx.push_back(i * 2);
            vecIdx.push_back(i * 2 + 2);
            vecIdx.push_back(i * 2 + 3);
        }


        pMesh = CreateRef<Mesh>();
        pMesh->Create(arrCube, 24, vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"CubeMesh", pMesh);
        vecIdx.clear();

        // ===========
        // Sphere Mesh
        // ===========
        fRadius = 0.5f;

        // Top
        v.vPos = Vec3(0.f, fRadius, 0.f);
        v.vUV = Vec2(0.5f, 0.f);
        v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        v.vNormal = v.vPos;
        v.vNormal.Normalize();
        v.vTangent = Vec3(1.f, 0.f, 0.f);
        v.vBinormal = Vec3(0.f, 0.f, 1.f);
        vecVtx.push_back(v);

        // Body
        UINT iStackCount = 40; // 가로 분할 개수
        UINT iSliceCount = 40; // 세로 분할 개수

        float fStackAngle = XM_PI / iStackCount;
        float fSliceAngle = XM_2PI / iSliceCount;

        float fUVXStep = 1.f / (float)iSliceCount;
        float fUVYStep = 1.f / (float)iStackCount;

        for (UINT i = 1; i < iStackCount; ++i)
        {
            float phi = i * fStackAngle;

            for (UINT j = 0; j <= iSliceCount; ++j)
            {
                float theta = j * fSliceAngle;

                v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
                    , fRadius * cosf(i * fStackAngle)
                    , fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
                v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
                v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
                v.vNormal = v.vPos;
                v.vNormal.Normalize();

                v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
                v.vTangent.y = 0.f;
                v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
                v.vTangent.Normalize();

                v.vTangent.Cross(v.vNormal, v.vBinormal);
                v.vBinormal.Normalize();

                vecVtx.push_back(v);
            }
        }

        // Bottom
        v.vPos = Vec3(0.f, -fRadius, 0.f);
        v.vUV = Vec2(0.5f, 1.f);
        v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
        v.vNormal = v.vPos;
        v.vNormal.Normalize();

        v.vTangent = Vec3(1.f, 0.f, 0.f);
        v.vBinormal = Vec3(0.f, 0.f, -1.f);
        vecVtx.push_back(v);

        // 인덱스
        // 북극점
        for (UINT i = 0; i < iSliceCount; ++i)
        {
            vecIdx.push_back(0);
            vecIdx.push_back(i + 2);
            vecIdx.push_back(i + 1);
        }

        // 몸통
        for (UINT i = 0; i < iStackCount - 2; ++i)
        {
            for (UINT j = 0; j < iSliceCount; ++j)
            {
                // + 
                // | \
    			// +--+
                vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
                vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
                vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

                // +--+
                //  \ |
                //    +
                vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
                vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
                vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
            }
        }

        // 남극점
        UINT iBottomIdx = (UINT)vecVtx.size() - 1;
        for (UINT i = 0; i < iSliceCount; ++i)
        {
            vecIdx.push_back(iBottomIdx);
            vecIdx.push_back(iBottomIdx - (i + 2));
            vecIdx.push_back(iBottomIdx - (i + 1));
        }

        pMesh = CreateRef<Mesh>();
        pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
        AddRes<Mesh>(L"SphereMesh", pMesh);
        vecIdx.clear();
        vecIdx.clear();
    }

    void CResMgr::CreateDefaultTexture()
    {
        // NoiseTexture
        LoadRelative<Texture>(L"texture\\noise\\noise_01.png");
        LoadRelative<Texture>(L"texture\\noise\\noise_02.png");
        LoadRelative<Texture>(L"texture\\noise\\noise_03.jpg");

        // 텍스쳐 로딩
        LoadRelative<Texture>(L"texture\\Player.bmp");
        LoadRelative<Texture>(L"texture\\smokeparticle.png");
        LoadRelative<Texture>(L"texture\\Character.png");
        LoadRelative<Texture>(L"texture\\link.png");

        LoadRelative<Texture>(L"texture\\particle\\smokeparticle.png");
        LoadRelative<Texture>(L"texture\\particle\\CartoonSmoke.png");
        LoadRelative<Texture>(L"texture\\particle\\Bubbles50px.png");

        LoadRelative<Texture>(L"texture\\TILE.bmp");

        // 텍스쳐 생성
        CreateTexture(L"UAVTex",
            1024,
            1024,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
    }

    void CResMgr::CreateDefaultGraphicsShader()
    {
        AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
        AddInputLayout(DXGI_FORMAT_R32G32B32A32_FLOAT, "COLOR");
        AddInputLayout(DXGI_FORMAT_R32G32_FLOAT, "TEXCOORD");
        AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "TANGENT");
        AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "BINORMAL");
        AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "NORMAL");

        Ref<GraphicsShader> pShader = nullptr;

        // Std2D Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\MyShader.fx", "VS_My");
        pShader->CreatePixelShader(L"shader\\MyShader.fx", "PS_My");
        pShader->SetRSType(eRSType::CullNone);
        pShader->SetDomain(eShaderDomain::Mask);

        pShader->AddScalarParam(eScalarParam::Int0, "Test Int");
        pShader->AddScalarParam(eScalarParam::Float2, "Test Float");
        pShader->AddScalarParam(eScalarParam::Vec2_3, "Test Vec2");
        pShader->AddScalarParam(eScalarParam::Vec4_2, "Test Vec4");
        pShader->AddTexureParam(eTexParam::Tex0, "Output Texture 1");
        pShader->AddTexureParam(eTexParam::Tex1, "Output Texture 2");
        pShader->AddTexureParam(eTexParam::Tex2, "Output Texture 3");

        AddRes<GraphicsShader>(L"Std2DShader", pShader);

        // Std2dAlphaBlend Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
        pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_AlphaBlend");
        pShader->SetRSType(eRSType::CullNone);
        pShader->SetBSType(eBSType::AlphaBlend);
        pShader->SetDSType(eDSType::NoWrite);
        pShader->SetDomain(eShaderDomain::Transparent);

        pShader->AddTexureParam(eTexParam::Tex0, "Output Texture");


        AddRes<GraphicsShader>(L"Std2DAlphaBlendShader", pShader);


        // Grid Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\editor.fx", "VS_Grid");
        pShader->CreatePixelShader(L"shader\\editor.fx", "PS_Grid");

        pShader->SetRSType(eRSType::CullNone);
        pShader->SetBSType(eBSType::AlphaBlend);
        pShader->SetDSType(eDSType::NoWrite);

        pShader->SetDomain(eShaderDomain::Transparent);

        AddRes<GraphicsShader>(L"GridShader", pShader);

        // DebugDraw Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\debugdraw.fx", "VS_DebugDraw");
        pShader->CreatePixelShader(L"shader\\debugdraw.fx", "PS_DebugDraw");

        pShader->SetRSType(eRSType::CullNone);
        pShader->SetBSType(eBSType::AlphaBlend);
        pShader->SetDSType(eDSType::NoWrite);
        pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
        pShader->SetDomain(eShaderDomain::Transparent);

        AddRes<GraphicsShader>(L"DebugDrawShader", pShader);


        // DeadCell Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\test.fx", "VS_DeadCell");
        pShader->CreatePixelShader(L"shader\\test.fx", "PS_DeadCell");

        pShader->SetRSType(eRSType::CullNone);
        pShader->SetBSType(eBSType::Default);
        pShader->SetDSType(eDSType::Less);
        pShader->SetDomain(eShaderDomain::Mask);

        AddRes<GraphicsShader>(L"DeadCellShader", pShader);


        // ParticleRenderShader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\particlerender.fx", "VS_ParticleRender");
        pShader->CreateGeometryShader(L"shader\\particlerender.fx", "GS_ParticleRender");
        pShader->CreatePixelShader(L"shader\\particlerender.fx", "PS_ParticleRender");

        pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        pShader->SetRSType(eRSType::CullNone);
        pShader->SetBSType(eBSType::AlphaBlend);
        pShader->SetDSType(eDSType::NoWrite);
        pShader->SetDomain(eShaderDomain::Transparent);

        AddRes<GraphicsShader>(L"ParticleRenderShader", pShader);


        // PostProcess Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_PostProcess");
        pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_PostProcess");

        pShader->SetDSType(eDSType::NoTestNoWrite);
        pShader->SetDomain(eShaderDomain::PostProcess);

        AddRes<GraphicsShader>(L"PostProcessShader", pShader);


        // TileMap Shader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
        pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");

        pShader->SetDomain(eShaderDomain::Mask);
        pShader->SetRSType(eRSType::CullNone);

        AddRes<GraphicsShader>(L"TileMapShader", pShader);

        // Std3DShader
        pShader = CreateRef<GraphicsShader>();
        pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
        pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");

        pShader->SetRSType(eRSType::CullBack);
        pShader->SetDomain(eShaderDomain::Opaque);

        pShader->AddScalarParam(eScalarParam::Float0, "Specular Coefficient");

        AddRes<GraphicsShader>(L"Std3DShader", pShader);
    }

    void CResMgr::CreateDefaultComputeShader()
    {
        Ref<ComputeShader> pShader = nullptr;

        // Paint Shader
        pShader = CreateRef<PaintShader>();
        pShader->CreateComputeShader(L"shader\\compute.fx", "CS_Paint");
        AddRes<ComputeShader>(L"PaintShader", pShader);

        // Particle Update Shader
        pShader = CreateRef<ParticleUpdateShader>();
        pShader->CreateComputeShader(L"shader\\particleupdate.fx", "CS_ParticleUpdate");
        AddRes<ComputeShader>(L"ParticleUpdateShader", pShader);

        //pShader = new ParticleUpdateShader;
        //pShader->CreateComputeShader(L"shader\\particleupdate.fx", "CS_ParticleUpdate");
        //AddRes<ComputeShader>(L"ParticleUpdateShader", pShader);
        //
        //pShader = new ParticleUpdateShader;
        //pShader->CreateComputeShader(L"shader\\particleupdate.fx", "CS_ParticleUpdate");
        //AddRes<ComputeShader>(L"ParticleUpdateShader", pShader);
    }

    void CResMgr::CreateDefaultMaterial()
    {
        Ref<Material> pMtrl;

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"Std2DShader"));
        AddRes<Material>(L"Std2DMtrl", pMtrl);
        
        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"Std2DAlphaBlendShader"));
        AddRes<Material>(L"Std2DAlphaBlendMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"GridShader"));
        AddRes<Material>(L"GridMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"DebugDrawShader"));
        AddRes<Material>(L"DebugDrawMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"DeadCellShader"));
        AddRes<Material>(L"DeadCellMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"ParticleRenderShader"));
        AddRes<Material>(L"ParticleRenderMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"PostProcessShader"));
        AddRes<Material>(L"PostProcessMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"TileMapShader"));
        AddRes<Material>(L"TileMapMtrl", pMtrl);

        pMtrl = CreateRef<Material>();
        pMtrl->SetShader(FindRes<GraphicsShader>(L"Std3DShader"));
        AddRes<Material>(L"Std3DMtrl", pMtrl);
    }

    void CResMgr::AddInputLayout(DXGI_FORMAT format, const char* semanticName)
    {
        D3D11_INPUT_ELEMENT_DESC LayoutDesc = {};
        LayoutDesc.AlignedByteOffset        = m_iLayoutOffset;
        LayoutDesc.Format                   = format;
        LayoutDesc.InputSlot                = 0;
        LayoutDesc.InputSlotClass           = D3D11_INPUT_PER_VERTEX_DATA;
        LayoutDesc.SemanticName             = semanticName;
        LayoutDesc.SemanticIndex            = 0;
        m_vecLayoutInfo.push_back(LayoutDesc);

        m_iLayoutOffset += GetSizeofFormat(format);
    }


    void CResMgr::CreateFromAtlasFile(const fs::path& atlasPath)
    {
        wstring atlasName = atlasPath.stem().wstring();

        fs::path atlasFullPath = CPathMgr::GetInst()->GetContentPath();
        atlasFullPath += atlasPath;

        std::ifstream atlas(atlasFullPath, std::ios::binary);
        assert(atlas);

        char signature[5] = {};
        atlas.read(signature, 4);
        assert(strcmp(signature, "BATL") == 0);

        while (true)
        {
            char fileNameLen = 0;
            atlas.read(&fileNameLen, sizeof(char));
            if (fileNameLen == 0)
            {
                // End of Atlas File
                fs::path trackPath = atlasFullPath / L".." / (atlasName + L"_tracks.json");
                trackPath          = trackPath.lexically_normal();

                if (fs::exists(trackPath))
                {
                    // TODO yaml 이식
                    /*std::ifstream f(trackPath);
                YAML::Node          data = YAML::Load(f);

                for (const auto& animData : data.items())
                {
                    string     animName      = animData.key();
                    AnimClip* animationClip = FindRes<AnimClip>(
                        atlasName + L"/" + StringToWtring(animName)).Get();
                    animationClip->SetHasBone(true);

                    for (const auto& boneData : animData.value().items())
                    {
                        string boneName      = boneData.key();
                        auto   bonePositions = boneData.value();

                        for (int i = 0; i < bonePositions.size(); i += 3)
                        {
                            Vec3 bonePos;
                            bonePos.x = bonePositions[i];
                            bonePos.y = bonePositions[i + 1];
                            bonePos.z = bonePositions[i + 2];

                            animationClip->AddBonePosition(boneName, bonePos);
                        }
                    }
                }*/
                }

                break;
            }

            char fileName[256] = {};
            atlas.read(fileName, fileNameLen);

            fs::path texturePath  = atlasFullPath / ".." / fileName;
            texturePath           = texturePath.lexically_normal();
            Ref<Texture> texture = CResMgr::GetInst()->Load<Texture>(texturePath);
            texture->SetType(eTextureType::Multiple);
            texture->SetPixelsPerUnit(50);

            fs::path normalTexturePath = texturePath;
            normalTexturePath.replace_filename(texturePath.stem().wstring() + L"_n.png");
            Ref<Texture> normalTexture;
            if (fs::exists(normalTexturePath))
            {
                normalTexture = CResMgr::GetInst()->Load<Texture>(normalTexturePath);
            }


            const Vec2     textureSize = texture->GetSize();
            vector<string> namesNeedTrailingIndex;

            while (true)
            {
                char nameLen = 0;
                atlas.read(&nameLen, sizeof(char));
                if (nameLen == 0)
                {
                    // End of Texture File
                    CreateAnimClipFromTex(atlasName, texture, normalTexture);
                    break;
                }

                string name;
                name.resize(nameLen);
                atlas.read(name.data(), nameLen);

                char index = 0;
                atlas.read(&index, sizeof(char));


                // 인덱스가 1 이상이라는 뜻은 동일 이름에 인덱스만 다른 포맷임.
                // 스프라이트 별로 이름을 구분하기 위해 트레일링 인덱스를 이름에 붙여줘야 함.
                if (index > 0 ||
                    std::find(namesNeedTrailingIndex.begin(), namesNeedTrailingIndex.end(), name) != namesNeedTrailingIndex.
                    end())
                {
                    static const string DELIMITER = "_";

                    // 기존의 인덱스 0 스프라이트도 트레일링 인덱스를 이름에 붙여줌
                    if (index == 1)
                    {
                        SpriteFrm frm;
                        if (texture->FindFrame(name, &frm))
                        {
                            string tempName = name;

                            std::stringstream ss;
                            ss << std::setw(2) << std::setfill('0') << '0';
                            std::string trailingIndex = ss.str();

                            tempName += DELIMITER;
                            tempName += trailingIndex;

                            texture->AddFrame(frm, tempName);
                            texture->RemoveFrame(name);
                        }
                        // 만약 없다면 체크해둠으로써 트레일링 인덱스가 붙도록 함
                        else
                        {
                            namesNeedTrailingIndex.push_back(name);
                        }
                    }


                    std::stringstream ss;
                    ss << std::setw(2) << std::setfill('0') << std::to_string(index);
                    std::string trailingIndex = ss.str();

                    name += DELIMITER;
                    name += trailingIndex;
                }


                /*
            // 이름 속에 인덱스가 있는지 검사
            if (index == 0)
            {
                static const string DELIMITER = "_";

                const size_t delimiterPos = name.rfind(DELIMITER);
                if (delimiterPos != string::npos)
                {
                    string indexInName = name.substr(delimiterPos + DELIMITER.length(), name.length());
                    index              = std::stoi(indexInName);

                    name.erase(delimiterPos, name.length());
                }
            }*/

                bool rotate = false;
                atlas.read((char*)&rotate, sizeof(bool));

                struct RectShort
                {
                    short x;
                    short y;
                };

                RectShort leftTop;
                atlas.read((char*)&leftTop, sizeof(RectShort));

                RectShort size;
                atlas.read((char*)&size, sizeof(RectShort));

                RectShort offset;
                atlas.read((char*)&offset, sizeof(RectShort));

                RectShort origSize;
                atlas.read((char*)&origSize, sizeof(RectShort));

                SpriteFrm frm;
                frm.LeftTop  = Vec2{(float)leftTop.x, (float)leftTop.y} / textureSize;
                frm.Size     = Vec2{(float)size.x, (float)size.y} / textureSize;
                frm.Offset   = Vec2{(float)offset.x, (float)offset.y} / textureSize;
                frm.OrigSize = Vec2{(float)origSize.x, (float)origSize.y} / textureSize;

                texture->AddFrame(frm, name);

                // if (index > 0)
                // {
                // 	wstring wnameNoSuffix = StringToWtring(nameNoSuffix);
                // 	AnimClip* animationClip = FindRes<AnimClip>(wnameNoSuffix).Get();
                // 	if (!animationClip)
                // 	{
                // 		animationClip = new AnimClip(true);
                // 		AddRes<AnimClip>(wnameNoSuffix, animationClip);
                // 	}
                //
                // 	AnimationFrm animFrm;
                // 	animFrm.SpriteName = name;
                // 	animFrm.Sprite = frm;
                // 	animFrm.Duration = 0.f;
                //
                // 	animationClip->AddFrame(animFrm);
                // }
            }
        }
    }
    
}
