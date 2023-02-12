#pragma once
#include "RenderComponent.h"

namespace tomato
{
    class StructuredBuffer;
    class TileMapComponent : public RenderComponent
    {
    private:
        Ref<Texture> m_AtlasTex;   // Ÿ�ϸ��� ����ϴ� ��Ʋ�� �̹���
        Vec2         m_vTileCount; // Ÿ�ϸ� ���� ���� ����

        vector<tTile>     m_vecTile;
        StructuredBuffer* m_TileBuffer; // �� Ÿ���� ��Ʋ�� �������� ����ü

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
