#include "pch.h"
#include "CEventMgr.h"

namespace tomato
{/*
    CEventMgr::CEventMgr()
    {
    }

    CEventMgr::~CEventMgr()
    {
    }

    void CEventMgr::tick()
    {
        m_bLevelChanged = false;

        // �޸� ����
        for (size_t i = 0; i < m_vecGarbage.size(); ++i)
        {		
            delete m_vecGarbage[i];
            m_bLevelChanged = true;
        }
        m_vecGarbage.clear();



        // �̺�Ʈ ó��
        for (size_t i = 0; i < m_vecEvent.size(); ++i)
        {
            switch (m_vecEvent[i].eType)
            {
            case eEventType::CreateObject:
                {
                    // wParam : GameObject Adress
                    // lParam : Layer Index
                    Entity* pNewObj = (Entity*)m_vecEvent[i].wParam;
                    eLayer  iLayerIdx = (eLayer)m_vecEvent[i].lParam;

                    const auto& pLevel = CLevelMgr::GetInst()->GetCurLevel();
                    pLevel->AddGameObject(pNewObj, iLayerIdx);		
                    m_bLevelChanged = true;
                }
                break;
            case eEventType::DeleteObject:
                {
                    // wParam : GameObject Adress
                    Entity* pObj = (Entity*)m_vecEvent[i].wParam;

                    if (!pObj->IsDead())
                    {				
                        // ����ó���� �ֻ��� �θ� �������� �ִ´�.
                        m_vecGarbage.push_back(pObj);

                        // ������ ������Ʈ ����, ��� �ڽĿ�����Ʈ�� Dead üũ�Ѵ�.
                        static list<Entity*> queue;				
                        queue.push_back(pObj);
                        while (!queue.empty())
                        {
                            Entity* pObj = queue.front();
                            queue.pop_front();

                            const vector<Entity*>& vecChild = pObj->GetChildObject();
                            for (size_t i = 0; i < vecChild.size(); ++i)
                            {
                                queue.push_back(vecChild[i]);
                            }

                            pObj->m_bDead = true;
                        }				
                    }
                }
                break;

            case eEventType::AddChild:
                {
                    // wParam : Child Adress, lParam : Parent Adress
                    Entity* pParent = (Entity*)m_vecEvent[i].lParam;
                    Entity* pChild = (Entity*)m_vecEvent[i].wParam;

                    pParent->AddChild(pChild);
                    m_bLevelChanged = true;
                }
                break;

            case eEventType::DeleteRes:
                {
                    // wParam : RES_TYPE, lParam : Resource Adress
                    if (!CResMgr::GetInst()->DeleteRes((eResType)m_vecEvent[i].wParam, ((CRes*)m_vecEvent[i].lParam)->GetPath()))
                    {
                        MessageBox(nullptr, L"���ҽ� ���� ����", L"����", MB_OK);
                    }
                }

                break;
            case eEventType::ChangeLevelState:
                {
                    // wParam : Level State
                    m_bLevelChanged = true;			
                    CLevelMgr::GetInst()->ChangeLevelState((eLevelState)m_vecEvent[i].wParam);
                }
                break;		

            case eEventType::ChangeLevel:
                {
                    m_bLevelChanged = true;
                }
                break;
            case eEventType::End:
                break;
            default:
                break;
            }
        }

        m_vecEvent.clear();
    }*/
}
