#pragma once
#include "Component.h"

namespace tomato
{
    class Material;
    class RenderComponent : public Component
    {
    public:
        RenderComponent() = default;
        ~RenderComponent() override = default;

        void SetMaterial(const Ref<Material>& material) { m_Material = material; }

        [[nodiscard]] const Ref<Material>& GetMaterial() const { return m_Material; }

    protected:
        Ref<Material> m_Material;
    };
}
