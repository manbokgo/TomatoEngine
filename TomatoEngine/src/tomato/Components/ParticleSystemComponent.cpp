#include "pch.h"
#include "ParticleSystemComponent.h"

#include "tomato/Components/TransformComponent.h"
#include "tomato/Core/CResMgr.h"
#include "tomato/Core/CTimeMgr.h"
#include "tomato/Renderer/StructuredBuffer.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Mesh.h"
#include "tomato/Resources/ParticleUpdateShader.h"

namespace tomato
{
    ParticleSystemComponent::ParticleSystemComponent()
        :  m_iMaxCount(1000)
        , m_iAliveCount(0)
        , m_vStartScale(Vec4(50.f, 50.f, 1.f, 0.f))
        , m_vEndScale(Vec4(10.f, 10.f, 1.f, 0.f))
        , m_vStartColor(Vec4(0.24f, 0.28f, 0.8f, 1.f))
        , m_vEndColor(Vec4(0.78f, 0.74f, 0.9f, 1.f))
        , m_vMinMaxSpeed(Vec2(100.f, 300.f))
        , m_vMinMaxLifeTime(Vec2(0.2f, 1.f))
        , m_fSpawnRange(200.f)
        , m_Frequency(5.f)
        , m_fAccTime(0.f)
        , m_ParticleBuffer(nullptr)
        , m_ParticleShare(nullptr)
        , m_WorldSpawn(1)
    {
        // m_Mesh = CResMgr::GetInst()->FindRes<Mesh>(L"PointMesh");
        m_Material = CResMgr::GetInst()->FindRes<Material>(L"ParticleRenderMtrl");
        m_UpdateCS = CResMgr::GetInst()->FindRes<ParticleUpdateShader>(L"ParticleUpdateShader");

        //GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<Texture>(L"texture\\particle\\smokeparticle.png"));
        //GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_0, CResMgr::GetInst()->FindRes<Texture>(L"texture\\particle\\CartoonSmoke.png"));	
        GetMaterial()->SetTexParam(eTexParam::Tex0, CResMgr::GetInst()->FindRes<Texture>(L"texture\\particle\\Bubbles50px.png"));


        // 구조화버퍼 생성
        m_ParticleBuffer = new StructuredBuffer;
        m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, eSBType::UAVInc, nullptr);

        m_ParticleShare = new StructuredBuffer;
        m_ParticleShare->Create(sizeof(tParticleShare), 1, eSBType::UAVInc, nullptr, true);
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
        if (nullptr != m_ParticleBuffer)
            delete m_ParticleBuffer;

        if (nullptr != m_ParticleShare)
            delete m_ParticleShare;
    }

    void ParticleSystemComponent::OnLateUpdate()
    {
        /*Vec3 vPos = Transform()->GetRelativePos();
	vPos.x += 500.f * DT;
	Transform()->SetRelativePos(vPos);*/


        // 파티클 생성 시간
        float fAliveTime = 1.f / m_Frequency;

        // 누적시간
        m_fAccTime += DT;

        // 누적시간이 파티클 생성 시간을 넘어서면
        if (fAliveTime < m_fAccTime)
        {
            float f = (m_fAccTime / fAliveTime);
            UINT  iAliveCount = (UINT)f;
            m_fAccTime = f - floorf(f);

            //tParticleShare share = { iAliveCount, };
            tParticleShare share = { 10, };
            m_ParticleShare->SetData(&share, 1);
        }
        else
        {
            tParticleShare share = {};
            m_ParticleShare->SetData(&share, 1);
        }

        m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
        m_UpdateCS->SetParticleShareData(m_ParticleShare);
        m_UpdateCS->SetWorldSpawn(m_WorldSpawn);
        m_UpdateCS->SetObjectWorldPos(Transform()->GetWorldPos());
        m_UpdateCS->SetSpawnRange(m_fSpawnRange);
        m_UpdateCS->SetMinMaxSpeed(m_vMinMaxSpeed);
        m_UpdateCS->SetMinMaxLifeTime(m_vMinMaxLifeTime);



        m_UpdateCS->Execute();
    }

    void ParticleSystemComponent::render()
    {
        // 위치정보 업데이트
        Transform()->UpdateData();
        m_ParticleBuffer->UpdateData(16, ePipelineStage::VS | ePipelineStage::GS | ePipelineStage::PS);

        // 재질 업데이트
        GetMaterial()->SetScalarParam(eScalarParam::Int1, &m_WorldSpawn);
        GetMaterial()->SetScalarParam(eScalarParam::Vec4_0, &m_vStartScale);
        GetMaterial()->SetScalarParam(eScalarParam::Vec4_1, &m_vEndScale);
        GetMaterial()->SetScalarParam(eScalarParam::Vec4_2, &m_vStartColor);
        GetMaterial()->SetScalarParam(eScalarParam::Vec4_3, &m_vEndColor);
        GetMaterial()->Bind();

        // 렌더링
        // GetMesh()->RenderInstanced(m_iMaxCount);

        // 클리어
        m_ParticleBuffer->Clear();
    }

    /*
void ParticleSystem::SaveToFile(FILE* _File)
{
	RenderComponent::SaveToFile(_File);

	// 컴퓨트 쉐이더 정보 저장
	bool bCS = !!m_UpdateCS;
	fwrite(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		SaveWStringToFile(m_UpdateCS->GetPath(), _File);
	}	

	// 파티클 옵션
	fwrite(&m_iMaxCount, sizeof(UINT), 1, _File);
	fwrite(&m_iAliveCount, sizeof(UINT), 1, _File);
	fwrite(&m_vStartScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vStartColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fwrite(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fwrite(&m_fSpawnRange, sizeof(float), 1, _File);
	fwrite(&m_Frequency, sizeof(float), 1, _File);
	fwrite(&m_fAccTime, sizeof(float), 1, _File);
	fwrite(&m_WorldSpawn, sizeof(int), 1, _File);
}

void ParticleSystem::LoadFromFile(FILE* _File)
{
	RenderComponent::LoadFromFile(_File);

	// 컴퓨트쉐이더 참조
	bool bCS = false;
	fread(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		wstring strKey;
		LoadWStringFromFile(strKey, _File);
		m_UpdateCS = std::dynamic_pointer_cast<ParticleUpdateShader>(CResMgr::GetInst()->FindRes<ComputeShader>(strKey));
	}

	// 파티클 옵션
	fwrite(&m_iMaxCount, sizeof(UINT), 1, _File);
	fwrite(&m_iAliveCount, sizeof(UINT), 1, _File);
	fwrite(&m_vStartScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vStartColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fwrite(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fwrite(&m_fSpawnRange, sizeof(float), 1, _File);
	fwrite(&m_Frequency, sizeof(float), 1, _File);
	fwrite(&m_fAccTime, sizeof(float), 1, _File);
	fwrite(&m_WorldSpawn, sizeof(int), 1, _File);
}
*/

    
}
