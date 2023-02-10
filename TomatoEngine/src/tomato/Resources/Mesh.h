#pragma once
#include "Res.h"

namespace tomato
{
    class Mesh final : public Res
    {
    public:
        Mesh();
        ~Mesh() override;

        void Save(const fs::path& path) override {};
        int  Load(const fs::path& path) override { return S_OK; };

        int  Create(void* vtxSysMem, size_t vtxCount, void* idxSysMem, size_t idxCount);
        void Render();
        void RenderInstanced(UINT count);

    private:
        void Bind();

    private:
        ComPtr<ID3D11Buffer> m_VB;
        D3D11_BUFFER_DESC    m_tVBDesc;

        ComPtr<ID3D11Buffer> m_IB;
        D3D11_BUFFER_DESC    m_tIBDesc;
        UINT                 m_iIdxCount;
    };
}
