#pragma once

namespace tomato
{
    class RenderComponent;
    struct RenderGraphData;
    struct CameraData;
    class MyRenderer
    {
    public:
        MyRenderer() = default;
        ~MyRenderer() = default;

        static void Init();
        static void Shutdown();

        static void BeginScene(const CameraData& cameraData);
        static void EndScene(const Ref<RenderGraphData>& renderGraphData);
        static void Flush();

        static void Draw(RenderComponent* renderComponent);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
        };

        static void ResetStats();
        [[nodiscard]] static Statistics GetStats();

    private:
        
    };
}
