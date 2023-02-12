#include "pch.h"
#include "Component.h"

#include "tomato/Scene/Entity.h"

#define GET_OTHER_COMPONENT_DEFINITION(ComponentName) ComponentName##Component* Component::ComponentName() { return m_GameObject->ComponentName(); }

namespace tomato
{
    GET_OTHER_COMPONENT_DEFINITION(Transform);
    GET_OTHER_COMPONENT_DEFINITION(MeshRender);
    GET_OTHER_COMPONENT_DEFINITION(Camera);
    GET_OTHER_COMPONENT_DEFINITION(Collider2D);
    GET_OTHER_COMPONENT_DEFINITION(Rigidbody2D);
    GET_OTHER_COMPONENT_DEFINITION(Animator);
    GET_OTHER_COMPONENT_DEFINITION(Light2D);
    GET_OTHER_COMPONENT_DEFINITION(ParticleSystem);
    GET_OTHER_COMPONENT_DEFINITION(TileMap);
}
