#include "pch.h"
#include "AnimatorComponent.h"

#include "MeshRenderComponent.h"
#include "TransformComponent.h"
#include "tomato/Core/CTimeMgr.h"
#include "tomato/Resources/Material.h"
#include "tomato/Resources/Texture.h"

#include "tomato/Scene/Entity.h"

namespace tomato
{
    struct MyVec4 : public Vec4
    {
        using Vec4::Vec4;
    };


    AnimatorComponent::AnimatorComponent() {}

    AnimatorComponent::~AnimatorComponent() {}

    void AnimatorComponent::Start()
    {
        MyVec4 a(4, 4, 4, 4);
        MyVec4 b(3,3,3,3);
        MyVec4 c = a + b;
        m_Parameters = m_Controller->GetParameters();
        m_CurState   = m_Controller->GetDefaultState();
        m_AnyState   = m_Controller->GetAnyState();
    }

    bool AnimatorComponent::conditionCheck(const AnimCondition& condition) const
    {
        const AnimParameter parameter = m_Parameters.at(condition.Parameter);

        switch (condition.Mode)
        {
        case eAnimConditionMode::If:
            if (parameter.Value)
            {
                return true;
            }
            break;
        case eAnimConditionMode::IfNot:
            if (!parameter.Value)
            {
                return true;
            }
            break;
        case eAnimConditionMode::GreaterOrEqual:
            if (parameter.Value >= condition.Threshold)
            {
                return true;
            }
            break;
        case eAnimConditionMode::Less:
            if (parameter.Value < condition.Threshold)
            {
                return true;
            }
            break;
        case eAnimConditionMode::Equal:
            if (parameter.Value == condition.Threshold)
            {
                return true;
            }
            break;
        case eAnimConditionMode::NotEqual:
            if (parameter.Value != condition.Threshold)
            {
                return true;
            }
            break;
        default:
            ASSERT(false);
        }

        return false;
    }

    bool AnimatorComponent::UpdateState(const AnimState* testState)
    {
        const vector<AnimTransition>& transitions = testState->GetTransitions();

        for (const AnimTransition& transition : transitions)
        {
            if (transition.IsExit && !m_bFinish)
            {
                continue;
            }

            bool conditionMet = true;
            for (const AnimCondition& condition : transition.Conditions)
            {
                conditionMet = conditionCheck(condition);
                if (!conditionMet)
                {
                    break;
                }
            }
            if (!conditionMet)
            {
                continue;
            }


            m_CurState = transition.DestState;
            Reset();

            return true;
        }

        return false;
    }

    void AnimatorComponent::OnUpdate()
    {
        UpdateState(m_AnyState);
        while (UpdateState(m_CurState)) {};

        for (auto && [name, parameter] : m_Parameters)
        {
            if (parameter.IsTrigger)
            {
                parameter.Value = false;
            }
        }
    }

    void AnimatorComponent::progressIndex(const vector<AnimationFrm>& frames)
    {
        if (m_bFinish)
        {
            return;
        }

        // 런타임 중 프레임 수정으로 인한 프레임 갯수 변경 상황 처리
        if (frames.size() <= m_CurIdx)
        {
            m_CurIdx  = (int)frames.size() - 1;
            m_bFinish = true;
            return;
        }

        // 시간 체크
        m_AccTime += DT;

        // 누적 시간이 해당 프레임 유지시간을 넘어서면 다음프레임으로 넘어감
        if (frames[m_CurIdx].Duration < m_AccTime)
        {
            // TODO 듀레이션 빼기 while
            m_AccTime = 0.f;
            ++m_CurIdx;

            // 최대 프레임에 도달하면 Finish 상태로 전환
            if (frames.size() <= m_CurIdx)
            {
                if (m_CurState->IsRepeat())
                {
                    Reset();
                }
                else
                {
                    m_CurIdx  = (int)frames.size() - 1;
                    m_bFinish = true;
                }
            }

            if (!m_bFinish && !frames[m_CurIdx].Events.empty())
            {
                for (const auto & event : frames[m_CurIdx].Events)
                {
                    event();
                }
            }
        }
    }

    void AnimatorComponent::OnLateUpdate()
    {
        const vector<AnimationFrm>& frames = m_CurState->GetClip()->GetFrames();
        progressIndex(frames);


        if (frames[m_CurIdx].SpriteTexture->GetType() == eTextureType::Multiple)
        {
            const UINT ppu = frames[m_CurIdx].SpriteTexture->GetPixelsPerUnit();
            const Vec2 textureSize = frames[m_CurIdx].SpriteTexture->GetSize();

            Vec3 scaleModifier;
            scaleModifier.x = frames[m_CurIdx].Sprite.OrigSize.x * textureSize.x / (float)ppu;
            scaleModifier.y = frames[m_CurIdx].Sprite.OrigSize.y * textureSize.y / (float)ppu;
            scaleModifier.z = 1.f;

            Transform()->SetScaleModifier(scaleModifier);
        }
        else
        {
            Transform()->SetScaleModifier(Vec3(1.f, 1.f, 1.f));
        }


        const auto& curMaterial = GetGameObject()->MeshRender()->GetMaterial();
        curMaterial->SetTexParam(eTexParam::Tex0, frames[m_CurIdx].SpriteTexture, frames[m_CurIdx].Sprite);
        curMaterial->SetTexParam(eTexParam::Tex1, frames[m_CurIdx].NormalTexture, frames[m_CurIdx].Sprite);
    }

    /*
void Animator2D::AddAnimation(const Ref<AnimClip>& _Clip)
{
    AnimState* pAnim = FindAnimation(_Clip->GetName());
    if (pAnim)
    {
        return;
    }

    pAnim = new AnimState;
    pAnim->SetClip(_Clip);
    pAnim->SetName(_Clip.Get()->GetName());

    pAnim->m_Animator = this;
    m_mapAnim.insert(make_pair(_Clip->GetName(), pAnim));
}

AnimState* Animator2D::FindAnimation(const wstring& _strKey)
{
    map<wstring, AnimState*>::iterator iter = m_mapAnim.find(_strKey);

    if (iter == m_mapAnim.end())
        return nullptr;

    return iter->second;
}
*/

    void AnimatorComponent::Play(const string& name) {}
    void AnimatorComponent::Play(AnimState* animState) {}
    
}
