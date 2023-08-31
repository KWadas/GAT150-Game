#include "Enemy.h"
#include "Player.h"

#include "Input/InputSystem.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"

namespace kiko
{
    CLASS_DEFINITION(Enemy);

    bool Enemy::Initialize()
    {
        Actor::Initialize();

        m_physicsComponent = GetComponent<kiko::PhysicsComponent>();

        return true;
    }

    void Enemy::Update(float dt)
    {
        Actor::Update(dt);

        kiko::vec2 forward = kiko::vec2{ 0 , -1 }.Rotate(transform.rotation);
        Player* player = m_scene->GetActor<Player>();
        if (player)
        {
            kiko::Vector2 direction = player->transform.position - transform.position;
            m_physicsComponent->ApplyForce(direction.Normalized() * speed);
        }


        //transform.position += forward * m_speed * MEN::g_time.GetDeltaTime();

        transform.position.x = kiko::Wrap(transform.position.x, (float)kiko::g_renderer.GetWidth());
        transform.position.y = kiko::Wrap(transform.position.y, (float)kiko::g_renderer.GetHeight());

    }

    void Enemy::OnCollisionEnter(Actor* other)
    {
        
    }

    void Enemy::OnCollisionExit(Actor* other)
    {
        if (other->tag == "Ground") groundCount++;
    }

    void Enemy::Read(const json_t& value)
    {
        Actor::Read(value);

        READ_DATA(value, speed);
    }
}