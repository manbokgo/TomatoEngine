#include "pch.h"
#include "Renderer2D.h"

#include "ConstBuffer.h"
#include "Device.h"
#include "RenderGraphData.h"
#include "StructuredBuffer.h"

#include "tomato/Components/SpriteRenderComponent.h"
#include "tomato/Components/TransformComponent.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    Renderer2D::Renderer2DData Renderer2D::s_Data;
    StructuredBuffer*          Renderer2D::s_StructuredBuffer;

    void Renderer2D::Init()
    {
        s_Data.RenderComponents.reserve(16);
        s_Data.ConstantData.reserve(16);

        s_StructuredBuffer = new StructuredBuffer();
        s_StructuredBuffer->Create(sizeof(Renderer2DConstant), 16, eSBType::SRVOnly, nullptr, true);
    }

    void Renderer2D::Shutdown()
    {
        delete s_StructuredBuffer;
    }

    void Renderer2D::BeginScene(const CameraData& cameraData)
    {
        s_Data.RenderComponents.clear();
        s_Data.ConstantData.clear();

        g_transform.matView = cameraData.View.Transpose();
        g_transform.matProj = cameraData.Projection.Transpose();
        g_transform.matVP = cameraData.ViewProjection.Transpose();

        ConstBuffer* pCB = Device::GetInst()->GetConstBuffer(eCBType::Transform);

        pCB->SetData(&g_transform);
        pCB->UpdateData(ePipelineStage::AllStage);
    }

    void Renderer2D::EndScene(const Ref<RenderGraphData>& renderGraphData)
    {
        CONTEXT->OMSetRenderTargets(1, renderGraphData->RenderTarget->GetRTV().GetAddressOf(),
            renderGraphData->DepthStencil->GetDSV().Get());

        // TODO viewport 리사이즈
        D3D11_VIEWPORT m_tViewPort;
        m_tViewPort.TopLeftX = 0;
        m_tViewPort.TopLeftY = 0;
        m_tViewPort.Width = 1280;
        m_tViewPort.Height = 720;
        m_tViewPort.MinDepth = 0.f;
        m_tViewPort.MaxDepth = 1.f;

        CONTEXT->RSSetViewports(1, &m_tViewPort);

        // constexpr float arrColor[4] = {0.2f, 0.2f, 0.2f, 1.f};
        // CONTEXT->ClearRenderTargetView(renderGraphData->RenderTarget->GetRTV().Get(), arrColor);
        // CONTEXT->ClearDepthStencilView(renderGraphData->DepthStencil->GetDSV().Get(),
        //     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

        Flush();

        CONTEXT->OMSetRenderTargets(0, nullptr, nullptr);
    }


    // https://stackoverflow.com/a/17074810/12340441
    template <typename T, typename Compare>
    std::vector<std::size_t> SortPermutation(const std::vector<T>& vec, Compare compare)
    {
        std::vector<std::size_t> p(vec.size());
        std::iota(p.begin(), p.end(), 0);
        std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
        return p;
    }

    template <typename T>
    std::vector<T> ApplyPermutation(const std::vector<T>& vec, const std::vector<std::size_t>& p)
    {
        std::vector<T> sortedVec(vec.size());

        std::transform(p.begin(), p.end(), sortedVec.begin(), [&](std::size_t i) { return vec[i]; });
        return sortedVec;
    }


    void Renderer2D::Flush()
    {
        // 도메인 별로
        // 머티리얼 별로
        // 메쉬 별로
        // 상수 버퍼 바인딩
        // 드로우콜

        // Sort
        std::vector<std::size_t>      p = SortPermutation(s_Data.RenderComponents,
            [](SpriteRenderComponent* a, SpriteRenderComponent* b) noexcept -> bool
            {
                const auto& aMaterial = a->GetMaterial();
                const auto& bMaterial = b->GetMaterial();

                if (aMaterial->GetShader()->GetDomain() < bMaterial->GetShader()->GetDomain()) return true;
                if (aMaterial->GetShader()->GetDomain() > bMaterial->GetShader()->GetDomain()) return false;

                return aMaterial < bMaterial;
            });

        s_Data.RenderComponents = ApplyPermutation(s_Data.RenderComponents, p);
        s_Data.ConstantData = ApplyPermutation(s_Data.ConstantData, p);


        /*
        std::sort(s_Data.RenderComponents.begin(), s_Data.RenderComponents.end(),
            [](SpriteMaterial a, SpriteMaterial b) noexcept -> bool
            {
                const auto& aMaterial = a.RenderComponent->GetCurMaterial();
                const auto& bMaterial = b.RenderComponent->GetCurMaterial();

                if (aMaterial->GetShader()->GetDomain() < bMaterial->GetShader()->GetDomain()) return true;
                if (aMaterial->GetShader()->GetDomain() > bMaterial->GetShader()->GetDomain()) return false;

                if (aMaterial < bMaterial) return true;
                if (aMaterial > bMaterial) return false;

                return a.RenderComponent->GetMesh() < b.RenderComponent->GetMesh();
            });*/


        eShaderDomain batchDomain = eShaderDomain::Opaque;
        Material*     batchMaterial = nullptr;
        UINT          batchStart = 0;

        const UINT renderCount = (UINT)s_Data.RenderComponents.size();
        for (UINT pos = 0; pos < renderCount; ++pos)
        {
            const SpriteRenderComponent* renderComponent = s_Data.RenderComponents[pos];

            Material*     material = renderComponent->GetMaterial().get();
            eShaderDomain domain = material->GetShader()->GetDomain();
            if (domain != batchDomain)
            {
                // 불필요함. 디버그용?
                batchDomain = domain;
            }

            if (material != batchMaterial)
            {
                // 0번 인덱스는 그냥 넘어간다
                if (pos > batchStart)
                {
                    s_StructuredBuffer->SetData(&s_Data.ConstantData[batchStart], pos - batchStart);
                    // TODO 한번만 바인딩하고 싶은데 크기 넘어서면 기존 버퍼가 날아감
                    s_StructuredBuffer->UpdateData(44, VS | PS);
                    // mesh->RenderInstanced(pos - batchStart);
                    ++s_Data.Stats.DrawCalls;
                }

                material->Bind();
                batchMaterial = material;
                batchStart = pos;
            }
        }

        // Flush the final batch.
        if (batchMaterial)
        {
            s_StructuredBuffer->SetData(&s_Data.ConstantData[batchStart], renderCount - batchStart);
            s_StructuredBuffer->UpdateData(44, VS | PS);
            // batchMesh->RenderInstanced(renderCount - batchStart);
            ++s_Data.Stats.DrawCalls;
        }
    }

    void Renderer2D::Draw(SpriteRenderComponent* renderComponent)
    {
        Renderer2DConstant sprite;
        sprite.WorldMat = renderComponent->Transform()->GetWorldMat().Transpose();
        sprite.SpriteInfo = renderComponent->GetMaterial()->GetCB().SpriteInfo;

        s_Data.RenderComponents.push_back(renderComponent);
        s_Data.ConstantData.push_back(sprite);
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }
}
