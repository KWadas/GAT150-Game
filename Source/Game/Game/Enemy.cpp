#include "Framework/Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "SpazerWave.h"
#include "Weapon.h"
#include "Renderer/Renderer.h"
#include "Framework/Framework.h"
#include "Audio/AudioSystem.h"

bool Enemy::Initialize()
{
    Actor::Initialize();

    auto collisionComponent = GetComponent<kiko::CollisionComponent>();
    if (collisionComponent)
    {
        auto renderComponent = GetComponent<kiko::RenderComponent>();
        if (renderComponent)
        {
            float scale = transform.scale;
            collisionComponent->m_radius = GetComponent<kiko::RenderComponent>()->GetRadius() * scale;
        }
    }

    return true;
}

void Enemy::Update(float dt)
{
    Actor::Update(dt);

    kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(transform.rotation);
    Player* player = m_scene->GetActor<Player>();
    if (player)
    {
        kiko::Vector2 direction = player->transform.position - transform.position;
        // turns towards player
        float turnAngle = kiko::vec2::SignedAngle(forward, direction.Normalized());

        transform.rotation += turnAngle * dt;
        // checks if player is in front
        if (std::fabs(turnAngle) < kiko::DegreesToRadians(30.0f))
        {
            // I see you!
        }

    }
    
    transform.position += forward * m_speed * kiko::g_time.getDeltaTime();
    transform.position.x = kiko::Wrap(transform.position.x, (float)kiko::g_renderer.GetWidth());
    transform.position.y = kiko::Wrap(transform.position.y, (float)kiko::g_renderer.GetHeight());


    /*m_fireTimer -= dt;
    if (m_fireTimer <= 0.0f)
    {
        kiko::Transform m_transform{ transform.position, transform.rotation, 1};
        std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>(400.0f, transform);
        weapon->tag = "Enemy";
        m_scene->Add(std::move(weapon));
        m_fireTimer = m_fireRate;
    }*/
}

void Enemy::OnCollision(Actor* other)
{
    
    if (other->tag == "Player") 
    {
        kiko::g_audioSystem.PlayOneShot("enemyDestroyed", false);
        m_game->AddPoints(m_points);
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

        data.color = kiko::Color{ 1, 1, 0, 1 };

        kiko::Transform transform{ this->transform.position, 0, 1 };
        auto emitter = std::make_unique<kiko::Emitter>(transform, data);
        emitter->lifespan = 0.1f;
        m_scene->Add(std::move(emitter));
    }
}
