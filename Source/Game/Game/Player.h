#pragma once
#include "Framework/Actor.h"
#include "Framework/Components/PhysicsComponent.h"

class Player : public kiko::Actor
{
public:
    Player(float speed, float turnRate, const kiko::Transform& transform) :
        Actor{ transform },
        m_speed{ speed },
        m_turnRate{ turnRate }
    {}

    bool Initialize() override;
    void Update(float dt) override;
    void OnCollision(Actor* other) override;

    void LevelUp(int gameTimer);
    void setLevelUpTime(int gameTimer) { m_levelUpTime = 10 + gameTimer; }

    int m_levelUpTime = 10;
    int m_power = 1;
private:
    float m_speed = 0;
    float m_turnRate = 0;
    float m_health = 10.0f;
    float m_laserTime = 0;

    kiko::PhysicsComponent* m_physicsComponent = nullptr;
};