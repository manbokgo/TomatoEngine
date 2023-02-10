#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class MeshRenderComponent final : public RenderComponent
    {
    public:
        void OnUpdate() override;
        void OnLateUpdate() override;
        void render() override;

        CLONE(MeshRenderComponent);

    public:
        MeshRenderComponent();
        ~MeshRenderComponent() override;
    };
}
