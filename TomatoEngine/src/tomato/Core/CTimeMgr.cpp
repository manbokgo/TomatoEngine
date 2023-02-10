#include "pch.h"
#include "CTimeMgr.h"

namespace tomato
{
    CTimeMgr::CTimeMgr()
        : m_llFrequence{}
        , m_llCurCount{}
        , m_llPrevCount{}
        , m_fAccTime(0.f)
        , m_fDeltaTime(0.f)
    {

    }

    CTimeMgr::~CTimeMgr()
    {

    }

    void CTimeMgr::init()
    {
        // 초당 진동수 1000
        // GetTickCount();

        // 초당 진동수 천만
        QueryPerformanceFrequency(&m_llFrequence);

        // 현재 카운팅
        QueryPerformanceCounter(&m_llPrevCount);
    }

    void CTimeMgr::tick()
    {
        QueryPerformanceCounter(&m_llCurCount);

        m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequence.QuadPart;

        m_llPrevCount.QuadPart = m_llCurCount.QuadPart;

        // 60 프레임을 초과할 경우 강제 방어
        if ((1.f / 60.f) < m_fDeltaTime)
        {
            m_fDeltaTime = (1.f / 60.f);
        }

        g_global.fDT = m_fDeltaTime;
        g_global.fAccTime += m_fDeltaTime;
    }

    void CTimeMgr::render()
    {
        static int iCount = 0;
        ++iCount;

        // 1 초에 한번
        m_fAccTime += m_fDeltaTime;
        if (1.f < m_fAccTime)
        {
            wchar_t szFloat[50] = {};
            float   FPS = 1.f / m_fDeltaTime;
            swprintf_s(szFloat, 50, L"DeltaTime : %d", iCount);
            int iLen = (int)wcsnlen_s(szFloat, 50);
            //TextOut(_dc, 10, 10, szFloat, iLen);
		
            // 누적시간, 카운트 초기화
            m_fAccTime = 0.f;
            iCount = 0;
        }
    }
    
}
