#pragma once
#include <SFML/System/Vector2.hpp>
#include "Component.h"
#include "../Config/GameConfig.h"

class KinematicsComponent : public Component
{
public:
    // config based runtime state
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float angularVelocity;
    float angularAcceleration;
    KinematicsBehavior behavior;
    float orbitRadius;
    float orbitAngularVelocity;
    float pulseFrequency;
    float pulseAmplitude;
    float drag;
    float mass;
    bool isStatic;

    // calculated runtime state
    sf::Vector2f *targetPoint{nullptr};
    float orbitAngle{0.f};
    float currentTime{0.f};
    sf::Vector2f baseScale{1.f, 1.f};

    KinematicsComponent(const KinematicsComponentData &data)
        : velocity(data.velocity)
        , acceleration(data.acceleration)
        , angularVelocity(data.angularVelocity)
        , angularAcceleration(data.angularAcceleration)
        , behavior(data.behavior)
        , orbitRadius(data.orbitRadius)
        , orbitAngularVelocity(data.orbitAngularVelocity)
        , pulseFrequency(data.pulseFrequency)
        , pulseAmplitude(data.pulseAmplitude)
        , drag(data.drag)
        , mass(data.mass)
        , isStatic(data.isStatic)
    {}

    virtual const char *getName() const override { return "KinematicsComponent"; }
};
