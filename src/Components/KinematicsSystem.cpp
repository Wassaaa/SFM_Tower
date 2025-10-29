#include "KinematicsSystem.h"
#include "../Entity.h"
#include "KinematicsComponent.h"
#include "TransformComponent.h"
#include "../MathUtils.h"

void KinematicsSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        auto *kinematics = entity->getComponent<KinematicsComponent>();
        auto *transform = entity->getComponent<TransformComponent>();

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
    }
}
