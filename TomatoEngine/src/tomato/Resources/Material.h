#pragma once
#include "Res.h"
#include "GraphicsShader.h"

#include "tomato/Core/IName.h"

namespace tomato
{
    class Material final : public Res, public IName
    {
    public:
        Material();
        Material(const Material& other);
        ~Material() override;

        void Save(const fs::path& path) override {};
        int  Load(const fs::path& path) override { return S_OK; };

        void        Bind();
        static void Clear();

        void SetScalarParam(eScalarParam scalarParam, void* data);
        void GetScalarParam(eScalarParam scalarParam, void* outData);

        void SetTexParam(eTexParam texParam, const Ref<Texture>& texture);
        void SetTexParam(eTexParam texParam, const Ref<Texture>& texture, const SpriteFrm& spriteFrm);

        [[nodiscard]] const Ref<Texture>& GetTexParam(eTexParam texParam) const { return m_arrTex[(UINT)texParam]; }
        [[nodiscard]] const tMtrlConst& GetCB() const { return m_CB; }
        [[nodiscard]] const Ref<GraphicsShader>& GetShader() const { return m_Shader; }

        void SetShader(const Ref<GraphicsShader>& shader) { m_Shader = shader; }

    private:
        tMtrlConst          m_CB;
        array<Ref<Texture>, (UINT)eTexParam::End> m_arrTex;
        Ref<GraphicsShader> m_Shader;
    };
}
