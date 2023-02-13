#include "pch.h"
#include "Renderer3D.h"

#include "ConstBuffer.h"
#include "Device.h"
#include "RenderGraphData.h"
#include "StructuredBuffer.h"

#include "tomato/Components/MeshRenderComponent.h"
#include "tomato/Components/TransformComponent.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Mesh.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    Renderer3D::Renderer3D() {}

    Renderer3D::~Renderer3D() {}
    
    void Renderer3D::Init()
    {
        s_Data.RenderComponents.reserve(16);
        s_Data.ConstantData.reserve(16);

        s_StructuredBuffer = new StructuredBuffer();
        s_StructuredBuffer->Create(sizeof(Renderer3DConstant), 16, eSBType::SRVOnly, nullptr, true);

        s_Light3DBuffer = new StructuredBuffer();
        s_Light3DBuffer->Create(sizeof(tLightInfo), 2, eSBType::SRVOnly, nullptr, true);
    }

    void Renderer3D::Shutdown()
    {
        // TODO Unique
        delete s_StructuredBuffer;
        delete s_Light3DBuffer;
    }

    void Renderer3D::BeginScene(const CameraData& cameraData, vector<tLightInfo>&& lights)
    {
        s_Data.RenderComponents.clear();
        s_Data.ConstantData.clear();
        s_Data.Lights = std::move(lights);

        g_transform.matView = cameraData.View.Transpose();
        g_transform.matProj = cameraData.Projection.Transpose();
        g_transform.matVP = cameraData.ViewProjection.Transpose();

        static ConstBuffer* pCB = Device::GetInst()->GetConstBuffer(eCBType::Transform);

        pCB->SetData(&g_transform);
        pCB->UpdateData(ePipelineStage::AllStage);
        
        s_Light3DBuffer->SetData(s_Data.Lights.data(), (UINT)s_Data.Lights.size());
        s_Light3DBuffer->UpdateData(14, ePipelineStage::AllStage);
        g_global.iLight3DCount = (UINT)s_Data.Lights.size();

        static ConstBuffer* pGlobalCB = Device::GetInst()->GetConstBuffer(eCBType::Global);
        pGlobalCB->SetData(&g_global);
        pGlobalCB->UpdateData(ePipelineStage::AllStage);
    }

    void Renderer3D::EndScene(const Ref<RenderGraphData>& renderGraphData)
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

        constexpr float arrColor[4] = {0.2f, 0.2f, 0.2f, 1.f};
        CONTEXT->ClearRenderTargetView(renderGraphData->RenderTarget->GetRTV().Get(), arrColor);
        CONTEXT->ClearDepthStencilView(renderGraphData->DepthStencil->GetDSV().Get(),
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

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


    void Renderer3D::Flush()
    {
        // 도메인 별로
        // 머티리얼 별로
        // 메쉬 별로
        // 상수 버퍼 바인딩
        // 드로우콜

        // Sort
        std::vector<std::size_t>    p = SortPermutation(s_Data.RenderComponents,
            [](MeshRenderComponent* a, MeshRenderComponent* b) noexcept -> bool
            {
                const auto& aMaterial = a->GetMaterial();
                const auto& bMaterial = b->GetMaterial();

                if (aMaterial->GetShader()->GetDomain() < bMaterial->GetShader()->GetDomain()) return true;
                if (aMaterial->GetShader()->GetDomain() > bMaterial->GetShader()->GetDomain()) return false;

                if (aMaterial < bMaterial) return true;
                if (aMaterial > bMaterial) return false;

                return a->GetMesh() < b->GetMesh();
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
        Mesh*         batchMesh = nullptr;
        UINT          batchStart = 0;

        const UINT renderCount = (UINT)s_Data.RenderComponents.size();
        for (UINT pos = 0; pos < renderCount; ++pos)
        {
            MeshRenderComponent* renderComponent = s_Data.RenderComponents[pos];

            Material*     material = renderComponent->GetMaterial().get();
            eShaderDomain domain = material->GetShader()->GetDomain();
            if (domain != batchDomain)
            {
                // 사실 불필요함. 디버그용?
                batchDomain = domain;
            }

            if (material != batchMaterial)
            {
                material->Bind();
                batchMaterial = material;
            }

            Mesh* mesh = renderComponent->GetMesh().get();
            if (mesh != batchMesh)
            {
                if (pos > batchStart)
                {
                    s_StructuredBuffer->SetData(&s_Data.ConstantData[batchStart], pos - batchStart);
                    // TODO 한번만 바인딩하고 싶은데 크기 넘어서면 기존 버퍼가 날아감
                    s_StructuredBuffer->UpdateData(44, VS | PS);
                    mesh->RenderInstanced(pos - batchStart);
                    ++s_Data.Stats.DrawCalls;
                }

                batchMesh = mesh;
                batchStart = pos;
            }
        }

        // Flush the final batch.
        if (batchMesh)
        {
            s_StructuredBuffer->SetData(&s_Data.ConstantData[batchStart], renderCount - batchStart);
            s_StructuredBuffer->UpdateData(44, VS | PS);
            batchMesh->RenderInstanced(renderCount - batchStart);
            ++s_Data.Stats.DrawCalls;
        }
    }

    void Renderer3D::Draw(MeshRenderComponent* renderComponent)
    {
        Renderer3DConstant sprite;
        sprite.WorldMat = renderComponent->Transform()->GetWorldMat().Transpose();

        s_Data.RenderComponents.push_back(renderComponent);
        s_Data.ConstantData.push_back(sprite);
    }

    void Renderer3D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }
}
