#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class ParticleUpdateShader;
    class StructuredBuffer;
    class ParticleSystemComponent : public RenderComponent
    {
    private:
        UINT m_iMaxCount;
        UINT m_iAliveCount;

        Vec4 m_vStartScale;
        Vec4 m_vEndScale;

        Vec4 m_vStartColor;
        Vec4 m_vEndColor;

        Vec2 m_vMinMaxSpeed;
        Vec2 m_vMinMaxLifeTime;

        float m_fSpawnRange;

        float m_Frequency; // 초당 파티클 생성 수
        float m_fAccTime;  // 시간 누적

        int m_WorldSpawn;

        StructuredBuffer*         m_ParticleBuffer;
        StructuredBuffer*         m_ParticleShare;
        Ref<ParticleUpdateShader> m_UpdateCS;


    public:
        void SetWorldSpawn(bool _bSet) { m_WorldSpawn = (int)_bSet; }


    public:
        virtual void OnLateUpdate() override;
        virtual void render() override;


    public:
        CLONE(ParticleSystemComponent);
    public:
        ParticleSystemComponent();
        ~ParticleSystemComponent();
    };
}
