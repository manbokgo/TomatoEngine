#pragma once
#include "Component.h"

namespace tomato
{
    class TransformComponent final : public Component
    {
        friend class SceneViewport;
        friend class Scene;

    public:
        const Matrix& GetWorldMat() const { return m_matWorld; }

    public:
        Vec3 m_Translation;
        Vec3 m_Scale;
        Vec3 m_Rotation;

        Vec3 m_ScaleModifier; // PixelsPerUnit for 2D


        Vec3   m_vRelativeDir[(UINT)eDir::End];
        Vec3   m_vWorldDir[(UINT)eDir::End];
        Matrix m_matWorld;

        bool m_bIgnParentScale;

    public:
        void MoveRelativePos(Vec3 _vPos) { m_Translation += _vPos; }
        void MoveRelativePosX(float x) { m_Translation.x += x; }
        void MoveRelativePosY(float y) { m_Translation.y += y; }
        void MoveRelativePosZ(float z) { m_Translation.z += z; }

        void SetRelativePos(Vec3 _vPos) { m_Translation = _vPos; }
        void SetRelativeScale(Vec3 _vScale) { m_Scale = _vScale; }
        void SetRelativeRotation(Vec3 _vRotation) { m_Rotation = _vRotation; }

        void SetRelativePos(float _x, float _y, float _z) { m_Translation = Vec3(_x, _y, _z); }
        void SetRelativeScale(float _x, float _y, float _z) { m_Scale = Vec3(_x, _y, _z); }
        void SetRelativeRotation(float _x, float _y, float _z) { m_Rotation = Vec3(_x, _y, _z); }

        [[nodiscard]] Vec3 GetScaleModifier() const { return m_ScaleModifier; }
        void               SetScaleModifier(Vec3 scaleModifier) { m_ScaleModifier = scaleModifier; }


        Vec3 GetRelativePos() { return m_Translation; }
        Vec3 GetWorldPos() { return m_matWorld.Translation(); }

        Vec3 GetRelativeScale() { return m_Scale; }
        Vec3 GetWorldScale();

        Vec3 GetRelativeRotation() { return m_Rotation; }
        Vec3 GetRelativeDir(eDir _eType) { return m_vRelativeDir[(UINT)_eType]; }
        Vec3 GetWorldDir(eDir _eType) { return m_vWorldDir[(UINT)_eType]; }

        void SetIgnoreParentScale(bool _bIgn) { m_bIgnParentScale = _bIgn; }
        bool IsIgnoreParentScale() { return m_bIgnParentScale; }


    public:
        virtual void OnUpdate() override;
        virtual void OnLateUpdate() override;

        void UpdateData();

    public:
        virtual void SaveToFile(FILE* _File);
        virtual void LoadFromFile(FILE* _File);

        CLONE(TransformComponent);
    public:
        TransformComponent();
        virtual ~TransformComponent();
    };
}
