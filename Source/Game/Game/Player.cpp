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
    transform.rotation += rotate * m_turnRate * kiko::g_time.getDeltaTime();

    float thrust = 0;
    if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_W)) thrust = 0.5;

    if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_LSHIFT) && kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_LSHIFT) && !destroyed)
    {
        thrust = 1;
        kiko::EmitterData data;
        data.burst = true;
        data.burstCount = 100;
        data.spawnRate = 0;
        data.angle = 180;
        data.angleRange = kiko::Pi / 3;
        data.lifetimeMin = 0.5f;
        data.lifetimeMin = 1.0f;
        data.speedMin = 50;
        data.speedMax = 250;
        data.damping = 0.5f;

        data.color = kiko::Color{ 1, 1, 0, 1 };

        kiko::Transform transform{ transform.position, 0, 1 };
        auto emitter = std::make_unique<kiko::Emitter>(transform, data);
        emitter->lifespan = 0.1f;
        m_scene->Add(std::move(emitter));
    }

    kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(transform.rotation);

    m_physicsComponent->ApplyForce(forward * m_speed * thrust);

    transform.position.x = kiko::Wrap(transform.position.x, (float)kiko::g_renderer.GetWidth());
    transform.position.y = kiko::Wrap(transform.position.y, (float)kiko::g_renderer.GetHeight());

    // fire weapon

    if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE) && !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_SPACE) && !destroyed)
    {
        auto weapon = INSTANTIATE(Weapon, "Rocket");
        weapon->transform = { transform.position, transform.rotation + kiko::DegreesToRadians(10.0f), 1 };
        weapon->Initialize();
        m_scene->Add(std::move(weapon));
        //// create weapon
        //for (int i = 0; i < m_power; i++) {
        //    kiko::Transform transform1{ transform.position, transform.rotation + kiko::DegreesToRadians((5.0f * i) - (5 * (i + 1) * i))};
        //    std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>(800.0f, transform1);
        //    weapon->tag = "Player";

        //    std::unique_ptr<kiko::SpriteComponent> component = std::make_unique < kiko::SpriteComponent>();
        //    component->m_texture = GET_RESOURCE(kiko::Texture, "rocket.png", kiko::g_renderer);
        //    weapon->AddComponent(std::move(component));

        //    auto collisionComponent = std::make_unique<kiko::CircleCollsionComponent>();
        //    collisionComponent->m_radius = 30.0f;
        //    weapon->AddComponent(std::move(collisionComponent));

        //    weapon->Initialize();
        //    m_scene->Add(std::move(weapon));
        //}
    }

    //
    //if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_E) && kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_E) && !destroyed && m_laserTime <= 0.5f)
    //{
    //    m_laserTime += dt;
    //    kiko::Transform transform1{ transform.position, transform.rotation, 1};
    //    std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>(400.0f, transform1);
    //    weapon->transform.scale *= 2.0f;
    //    weapon->tag = "Player";

    //    std::unique_ptr<kiko::SpriteComponent> component = std::make_unique < kiko::SpriteComponent>();
    //    component->m_texture = GET_RESOURCE(kiko::Texture, "rocket.png", kiko::g_renderer);
    //    weapon->AddComponent(std::move(component));

    //    auto collisionComponent = std::make_unique<kiko::CircleCollsionComponent>();
    //    collisionComponent->m_radius = 30.0f;
    //    weapon->AddComponent(std::move(collisionComponent));

    //    weapon->Initialize();
    //    m_scene->Add(std::move(weapon));
    //}
    //
    //if(!kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_E) && !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_E) && m_laserTime > 0.0f)
    //{
    //    m_laserTime -= dt;
    //    if (m_laserTime < 0.0f) {
    //        m_laserTime = 0.0f;
    //    }
    //}

    //if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_T)) kiko::g_time.SetTimeScale(0.5f);
    //else kiko::g_time.SetTimeScale(1.0f);
}

void Player::LevelUp(int gameTimer) {
    if (gameTimer == m_levelUpTime) {
        m_levelUpTime += 10;
        m_power += 1;
    }
}

void Player::OnCollision(Actor* other)
{
    if (other->tag == "Enemy") 
    {
        kiko::g_audioSystem.PlayOneShot("destroyed", false);
        m_game->SetLives(m_game->GetLives() - 1);
        destroyed = true;

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

        dynamic_cast<SpazerWave*>(m_game)->SetState(SpazerWave::eState::PlayerDeadStart);
    }
}
