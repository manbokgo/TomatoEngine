#include "pch.h"
#include "PaintShader.h"

namespace tomato
{
    PaintShader::PaintShader()
        : ComputeShader(32, 32, 1)
    {
    }

    PaintShader::~PaintShader()
    {
    }

    void PaintShader::UpdateData()
    {
        // Target Textrue �� U0 �� ���ε�
        m_pTargetTex->BindCS(0, false);

        // �׷� �� ���
        m_iGroupX = (UINT)(m_pTargetTex->GetSize().x / m_iGroupPerThreadX + 1);
        m_iGroupY = (UINT)(m_pTargetTex->GetSize().y / m_iGroupPerThreadY + 1);
        m_iGroupZ = 1;
    }

    void PaintShader::Clear()
    {
        Texture::ClearCS(0);
    }
    
}
