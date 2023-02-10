#pragma once
#include "Component.h"

namespace tomato
{
    class Mesh;
    class Material;
    class RenderComponent : public Component
    {
    private:
        Ref<Mesh> m_pMesh;

        Ref<Material> m_pSharedMtrl;  // ���� ����   
        Ref<Material> m_pDynamicMtrl; // ���� ����
        Ref<Material> m_pCurMtrl;     // ���� ��� ���� ����


    public:
        virtual void render() = 0;

    public:
        void              SetMesh(Ref<Mesh> _pMesh) { m_pMesh = _pMesh; }
        const Ref<Mesh>& GetMesh() const { return m_pMesh; }

        void SetSharedMaterial(Ref<Material> _pMtrl)
        {
            m_pSharedMtrl = _pMtrl;
            m_pCurMtrl = _pMtrl;
        };
        Ref<Material> GetSharedMaterial();

        Ref<Material> GetCurMaterial() { return m_pCurMtrl; }
        Ref<Material> GetDynamicMaterial();


    public:
        virtual RenderComponent* Clone() = 0;
    public:
        RenderComponent();
        RenderComponent(const RenderComponent& _origin);
        ~RenderComponent();
    };
}
