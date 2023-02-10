#include "pch.h"
#include "CResMgr.h"

#include "tomato/Resources/Texture.h"

namespace tomato
{
    CResMgr::CResMgr()
        : m_iLayoutOffset(0)
    {

    }

    CResMgr::~CResMgr()
    {
        Sound::g_pFMOD->release();
    }


    Ref<Texture> CResMgr::CreateTexture(const fs::path& path, UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag)
    {
        Ref<Texture> pTex = FindRes<Texture>(path);
        assert(!pTex);

        pTex = CreateRef<Texture>();
        pTex->Create(width, height, format, bindFlag);

        m_ResMap[(UINT)eResType::Texture].emplace(path, pTex);
        return pTex;
    }

    Ref<Texture> CResMgr::CreateTexture(const fs::path& path, const ComPtr<ID3D11Texture2D>& tex2D)
    {
        Ref<Texture> pTex = FindRes<Texture>(path);
        assert(!pTex);
		
        pTex = CreateRef<Texture>();
        pTex->Create(tex2D);

        m_ResMap[(UINT)eResType::Texture].emplace(path, pTex);
        return pTex;
    }
}
