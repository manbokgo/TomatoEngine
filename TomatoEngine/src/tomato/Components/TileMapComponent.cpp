#include "pch.h"
#include "TileMapComponent.h"

#include "tomato/Components/TransformComponent.h"
#include "tomato/Core/CResMgr.h"
#include "tomato/Renderer/StructuredBuffer.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Mesh.h"

namespace tomato
{
    TileMapComponent::TileMapComponent()
        : m_bDataChanged(false)
    {
        SetMesh(CResMgr::GetInst()->FindRes<Mesh>(L"RectMesh"));
        SetSharedMaterial(CResMgr::GetInst()->FindRes<Material>(L"TileMapMtrl"));

        m_TileBuffer = new StructuredBuffer;
    }

    TileMapComponent::~TileMapComponent()
    {
        if (nullptr != m_TileBuffer)
            delete m_TileBuffer;
    }

    void TileMapComponent::SetTileCount(UINT _iWidth, UINT _iHeight)
    {
        m_vTileCount = Vec2((float)_iWidth, (float)_iHeight);

        m_vecTile.clear();
        m_vecTile.resize(_iWidth * _iHeight);

        if ( m_TileBuffer->GetElementCount() < _iWidth * _iHeight)
        {
            m_TileBuffer->Create(sizeof(tTile), _iWidth * _iHeight, eSBType::SRVOnly, nullptr, true);
        }

        //// 타일 데이터 강제 세팅
        //float fWidth = (float)m_AtlasTex->GetWidth();
        //float fHeight = (float)m_AtlasTex->GetHeight();

        //Vec2 leftTop = Vec2(64.f, 64.f);
        //Vec2 size = Vec2(64.f, 64.f);

        //leftTop /= Vec2(fWidth, fHeight);
        //size /= Vec2(fWidth, fHeight);

        //for (size_t i = 0; i < m_vecTile.size(); ++i)
        //{
        //	m_vecTile[i].leftTop = leftTop;
        //	m_vecTile[i].size = size;
        //}

        //m_vecTile[0].leftTop = Vec2(0.f, 0.f);

        //m_TileBuffer->SetData(m_vecTile.data(), (UINT)(m_vTileCount.x * m_vTileCount.y));
    }

    void TileMapComponent::OnLateUpdate()
    {
	
    }

    void TileMapComponent::render()
    {
        Transform()->UpdateData();
	
        if (m_bDataChanged)
        {
            m_TileBuffer->SetData(m_vecTile.data(), (UINT)(m_vTileCount.x * m_vTileCount.y));
        }	
        m_TileBuffer->UpdateData(17, ePipelineStage::PS);
	
        Vec2 vLeftTop = Vec2(64.f, 64.f);
        Vec2 vSlice = Vec2(64.f, 64.f);

        GetCurMaterial()->SetScalarParam(eScalarParam::Vec2_0, &vLeftTop);
        GetCurMaterial()->SetScalarParam(eScalarParam::Vec2_1, &vSlice);
        GetCurMaterial()->SetScalarParam(eScalarParam::Vec2_2, &m_vTileCount);

        GetCurMaterial()->SetTexParam(eTexParam::Tex0, m_AtlasTex);

        GetCurMaterial()->Bind();

        GetMesh()->Render();
    }

    
}
