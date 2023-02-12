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
		RenderGraphData(uint32_t width, uint32_t height);
		void Resize(uint32_t width, uint32_t height);

		Ref<Texture> RenderTarget;
		Ref<Texture> DepthStencil;
    };
}
