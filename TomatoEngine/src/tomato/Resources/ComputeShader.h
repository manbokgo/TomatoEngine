#pragma once
#include "Shader.h"

namespace tomato
{
    class ComputeShader :        public Shader
    {
    private:
        ComPtr<ID3DBlob>            m_CSBlob;
        ComPtr<ID3D11ComputeShader> m_CS;

    protected:
        tMtrlConst m_Param; // CS 에 전달할 상수 값

        UINT m_iGroupX;
        UINT m_iGroupY;
        UINT m_iGroupZ;

        const UINT m_iGroupPerThreadX;
        const UINT m_iGroupPerThreadY;
        const UINT m_iGroupPerThreadZ;

    public:
        void CreateComputeShader(const wstring& _strRelativePath, const string& _strFuncName);
        void Execute();

        virtual void UpdateData() = 0;
        virtual void Clear() = 0;

    public:
        ComputeShader(UINT _iGroupPerX, UINT _iGroupPerY, UINT _iGroupPerZ);
        ~ComputeShader();
    };

    
}
