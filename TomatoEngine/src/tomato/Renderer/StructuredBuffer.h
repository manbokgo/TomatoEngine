#pragma once

namespace tomato
{
    class StructuredBuffer
    {
    private:
        ComPtr<ID3D11Buffer>              m_SB;
        ComPtr<ID3D11ShaderResourceView>  m_SRV;
        ComPtr<ID3D11UnorderedAccessView> m_UAV;

        ComPtr<ID3D11Buffer> m_WriteBuffer;
        ComPtr<ID3D11Buffer> m_ReadBuffer;

        D3D11_BUFFER_DESC m_tDesc;
        eSBType           m_eType;

        UINT m_iElementSize;
        UINT m_iElementCount;

        UINT m_iRecentRegisterNum;   // �ֱ� ����� t �������� ��ȣ
        UINT m_iRecentRegisterNumRW; // �ֱ� ����� u �������� ��ȣ

    public:
        int Create(UINT _iElementSize, UINT _iElementCount, eSBType _eType, void* _pInitial, bool _CPUAccess = false);    
        UINT GetElementSize() { return m_iElementSize; }
        UINT GetElementCount() { return m_iElementCount; }

        void SetData(void* _pSrc, UINT _iElementCount);
        void GetData(void* _pDst, UINT _iSizeByte = 0);

        // PIPELINE_STAGE
        void UpdateData(UINT _iRegisterNum, UINT _PipelineStage);
        void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes);

        void Clear();

    public:
        CLONE(StructuredBuffer);

    public:
        StructuredBuffer();
        StructuredBuffer(const StructuredBuffer& _origin);
        ~StructuredBuffer();
    };

    
}