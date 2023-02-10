#include "pch.h"
#include "Res.h"

namespace tomato
{
    Res::Res(eResType resType)
        : m_eResType(resType)
    {
    }

    Res::Res(const Res& other)
        : m_eResType(other.m_eResType)
    {
    }
}
