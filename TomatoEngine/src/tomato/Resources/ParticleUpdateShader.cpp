#include "pch.h"
#include "ParticleUpdateShader.h"

#include "tomato/Renderer/StructuredBuffer.h"

namespace tomato
{
    ParticleUpdateShader::ParticleUpdateShader()
        : ComputeShader(128, 1, 1)
        , m_ParticleBuffer(nullptr)
    {
    }

    ParticleUpdateShader::~ParticleUpdateShader()
    {
    }


    void ParticleUpdateShader::SetParticleBuffer(StructuredBuffer* _Buffer)
    {
        assert(_Buffer);

        m_ParticleBuffer = _Buffer;
        m_Param.iArr[0] = m_ParticleBuffer->GetElementCount();
    }

    void ParticleUpdateShader::UpdateData()
    {
        assert(m_ParticleBuffer);
        assert(m_ParticleShare);

        m_ParticleBuffer->UpdateData_CS(0, false);
        m_ParticleShare->UpdateData_CS(1, false);

        m_iGroupX = m_Param.iArr[0] / m_iGroupPerThreadX + 1;
        m_iGroupY = 1;
        m_iGroupZ = 1;
    }

    void ParticleUpdateShader::Clear()
    {
        m_ParticleBuffer->Clear();
        m_ParticleShare->Clear();
    }
    
}
