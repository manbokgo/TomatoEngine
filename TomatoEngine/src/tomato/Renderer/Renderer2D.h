#pragma once

namespace tomato
{
    class StructuredBuffer;
    class SpriteRenderComponent;
    struct RenderGraphData;
    struct CameraData;

    class Renderer2D
    {
    public:
        Renderer2D() = default;
        ~Renderer2D() = default;

        static void Init();
        static void Shutdown();

        static void BeginScene(const CameraData& cameraData);
        static void EndScene(const Ref<RenderGraphData>& renderGraphData);
        static void Flush();

        static void Draw(SpriteRenderComponent* renderComponent);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
        };

        static void ResetStats();
        [[nodiscard]] static Statistics GetStats();

    private:
        struct Renderer2DConstant
        {
            Matrix                                 WorldMat;
            array<SpriteFrm, (UINT)eTexParam::End> SpriteInfo;
        };

        struct Renderer2DData
        {
            vector<SpriteRenderComponent*> RenderComponents;
            vector<Renderer2DConstant>     ConstantData;
            Renderer2D::Statistics         Stats;
        };

        static Renderer2DData    s_Data;
        static StructuredBuffer* s_StructuredBuffer;
    };
}
