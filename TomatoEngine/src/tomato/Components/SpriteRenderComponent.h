#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class SpriteRenderComponent final : public RenderComponent
    {
    public:
        SpriteRenderComponent() = default;
        ~SpriteRenderComponent() override = default;

    private:
        Ref<Texture> m_Texture;

        Vec4 m_Color = Vec4::One;
        int  m_SortingOrder = 0;
    };
}
