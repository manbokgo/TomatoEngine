#include "pch.h"
#include "Device.h"

#include "tomato/Renderer/ConstBuffer.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    Device::Device()
        : m_hWnd(nullptr)
        , m_tViewPort{}
        , m_arrCB{}
        , m_arrRS{}
        , m_arrDS{}
        , m_arrBS{} {}

    Device::~Device()
    {
        for (UINT i = 0; i < (UINT)eCBType::End; ++i)
        {
            if (nullptr != m_arrCB[i])
                delete m_arrCB[i];
        }
    }

    int Device::init(HWND _hWnd, Vec2 _vResolution)
    {
        m_hWnd = _hWnd;
        m_vRenderResolution = _vResolution;
        g_global.vRenderResolution = m_vRenderResolution;

        UINT              iDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0;


        ID3D11Device* pDevice = nullptr;
        HRESULT       hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr
            , iDeviceFlag, nullptr, 0
            , D3D11_SDK_VERSION
            , m_pDevice.GetAddressOf()
            , &FeatureLevel
            , m_pDeviceContext.GetAddressOf());

        UINT iQuality = 0;
        DEVICE->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &iQuality);


        // 스왚체인 만들기
        if (FAILED(CreateSwapchain()))
        {
            MessageBox(nullptr, L"스왚체인 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }

        // RenderTarget, DepthStencilTarget 만들기
        if (FAILED(CreateTarget()))
        {
            MessageBox(nullptr, L"타겟 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }


        // ViewPort
        m_tViewPort.TopLeftX = 0;
        m_tViewPort.TopLeftY = 0;
        m_tViewPort.Width = _vResolution.x;
        m_tViewPort.Height = _vResolution.y;
        m_tViewPort.MinDepth = 0.f;
        m_tViewPort.MaxDepth = 1.f;

        m_pDeviceContext->RSSetViewports(1, &m_tViewPort);


        // 샘플러 생성 및 바인딩
        if (FAILED(CreateSampler()))
        {
            MessageBox(nullptr, L"샘플러 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }


        // 상수버퍼 생성
        if (FAILED(CreateConstBuffer()))
        {
            MessageBox(nullptr, L"상수버퍼 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }

        // 레스터라이저 스테이트 생성
        if (FAILED(CreateRasterizerState()))
        {
            MessageBox(nullptr, L"레스터라이저 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }

        // DepthStencil 스테이트 생성
        if (FAILED(CreateDepthStencilState()))
        {
            MessageBox(nullptr, L"뎊스 스텐실 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }


        // 블렌드 스테이트 생성
        if (FAILED(CreateBlendState()))
        {
            MessageBox(nullptr, L"블렌드 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
            return E_FAIL;
        }

        return S_OK;
    }


    int Device::CreateSwapchain()
    {
        DXGI_SWAP_CHAIN_DESC desc = {};

        desc.OutputWindow = m_hWnd; // Front Buffer 를 출력시킬 윈도우 핸들
        desc.Windowed = true;       // 윈도우, 전체화면 모드
        desc.BufferCount = 1;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 이전 프레임 장면을 유지하지 않는다.

        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
        desc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.RefreshRate.Numerator = 60;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        ComPtr<IDXGIDevice>  pDXGIDevice = nullptr;
        ComPtr<IDXGIAdapter> pAdapter = nullptr;
        ComPtr<IDXGIFactory> pFactory = nullptr;


        if (FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
        {
            return E_FAIL;
        }

        if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf())))
        {
            return E_FAIL;
        }

        if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
        {
            return E_FAIL;
        }

        if (FAILED(pFactory->CreateSwapChain(m_pDevice.Get(), &desc, m_pSwapChain.GetAddressOf())))
        {
            return E_FAIL;
        }

        return S_OK;
    }

    int Device::CreateTarget()
    {
        HRESULT                 hr;
        ComPtr<ID3D11Texture2D> tex2D;
        hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)tex2D.GetAddressOf());
    
        m_pRenderTargetTex = CreateRef<Texture>();
        m_pRenderTargetTex->Create(tex2D);
        tex2D.Reset();

        m_pDepthStencilTex = CreateRef<Texture>();
        m_pDepthStencilTex->Create((UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
            , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

        return hr;
    }

    int Device::CreateSampler()
    {
        HRESULT hr = S_OK;

        // 샘플러 스테이트 생성
        D3D11_SAMPLER_DESC desc = {};

        desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = D3D11_FILTER_ANISOTROPIC;

        hr = DEVICE->CreateSamplerState(&desc, m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());


        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        hr = DEVICE->CreateSamplerState(&desc, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());


        // 샘플러 바인딩
        CONTEXT->VSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());
        CONTEXT->HSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());
        CONTEXT->DSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());
        CONTEXT->GSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());
        CONTEXT->PSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());
        CONTEXT->CSSetSamplers((UINT)eSamplerType::Anisotropic, 1,
            m_arrSampler[(UINT)eSamplerType::Anisotropic].GetAddressOf());

        CONTEXT->VSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());
        CONTEXT->HSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());
        CONTEXT->DSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());
        CONTEXT->GSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());
        CONTEXT->PSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());
        CONTEXT->CSSetSamplers((UINT)eSamplerType::Point, 1, m_arrSampler[(UINT)eSamplerType::Point].GetAddressOf());

        return hr;
    }

    int Device::CreateConstBuffer()
    {
        m_arrCB[(UINT)eCBType::Transform] = new ConstBuffer(eCBType::Transform);
        m_arrCB[(UINT)eCBType::Transform]->Create(sizeof(tTransform));

        m_arrCB[(UINT)eCBType::Material] = new ConstBuffer(eCBType::Material);
        m_arrCB[(UINT)eCBType::Material]->Create(sizeof(tMtrlConst));

        m_arrCB[(UINT)eCBType::Global] = new ConstBuffer(eCBType::Global);
        m_arrCB[(UINT)eCBType::Global]->Create(sizeof(tGlobalData));

        return S_OK;
    }

    int Device::CreateRasterizerState()
    {
        HRESULT hr = S_OK;

        m_arrRS[(UINT)eRSType::CullBack] = nullptr;


        D3D11_RASTERIZER_DESC desc = {};

        // Cull Front Mode
        desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
        desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

        hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)eRSType::CullFront].GetAddressOf());


        // Cull None Mode
        desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

        hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)eRSType::CullNone].GetAddressOf());


        // WireFrame Mode
        desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

        hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)eRSType::WireFrame].GetAddressOf());


        return hr;
    }

    int Device::CreateDepthStencilState()
    {
        HRESULT hr = S_OK;

        // Less
        m_arrDS[(UINT)eDSType::Less] = nullptr;


        D3D11_DEPTH_STENCIL_DESC desc = {};

        // Greater
        desc.DepthEnable = true;
        desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.StencilEnable = false;

        DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)eDSType::Greater].GetAddressOf());

        // NO_WRITE
        desc.DepthEnable = true;
        desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

        desc.StencilEnable = false;

        DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)eDSType::NoWrite].GetAddressOf());

        // NO_TEST_NO_WRITE
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.StencilEnable = false;

        DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)eDSType::NoTestNoWrite].GetAddressOf());

        return hr;
    }

    int Device::CreateBlendState()
    {
        HRESULT hr = S_OK;

        // Default 블렌딩
        m_arrBS[(UINT)eBSType::Default] = nullptr;


        D3D11_BLEND_DESC desc = {};

        // Alpha Blend
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;


        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


        hr = DEVICE->CreateBlendState(&desc, m_arrBS[(UINT)eBSType::AlphaBlend].GetAddressOf());


        // One One Blend
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;

        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        DEVICE->CreateBlendState(&desc, m_arrBS[(UINT)eBSType::OneOne].GetAddressOf());


        return hr;
    }

    void Device::TargetClear()
    {
        float arrColor[4] = {0.2f, 0.2f, 0.2f, 1.f};
        m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetTex->GetRTV().Get(), arrColor);
        m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.f, 0);
    }

    void Device::TargetSet()
    {
        m_pDeviceContext->OMSetRenderTargets(0, 0, 0);
        m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetTex->GetRTV().GetAddressOf(),
            m_pDepthStencilTex->GetDSV().Get());
    }

    void Device::OnWindowResize(uint32_t width, uint32_t height)
    {
        // TODO 한번에 처리
        if (!m_pDeviceContext)
        {
            return;
        }

        m_vRenderResolution = Vec2{ (float)width, (float)height };

        m_pDeviceContext->OMSetRenderTargets(0, 0, 0);
    
        m_pRenderTargetTex.reset();
        m_pDepthStencilTex.reset();
    
        HRESULT hr;
        hr = m_pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        // Perform error handling here!

        // Get buffer and create a render-target-view.
        ComPtr<ID3D11Texture2D> pBuffer;
        hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBuffer.GetAddressOf());
        // Perform error handling here!

        m_pRenderTargetTex = CreateRef<Texture>();
        m_pRenderTargetTex->Create(pBuffer);

        m_pDepthStencilTex = CreateRef<Texture>();
        m_pDepthStencilTex->Create((UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
            , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);
    
        // Set up the viewport.
        m_tViewPort.Width = (float)width;
        m_tViewPort.Height = (float)height;
        m_tViewPort.MinDepth = 0.0f;
        m_tViewPort.MaxDepth = 1.0f;
        m_tViewPort.TopLeftX = 0;
        m_tViewPort.TopLeftY = 0;
        m_pDeviceContext->RSSetViewports(1, &m_tViewPort);
    }


    void Device::Present()
    {
        m_pSwapChain->Present(m_bVSync, 0);
    }
    
}
