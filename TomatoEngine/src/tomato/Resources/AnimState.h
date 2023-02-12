#pragma once

#include "AnimClip.h"
#include "tomato/Core/IName.h"

namespace tomato
{
    class AnimatorComponent;
    enum class eAnimConditionMode
    {
        If,
        IfNot,
        GreaterOrEqual,
        Less,
        Equal,
        NotEqual
    };

    struct AnimCondition
    {
        eAnimConditionMode Mode = eAnimConditionMode::If;
        string             Parameter;
        float              Threshold = 0.f;
    };


    struct AnimTransition
    {
        vector<AnimCondition> Conditions;
        AnimState*           DestState = nullptr;
        bool                  IsExit = false;
    };


    class AnimState : public IName
    {
        friend class Animator;

    public:
        AnimState();
        ~AnimState() override;
        CLONE(AnimState);

        void AddTransition(const AnimTransition& transition)
        {
            m_Transitions.push_back(transition);
        }

        void SetClip(const Ref<AnimClip>& clip) { m_Clip = clip; }
        void SetRepeat(bool bRepeat) { m_bRepeat = bRepeat; }

        [[nodiscard]] const Ref<AnimClip>&         GetClip() const { return m_Clip; }
        [[nodiscard]] bool                          IsRepeat() const { return m_bRepeat; }
        [[nodiscard]] const vector<AnimTransition>& GetTransitions() const { return m_Transitions; }
    

    private:
        Ref<AnimClip>         m_Clip;
        vector<AnimTransition> m_Transitions;

        bool m_bRepeat;
    };
    
}
