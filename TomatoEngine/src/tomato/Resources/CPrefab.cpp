#include "pch.h"
#include "CPrefab.h"

#include "tomato/Scene/Entity.h"

namespace tomato
{
    CPrefab::CPrefab()
        : Res(eResType::Prefab)
        , m_pProtoObj(nullptr)
    {
    }

    CPrefab::CPrefab(Entity* _pProto)
        : Res(eResType::Prefab)
        , m_pProtoObj(_pProto)
    {
        // if (_pProto->GetLayerIdx() != eLayer::End)
        // {
            m_pProtoObj = _pProto->Clone();
        // }
    }

    CPrefab::~CPrefab()
    {
        if (nullptr != m_pProtoObj)
            delete m_pProtoObj;
    }

    Entity* CPrefab::Instantiate()
    {
        if (nullptr == m_pProtoObj)
            return nullptr;

        return m_pProtoObj->Clone();
    }
}
