#pragma once
#include "Framework/Actor.h"

class Enemy : public kiko::Actor
{
public:
    Enemy(float speed, float fireRate, float turnRate, const kiko::Transform& transform, int points) :
        Actor{ transform },
        m_speed{ speed },
        m_turnRate{ turnRate },
        m_fireRate{ fireRate },
        m_points{ points }
    {
        m_fireTimer = m_fireRate;
    }

    void Update(float dt) override;
    void OnCollision(Actor* other) override;

private:
    float m_speed = 0;
    float m_turnRate = 0;
    int m_points = 0;
    float m_fireRate = 0;
    float m_fireTimer = 0;
};