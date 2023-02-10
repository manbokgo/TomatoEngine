#include "pch.h"
#include "Material.h"

#include "GraphicsShader.h"
#include "Texture.h"
#include "tomato/Core/CPathMgr.h"
#include "tomato/Renderer/ConstBuffer.h"
#include "tomato/Renderer/Device.h"

namespace tomato
{
    Material::Material()
        : Res(eResType::Material)
        , m_CB{} {
	
    }

    Material::Material(const Material& other)
        : Res(other)
        , m_CB(other.m_CB)
        , m_Shader(other.m_Shader)
    {
        for (UINT i = 0; i < (UINT)eTexParam::End; ++i)
        {
            m_arrTex[i] = other.m_arrTex[i];
        }	
    }

    Material::~Material()
    {

    }


    void Material::Bind()
    {
        // 텍스쳐 업데이트
        for (UINT i = 0; i < (UINT)eTexParam::End; ++i)
        {
            if (m_arrTex[i])
            {
                m_arrTex[i]->Bind(i, AllStage);
            }
            else
            {
                m_CB.SpriteInfo[i] = SpriteFrm();
            }
        }
	
        // 상수 업데이트
        ConstBuffer* pCB = Device::GetInst()->GetConstBuffer(eCBType::Material);
        pCB->SetData(&m_CB);
        pCB->UpdateData(ePipelineStage::AllStage);

        // 쉐이더 업데이트
        m_Shader->Bind();
    }

    void Material::Clear()
    {
        for (UINT i = 0; i < (UINT)eTexParam::End; ++i)
        {
            Texture::Clear(i);
        }	
    }


    void Material::SetScalarParam(eScalarParam scalarParam, void* data)
    {
        switch (scalarParam)
        {
        case eScalarParam::Int0:		
        case eScalarParam::Int1:		
        case eScalarParam::Int2:		
        case eScalarParam::Int3:
            m_CB.iArr[(UINT)scalarParam] = *(int*)data;
            break;
        case eScalarParam::Float0:
        case eScalarParam::Float1:
        case eScalarParam::Float2:
        case eScalarParam::Float3:
            m_CB.fArr[(UINT)scalarParam - (UINT)eScalarParam::Float0] = *(float*)data;
            break;
        case eScalarParam::Vec2_0:
        case eScalarParam::Vec2_1:
        case eScalarParam::Vec2_2:
        case eScalarParam::Vec2_3:
            m_CB.v2Arr[(UINT)scalarParam - (UINT)eScalarParam::Vec2_0] = *(Vec2*)data;
            break;
        case eScalarParam::Vec4_0:
        case eScalarParam::Vec4_1:
        case eScalarParam::Vec4_2:
        case eScalarParam::Vec4_3:
            m_CB.v4Arr[(UINT)scalarParam - (UINT)eScalarParam::Vec4_0] = *(Vec4*)data;
            break;
        case eScalarParam::Mat0:
        case eScalarParam::Mat1:
        case eScalarParam::Mat2:
        case eScalarParam::Mat3:
            m_CB.matArr[(UINT)scalarParam - (UINT)eScalarParam::Mat0] = *(Matrix*)data;
            break;	
        }
    }

    void Material::GetScalarParam(eScalarParam scalarParam, void* outData)
    {
        switch (scalarParam)
        {
        case eScalarParam::Int0:
        case eScalarParam::Int1:
        case eScalarParam::Int2:
        case eScalarParam::Int3:
            *(int*)outData = m_CB.iArr[(UINT)scalarParam];
            break;
        case eScalarParam::Float0:
        case eScalarParam::Float1:
        case eScalarParam::Float2:
        case eScalarParam::Float3:
            *(float*)outData = m_CB.fArr[(UINT)scalarParam - (UINT)eScalarParam::Float0];
            break;
        case eScalarParam::Vec2_0:
        case eScalarParam::Vec2_1:
        case eScalarParam::Vec2_2:
        case eScalarParam::Vec2_3:
            *(Vec2*)outData = m_CB.v2Arr[(UINT)scalarParam - (UINT)eScalarParam::Vec2_0];
            break;
        case eScalarParam::Vec4_0:
        case eScalarParam::Vec4_1:
        case eScalarParam::Vec4_2:
        case eScalarParam::Vec4_3:
            *(Vec4*)outData = m_CB.v4Arr[(UINT)scalarParam - (UINT)eScalarParam::Vec4_0];
            break;
        case eScalarParam::Mat0:
        case eScalarParam::Mat1:
        case eScalarParam::Mat2:
        case eScalarParam::Mat3:
            *(Matrix*)outData = m_CB.matArr[(UINT)scalarParam - (UINT)eScalarParam::Mat0];
            break;
        }
    }

    void Material::SetTexParam(eTexParam texParam, const Ref<Texture>& texture)
    {
        SpriteFrm spriteFrm;
        spriteFrm.Size = Vec2{1.f, 1.f};
        spriteFrm.OrigSize = Vec2{ 1.f, 1.f };

        m_arrTex[(UINT)texParam] = texture;
        m_CB.SpriteInfo[(UINT)texParam] = spriteFrm;
    }

    void Material::SetTexParam(eTexParam texParam, const Ref<Texture>& texture, const SpriteFrm& spriteFrm)
    {
        m_arrTex[(UINT)texParam] = texture;
        m_CB.SpriteInfo[(UINT)texParam] = spriteFrm;
    }
}
