#include "pch.h"
#include "EditorCamera.h"

namespace tomato
{
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_Fov(fov)
        , m_AspectRatio(aspectRatio)
        , m_NearClip(nearClip)
        , m_FarClip(farClip)
        , m_Projection(XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip)) { }

    void EditorCamera::OnUpdate([[maybe_unused]] Timestep timestep)
    {
        float cosYaw = std::cos(m_Yaw);
        float sinYaw = std::sin(m_Yaw);
        float cosPitch = std::cos(m_Pitch);
        float sinPitch = std::sin(m_Pitch);

        m_Forward.x = cosYaw * cosPitch;
        m_Forward.y = sinPitch;
        m_Forward.z = sinYaw * cosPitch;

        m_Forward.Normalize();
        m_Right = Vec3{0, 1, 0}.Cross(m_Forward);
        m_Right.Normalize();

        m_Forward.Cross(m_Right).Normalize(m_Up);

        m_ViewMatrix = XMMatrixLookToLH(m_Position, m_Forward, m_Up);
    }

    void EditorCamera::SetViewportSize(float width, float height)
    {
        m_AspectRatio = width / height;
        m_Projection = XMMatrixPerspectiveFovLH(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
    }
}
