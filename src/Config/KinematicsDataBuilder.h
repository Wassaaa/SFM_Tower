#pragma once
#include <SFML/Graphics.hpp>
#include "../Types.h"

struct KinematicsComponentData
{
    sf::Vector2f velocity{0.f, 0.f};
    sf::Vector2f acceleration{0.f, 0.f};
    float angularVelocity{0.f};
    float angularAcceleration{0.f};
    sf::Vector2f scaleVelocity{0.f, 0.f};
    KinematicsBehavior behavior{KinematicsBehavior::None};
    float orbitRadius{0.f};
    float orbitAngularVelocity{0.f};
    float pulseFrequency{5.f};
    float pulseAmplitude{0.2f};
    float drag{0.f};
    float mass{1.f};
    sf::Vector2f gravity{0.f, 2000.f};
    float restitution{0.5f};
    bool isStatic{false};
};

class KinematicsDataBuilder
{
public:
    KinematicsDataBuilder &setVelocity(const sf::Vector2f &velocity)
    {
        m_data.velocity = velocity;
        return *this;
    }
    KinematicsDataBuilder &setAcceleration(const sf::Vector2f &acceleration)
    {
        m_data.acceleration = acceleration;
        return *this;
    }
    KinematicsDataBuilder &setAngularVelocity(float angularVelocity)
    {
        m_data.angularVelocity = angularVelocity;
        return *this;
    }
    KinematicsDataBuilder &setAngularAcceleration(float angularAcceleration)
    {
        m_data.angularAcceleration = angularAcceleration;
        return *this;
    }
    KinematicsDataBuilder &setScaleVelocity(const sf::Vector2f &scaleVelocity)
    {
        m_data.scaleVelocity = scaleVelocity;
        return *this;
    }
    KinematicsDataBuilder &setBehavior(KinematicsBehavior behavior)
    {
        m_data.behavior = behavior;
        return *this;
    }
    KinematicsDataBuilder &setOrbitRadius(float orbitRadius)
    {
        m_data.orbitRadius = orbitRadius;
        return *this;
    }
    KinematicsDataBuilder &setOrbitAngularVelocity(float orbitAngularVelocity)
    {
        m_data.orbitAngularVelocity = orbitAngularVelocity;
        return *this;
    }
    KinematicsDataBuilder &setPulseFrequency(float pulseFrequency)
    {
        m_data.pulseFrequency = pulseFrequency;
        return *this;
    }
    KinematicsDataBuilder &setPulseAmplitude(float pulseAmplitude)
    {
        m_data.pulseAmplitude = pulseAmplitude;
        return *this;
    }
    KinematicsDataBuilder &setDrag(float drag)
    {
        m_data.drag = drag;
        return *this;
    }
    KinematicsDataBuilder &setMass(float mass)
    {
        m_data.mass = mass;
        return *this;
    }
    KinematicsDataBuilder &setRestition(float restitution)
    {
        m_data.restitution = restitution;
        return *this;
    }
    KinematicsDataBuilder &setGravity(sf::Vector2f gravity)
    {
        m_data.gravity = gravity;
        return *this;
    }
    KinematicsDataBuilder &setStatic(bool isStatic)
    {
        m_data.isStatic = isStatic;
        return *this;
    }
    KinematicsComponentData build() const { return m_data; }

private:
    KinematicsComponentData m_data;
};
