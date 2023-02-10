#pragma once
#include "Res.h"

namespace tomato
{
    class Shader : public Res
    {
    public:
        Shader(eResType _eResType);
        ~Shader();

        void Save(const fs::path& path) override {};
        int  Load(const fs::path& path) override { return S_OK; };
        
        void AddScalarParam(eScalarParam _eParam, const string& _name)
        {
            m_vecScalarParam.push_back(tScalarParam{ _eParam, _name });
        }

        void AddTexureParam(eTexParam _eParam, const string& _name)
        {
            m_vecTexParam.push_back(tTextureParam{ _eParam, _name });
        }

        const vector<tScalarParam>& GetScalarParam() { return m_vecScalarParam; }
        const vector<tTextureParam>& GetTextureParam() { return m_vecTexParam; }

    protected:
        vector<tScalarParam>  m_vecScalarParam;
        vector<tTextureParam> m_vecTexParam;

        ComPtr<ID3DBlob> m_ErrBlob;
    };
}
