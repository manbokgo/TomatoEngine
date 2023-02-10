#include "pch.h"
#include "CPathMgr.h"

namespace tomato
{
    CPathMgr::CPathMgr() {}

    CPathMgr::~CPathMgr()
    {

    }

    void CPathMgr::Init()
    {
        const fs::path contentPath = fs::current_path() / "../bin/content/";
        m_szContentPath = contentPath.lexically_normal();
    }
    
}
