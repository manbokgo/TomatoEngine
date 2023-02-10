#include "pch.h"
#include "Sound.h"

namespace tomato
{
    FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                                 , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                                 , void* commanddata1, void* commanddata2);

    FMOD::System* Sound::g_pFMOD = nullptr;

    Sound::Sound()
        : Res(eResType::Sound)
        , m_pSound(nullptr)
    {
    }

    Sound::~Sound()
    {
        if (nullptr != m_pSound)
        {
            m_pSound->release();
            m_pSound = nullptr;
        }
    }

    int Sound::Play(int _iRoopCount, float _fVolume, bool _bOverlap)
    {
        if (_iRoopCount <= -1)
        {
            assert(nullptr);
        }

        // ����ǰ� �ִ� ä���� �ִµ�, �ߺ������ ������� �ʾҴ� -> ��� ����
        if (!_bOverlap && !m_listChannel.empty())
        {
            return -1;
        }

        _iRoopCount -= 1;

        FMOD::Channel* pChannel = nullptr;
        g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

        pChannel->setVolume(_fVolume);

        pChannel->setCallback(CHANNEL_CALLBACK);
        pChannel->setUserData(this);

        pChannel->setMode(FMOD_LOOP_NORMAL);
        pChannel->setLoopCount(_iRoopCount);

        m_listChannel.push_back(pChannel);

        int iIdx = -1;
        pChannel->getIndex(&iIdx);

        return iIdx;
    }

    void Sound::Stop()
    {
        list<FMOD::Channel*>::iterator iter;

        while (!m_listChannel.empty())
        {
            iter = m_listChannel.begin();
            (*iter)->stop();
        }
    }

    void Sound::SetVolume(float _f, int _iChannelIdx)
    {
        list<FMOD::Channel*>::iterator iter = m_listChannel.begin();

        int iIdx = -1;
        for (; iter != m_listChannel.end(); ++iter)
        {
            (*iter)->getIndex(&iIdx);
            if (_iChannelIdx == iIdx)
            {
                (*iter)->setVolume(_f);
                return;
            }
        }
    }

    void Sound::RemoveChannel(FMOD::Channel* _pTargetChannel)
    {
        list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
        for (; iter != m_listChannel.end(); ++iter)
        {
            if (*iter == _pTargetChannel)
            {
                m_listChannel.erase(iter);
                return;
            }
        }
    }


    // =========
    // Call Back
    // =========
    FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                                 , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                                 , void* commanddata1, void* commanddata2)
    {
        FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
        Sound* pSound = nullptr;

        switch (controltype)
        {
        case FMOD_CHANNELCONTROL_CALLBACK_END:
            {
                cppchannel->getUserData((void**)&pSound);
                pSound->RemoveChannel(cppchannel);
            }
            break;
        }

        return FMOD_OK;
    }
    
}
