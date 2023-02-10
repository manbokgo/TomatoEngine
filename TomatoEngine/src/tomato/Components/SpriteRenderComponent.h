#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class SpriteRenderComponent : public RenderComponent
    {
    public:
        SpriteRenderComponent() = default;
        ~SpriteRenderComponent() override = default;

        void render() override;

        CLONE(SpriteRenderComponent);

    private:
        Vec4 m_Color;
        Ref<Texture> m_Texture;
    };
}
