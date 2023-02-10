#pragma once
namespace tomato
{
    class CTimeMgr
        : public Singleton<CTimeMgr>
    {	
    private:
        LARGE_INTEGER m_llFrequence;
        LARGE_INTEGER m_llCurCount;
        LARGE_INTEGER m_llPrevCount;

        float m_fAccTime;
        float m_fDeltaTime;

    public:
        void init();
        void tick();
        void render();

    public:
        float GetDeltaTime() { return m_fDeltaTime; }

    public:
        CTimeMgr();
        ~CTimeMgr();
    };

    
}
