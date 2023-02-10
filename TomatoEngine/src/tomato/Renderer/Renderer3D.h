#pragma once

namespace tomato
{
	class Shader;
	class ShaderLibrary;
	class UniformBuffer;
	struct Submesh;
	struct RenderGraphData;
	class Entity;
	struct CameraData;

	class Renderer3D
	{
	public:
		static constexpr uint32_t MAX_NUM_LIGHTS = 200;
		static constexpr uint32_t MAX_NUM_DIR_LIGHTS = 3;

		static void Init();
		static void Shutdown();

		static void BeginScene(const CameraData& cameraData, Entity* cubemap, std::vector<Entity*>&& lights);
		static void EndScene(const Ref<RenderGraphData>& renderTarget);

		static void DrawCube();
		static void DrawQuad();
		static void ReserveMeshes(size_t count);
		// static void SubmitMesh(const Matrix& transform, const Submesh& submesh, MeshComponent::CullModeType cullMode);

		[[nodiscard]] static ShaderLibrary& GetShaderLibrary() { return s_ShaderLibrary; }

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t IndexCount = 0;
		};

		static void ResetStats();
		[[nodiscard]] static Statistics GetStats();

	private:
		static void SetupCameraData(const CameraData& cameraData);
		static void SetupLightsData();
		static void Flush(const Ref<RenderGraphData>& renderGraphData);
		static void FXAAPass(const Ref<RenderGraphData>& renderGraphData);
		static void CompositePass(const Ref<RenderGraphData>& renderGraphData);
		static void BloomPass(const Ref<RenderGraphData>& renderGraphData);
		static void LightingPass(const Ref<RenderGraphData>& renderGraphData);
		static void RenderPass(const Ref<Texture>& renderTarget);
		static void ShadowMapPass();

	private:
		struct MeshData
		{
			/*Matrix Transform;
			Submesh SubmeshGeometry;
			MeshComponent::CullModeType CullMode;

			MeshData(const glm::mat4& transform, const Submesh& submesh, const MeshComponent::CullModeType cullMode)
				: Transform(transform), SubmeshGeometry(submesh), CullMode(cullMode)
			{
			}*/
		};

		static Statistics s_Stats;
		static ShaderLibrary s_ShaderLibrary;
		static std::vector<Renderer3D::MeshData> s_Meshes;
		static Ref<Texture> s_BRDFLutTexture;
		static Ref<Shader> s_Shader;
		static Ref<Shader> s_LightingShader;
		static Ref<Shader> s_ShadowMapShader;
		static Ref<Shader> s_CubemapShader;
		static Ref<Shader> s_GaussianBlurShader;
		static Ref<Shader> s_FxaaShader;
		static Ref<Shader> s_HdrShader;
		static Ref<Shader> s_BloomShader;
		// static Ref<VertexArray> s_QuadVertexArray;
		// static Ref<VertexArray> s_CubeVertexArray;
		static Ref<UniformBuffer> s_UbCamera;
		static Ref<UniformBuffer> s_UbPointLights;
		static Ref<UniformBuffer> s_UbDirectionalLights;

		static Entity s_Skylight;
		static std::vector<Entity> s_SceneLights;

	public:

		enum class TonemappingType { None = 0, ACES, Filmic, Uncharted };

		static TonemappingType Tonemapping;
		static float Exposure;
		static bool UseBloom;
		static float BloomStrength;
		static float BloomThreshold;
		static float BloomKnee;
		static float BloomClamp;
		static bool UseFXAA;
		static Vec2 FXAAThreshold;			// x: current threshold, y: relative threshold
		static Vec4 VignetteColor;			// rgb: color, a: intensity
		static Vec4 VignetteOffset;		// xy: offset, z: useMask, w: enable/disable effect
		static Ref<Texture> VignetteMask;
	};
}