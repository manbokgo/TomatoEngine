#include "pch.h"
#include "RenderGraphData.h"

#include "tomato/Resources/Texture.h"

namespace tomato
{
    RenderGraphData::RenderGraphData(uint32_t width, uint32_t height) {
        if (width <= 0 || height <= 0)
        {
            TOMATO_CORE_ERROR("Cannot create a RenderGraph with {}x{} dimensions, making it 1x1", width, height);
            width = 1;
            height = 1;
        }
			
        {
            RenderPassTarget = CreateRef<Texture>();
            RenderPassTarget->Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET);
        }

        {
            DepthStecilTarget = CreateRef<Texture>();
            DepthStecilTarget->Create(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);
        }

        {
            SRView = CreateRef<Texture>();
            SRView->Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
        }
    }

    void RenderGraphData::Resize(uint32_t width, uint32_t height) {
    }
}
