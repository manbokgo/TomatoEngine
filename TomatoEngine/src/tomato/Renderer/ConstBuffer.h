#pragma once

namespace tomato
{
    class ConstBuffer
    {
        friend class Device;

    private:
        ComPtr<ID3D11Buffer> m_CB;
        D3D11_BUFFER_DESC    m_tDesc;
        const eCBType        m_eCBType;

    private:
        int Create(size_t _iBufferSize);

    public:    
        // SysMem -> GPU
        void SetData(void* _pSrc, size_t _iSize); 

        // SysMem -> GPU
        void SetData(void* _pSrc); 

        // _iPipelineStage : PIPELINE_STAGE, register binding
        void UpdateData(UINT _iPipelineStage); 

        void UpdateData_CS();


        CLONE_ASSERT(ConstBuffer);
    public:
        ConstBuffer(eCBType _CBType);
        ~ConstBuffer();


    };

    
}
