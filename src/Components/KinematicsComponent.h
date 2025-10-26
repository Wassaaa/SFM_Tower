#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "../Config/GameConfig.h"

class ComponentContainer;

class KinematicsComponent : public Component
{
public:
    KinematicsComponent(const KinematicsComponentData &data);
    ~KinematicsComponent() = default;

    void update(float dt, ComponentContainer &container);

    virtual const char *getName() const override { return "KinematicsComponent"; }

    // Getters
    sf::Vector2f getVelocity() const { return velocity; }
    sf::Vector2f getAcceleration() const { return acceleration; }
    float getAngularVelocity() const { return angularVelocity; }
    KinematicsBehavior getBehavior() const { return behavior; }

    // Setters
    void setVelocity(const sf::Vector2f &vel) { velocity = vel; }
    void setAcceleration(const sf::Vector2f &accel) { acceleration = accel; }
    void setAngularVelocity(float angVel) { angularVelocity = angVel; }
    void setTargetPoint(sf::Vector2f *target) { targetPoint = target; }

private:
    void updateHoming(float dt, sf::Transformable &transform);
    void updateOrbital(float dt, sf::Transformable &transform);
    void updateExtending(float dt, sf::Transformable &transform);
    void updatePulsing(float dt, sf::Transformable &transform);

    const KinematicsComponentData &m_data;

    // Linear motion
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float speed;

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
    float orbitAngularVelocity{0.f};
    float pulseFrequency{5.f};
    float pulseAmplitude{0.2f};
    float currentTime{0.f};

    // Physics properties
    float drag{0.f};
    float mass{1.f};
};
