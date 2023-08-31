#include "Player.h"

#include "Audio/AudioSystem.h"
#include "Input/InputSystem.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"

namespace kiko
{
    CLASS_DEFINITION(Player);

    bool Player::Initialize()
    {
        Actor::Initialize();

        m_physicsComponent = GetComponent<PhysicsComponent>();
        m_spriteAnimComponent = GetComponent<SpriteAnimComponent>();

        return true;
    }

    void Player::Update(float dt)
    {
        Actor::Update(dt);

        if (s != PlayerState::ATTACK)
        {
            attackFrame = 1;
        }

        bool onGround = (groundCount > 0);
        vec2 velocity = m_physicsComponent->m_velocity;

        // movement
        float dir = 0;
        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_A)) dir = -1;
        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_D)) dir = 1;

        if (dir)
        {
            velocity.x += speed * dir * ((onGround) ? 1 : 0.25f) * dt;
            velocity.x = Clamp(velocity.x, -maxSpeed, maxSpeed);
            m_physicsComponent->SetVelocity(velocity);
        }


        switch (s)
        {
        case PlayerState::IDLE:
            {
                m_spriteAnimComponent->SetSequence("idle");

                if (onGround && std::fabs(velocity.x) > 0.2f)
                {
                    s = PlayerState::RUN;
                }
                if (onGround &&
                    kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE) &&
                    !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_SPACE))
                {
                    kiko::vec2 up = kiko::vec2{ 0, -1 };
                    m_physicsComponent->SetVelocity(velocity + (up * jump));
                    s = PlayerState::JUMP;
                }
                if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_E) &&
                    !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_E))
                {
                    s = PlayerState::ATTACK;
                }
            }
            break;
        case PlayerState::RUN:
            {
                m_spriteAnimComponent->flipH = (dir < 0);
                m_spriteAnimComponent->SetSequence("run");

                if (onGround && std::fabs(velocity.x) <= 0.2f)
                {
                    s = PlayerState::IDLE;
                }
                if (onGround &&
                    kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE) &&
                    !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_SPACE))
                {
                    kiko::vec2 up = kiko::vec2{ 0, -1 };
                    m_physicsComponent->SetVelocity(velocity + (up * jump));
                    s = PlayerState::JUMP;
                }
                if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_E))
                {
                    s = PlayerState::ATTACK;
                }
            }
            break;
        case PlayerState::JUMP:
            {
                m_physicsComponent->SetGravityScale((velocity.y > 0) ? 3 : 2);
                if (!onGround)
                {
                    if (velocity.y > 0)
                    {
                        m_spriteAnimComponent->SetSequence("fall");
                    }
                }

                if (onGround && std::fabs(velocity.x) > 0.2f)
                {
                    s = PlayerState::RUN;
                }
                if (onGround && std::fabs(velocity.x) <= 0.2f)
                {
                    s = PlayerState::IDLE;
                }
            }
            break;
        case PlayerState::ATTACK:
            {
                m_physicsComponent->SetVelocity(0);

                attackFrame -= dt;

                if (attackFrame >= 0.7) {
                    m_spriteAnimComponent->SetSequence("attack");
                }
                else {
                    s = PlayerState::IDLE;
                }
            }
        }
    }

    void Player::OnCollisionEnter(Actor* other)
    {
        if (other->tag == "Enemy")
        {
            if (m_spriteAnimComponent->textureName == "attack") {
                EVENT_DISPATCH("OnAddPoints", 100);
                other->destroyed = true;
            }
            else {
                destroyed = true;
                EVENT_DISPATCH("OnPlayerDead", 0);
            }
        }
        if (other->tag == "Coin")
        {
            EVENT_DISPATCH("OnAddPoints", 100);
            kiko::g_audioSystem.PlayOneShot("coin", false);
            other->destroyed = true;
        }

        if (other->tag == "Ground") groundCount++;
    }

    void Player::OnCollisionExit(Actor* other)
    {
        if (other->tag == "Ground") groundCount--;
    }

    void Player::Read(const json_t& value)
    {
        Actor::Read(value);

        READ_DATA(value, speed);
        READ_DATA(value, maxSpeed);
        READ_DATA(value, jump);
    }
}