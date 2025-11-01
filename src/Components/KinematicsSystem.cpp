#include "KinematicsSystem.h"
#include "../Entity.h"
#include "KinematicsComponent.h"
#include "TransformComponent.h"
#include "DirectionComponent.h"
#include "../MathUtils.h"

void KinematicsSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        auto *kinematics = entity->getComponent<KinematicsComponent>();
        auto *transform = entity->getComponent<TransformComponent>();
        auto *dir = entity->getComponent<DirectionComponent>();

        if (!kinematics || !transform || !kinematics->isEnabled() || !transform->isEnabled())
            continue;

        kinematics->currentTime += dt;
        // Accelerate
        if (hasFlag(kinematics->behavior, KinematicsBehavior::Accelerate)) {
            kinematics->velocity += kinematics->acceleration * dt;
            kinematics->angularVelocity += kinematics->angularAcceleration * dt;
        }
        // Drag
        if (kinematics->drag > 0.f) {
            float dampingFactor = 1.f / (1.f + kinematics->drag * dt);
            kinematics->velocity *= dampingFactor;
        }
        // Homing
        if (hasFlag(kinematics->behavior, KinematicsBehavior::Homing)) {
            if (kinematics->targetPoint) {
                sf::Vector2f dir = *kinematics->targetPoint - transform->position;
                dir = VecNormalized(dir);

                // guide the velocity
                kinematics->velocity = dir * VecLength(kinematics->velocity);
            }
        }
        // Orbital
        if (hasFlag(kinematics->behavior, KinematicsBehavior::Orbital)) {
            if (kinematics->targetPoint) {
                kinematics->orbitAngle += kinematics->orbitAngularVelocity * dt;
                float radians = ToRadians(kinematics->orbitAngle);

                sf::Vector2f offset(std::cos(radians) * kinematics->orbitRadius,
                                    std::sin(radians) * kinematics->orbitRadius);
                transform->position = *kinematics->targetPoint + offset;
            }
        }
        // Actual movement based on Velocity
        else if (hasFlag(kinematics->behavior, KinematicsBehavior::Linear) ||
                 hasFlag(kinematics->behavior, KinematicsBehavior::Accelerate) ||
                 hasFlag(kinematics->behavior, KinematicsBehavior::Homing)) {
            transform->position += kinematics->velocity * dt;
        }
        // Rotatingp
        if (hasFlag(kinematics->behavior, KinematicsBehavior::Rotating)) {
            transform->rotation += kinematics->angularVelocity * dt;
        }
        // Pulsing
        if (hasFlag(kinematics->behavior, KinematicsBehavior::Pulsing)) {
            float pulseMultiplier =
                1.f + kinematics->pulseAmplitude *
                          std::sin(kinematics->currentTime * kinematics->pulseFrequency);
            transform->scale = kinematics->baseScale * pulseMultiplier;
        }
        // Face Target
        if (hasFlag(kinematics->behavior, KinematicsBehavior::FaceTarget) &&
            kinematics->targetPoint) {
            // Find the target angle
            sf::Vector2f diff = *kinematics->targetPoint - transform->position;
            float targetAngle = ToDegrees(std::atan2(diff.y, diff.x));

            // Find the shortest angle to turn
            float currentAngle = transform->rotation;
            float angleDiff = targetAngle - currentAngle;

            // Normalize angle diff to be between -180 and 180
            while (angleDiff > 180.f) angleDiff -= 360.f;
            while (angleDiff < -180.f) angleDiff += 360.f;

            // Apply turn speed (angularVelocity)
            float turnSpeed = kinematics->angularVelocity * dt;

            if (std::abs(angleDiff) < turnSpeed) {
                // Snap to target
                transform->rotation = targetAngle;
            }
            else if (angleDiff > 0.f) {
                // Turn counter-clockwise
                transform->rotation += turnSpeed;
            }
            else {
                // Turn clockwise
                transform->rotation -= turnSpeed;
            }
        }
        // Fallback Simple L/R facing
        else if (dir) {
            if (kinematics->velocity.x > EPSILON)
                dir->faceRight();
            else if (kinematics->velocity.x < -EPSILON)
                dir->faceLeft();
        }
    }
}
