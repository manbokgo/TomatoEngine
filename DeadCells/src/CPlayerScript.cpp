#include "pch.h"
#include "CPlayerScript.h"

#include "tomato/Components/AnimatorComponent.h"
#include "tomato/Components/Collider2DComponent.h"
#include "tomato/Components/TransformComponent.h"
#include "tomato/Core/CKeyMgr.h"
#include "tomato/Resources/AnimClip.h"
#include "tomato/Resources/Texture.h"
#include "tomato/Scene/Entity.h"

namespace tomato
{ 
    CPlayerScript::CPlayerScript()
    {
        AddScriptField("Player Move Speed", FieldType::Float, &m_fSpeed);
        AddScriptField("Player Jump Height", FieldType::Float, &m_JumpStartSpeed);
    }

    CPlayerScript::~CPlayerScript() {}

    void CPlayerScript::OnStart()
    {
        m_RigidBody = GetGameObject()->Rigidbody2D();
        m_Animator  = GetGameObject()->Animator();
        m_Collider  = GetGameObject()->Collider2D();
    }


    void CPlayerScript::MoveBone(const int& curAnimIndex, const Ref<AnimClip>& animationClip2D, Entity* bone, string boneName) {
        const Vec3          headBonePos = animationClip2D->GetBonePosition(boneName, curAnimIndex);
        const AnimationFrm& frm         = animationClip2D->GetFrame(curAnimIndex);
        const UINT          ppu = frm.SpriteTexture->GetPixelsPerUnit();

        const Vec2 OrigSizeInPixel     = frm.Sprite.OrigSize * frm.SpriteTexture->GetSize();
        Vec3       relativeHeadBonePos = headBonePos - OrigSizeInPixel / 2.f;
        relativeHeadBonePos.y          = -relativeHeadBonePos.y;

        relativeHeadBonePos.x /= (float)ppu;
        relativeHeadBonePos.y /= (float)ppu;
        relativeHeadBonePos.z = -1.f;
        if (m_Flip)
        {
            relativeHeadBonePos.z = 1.f;
        }

        bone->Transform()->SetRelativePos(relativeHeadBonePos);
    }

