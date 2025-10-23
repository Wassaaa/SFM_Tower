#include "KinematicsComponent.h"
#include "VisualComponent.h"
#include "CollisionComponent.h"
#include <cmath>

KinematicsComponent::KinematicsComponent(const KinematicsComponentData &data)
    : m_data(data)
    , velocity(data.velocity)
    , acceleration(data.acceleration)
    , angularVelocity(data.angularVelocity)
    , angularAcceleration(data.angularAcceleration)
    , scaleVelocity(data.scaleVelocity)
    , behavior(data.behavior)
    , orbitRadius(data.orbitRadius)
{}

void KinematicsComponent::update(float dt, sf::Transformable &visualTransform,
                                 sf::Transformable &collisionTransform)
{
    currentTime += dt;

    switch (behavior) {
    case KinematicsBehavior::Linear:
        updateLinear(dt, visualTransform);
        break;
    case KinematicsBehavior::Accelerate:
        updateAccelerate(dt, visualTransform);
        break;
    case KinematicsBehavior::Homing:
        updateHoming(dt, visualTransform);
        break;
    case KinematicsBehavior::Orbital:
        updateOrbital(dt, visualTransform);
        break;
    case KinematicsBehavior::Extending:
        updateExtending(dt, visualTransform);
        break;
    case KinematicsBehavior::Sweeping:
        updateSweeping(dt, visualTransform);
        break;
    case KinematicsBehavior::Pulsing:
        updatePulsing(dt, visualTransform);
        break;
    }

    // Sync collision with visual
    collisionTransform.setPosition(visualTransform.getPosition());
    collisionTransform.setRotation(visualTransform.getRotation());
    collisionTransform.setScale(visualTransform.getScale());
}

void KinematicsComponent::updateLinear(float dt, sf::Transformable &transform)
{
    transform.move(velocity * dt);
    transform.rotate(angularVelocity * dt);
}

void KinematicsComponent::updateAccelerate(float dt, sf::Transformable &transform)
{
    velocity += acceleration * dt;
    angularVelocity += angularAcceleration * dt;
    transform.move(velocity * dt);
    transform.rotate(angularVelocity * dt);
}

void KinematicsComponent::updateHoming(float dt, sf::Transformable &transform)
{
    if (targetPoint) {
        sf::Vector2f currentPos = transform.getPosition();
        sf::Vector2f direction = *targetPoint - currentPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length > 0.f) {
            direction /= length; // Normalize
            float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            velocity = direction * speed;
        }
    }
    transform.move(velocity * dt);
}

void KinematicsComponent::updateOrbital(float dt, sf::Transformable &transform)
{
    if (targetPoint) {
        orbitAngle += angularVelocity * dt;
        float radians = orbitAngle * 3.14159f / 180.f;

        sf::Vector2f offset(std::cos(radians) * orbitRadius, std::sin(radians) * orbitRadius);
        transform.setPosition(*targetPoint + offset);
        transform.setRotation(orbitAngle + 90.f); // Face direction of movement
    }
}

void KinematicsComponent::updateExtending(float dt, sf::Transformable &transform)
{
    sf::Vector2f currentScale = transform.getScale();
    transform.setScale(currentScale + scaleVelocity * dt);
}

void KinematicsComponent::updateSweeping(float dt, sf::Transformable &transform)
{
    transform.rotate(angularVelocity * dt);
}

void KinematicsComponent::updatePulsing(float dt, sf::Transformable &transform)
{
    // Pulse using sine wave
    float pulseScale = 1.f + 0.2f * std::sin(currentTime * 5.f);
    transform.setScale(sf::Vector2f(pulseScale, pulseScale));
}
