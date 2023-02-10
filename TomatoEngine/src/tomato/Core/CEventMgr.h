#pragma once
#include "singleton.h"

namespace tomato
{
    class Entity;
    class CEventMgr :
        public Singleton<CEventMgr>
    {
    private:
        vector<tEvent>  m_vecEvent;
        vector<Entity*> m_vecGarbage;

        bool m_bLevelChanged;

    public:
        void tick();

    public:
        void AddEvent(tEvent& _evn){m_vecEvent.push_back(_evn);}

        bool IsLevelChanged() { return m_bLevelChanged; }
        void SetLevelChanged(bool bLevelChanged) { m_bLevelChanged = bLevelChanged; }


    public:
        CEventMgr();
        ~CEventMgr();
    };

    
}
