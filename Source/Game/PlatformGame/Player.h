#pragma once
#include "Framework/Actor.h"
#include "Framework/Components/PhysicsComponent.h"
#include "Framework/Components/SpriteAnimComponent.h"

namespace kiko
{
    enum PlayerState
    {
        IDLE,
        RUN,
        JUMP,
        ATTACK
    };
    
    class Player : public kiko::Actor
    {
    public:
        CLASS_DECLARATION(Player);

        bool Initialize() override;
        void Update(float dt) override;

        void OnCollisionEnter(Actor* other) override;
        void OnCollisionExit(Actor* other) override;

        PlayerState s = PlayerState::IDLE;
    private:
        float speed = 0;
        float maxSpeed = 0;
        float jump = 0;
        int groundCount = 0;
        float attackFrame = 1;

        class PhysicsComponent* m_physicsComponent = nullptr;
        class SpriteAnimComponent* m_spriteAnimComponent = nullptr;
    };
}