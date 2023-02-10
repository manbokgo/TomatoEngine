#pragma once
namespace tomato
{
    class ConstBuffer;

    class Device : public Singleton<Device>
    {
    public:
        Device();
        ~Device();

        int  init(HWND _hWnd, Vec2 _vResolution);
        void TargetClear();
        void TargetSet();
        void OnWindowResize(uint32_t width, uint32_t height);
        void Present();
        
        int CreateSwapchain();
        int CreateTarget();
        int CreateSampler();
        int CreateConstBuffer();
        int CreateRasterizerState();
        int CreateDepthStencilState();
        int CreateBlendState();



        [[nodiscard]] ID3D11Device*        GetDevice() const { return m_pDevice.Get(); }
        [[nodiscard]] ID3D11DeviceContext* GetContext() const { return m_pDeviceContext.Get(); }
        [[nodiscard]] ConstBuffer*         GetConstBuffer(eCBType _eType) const { return m_arrCB[(UINT)_eType]; }

        [[nodiscard]] ComPtr<ID3D11RasterizerState> GetRasterizerState(eRSType _eType) const
        {
            return m_arrRS[(UINT)_eType];
        }

        [[nodiscard]] ComPtr<ID3D11BlendState> GetBlendState(eBSType _eType) const { return m_arrBS[(UINT)_eType]; }

        [[nodiscard]] ComPtr<ID3D11DepthStencilState> GetDepthStencilState(eDSType _eType) const
        {
            return m_arrDS[(UINT)_eType];
        }

        [[nodiscard]] Vec2 GetRenderResolution() const { return m_vRenderResolution; }

        [[nodiscard]] bool IsVSync() const { return m_bVSync; }
        void               SetVSync(bool bVSync) { m_bVSync = bVSync; }

    private:
        HWND m_hWnd;
        bool m_bVSync = true;

        ComPtr<ID3D11Device>        m_pDevice;        // GPU �޸� ����
        ComPtr<ID3D11DeviceContext> m_pDeviceContext; // GPU Rendering

        Ref<Texture> m_pRenderTargetTex;
        Ref<Texture> m_pDepthStencilTex;

        ComPtr<IDXGISwapChain> m_pSwapChain; // RenderTarget(FrontBuffer, BackBuffer) �� ���� �� ���� ��ü ����
        D3D11_VIEWPORT         m_tViewPort;  // ����۸� �����쿡 �׸� ����(��ġ, ũ��) ����

        Vec2 m_vRenderResolution; // ������ ���� �ػ�

        ComPtr<ID3D11SamplerState> m_arrSampler[(UINT)eSamplerType::End];

        ConstBuffer* m_arrCB[(UINT)eCBType::End];

        ComPtr<ID3D11RasterizerState>   m_arrRS[(UINT)eRSType::End];
        ComPtr<ID3D11DepthStencilState> m_arrDS[(UINT)eDSType::End];
        ComPtr<ID3D11BlendState>        m_arrBS[(UINT)eBSType::End];
    };
}
