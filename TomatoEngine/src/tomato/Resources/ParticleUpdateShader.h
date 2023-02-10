#pragma once
#include "ComputeShader.h"

namespace tomato
{
    class StructuredBuffer;

    class ParticleUpdateShader :    public ComputeShader
    {
    public:
        StructuredBuffer* m_ParticleBuffer;
        StructuredBuffer* m_ParticleShare;


    public:
        void SetParticleBuffer(StructuredBuffer* _Buffer);
        void SetParticleShareData(StructuredBuffer* _Buffer){ ASSERT(_Buffer); m_ParticleShare = _Buffer; }
        void SetWorldSpawn(bool _bWorldSpawn) {m_Param.iArr[1] = (int)_bWorldSpawn; }
        void SetSpawnRange(float _Range) { m_Param.fArr[0] = _Range; }
        void SetObjectWorldPos(Vec3 _vWorldPos) { m_Param.v4Arr[0] = { _vWorldPos, 0.f }; }
        void SetMinMaxSpeed(Vec2 _vSpeed) { m_Param.v2Arr[0] = _vSpeed; }
        void SetMinMaxLifeTime(Vec2 _vLifeTime) { m_Param.v2Arr[1] = _vLifeTime; }

    public:
        virtual void UpdateData() override;
        virtual void Clear() override;

    public:
        ParticleUpdateShader();
        ~ParticleUpdateShader();
    };

    
}
