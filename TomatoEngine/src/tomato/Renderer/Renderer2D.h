#pragma once

namespace tomato
{
	class Texture2D;
	struct RenderGraphData;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Matrix& viewProjection);
		static void EndScene(const Ref<RenderGraphData>& renderGraphData);
		static void Flush();

		//Primitives
		static void DrawQuad(const Vec2& position, float rotation, const Vec2& size, const Ref<Texture2D>& texture = nullptr, const Vec4& tintColor = Vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawQuad(const Vec3& position, float rotation, const Vec2& size, const Ref<Texture2D>& texture = nullptr, const Vec4& tintColor = Vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawQuad(const Matrix& transform, const Vec4& color);
		static void DrawQuad(const Matrix& transform, const Ref<Texture2D>& texture = nullptr, const Vec4& tintColor = Vec4(1.0f), float tilingFactor = 1.0f);

		static void DrawLine(const Vec3& p0, const Vec3& p1, const Vec4& color);

		static void DrawRect(const Vec3& position, const Vec2& size, const Vec4& color);
		static void DrawRect(const Matrix& transform, const Vec4& color);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			[[nodiscard]] uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			[[nodiscard]] uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
			[[nodiscard]] uint32_t GetTotalTriangleCount() const { return QuadCount * 2; }
		};

		static void ResetStats();
		[[nodiscard]] static Statistics GetStats();
	private:
		static void StartBatch();
		static void NextBatch();
	};
}

