#pragma once
#include "Component.h"

namespace tomato
{
    class TransformComponent final : public Component
    {
        friend class Scene;
        friend class SceneViewport;
        friend class PropertiesPanel;

    public:
        TransformComponent();
        ~TransformComponent() override;

        void OnUpdate() override;
        void OnLateUpdate() override;

        void UpdateData();

    public:
        void MoveRelativePos(Vec3 pos) { m_Position += pos; }
        void MoveRelativePosX(float x) { m_Position.x += x; }
        void MoveRelativePosY(float y) { m_Position.y += y; }
        void MoveRelativePosZ(float z) { m_Position.z += z; }

        void SetRelativePos(Vec3 pos) { m_Position = pos; }
        void SetRelativeScale(Vec3 scale) { m_Scale = scale; }
        void SetRelativeRotation(Vec3 rotation) { m_Rotation = rotation; }

        void SetScaleModifier(Vec3 scaleModifier) { m_ScaleModifier = scaleModifier; }

        void SetIgnoreParentScale(bool ignore) { m_bIgnoreParentScale = ignore; }


        [[nodiscard]] const Matrix& GetWorldMat() const { return m_WorldMat; }

        [[nodiscard]] Vec3 GetRelativePos() const { return m_Position; }
        [[nodiscard]] Vec3 GetWorldPos() const { return m_WorldMat.Translation(); }

        [[nodiscard]] Vec3 GetRelativeScale() const { return m_Scale; }
        [[nodiscard]] Vec3 GetWorldScale() const;

        [[nodiscard]] Vec3 GetRelativeRotation() const { return m_Rotation; }

        [[nodiscard]] Vec3 GetRelativeDir(eDir type) const { return m_RelativeDir[(UINT)type]; }
        [[nodiscard]] Vec3 GetWorldDir(eDir type) const { return m_WorldDir[(UINT)type]; }

        [[nodiscard]] Vec3 GetScaleModifier() const { return m_ScaleModifier; }

        [[nodiscard]] bool IsIgnoreParentScale() const { return m_bIgnoreParentScale; }

    private:
        Vec3 m_Position = Vec3::Zero;
        Vec3 m_Scale = Vec3::One;
        Vec3 m_Rotation = Vec3::Zero;

        Vec3 m_ScaleModifier = Vec3::One; // PixelsPerUnit for 2D

        array<Vec3, (UINT)eDir::End> m_RelativeDir;
        array<Vec3, (UINT)eDir::End> m_WorldDir;

        Matrix m_WorldMat = Matrix::Identity;

        bool m_bIgnoreParentScale = false;
    };
}
