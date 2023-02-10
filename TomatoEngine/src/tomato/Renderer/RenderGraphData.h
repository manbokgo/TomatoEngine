#pragma once

namespace tomato
{
	struct CameraData
	{
		Matrix View;
		Matrix Projection;
		Matrix ViewProjection;
		Vec3 Position;
	};

	struct RenderGraphData
	{
		Ref<Texture> RenderPassTarget;
		Ref<Texture> DepthStecilTarget;
		Ref<Texture> SRView;

		RenderGraphData(uint32_t width, uint32_t height);
        void Resize(uint32_t width, uint32_t height);
    };
}
