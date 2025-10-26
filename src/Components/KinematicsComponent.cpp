#include "KinematicsComponent.h"
#include "VisualComponent.h"
#include "CollisionComponent.h"
#include "ComponentContainer.h"
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
    , drag(data.drag)
    , mass(data.mass)
{}

void KinematicsComponent::update(float dt, ComponentContainer &container)
{
    currentTime += dt;

    // Get components if they exist
    auto *visual = container.getComponent<VisualComponent>();
    auto *collision = container.getComponent<CollisionComponent>();

    // If neither exists, nothing to do
    if (!visual && !collision) {
        return;
    }

    // Use visual as primary transform, collision as fallback
    sf::Transformable *primary = visual ? static_cast<sf::Transformable *>(visual)
                                        : static_cast<sf::Transformable *>(collision);

    // Apply behaviors - can have multiple active at once!
    if (hasFlag(behavior, KinematicsBehavior::Accelerate)) {
        velocity += acceleration * dt;
        speed = VecLength(velocity);
        angularVelocity += angularAcceleration * dt;
    }

    // Apply drag (linear damping)
    if (drag > 0.f) {
        float dampingFactor = 1.f / (1.f + drag * dt);
        velocity *= dampingFactor;
    }

    if (hasFlag(behavior, KinematicsBehavior::Homing)) {
        updateHoming(dt, *primary);
    }

    if (hasFlag(behavior, KinematicsBehavior::Orbital)) {
        updateOrbital(dt, *primary);
    }
    else if (hasFlag(behavior, KinematicsBehavior::Linear) ||
             hasFlag(behavior, KinematicsBehavior::Accelerate)) {
        // Only move linearly if not orbiting (orbital sets position directly)
        primary->move(velocity * dt);
    }

    if (hasFlag(behavior, KinematicsBehavior::Rotating)) {
        primary->rotate(angularVelocity * dt);
    }

    if (hasFlag(behavior, KinematicsBehavior::Extending)) {
        updateExtending(dt, *primary);
    }

    if (hasFlag(behavior, KinematicsBehavior::Pulsing)) {
        updatePulsing(dt, *primary);
    }

    // Sync collision with visual if both exist
    if (visual && collision) {
        collision->setPosition(visual->getPosition());
        collision->setRotation(visual->getRotation());
        collision->setScale(visual->getScale());
    }
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
