#pragma once

#include "tomato/Core/Timestep.h"

namespace tomato
{
    class EditorCamera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate([[maybe_unused]] Timestep timestep);

        void SetViewportSize(float width, float height);
        void SetPosition(const Vec3& position) { m_Position = position; }
        void SetYaw(float yaw) { m_Yaw = yaw; }         // In radians
        void SetPitch(float pitch) { m_Pitch = pitch; } // In radians

        [[nodiscard]] const Matrix& GetView() const { return m_ViewMatrix; }
        [[nodiscard]] const Matrix& GetProjection() const { return m_Projection; }
        [[nodiscard]] Matrix        GetViewProjection() const { return m_ViewMatrix * m_Projection; }

        [[nodiscard]] const Vec3& GetPosition() const { return m_Position; }
        [[nodiscard]] const Vec3& GetForward() const { return m_Forward; }
        [[nodiscard]] const Vec3& GetRight() const { return m_Right; }
        [[nodiscard]] const Vec3& GetUp() const { return m_Up; }
        [[nodiscard]] float       GetYaw() const { return m_Yaw; }     // In radians
        [[nodiscard]] float       GetPitch() const { return m_Pitch; } // In radians

    private:
        float m_Fov = XMConvertToRadians(45.0f); // In radians
        float m_AspectRatio = 1.777f;
        float m_NearClip = 0.03f;
        float m_FarClip = 2000.0f;

        Vec3 m_Position = Vec3(0, 0, -10);
        Vec3 m_Forward = Vec3(0, 0, 1);
        Vec3 m_Right = Vec3(1, 0, 0);
        Vec3 m_Up = Vec3(0, 1, 0);

        float m_Yaw = XMConvertToRadians(90.0f);  // In radians
        float m_Pitch = XMConvertToRadians(0.0f); // In radians

        Matrix m_ViewMatrix = Matrix::Identity;
        Matrix m_Projection = Matrix::Identity;
    };
}
