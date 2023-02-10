#pragma once
#include "ComputeShader.h"

#include "Texture.h"

namespace tomato
{
    class PaintShader :        public ComputeShader
    {
    private:
        Ref<Texture> m_pTargetTex;

    public:
        void SetTexture(Ref<Texture> _pTarget) 
        { 
            m_pTargetTex = _pTarget;
            m_Param.v2Arr[0] = _pTarget->GetSize();
        }

        void SetColor(Vec4 _vColor){m_Param.v4Arr[0] = _vColor;} 
    


    public:
        virtual void UpdateData() override;
        virtual void Clear() override;

    public:
        PaintShader();
        ~PaintShader();
    };

    
}
