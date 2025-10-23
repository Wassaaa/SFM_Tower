#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "../Config/GameConfig.h"

class KinematicsComponent : public Component
{
public:
    KinematicsComponent(const KinematicsComponentData &data);
    ~KinematicsComponent() = default;

    void update(float dt, sf::Transformable &visualTransform,
                sf::Transformable &collisionTransform);

    virtual const char *getName() const override
    {
        return "KinematicsComponent";
    }

    // Getters
    sf::Vector2f getVelocity() const
    {
        return velocity;
    }
    sf::Vector2f getAcceleration() const
    {
        return acceleration;
    }
    float getAngularVelocity() const
    {
        return angularVelocity;
    }
    KinematicsBehavior getBehavior() const
    {
        return behavior;
    }

    // Setters
    void setVelocity(const sf::Vector2f &vel)
    {
        velocity = vel;
    }
    void setAcceleration(const sf::Vector2f &accel)
    {
        acceleration = accel;
    }
    void setAngularVelocity(float angVel)
    {
        angularVelocity = angVel;
    }
    void setTargetPoint(sf::Vector2f *target)
    {
        targetPoint = target;
    }

private:
    void updateLinear(float dt, sf::Transformable &transform);
    void updateAccelerate(float dt, sf::Transformable &transform);
    void updateHoming(float dt, sf::Transformable &transform);
    void updateOrbital(float dt, sf::Transformable &transform);
    void updateExtending(float dt, sf::Transformable &transform);
    void updateSweeping(float dt, sf::Transformable &transform);
    void updatePulsing(float dt, sf::Transformable &transform);

    const KinematicsComponentData &m_data;

    // Linear motion
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

    // Rotational motion
    float angularVelocity;
    float angularAcceleration;

    // Scale animation
    sf::Vector2f scaleVelocity;

    // Behavior
    KinematicsBehavior behavior;

    // Behavior-specific data
    sf::Vector2f *targetPoint{nullptr}; // For homing/orbital
    float orbitRadius{0.f};
    float orbitAngle{0.f};
    float currentTime{0.f};
};
