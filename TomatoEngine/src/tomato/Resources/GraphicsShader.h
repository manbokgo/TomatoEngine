#pragma once
#include "Shader.h"

namespace tomato
{
    class GraphicsShader : public Shader
    {
    public:
        GraphicsShader();
        ~GraphicsShader() override;

        void CreateVertexShader(const wstring& _strRelativePath, const string& _strFuncName);
        //void CreateHullShader(const wstring& _strRelativePath, const string& _strFuncName);
        //void CreateDomainShader(const wstring& _strRelativePath, const string& _strFuncName);
        void CreateGeometryShader(const wstring& _strRelativePath, const string& _strFuncName);
        void CreatePixelShader(const wstring& _strRelativePath, const string& _strFuncName);

        void Bind();

        void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_eTopology = topology; }
        void SetRSType(eRSType rsType) { m_eRSType = rsType; }
        void SetBSType(eBSType bsType) { m_eBSType = bsType; }
        void SetDSType(eDSType dsType) { m_eDSType = dsType; }
        void SetDomain(eShaderDomain domain) { m_eDomain = domain; }

        [[nodiscard]] eShaderDomain GetDomain() const { return m_eDomain; }

    private:
        ComPtr<ID3DBlob> m_VSBlob;
        ComPtr<ID3DBlob> m_HSBlob;
        ComPtr<ID3DBlob> m_DSBlob;
        ComPtr<ID3DBlob> m_GSBlob;
        ComPtr<ID3DBlob> m_PSBlob;

        ComPtr<ID3D11VertexShader>   m_VS;
        ComPtr<ID3D11HullShader>     m_HS;
        ComPtr<ID3D11DomainShader>   m_DS;
        ComPtr<ID3D11GeometryShader> m_GS;
        ComPtr<ID3D11PixelShader>    m_PS;

        ComPtr<ID3D11InputLayout> m_Layout;

        eRSType m_eRSType = eRSType::CullBack;
        eBSType m_eBSType = eBSType::Default;
        eDSType m_eDSType = eDSType::Less;

        D3D11_PRIMITIVE_TOPOLOGY m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        eShaderDomain            m_eDomain = eShaderDomain::None;
    };
}
