#include "pch.h"
#include "ConstBuffer.h"

#include "Device.h"

namespace tomato
{
    ConstBuffer::ConstBuffer(eCBType _CBType)
        : m_tDesc{}
        , m_eCBType(_CBType)
    {
    }

    ConstBuffer::~ConstBuffer()
    {

    }


    int ConstBuffer::Create(size_t _iBufferSize)
    {
        // 상수버퍼 생성
        m_tDesc.ByteWidth = (UINT)_iBufferSize; // 16 의 배수로
        m_tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
        m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = DEVICE->CreateBuffer(&m_tDesc, nullptr, m_CB.GetAddressOf());

        return hr;
    }

    void ConstBuffer::SetData(void* _pSrc, size_t _iSize)
    {
        D3D11_MAPPED_SUBRESOURCE tSub = {};
        CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);

        memcpy(tSub.pData, _pSrc, _iSize);

        CONTEXT->Unmap(m_CB.Get(), 0);
    }

    void ConstBuffer::SetData(void* _pSrc)
    {
        D3D11_MAPPED_SUBRESOURCE tSub = {};
        CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);

        memcpy(tSub.pData, _pSrc, m_tDesc.ByteWidth);

        CONTEXT->Unmap(m_CB.Get(), 0);
    }

    void ConstBuffer::UpdateData(UINT _iPipelineStage)
    {
        if ((UINT)ePipelineStage::VS & _iPipelineStage)
            CONTEXT->VSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

        if ((UINT)ePipelineStage::HS & _iPipelineStage)
            CONTEXT->HSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

        if ((UINT)ePipelineStage::DS & _iPipelineStage)
            CONTEXT->DSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

        if ((UINT)ePipelineStage::GS & _iPipelineStage)
            CONTEXT->GSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());

        if ((UINT)ePipelineStage::PS & _iPipelineStage)
            CONTEXT->PSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());	
    }

    void ConstBuffer::UpdateData_CS()
    {
        CONTEXT->CSSetConstantBuffers((UINT)m_eCBType, 1, m_CB.GetAddressOf());
    }

    
}
