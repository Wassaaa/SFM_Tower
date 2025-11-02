#include "KinematicsSystem.h"
#include "../Entity.h"
#include "KinematicsComponent.h"
#include "TransformComponent.h"
#include "DirectionComponent.h"
#include "OwnerComponent.h"
#include "../MathUtils.h"

void KinematicsSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        auto *kinematics = entity->getComponent<KinematicsComponent>();
        auto *transform = entity->getComponent<TransformComponent>();
        if (!kinematics || !transform)
            continue;

        // We get DirectionComponent here once, even if only handleRotation uses it
        auto *dir = entity->getComponent<DirectionComponent>();

        kinematics->currentTime += dt;

        kinematics->lastPosition = transform->position;
        kinematics->teleported = false;

        // Accelerate, Drag, Homing
        handleVelocity(dt, kinematics);
        // 1. Attached, 2. Orbital, 3. Velocity movement Linear|Accelerate|Homing
        handlePosition(dt, kinematics, transform, entity.get());
        // Handle Facing and Flipping logic
        handleFacingDirection(kinematics, transform, dir, entity.get());
        // 1. FaceTarget, 2. Attached (match owner rotation), 3. Rotating based on AngularVelocity
        handleRotation(dt, kinematics, transform, entity.get());
        // Pulsing
        handleScaling(dt, kinematics, transform);

        if (kinematics->teleported && dt > EPSILON) {
            kinematics->velocity = (transform->position - kinematics->lastPosition) / dt;
        }
    }
}

void KinematicsSystem::handleVelocity(float dt, KinematicsComponent *kinematics)
{
    // Accelerate
    if (hasFlag(kinematics->behavior, KinematicsBehavior::Accelerate)) {
        kinematics->velocity += kinematics->acceleration * dt;
        kinematics->angularVelocity += kinematics->angularAcceleration * dt;
    }
    // Drag
    if (kinematics->drag > 0.f) {
        float dampingFactor = 1.f / (1.f + kinematics->drag * dt);
        kinematics->velocity *= dampingFactor;
        if (VecLengthSquared(kinematics->velocity) < 1.f) {
            kinematics->velocity = {0.f, 0.f};
        }
    }
    // Homing
    if (hasFlag(kinematics->behavior, KinematicsBehavior::Homing)) {
        if (kinematics->targetPoint) {
            sf::Vector2f dir = *kinematics->targetPoint - kinematics->targetCache;
            dir = VecNormalized(dir);
            kinematics->velocity = dir * VecLength(kinematics->velocity);
        }
    }
}

void KinematicsSystem::handlePosition(float dt, KinematicsComponent *kinematics,
                                      TransformComponent *transform, Entity *entity)
{
    // Priority 1: Attached (Overrides all other movement)
    if (hasFlag(kinematics->behavior, KinematicsBehavior::Attached)) {
        if (kinematics->targetPoint) {
            transform->position = *kinematics->targetPoint;
            kinematics->teleported = true;
        }
    }
    // Priority 2: Orbital (Overrides velocity movement)
    else if (hasFlag(kinematics->behavior, KinematicsBehavior::Orbital)) {
        if (kinematics->targetPoint) {
            kinematics->orbitAngle += kinematics->orbitAngularVelocity * dt;
            float radians = ToRadians(kinematics->orbitAngle);

            sf::Vector2f offset(std::cos(radians) * kinematics->orbitRadius,
                                std::sin(radians) * kinematics->orbitRadius);
            transform->position = *kinematics->targetPoint + offset;
            kinematics->teleported = true;
        }
    }
    // Priority 3: Standard velocity movement
    if (!kinematics->teleported && (hasFlag(kinematics->behavior, KinematicsBehavior::Linear) ||
                                    hasFlag(kinematics->behavior, KinematicsBehavior::Accelerate) ||
                                    hasFlag(kinematics->behavior, KinematicsBehavior::Homing))) {
        transform->position += kinematics->velocity * dt;
    }
}

void KinematicsSystem::handleFacingDirection(KinematicsComponent *kinematics,
                                             TransformComponent *transform, DirectionComponent *dir,
                                             Entity *entity)
{
    if (!dir) {
        return;
    }

    // Priority 1: Match Owner's Facing (if Attached)
    if (hasFlag(kinematics->behavior, KinematicsBehavior::Attached)) {
        if (auto *owner = entity->getComponent<OwnerComponent>()) {
            if (auto *ownerDir = owner->owner->getComponent<DirectionComponent>()) {
                dir->setFacing(ownerDir->getFacing());
            }
        }
    }
    // Priority 2: Face Velocity
    else if (!hasFlag(kinematics->behavior, KinematicsBehavior::FaceTarget)) {
        if (kinematics->velocity.x > EPSILON)
            dir->faceRight();
        else if (kinematics->velocity.x < -EPSILON)
            dir->faceLeft();
    }

    float absScaleX = std::abs(kinematics->baseScale.x);
    if (dir->isFacingLeft()) {
        transform->scale.x = -absScaleX;
    }
    else {
        transform->scale.x = absScaleX;
    }
    transform->scale.y = kinematics->baseScale.y;
}

void KinematicsSystem::handleRotation(float dt, KinematicsComponent *kinematics,
                                      TransformComponent *transform, Entity *entity)
{
    // Priority 1: Face Target (Overrides other rotation)
    if (hasFlag(kinematics->behavior, KinematicsBehavior::FaceTarget) && kinematics->targetPoint) {
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
            transform->rotation = targetAngle;
        }
        else if (angleDiff > 0.f) {
            transform->rotation += turnSpeed;
        }
        else {
            transform->rotation -= turnSpeed;
        }
    }
    // Priority 2: Match Owner's Rotation (if Attached)
    else if (hasFlag(kinematics->behavior, KinematicsBehavior::Attached)) {
        if (auto *owner = entity->getComponent<OwnerComponent>()) {
            if (auto *ownerTransform = owner->owner->getComponent<TransformComponent>()) {
                transform->rotation = ownerTransform->rotation;
            }
        }
    }
    // Priority 3: Simple Rotating
    else if (hasFlag(kinematics->behavior, KinematicsBehavior::Rotating)) {
        transform->rotation += kinematics->angularVelocity * dt;
    }
}

void KinematicsSystem::handleScaling(float dt, KinematicsComponent *kinematics,
                                     TransformComponent *transform)
{
    if (hasFlag(kinematics->behavior, KinematicsBehavior::Pulsing)) {
        float pulseMultiplier =
            1.f + kinematics->pulseAmplitude *
                      std::sin(kinematics->currentTime * kinematics->pulseFrequency);
        transform->scale *= pulseMultiplier;
    }
}