    void CPlayerScript::OnUpdate()
    {
        {
            const int&            curAnimIndex    = GetGameObject()->Animator()->GetCurIdx();
            const Ref<AnimClip>& animationClip2D = GetGameObject()->Animator()->GetCurState()->GetClip();

            // TODO 추상화
            if (animationClip2D->IsHasBone())
            {
                MoveBone(curAnimIndex, animationClip2D, m_Head, "headBone");
                MoveBone(curAnimIndex, animationClip2D, m_Hand, "Bip001 L Hand");
            }
        }

        m_Move = Vec2(0.f, 0.f);
        if (KEY_PRESSED(KEY::LEFT))
        {
            m_Move.x = -1.f;

            if (KEY_PRESSED(KEY::LSHIFT))
                m_Move.x *= 0.5f;
        }

        if (KEY_PRESSED(KEY::RIGHT))
        {
            m_Move.x = 1.f;

            if (KEY_PRESSED(KEY::LSHIFT))
                m_Move.x *= 0.5f;
        }


        if (KEY_PRESSED(KEY::LBTN))
        {
            m_Animator->SetParameter("attack", true);
        }

        if (m_PrevMove.x * m_Move.x < 0.f)
        {
            // 부호가 달라짐
            m_Animator->SetParameter("volte", true);
        }

        m_PrevMove = m_Move;

        if (m_JumpState == eJumpState::Grounded && KEY_PRESSED(KEY::SPACE))
        {
            m_JumpState = eJumpState::PrepareToJump;
        }
        else if (KEY_RELEASE(KEY::SPACE))
        {
            m_bStopJump = true;
        }


        // const bool isGrounded = m_RigidBody->IsGround();

        /*
    {
        m_bJump = false;
        switch (m_JumpState)
        {
        case eJumpState::PrepareToJump:
            m_JumpState = eJumpState::Jumping;
            m_bJump     = true;
            m_bStopJump = false;
            break;
        case eJumpState::Jumping:
            if (!isGrounded)
            {
                m_JumpState = eJumpState::InFlight;
            }
            break;
        case eJumpState::InFlight:
            if (isGrounded)
            {
                m_JumpState = eJumpState::Landed;
            }
            break;
        case eJumpState::Landed:
            m_JumpState = eJumpState::Grounded;
            break;
        }
    }*/

        {
            /*
        Vec2 velocity = m_RigidBody->GetVelocity();
        velocity.x    = m_Move.x * m_fSpeed;

        if (m_bJump && isGrounded)
        {
            velocity.y = m_JumpStartSpeed * 1.5f;

            m_bJump = false;
        }
        else if (m_bStopJump)
        {
            m_bStopJump = false;
            if (velocity.y > 0)
            {
                velocity.y = velocity.y * 0.5f;
            }
        }

        m_RigidBody->SetVelocity(velocity);
        */

            /*
        if (!isGrounded)
        {
            if (velocity.y > 20.f)
            {
                m_Animator->Play(L"beheaded/jumpUp", false);
            }
            else if (velocity.y < 20.f)
            {
                m_Animator->Play(L"beheaded/jumpDown", false);
            }
            else
            {
                m_Animator->Play(L"beheaded/jumpTransition", false);
            }
        }
        else
        {
            if (m_JumpState == eJumpState::Landed)
            {
                m_Animator->Play(L"beheaded/land", true);
            }
            else if (m_JumpState == eJumpState::Grounded)
            {
                if (m_Move.x != 0.f)
                {
                    m_Animator->Play(L"beheaded/run", true);
                }
                else
                {
                    m_Animator->Play(L"beheaded/idle", true);
                }
            }
        }
        */


            if (m_Move.x > 0.01f)
            {
                // TODO bool flipX 넣기
                Vec3 rotation = GetGameObject()->Transform()->GetRelativeRotation();
                rotation.y    = 0.f;
                m_Flip        = false;
                GetGameObject()->Transform()->SetRelativeRotation(rotation);
            }
            else if (m_Move.x < -0.01f)
            {
                Vec3                   rotation = GetGameObject()->Transform()->GetRelativeRotation();
                static constexpr float FLIP     = 1.f * XM_PI;
                rotation.y                      = FLIP;
                m_Flip                          = true;
                GetGameObject()->Transform()->SetRelativeRotation(rotation);
            }

            m_Animator->SetParameter("velocityX", std::abs(m_Move.x));
            // m_Animator->SetParameter("velocityY", velocity.y);
        }
    }


    Vec3 CPlayerScript::PosSolver(Collider2DComponent* _pOther)
    {
        const auto orig = GetGameObject()->Collider2D();

        const Vector2 playerLB = orig->GetFinalPos() - orig->GetFinalScale() / 2.f;
        const Vector2 playerRT = orig->GetFinalPos() + orig->GetFinalScale() / 2.f;
        const Vector2 otherLB  = _pOther->GetFinalPos() - _pOther->GetFinalScale() / 2.f;
        const Vector2 otherRT  = _pOther->GetFinalPos() + _pOther->GetFinalScale() / 2.f;

        Vector3 correctionMove;

        float mx; // x 방향으로 겹친 거리
        float my; // y 방향으로 겹친 거리
        float dx; // 수정되어야할 x 방향
        float dy; // 수정되어야할 y 방향

        const float leftCheck  = playerRT.x - otherLB.x;
        const float rightCheck = otherRT.x - playerLB.x;
        if (leftCheck < rightCheck) // 왼쪽 편에 겹쳤다
        {
            mx = leftCheck;
            dx = -1.f;
        }
        else // 오른쪽 편에 겹쳤다
        {
            mx = rightCheck;
            dx = 1.f;
        }

        const float topCheck    = playerRT.y - otherLB.y;
        const float bottomCheck = otherRT.y - playerLB.y;
        if (topCheck < bottomCheck) // 위쪽 편에 겹쳤다
        {
            my = topCheck;
            dy = -1.f;
        }
        else // 아래쪽 편에 겹쳤다
        {
            my = bottomCheck;
            dy = 1.f;
        }

        // 최종 수정 무브
        if (mx < my)
        {
            correctionMove.x += dx * mx;
        }
        else
        {
            correctionMove.y += dy * my;
        }

        return correctionMove;
    }
    
}
