#include "pch.h"
#include "AnimController.h"

namespace tomato
{
    AnimController::AnimController()
        : Res(eResType::AnimController) 
    {
        m_AnyState = AddState("Any");
    }

    AnimController::~AnimController()
    {
        Safe_Del_Map(m_AnimStates);
    }
}
