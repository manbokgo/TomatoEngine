#pragma once
#include "Component.h"

namespace tomato
{
    class Collider2DComponent : public Component
    {
    public:
        bool IsSensor = false;
        Vec2 Size = {0.5f, 0.5f};
        Vec2 Offset = {0.0f, 0.0f};

        float Density = 1.0f;

        float Friction = 0.2f;
        float Restitution = 0.0f;

        void* RuntimeFixture = nullptr;


        Collider2DComponent();
        ~Collider2DComponent() override;
        CLONE(Collider2DComponent);

        void OnLateUpdate() override;

        void SetOffsetPos(Vec2 _vOffset) { m_OffsetPos = _vOffset; }
        void SetScale(Vec2 _vScale) { m_Scale = _vScale; }
        void SetRotationX(float _fRadian) { m_Rot.x = _fRadian; }
        void SetRotationY(float _fRadian) { m_Rot.y = _fRadian; }
        void SetRotationZ(float _fRadian) { m_Rot.z = _fRadian; }
        void SetPivot(const Vec2& pivot) { m_Pivot = pivot; }
        void SetCollider2DType(eCollider2DType _type) { m_eType = _type; }
        void SetIgnoreObjectScale(bool _bSet) { m_bIgnoreObjectScale = _bSet; }

        [[nodiscard]] const Vec2&     GetPivot() const { return m_Pivot; }
        [[nodiscard]] eCollider2DType GetCollider2DType() const { return m_eType; }
        [[nodiscard]] float           GetRotationX() const { return m_Rot.x; }
        [[nodiscard]] float           GetRotationY() const { return m_Rot.y; }
        [[nodiscard]] float           GetRotationZ() const { return m_Rot.z; }
        [[nodiscard]] Vec2            GetOffsetPos() const { return m_OffsetPos; }
        [[nodiscard]] Vec2            GetScale() const { return m_Scale; }
        [[nodiscard]] Vec2            GetFinalPos() const { return m_FinalPos; }
        [[nodiscard]] Vec2            GetFinalScale() const { return m_FinalScale; }
        [[nodiscard]] const Matrix&   GetWorldMat() const { return m_matWorld; }


    private:
        Vec2 m_OffsetPos;
        Vec2 m_Scale;
        Vec3 m_Rot;
        Vec2 m_Pivot;

        Vec2   m_FinalPos;
        Vec2   m_FinalScale;
        Matrix m_matWorld;

        eCollider2DType m_eType;
        int             m_OverlapCount;
        bool            m_bIgnoreObjectScale;
    };
}
