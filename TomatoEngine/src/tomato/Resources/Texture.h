#pragma once
#include "Res.h"

namespace tomato
{
    enum class eTextureType;
    class Texture final : public Res
    {
        friend class CResMgr;
        friend class SpriteEditorUI;
        friend struct RenderGraphData;
        friend class AssetPanel;
        friend class Device;

    public:
        Texture();
        ~Texture() override;

        void Save(const fs::path& path) override;
        int  Load(const fs::path& path) override;

        void Bind(UINT registerNum, UINT pipelineStage);
        void BindCS(UINT registerNum, bool shaderRes);

        static void Clear(UINT registerNum);
        static void ClearCS(UINT registerNum);

        void AddFrame(SpriteFrm frame, string name);
        void AddFrame(SpriteFrm frame);

        bool FindFrame(string name, SpriteFrm* outFrm);
        bool RemoveFrame(string name);

        [[nodiscard]] const ScratchImage& GetImage() const { return m_Image; }
        [[nodiscard]] const ComPtr<ID3D11Texture2D>& GetTex2D() const { return m_Tex2D; }
        [[nodiscard]] const ComPtr<ID3D11RenderTargetView>& GetRTV() const { return m_RTV; }
        [[nodiscard]] const ComPtr<ID3D11DepthStencilView>& GetDSV() const { return m_DSV; }
        [[nodiscard]] const ComPtr<ID3D11ShaderResourceView>& GetSRV() const { return m_SRV; }
        [[nodiscard]] const ComPtr<ID3D11UnorderedAccessView>& GetUAV() const { return m_UAV; }
        [[nodiscard]] Vec2 GetSize() const { return {(float)m_Desc.Width, (float)m_Desc.Height}; }
        [[nodiscard]] eTextureType GetType() const { return m_Type; }
        [[nodiscard]] const unordered_map<string, SpriteFrm>& GetFrames() const { return m_Frames; }
        [[nodiscard]] const SpriteFrm& GetFrame(string spriteName) const;
        [[nodiscard]] UINT GetPixelsPerUnit() const { return m_PixelsPerUnit; }

        void SetPixelsPerUnit(UINT pixelsPerUnit) { m_PixelsPerUnit = pixelsPerUnit; }
        void SetType(eTextureType type) { m_Type = type; }

    private:
        void Create(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag);
        void Create(const ComPtr<ID3D11Texture2D>& tex2D);

    private:
        ScratchImage            m_Image;
        ComPtr<ID3D11Texture2D> m_Tex2D;
        D3D11_TEXTURE2D_DESC    m_Desc; 

        ComPtr<ID3D11RenderTargetView>    m_RTV;
        ComPtr<ID3D11DepthStencilView>    m_DSV;
        ComPtr<ID3D11ShaderResourceView>  m_SRV;
        ComPtr<ID3D11UnorderedAccessView> m_UAV;

        unordered_map<string, SpriteFrm> m_Frames;

        UINT         m_FrameDefaultIdx;
        UINT         m_PixelsPerUnit;
        eTextureType m_Type;
    };
}
