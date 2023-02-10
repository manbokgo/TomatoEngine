#pragma once
#include <tomato/Components/Script.h>

namespace tomato
{
    class AnimatorComponent;
    class Collider2DComponent;
    class Rigidbody2DComponent;
    class AnimClip;
    enum class eJumpState
    {
        Grounded,
        PrepareToJump,
        Jumping,
        InFlight,
        Landed
    };

    class CPlayerScript : public Script
    {
    private:
        float m_fAccTime = 0.f;
        float m_fSpeed = 300.f;
        float m_JumpStartSpeed = 500.f;

        Entity* m_Head;
        Entity* m_Hand;

        Rigidbody2DComponent* m_RigidBody;
        Collider2DComponent*  m_Collider;
        AnimatorComponent*    m_Animator;

        Vec2 m_Move;
        Vec2 m_PrevMove;

        bool       m_bStopJump;
        eJumpState m_JumpState;
        bool       m_bJump;
        bool       m_Flip;


    public:
        void OnStart() override;
        void MoveBone(const int& curAnimIndex, const Ref<AnimClip>& animationClip2D, Entity* bone, string boneName);
        void PerformMovement(Vec2 move, bool bYMovement);
        void OnUpdate() override;
        Vec3 PosSolver(Collider2DComponent* _pOther);

        // TODO 에디터로
        void SetHead(Entity* head) { m_Head = head; }
        void SetHand(Entity* hand) { m_Hand = hand; }

        const char* GetName() const override { return u8"Player Script"; };

        CPlayerScript();
        ~CPlayerScript() override;
    };
}
