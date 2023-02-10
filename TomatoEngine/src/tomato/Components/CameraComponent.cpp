#include "pch.h"
#include "CameraComponent.h"

namespace tomato
{
    CameraComponent::CameraComponent()
    {
        RecalculateProjection();
    }

    void CameraComponent::SetPerspective(float verticalFov, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;

        m_PerspectiveFOV = verticalFov;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;

        RecalculateProjection();
    }

    void CameraComponent::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;

        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;

        RecalculateProjection();
    }

    void CameraComponent::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjection();
    }

    void CameraComponent::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective)
        {
            m_Projection = XMMatrixPerspectiveFovLH(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear,
                m_PerspectiveFar);
        }
        else
        {
            const float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            const float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            const float orthoBottom = -m_OrthographicSize * 0.5f;
            const float orthoTop = m_OrthographicSize * 0.5f;

            m_Projection = XMMatrixOrthographicOffCenterLH(orthoLeft, orthoRight, orthoBottom, orthoTop,
                m_OrthographicNear, m_OrthographicFar);
        }
    }
}
