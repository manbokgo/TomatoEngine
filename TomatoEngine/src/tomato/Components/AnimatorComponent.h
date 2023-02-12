#pragma once
#include "Component.h"
#include "tomato/Resources/AnimController.h"

class CAnimClip;
class CAnimState;

namespace tomato
{
    class AnimatorComponent : public Component
    {
    public:
        AnimatorComponent();
        ~AnimatorComponent() override;

        CLONE(AnimatorComponent);

        void Start() override;

        void OnUpdate() override;
        void progressIndex(const vector<AnimationFrm>& frames);
        void OnLateUpdate() override;

        void SetController(const Ref<AnimController>& controller) { m_Controller = controller; }
        void SetParameter(const string& name, float value) { m_Parameters[name].Value = value; }


        [[nodiscard]] AnimController*     GetController() const { return m_Controller.get(); }
        [[nodiscard]] const AnimParameter& GetParameter(const string& name) const { return m_Parameters.at(name); }

        [[nodiscard]] const AnimState* GetCurState() const { return m_CurState; }
        [[nodiscard]] int               GetCurIdx() const { return m_CurIdx; }

        void Play(const string& name);
        void Play(AnimState* animState);

        void Reset()
        {
            m_AccTime = 0.f;
            m_CurIdx = 0;
            m_bFinish = false;
        }

    private:
        bool conditionCheck(const AnimCondition& condition) const;
        bool UpdateState(const AnimState* testState);

    private:
        Ref<AnimController> m_Controller;
        const AnimState*    m_CurState;
        const AnimState*    m_AnyState;

        map<string, AnimParameter> m_Parameters;

        int   m_CurIdx;
        float m_AccTime;
        bool  m_bFinish;
    };
}
