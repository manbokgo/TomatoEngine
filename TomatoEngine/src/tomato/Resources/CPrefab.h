#pragma once
#include "Res.h"

namespace tomato
{
    class Entity;
    class CPrefab :       public Res
    {
    private:
        Entity* m_pProtoObj;

    public:
        Entity* Instantiate();

    public:
        CPrefab();
        CPrefab(Entity* _pProto);
        ~CPrefab();
    };
}
