#pragma once

namespace tomato
{
    class StructuredBuffer;
    class MeshRenderComponent;
    struct RenderGraphData;
    struct CameraData;

    class Renderer3D : public Singleton<Renderer3D>
    {
        SINGLETON_CLASS(Renderer3D);

    public:
        void Init();
        void Shutdown();

        void BeginScene(const CameraData& cameraData, vector<tLightInfo>&& lights);
        void EndScene(const Ref<RenderGraphData>& renderGraphData);
        void Flush();

        void Draw(MeshRenderComponent* renderComponent);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
        };

        void                     ResetStats();
        [[nodiscard]] Statistics GetStats();

    private:
        struct Renderer3DConstant
        {
            Matrix WorldMat;
        };

        struct MeshRendererData
        {
            vector<MeshRenderComponent*> RenderComponents;
            vector<Renderer3DConstant>   ConstantData;
            vector<tLightInfo>           Lights;
            Renderer3D::Statistics       Stats;
        };

        MeshRendererData  s_Data;
        StructuredBuffer* s_StructuredBuffer = nullptr;
        StructuredBuffer* s_Light3DBuffer = nullptr;
    };
}
