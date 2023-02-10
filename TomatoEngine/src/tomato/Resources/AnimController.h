#pragma once

#include "Res.h"

#include "AnimState.h"

namespace tomato
{
    struct AnimParameter
    {
        float Value;
        bool  IsTrigger;
    };

    class AnimController final : public Res
    {
    public:
        AnimController();
        ~AnimController() override;

        void Save(const fs::path& path) override {};
        int  Load(const fs::path& path) override { return S_OK; };

        void AddParameter(const string& name, bool isTrigger = false)
        {
            m_Parameters[name] = AnimParameter{0.f, isTrigger};
        }

        void RemoveParameter(const string& name)
        {
            m_Parameters.erase(m_Parameters.find(name));
        }

        AnimState* AddState(const string& name)
        {
            AnimState* newState = new AnimState;
            newState->SetName(name);
            m_AnimStates[name] = newState;
            return newState;
        }

        void SetDefaultState(AnimState* defaultState) { m_DefaultState = defaultState; }

        [[nodiscard]] const map<string, AnimState*>&    GetAnimStates() const { return m_AnimStates; }
        [[nodiscard]] const map<string, AnimParameter>& GetParameters() const { return m_Parameters; }
        [[nodiscard]] AnimState*                        GetDefaultState() const { return m_DefaultState; }
        [[nodiscard]] AnimState*                        GetAnyState() const { return m_AnyState; }

    private:
        map<string, AnimState*>    m_AnimStates;
        map<string, AnimParameter> m_Parameters;

        AnimState* m_DefaultState;
        AnimState* m_AnyState;
    };
}
