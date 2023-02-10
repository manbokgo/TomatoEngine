#include "pch.h"
#include "Texture.h"

#include "tomato/Renderer/Device.h"

namespace tomato
{
    Texture::Texture()
        : Res(eResType::Texture)
        , m_Desc()
        , m_FrameDefaultIdx(0)
        , m_PixelsPerUnit(0)
        , m_Type(eTextureType::Single) {}

    Texture::~Texture() { }

    void Texture::Save(const fs::path& path) {}
    int  Texture::Load(const fs::path& path)
    {
        //TODO 메타데이터도 둬서 PPU 설정
        //TODO 스크래치 이미지 없애버리기
        HRESULT hr = E_FAIL;

        //TODO 소문자?
        const fs::path extension = path.extension();
        if (extension == L".dds" || extension == L".DDS")
        {
            hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
        }
        else if (extension == L".tga" || extension == L".TGA")
        {
            hr = LoadFromTGAFile(path.c_str(), nullptr, m_Image);
        }
        else // WIC (png, jpg, jpeg, bmp )
        {
            hr = LoadFromWICFile(path.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
        }

        if (FAILED(hr))
        {
            return hr;
        }


        // sysmem -> GPU
        hr = CreateShaderResourceView(DEVICE,
            m_Image.GetImages(),
            m_Image.GetImageCount(),
            m_Image.GetMetadata(),
            m_SRV.GetAddressOf());

        m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
        m_Tex2D->GetDesc(&m_Desc);

        return hr;
    }

    const SpriteFrm& Texture::GetFrame(string spriteName) const
    {
        assert(m_Frames.count(spriteName));
        return m_Frames.at(spriteName);
    }

    void Texture::Create(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag)
    {
        if (width == 0 || height == 0)
        {
            width = 1;
            height = 1;
        }

        // DepthStencilTexture
        m_Desc.BindFlags = bindFlag;

        m_Desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        m_Desc.CPUAccessFlags = 0;

        m_Desc.Format = format;
        m_Desc.Width = width;
        m_Desc.Height = height;
        m_Desc.ArraySize = 1;

        m_Desc.SampleDesc.Count = 1;
        m_Desc.SampleDesc.Quality = 0;

        m_Desc.MipLevels = 1;
        m_Desc.MiscFlags = 0;

        HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
        assert(!FAILED(hr));

        // View 생성
        if (bindFlag & D3D11_BIND_DEPTH_STENCIL)
        {
            hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
        }
        else
        {
            if (bindFlag & D3D11_BIND_RENDER_TARGET)
            {
                hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
                assert(!FAILED(hr));
            }

            if (bindFlag & D3D11_BIND_SHADER_RESOURCE)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
                tSRVDesc.Format = m_Desc.Format;
                tSRVDesc.Texture2D.MipLevels = 1;
                tSRVDesc.Texture2D.MostDetailedMip = 0;
                tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
                hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf());
                assert(!FAILED(hr));
            }

            if (bindFlag & D3D11_BIND_UNORDERED_ACCESS)
            {
                D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
                tUAVDesc.Format = m_Desc.Format;
                tUAVDesc.Texture2D.MipSlice = 0;
                tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
                hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &tUAVDesc, m_UAV.GetAddressOf());
                assert(!FAILED(hr));
            }
        }
    }

    void Texture::Create(const ComPtr<ID3D11Texture2D>& tex2D)
    {
        m_Tex2D = tex2D;
        m_Tex2D->GetDesc(&m_Desc);

        // View 생성
        HRESULT hr = S_OK;
        if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
        {
            hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
        }
        else
        {
            if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
            {
                hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
                assert(!FAILED(hr));
            }

            if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
                tSRVDesc.Format = m_Desc.Format;
                tSRVDesc.Texture2D.MipLevels = 1;
                tSRVDesc.Texture2D.MostDetailedMip = 0;
                tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
                hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf());
                assert(!FAILED(hr));
            }

            if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
            {
                D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
                tUAVDesc.Format = m_Desc.Format;
                tUAVDesc.Texture2D.MipSlice = 0;
                tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
                hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &tUAVDesc, m_UAV.GetAddressOf());
                assert(!FAILED(hr));
            }
        }
    }

    void Texture::Bind(UINT registerNum, UINT pipelineStage)
    {
        if ((UINT)ePipelineStage::VS & pipelineStage)
            CONTEXT->VSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());

        if ((UINT)ePipelineStage::HS & pipelineStage)
            CONTEXT->HSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());

        if ((UINT)ePipelineStage::DS & pipelineStage)
            CONTEXT->DSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());

        if ((UINT)ePipelineStage::GS & pipelineStage)
            CONTEXT->GSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());

        if ((UINT)ePipelineStage::PS & pipelineStage)
            CONTEXT->PSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
    }

    void Texture::BindCS(UINT registerNum, bool shaderRes)
    {
        if (shaderRes)
        {
            CONTEXT->CSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
        }
        else
        {
            assert(m_UAV.Get());

            UINT i = -1;
            CONTEXT->CSSetUnorderedAccessViews(registerNum, 1, m_UAV.GetAddressOf(), &i);
        }
    }

    void Texture::Clear(UINT registerNum)
    {
        CONTEXT->VSSetShaderResources(registerNum, 1, nullptr);
        CONTEXT->HSSetShaderResources(registerNum, 1, nullptr);
        CONTEXT->DSSetShaderResources(registerNum, 1, nullptr);
        CONTEXT->GSSetShaderResources(registerNum, 1, nullptr);
        CONTEXT->PSSetShaderResources(registerNum, 1, nullptr);
    }

    void Texture::ClearCS(UINT registerNum)
    {
        ID3D11UnorderedAccessView* pUAV = nullptr;
        UINT                       i = -1;
        CONTEXT->CSSetUnorderedAccessViews(registerNum, 1, &pUAV, &i);
    }

    void Texture::AddFrame(SpriteFrm frame, string name)
    {
        assert(m_Frames.count(name) == 0);
        m_Frames[name] = frame;
    }

    void Texture::AddFrame(SpriteFrm frame)
    {
        string arbitraryName = std::to_string(m_FrameDefaultIdx++);
        while (m_Frames.count(arbitraryName))
        {
            arbitraryName = std::to_string(m_FrameDefaultIdx++);
        }

        m_Frames[arbitraryName] = frame;
    }

    bool Texture::FindFrame(string name, SpriteFrm* outFrm)
    {
        const auto iter = m_Frames.find(name);
        if (iter == m_Frames.end())
        {
            return false;
        }

        *outFrm = iter->second;
        return true;
    }


    bool Texture::RemoveFrame(string name)
    {
        const auto iter = m_Frames.find(name);
        if (iter == m_Frames.end())
        {
            return false;
        }

        m_Frames.erase(iter);
        return true;
    }
}
