#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class StructuredBuffer;
    class TileMapComponent : public RenderComponent
    {
    private:
        Ref<Texture> m_AtlasTex;   // 타일맵이 사용하는 아틀라스 이미지
        Vec2         m_vTileCount; // 타일맵 가로 세로 개수

        vector<tTile>     m_vecTile;
        StructuredBuffer* m_TileBuffer; // 각 타일의 아틀라스 참조정보 구조체

        bool m_bDataChanged;

    public:
        void SetTileAtlas(Ref<Texture> _AtlasTex) { m_AtlasTex = _AtlasTex; }
        void SetTileCount(UINT _iWidth, UINT _iHeight);

    public:
        void render();

    public:
        CLONE(TileMapComponent);

    public:
        TileMapComponent();
        ~TileMapComponent();
    };
}
