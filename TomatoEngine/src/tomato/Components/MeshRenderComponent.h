#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class Mesh;
    class MeshRenderComponent final : public RenderComponent
    {
    public:
        MeshRenderComponent() = default;
        ~MeshRenderComponent() override = default;

        void SetMesh(const Ref<Mesh>& mesh) { m_Mesh = mesh; }

        [[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }

    private:
        Ref<Mesh> m_Mesh;
    };
}
