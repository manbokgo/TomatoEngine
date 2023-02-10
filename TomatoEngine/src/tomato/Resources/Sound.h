#pragma once
#include "Res.h"

namespace tomato
{
    class Sound : public Res
    {
        friend FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                                            , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                                            , void* commanddata1, void* commanddata2);

    public:
        Sound();
        ~Sound() override;

        // 0 (무한반복) 0 ~ 1(Volume)
        int  Play(int _iRoopCount, float _fVolume = 1.f, bool _bOverlap = false);
        void Stop();

        // 0 ~ 1
        void SetVolume(float _f, int _iChannelIdx);

    private:
        void RemoveChannel(FMOD::Channel* _pTargetChannel);

    public:
        static FMOD::System* g_pFMOD;

    private:
        FMOD::Sound*         m_pSound;
        list<FMOD::Channel*> m_listChannel;
    };
}
