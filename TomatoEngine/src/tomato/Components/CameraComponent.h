#pragma once
#include "tomato/Components/Component.h"

namespace tomato
{
    // TODO 레이어 마스크
    class CameraComponent : public Component
    {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };

    public:
        CameraComponent();
        ~CameraComponent() override = default;

        void SetPerspective(float verticalFov, float nearClip, float farClip);
        void SetOrthographic(float size, float nearClip, float farClip);
        void SetViewportSize(uint32_t width, uint32_t height);

        [[nodiscard]] float          GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        [[nodiscard]] float          GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        [[nodiscard]] float          GetPerspectiveFarClip() const { return m_PerspectiveFar; }
        [[nodiscard]] float          GetOrthographicSize() const { return m_OrthographicSize; }
        [[nodiscard]] float          GetOrthographicNearClip() const { return m_OrthographicNear; }
        [[nodiscard]] float          GetOrthographicFarClip() const { return m_OrthographicFar; }
        [[nodiscard]] ProjectionType GetProjectionType() const { return m_ProjectionType; }
        [[nodiscard]] const Matrix& GetProjection() const { return m_Projection; }

        void SetPerspectiveVerticalFOV(float verticalFov)
        {
            m_PerspectiveFOV = verticalFov;
            RecalculateProjection();
        }

        void SetPerspectiveNearClip(float nearClip)
        {
            m_PerspectiveNear = nearClip;
            RecalculateProjection();
        }

        void SetPerspectiveFarClip(float farClip)
        {
            m_PerspectiveFar = farClip;
            RecalculateProjection();
        }

        void SetOrthographicSize(float size)
        {
            m_OrthographicSize = size;
            RecalculateProjection();
        }

        void SetOrthographicNearClip(float nearClip)
        {
            m_OrthographicNear = nearClip;
            RecalculateProjection();
        }

        void SetOrthographicFarClip(float farClip)
        {
            m_OrthographicFar = farClip;
            RecalculateProjection();
        }

        void SetProjectionType(ProjectionType type)
        {
            m_ProjectionType = type;
            RecalculateProjection();
        }

    private:
        void RecalculateProjection();

    public:
        bool m_bPrimary = true;
        bool m_bFixedAspectRatio = false;

    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        float m_PerspectiveFOV = XMConvertToRadians(45.0f);
        float m_PerspectiveNear = 0.01f;
        float m_PerspectiveFar = 1000.0f;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f;
        float m_OrthographicFar = 1.0f;

        float m_AspectRatio = 0.0f;

        Matrix m_Projection = Matrix::Identity;
    };
}
