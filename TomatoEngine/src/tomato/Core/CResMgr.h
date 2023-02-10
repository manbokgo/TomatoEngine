#pragma once

#include "CPathMgr.h"
#include "tomato/Resources/AnimController.h"
#include "tomato/Resources/ComputeShader.h"
#include "tomato/Resources/CPrefab.h"
#include "tomato/Resources/GraphicsShader.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Mesh.h"
#include "tomato/Resources/Sound.h"
#include "tomato/Resources/Texture.h"

namespace tomato
{
    class Res;
    class CResMgr : public Singleton<CResMgr>
    {
        SINGLETON_CLASS(CResMgr);
        friend class CEventMgr;

    public:
        void Init();

        template<typename T>
        void AddRes(const fs::path& relativePath, const Ref<T>& resource);

        template <typename T>
        Ref<T> FindRes(const fs::path& relativePath);

        template <typename T>
        Ref<T> LoadRelative(const fs::path& relativePath);

        template <class T>
        Ref<T> Load(const fs::path& path);

        Ref<Texture> CreateTexture(const fs::path& path, UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag);
        Ref<Texture> CreateTexture(const fs::path& path, const ComPtr<ID3D11Texture2D>& tex2D);

        void CreateAnimClipFromTex(wstring atlasName, Ref<Texture> texture, Ref<Texture> normalTexture);
        void CreateDefaultAnimController();

        const vector<D3D11_INPUT_ELEMENT_DESC>& GetInputLayoutInfo() { return m_vecLayoutInfo; }

    private:
        void InitSound();
        void CreateDefaultMesh();
        void CreateDefaultTexture();
        void CreateDefaultGraphicsShader();
        void CreateDefaultComputeShader();
        void CreateDefaultMaterial();

        void AddInputLayout(DXGI_FORMAT format, const char* semanticName);
        void CreateFromAtlasFile(const fs::path& atlasPath);


    private:
        array<map<fs::path, Ref<Res>>, (UINT)eResType::End> m_ResMap;
        vector<D3D11_INPUT_ELEMENT_DESC> m_vecLayoutInfo;
        UINT                             m_iLayoutOffset;
    };

    template <typename T>
    eResType GetType()
    {
        const size_t hashCode = typeid(T).hash_code();
        if (hashCode == typeid(Mesh).hash_code())
        {
            return eResType::Mesh;
        }
        else if (hashCode == typeid(Texture).hash_code())
        {
            return eResType::Texture;
        }
        else if (hashCode == typeid(GraphicsShader).hash_code())
        {
            return eResType::GraphicsShader;
        }
        else if (hashCode == typeid(ComputeShader).hash_code())
        {
            return eResType::ComputeShader;
        }
        else if (hashCode == typeid(Material).hash_code())
        {
            return eResType::Material;
        }
        else if (hashCode == typeid(CPrefab).hash_code())
        {
            return eResType::Prefab;
        }
        else if (hashCode == typeid(AnimClip).hash_code())
        {
            return eResType::AnimClip;
        }
        else if (hashCode == typeid(AnimController).hash_code())
        {
            return eResType::AnimController;
        }
        else if (hashCode == typeid(Sound).hash_code())
        {
            return eResType::Sound;
        }
        else
        {
            return eResType::End;
        }
    }

    template <typename T>
    void CResMgr::AddRes(const fs::path& relativePath, const Ref<T>& resource)
    {
        assert(!FindRes<T>(relativePath));

        eResType resType = GetType<T>();
        m_ResMap[(UINT)resType].emplace(relativePath, resource);
    }

    template <typename T>
    Ref<T> CResMgr::FindRes(const fs::path& relativePath)
    {
        eResType resType = GetType<T>();
        const auto iter = m_ResMap[(UINT)resType].find(relativePath);
        if (iter == m_ResMap[(UINT)resType].end())
        {
            return nullptr;
        }

        return std::static_pointer_cast<T>(iter->second);
    }

    template <typename T>
    Ref<T> CResMgr::LoadRelative(const fs::path& relativePath)
    {
        Ref<T> resource = FindRes<T>(relativePath);
        if (resource)
        {
            return resource;
        }

        fs::path path = CPathMgr::GetInst()->GetContentPath();
        path += relativePath;

        resource = CreateRef<T>();
        if (FAILED(resource->Load(path)))
        {
            MessageBox(nullptr, path.c_str(), L"리소스 로딩 실패", MB_OK);
            return nullptr;
        }

        eResType resType = GetType<T>();
        m_ResMap[(UINT)resType].emplace(relativePath, resource);
        return resource;
    }

    template <typename T>
    Ref<T> CResMgr::Load(const fs::path& path)
    {
        const fs::path relativePath = path.lexically_relative(CPathMgr::GetInst()->GetContentPath());

        Ref<T> resource = FindRes<T>(relativePath);
        if (resource)
        {
            return resource;
        }

        resource = CreateRef<T>();
        if (FAILED(resource->Load(path)))
        {
            MessageBox(nullptr, path.c_str(), L"리소스 로딩 실패", MB_OK);
            return nullptr;
        }

        eResType resType = GetType<T>();
        m_ResMap[(UINT)resType].emplace(relativePath, resource);
        return resource;
    }
}
