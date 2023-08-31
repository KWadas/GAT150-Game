#include "GroundEnemy.h"
#include "Player.h"

#include "Input/InputSystem.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"

namespace kiko
{
    CLASS_DEFINITION(GroundEnemy);

    bool GroundEnemy::Initialize()
    {
        Actor::Initialize();

        m_physicsComponent = GetComponent<kiko::PhysicsComponent>();

        return true;
    }

    void GroundEnemy::Update(float dt)
    {
        Actor::Update(dt);

        float thrust = 0.5;

        Player* player = m_scene->GetActor<Player>();
        if (player)
        {
            kiko::Vector2 direction = player->transform.position - transform.position;
            m_physicsComponent->ApplyForce(direction.Normalized() * speed * thrust);
        }
    }

    void GroundEnemy::OnCollisionEnter(Actor* other)
    {

    }

    void GroundEnemy::OnCollisionExit(Actor* other)
    {
        if (other->tag == "Ground") groundCount++;
    }

    void GroundEnemy::Read(const json_t& value)
    {
        Actor::Read(value);

        READ_DATA(value, speed);
    }
}