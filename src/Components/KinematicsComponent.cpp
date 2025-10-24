#include "KinematicsComponent.h"
#include "VisualComponent.h"
#include "CollisionComponent.h"
#include "../MathUtils.h"
#include <cmath>

KinematicsComponent::KinematicsComponent(const KinematicsComponentData &data)
    : m_data(data)
    , velocity(data.velocity)
    , speed(std::sqrt(data.velocity.x * data.velocity.x + data.velocity.y * data.velocity.y))
    , acceleration(data.acceleration)
    , angularVelocity(data.angularVelocity)
    , angularAcceleration(data.angularAcceleration)
    , scaleVelocity(data.scaleVelocity)
    , behavior(data.behavior)
    , orbitRadius(data.orbitRadius)
    , orbitAngularVelocity(data.orbitAngularVelocity)
    , pulseFrequency(data.pulseFrequency)
    , pulseAmplitude(data.pulseAmplitude)
{}

void KinematicsComponent::update(float dt, sf::Transformable &visualTransform,
                                 sf::Transformable &collisionTransform)
{
    currentTime += dt;

    // Apply behaviors - can have multiple active at once!
    if (hasFlag(behavior, KinematicsBehavior::Accelerate)) {
        velocity += acceleration * dt;
        speed = VecLength(velocity);
        angularVelocity += angularAcceleration * dt;
    }

    if (hasFlag(behavior, KinematicsBehavior::Homing)) {
        updateHoming(dt, visualTransform);
    }

    if (hasFlag(behavior, KinematicsBehavior::Orbital)) {
        updateOrbital(dt, visualTransform);
    }
    else if (hasFlag(behavior, KinematicsBehavior::Linear) ||
             hasFlag(behavior, KinematicsBehavior::Accelerate)) {
        // Only move linearly if not orbiting (orbital sets position directly)
        visualTransform.move(velocity * dt);
    }

    if (hasFlag(behavior, KinematicsBehavior::Rotating)) {
        visualTransform.rotate(angularVelocity * dt);
    }

    if (hasFlag(behavior, KinematicsBehavior::Extending)) {
        updateExtending(dt, visualTransform);
    }

    if (hasFlag(behavior, KinematicsBehavior::Pulsing)) {
        updatePulsing(dt, visualTransform);
    }

    // Sync collision with visual
    collisionTransform.setPosition(visualTransform.getPosition());
    collisionTransform.setRotation(visualTransform.getRotation());
    collisionTransform.setScale(visualTransform.getScale());
}

void KinematicsComponent::updateHoming(float dt, sf::Transformable &transform)
{
    if (targetPoint) {
        sf::Vector2f currentPos = transform.getPosition();
        sf::Vector2f direction = *targetPoint - currentPos;
        direction = VecNormalized(direction);

        velocity = direction * speed;
    }
    transform.move(velocity * dt);
}

void KinematicsComponent::updateOrbital(float dt, sf::Transformable &transform)
{
    if (targetPoint) {
        orbitAngle += orbitAngularVelocity * dt;
        float radians = ToRadians(orbitAngle);

        sf::Vector2f offset(std::cos(radians) * orbitRadius, std::sin(radians) * orbitRadius);
        transform.setPosition(*targetPoint + offset);
    }
}

void KinematicsComponent::updateExtending(float dt, sf::Transformable &transform)
{
    sf::Vector2f currentScale = transform.getScale();
    transform.setScale(currentScale + scaleVelocity * dt);
}

void KinematicsComponent::updatePulsing(float dt, sf::Transformable &transform)
{
    // Pulse using sine wave with configurable frequency and amplitude
    float pulseScale = 1.f + pulseAmplitude * std::sin(currentTime * pulseFrequency);
    transform.setScale(sf::Vector2f(pulseScale, pulseScale));
}
