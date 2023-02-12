#pragma once

namespace tomato
{
    class StructuredBuffer;
    class MeshRenderComponent;
    struct RenderGraphData;
    struct CameraData;

    class Renderer3D
    {
    public:
        Renderer3D() = default;
        ~Renderer3D() = default;

        static void Init();
        static void Shutdown();

        static void BeginScene(const CameraData& cameraData);
        static void EndScene(const Ref<RenderGraphData>& renderGraphData);
        static void Flush();

        static void Draw(MeshRenderComponent* renderComponent);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
        };

        static void ResetStats();
        [[nodiscard]] static Statistics GetStats();

    private:
        struct Renderer3DConstant
        {
            Matrix WorldMat;
        };

        struct MeshRendererData
        {
            vector<MeshRenderComponent*> RenderComponents;
            vector<Renderer3DConstant>   ConstantData;
            Renderer3D::Statistics       Stats;
        };

        static MeshRendererData  s_Data;
        static StructuredBuffer* s_StructuredBuffer;

    };
}
