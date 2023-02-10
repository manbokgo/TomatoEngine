#include "pch.h"
#include "GraphicsShader.h"

#include "tomato/Core/CPathMgr.h"
#include "tomato/Core/CResMgr.h"
#include "tomato/Renderer/Device.h"

namespace tomato
{
    GraphicsShader::GraphicsShader()
        : Shader(eResType::GraphicsShader) { }

    GraphicsShader::~GraphicsShader() {}

    void GraphicsShader::CreateVertexShader(const wstring& _strRelativePath, const string& _strFuncName)
    {
        wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
        strFilePath += _strRelativePath;

        UINT Flag = 0;

        #ifdef _DEBUG
        Flag |= D3DCOMPILE_DEBUG;
        Flag |= D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif // _DEBUG


        HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
            , _strFuncName.c_str(), "vs_5_0", Flag, 0, m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

        if (FAILED(hr))
        {
            const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
            MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
        }

        hr = DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr,
            m_VS.GetAddressOf());


        const vector<D3D11_INPUT_ELEMENT_DESC>& vecLayout = CResMgr::GetInst()->GetInputLayoutInfo();

        hr = DEVICE->CreateInputLayout(vecLayout.data(), (UINT)vecLayout.size()
            , m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
            , m_Layout.GetAddressOf());

        assert(!FAILED(hr));
    }


    void GraphicsShader::CreateGeometryShader(const wstring& _strRelativePath, const string& _strFuncName)
    {
        wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
        strFilePath += _strRelativePath;

        HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
            , _strFuncName.c_str(), "gs_5_0", 0, 0, m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

        if (FAILED(hr))
        {
            const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
            MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
            assert(nullptr);
        }

        hr = DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer(), m_GSBlob->GetBufferSize(), nullptr,
            m_GS.GetAddressOf());

        assert(!FAILED(hr));
    }

    void GraphicsShader::CreatePixelShader(const wstring& _strRelativePath, const string& _strFuncName)
    {
        wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
        strFilePath += _strRelativePath;

        UINT Flag = 0;
        #ifdef _DEBUG
        Flag |= D3DCOMPILE_DEBUG;
        Flag |= D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif // _DEBUG


        HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
            , _strFuncName.c_str(), "ps_5_0", Flag, 0, m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

        if (FAILED(hr))
        {
            const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
            MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
            assert(nullptr);
        }


        hr = DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), nullptr,
            m_PS.GetAddressOf());

        assert(!FAILED(hr));
    }

    void GraphicsShader::Bind()
    {
        CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
        CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
        CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
        CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
        CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

        // 레스터라이즈 스테이트 설정
        ComPtr<ID3D11RasterizerState> RSState = Device::GetInst()->GetRasterizerState(m_eRSType);
        CONTEXT->RSSetState(RSState.Get());

        // 뎊스 스텐실 스테이트 설정
        ComPtr<ID3D11DepthStencilState> DSState = Device::GetInst()->GetDepthStencilState(m_eDSType);
        CONTEXT->OMSetDepthStencilState(DSState.Get(), 0);

        // 블렌드 스테이트 설정
        ComPtr<ID3D11BlendState> BSState = Device::GetInst()->GetBlendState(m_eBSType);
        CONTEXT->OMSetBlendState(BSState.Get(), nullptr, 0xffffff);

        CONTEXT->IASetInputLayout(m_Layout.Get());
        CONTEXT->IASetPrimitiveTopology(m_eTopology);
    }
}
