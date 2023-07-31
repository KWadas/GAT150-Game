#include "Framework/Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "SpazerWave.h"
#include "Weapon.h"
#include "Renderer/Renderer.h"
#include "Framework/Emitter.h"
#include "Renderer/ModelManager.h"
#include "Audio/AudioSystem.h"

void Enemy::Update(float dt)
{
    Actor::Update(dt);

    kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(m_transform.rotation);
    Player* player = m_scene->GetActor<Player>();
    if (player)
    {
        kiko::Vector2 direction = player->m_transform.position - m_transform.position;
        // turns towards player
        float turnAngle = kiko::vec2::SignedAngle(forward, direction.Normalized());

        m_transform.rotation += turnAngle * dt;
        // checks if player is in front
        if (std::fabs(turnAngle) < kiko::DegreesToRadians(30.0f))
        {
            // I see you!
        }

    }
    
    m_transform.position += forward * m_speed * kiko::g_time.getDeltaTime();
    m_transform.position.x = kiko::Wrap(m_transform.position.x, (float)kiko::g_renderer.GetWidth());
    m_transform.position.y = kiko::Wrap(m_transform.position.y, (float)kiko::g_renderer.GetHeight());


    m_fireTimer -= dt;
    if (m_fireTimer <= 0.0f)
    {
        kiko::Transform transform{ m_transform.position, m_transform.rotation, 1};
        std::unique_ptr<Weapon> weapon = std::make_unique<Weapon>(400.0f, transform, kiko::g_manager.Get("enemy_bullet.txt"));
        weapon->m_tag = "Enemy";
        m_scene->Add(std::move(weapon));
        m_fireTimer = m_fireRate;
    }
}

void Enemy::OnCollision(Actor* other)
{
    //Player* p = dynamic_cast<Player*>(other)
    
    if (other->m_tag == "Player") 
    {
        kiko::g_audioSystem.PlayOneShot("enemyDestroyed", false);
        m_game->AddPoints(m_points);
        m_destroyed = true;

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

        kiko::Transform transform{ m_transform.position, 0, 1 };
        auto emitter = std::make_unique<kiko::Emitter>(transform, data);
        emitter->m_lifespan = 0.1f;
        m_scene->Add(std::move(emitter));
    }
}
