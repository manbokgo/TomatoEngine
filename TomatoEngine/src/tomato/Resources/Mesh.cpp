#include "pch.h"
#include "Mesh.h"

#include "tomato/Renderer/Device.h"

namespace tomato
{
    Mesh::Mesh()
        : Res(eResType::Mesh)
        , m_tVBDesc{}
        , m_tIBDesc{}
        , m_iIdxCount(0) { }

    Mesh::~Mesh() { }

    void Mesh::Render()
    { 
        Bind();
        CONTEXT->DrawIndexed(m_iIdxCount, 0, 0);
    }

    void Mesh::RenderInstanced(UINT count)
    {
        Bind();
        CONTEXT->DrawIndexedInstanced(m_iIdxCount, count, 0, 0, 0);
    }

    int Mesh::Create(void* vtxSysMem, size_t vtxCount, void* idxSysMem, size_t idxCount)
    {
        HRESULT hr = S_OK;

        // System -> GPU	
        m_tVBDesc.ByteWidth = UINT(sizeof(Vtx) * vtxCount);
        m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
        m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;
        m_tVBDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA tSubData = {};
        tSubData.pSysMem = vtxSysMem;

        hr = DEVICE->CreateBuffer(&m_tVBDesc, &tSubData, &m_VB);

        // IndexBuffer ¸¸µé±â	
        m_iIdxCount = (UINT)idxCount;
        m_tIBDesc.ByteWidth = sizeof(UINT) * m_iIdxCount;
        m_tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
        m_tIBDesc.Usage = D3D11_USAGE_DEFAULT;
        m_tIBDesc.CPUAccessFlags = 0;

        tSubData = {};
        tSubData.pSysMem = idxSysMem;

        hr = DEVICE->CreateBuffer(&m_tIBDesc, &tSubData, &m_IB);

        return hr;
    }

    void Mesh::Bind()
    {
        constexpr UINT stride = sizeof(Vtx);
        constexpr UINT offset = 0;
        CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);
        CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
    }
}
