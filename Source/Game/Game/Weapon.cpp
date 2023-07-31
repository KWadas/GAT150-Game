#include "Weapon.h"
#include "Framework/Scene.h"
#include "Framework/Emitter.h"

void Weapon::Update(float dt)
{
    Actor::Update(dt);
    
    kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(m_transform.rotation);
    m_transform.position += forward * m_speed * kiko::g_time.getDeltaTime();
    m_transform.position.x = kiko::Wrap(m_transform.position.x, (float)kiko::g_renderer.GetWidth());
    m_transform.position.y = kiko::Wrap(m_transform.position.y, (float)kiko::g_renderer.GetHeight());
}

void Weapon::OnCollision(Actor* other)
{
    if (other->m_tag != m_tag && !m_destroyed)
    {   
        m_lifespan = 0.0f;
        m_destroyed = true;

        kiko::EmitterData data;
        data.burst = true;
        data.burstCount = 50;
        data.spawnRate = 0;
        data.angle = 0;
        data.angleRange = kiko::Pi;
        data.lifetimeMin = 0.5f;
        data.lifetimeMin = 1.5f;
        data.speedMin = 50;
        data.speedMax = 250;
        data.damping = 0.75f;

        data.color = kiko::Color{ 0.75, 0, 1, 1 };

        kiko::Transform transform{ m_transform.position, 0, 1 };
        auto emitter = std::make_unique<kiko::Emitter>(transform, data);
        emitter->m_lifespan = 0.1f;
        m_scene->Add(std::move(emitter));
    }

}
