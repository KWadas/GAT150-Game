#include "Player.h"
#include "Framework/Scene.h"
#include "Renderer/Renderer.h"
#include "Weapon.h"
#include "SpazerWave.h"
#include "Framework/Emitter.h"
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

        m_physicsComponent = GetComponent<kiko::PhysicsComponent>();
        auto collisionComponent = GetComponent<kiko::CollisionComponent>();
        if (collisionComponent)
        {
            auto renderComponent = GetComponent<kiko::RenderComponent>();
            if (renderComponent)
            {
                float scale = transform.scale;
                collisionComponent->m_radius = GetComponent<kiko::RenderComponent>()->GetRadius() * scale * 0.75f;
            }
        }

        return true;
    }

    void Player::Update(float dt)
    {
        Actor::Update(dt);

        // movement
        float rotate = 0;
        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_A)) rotate = -1;
        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_D)) rotate = 1;

        float thrust = 0;
        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_W)) thrust = 0.5;

        kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(transform.rotation);

        m_physicsComponent->ApplyForce(forward * m_speed * thrust);

        transform.position.x = kiko::Wrap(transform.position.x, (float)kiko::g_renderer.GetWidth());
        transform.position.y = kiko::Wrap(transform.position.y, (float)kiko::g_renderer.GetHeight());

        // fire weapon

        if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE) && !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_SPACE) && !destroyed)
        {
            auto weapon = INSTANTIATE(Weapon, "Rocket");
            weapon->transform = { transform.position + forward * 30, transform.rotation, 1 };
            weapon->tag = "Player";
            weapon->Initialize();
            m_scene->Add(std::move(weapon));
        }
    }

    void Player::LevelUp(int gameTimer) {
        if (gameTimer == m_levelUpTime) {
            m_levelUpTime += 10;
            m_power += 1;
        }
    }

    void Player::OnCollisionEnter(Actor* other)
    {
        if (other->tag == "Enemy")
        {
            kiko::g_audioSystem.PlayOneShot("destroyed", false);
            destroyed = true;
            kiko::EventManager::Instance().DispatchEvent("OnPlayerDead", 0);

            // create explosion
            kiko::EmitterData data;
            data.burst = true;
            data.burstCount = 100;
            data.spawnRate = 0;
            data.angle = 0;
            data.angleRange = kiko::Pi;
            data.lifetimeMin = 0.5f;
            data.lifetimeMin = 1.5f;
            data.speedMin = 50;
            data.speedMax = 250;
            data.damping = 0.5f;

            data.color = kiko::Color{ 1, 1, 1, 1 };

            kiko::Transform transform{ transform.position, 0, 1 };
            auto emitter = std::make_unique<kiko::Emitter>(transform, data);
            emitter->lifespan = 0.1f;
            m_scene->Add(std::move(emitter));
        }
    }

    void Player::Read(const json_t& value)
    {
        Actor::Read(value);

        READ_DATA(value, m_speed);
        READ_DATA(value, m_turnRate);
    }
}