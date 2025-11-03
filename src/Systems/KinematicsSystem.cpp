#include "KinematicsSystem.h"
#include "../Entity.h"
#include "../Components/KinematicsComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/OwnerComponent.h"
#include "../MathUtils.h"
#include "../Types.h"

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

        // Gravity, Accelerate, Drag, Homing
        handleVelocity(dt, kinematics, transform);
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

void KinematicsSystem::handleVelocity(float dt, KinematicsComponent *kin, TransformComponent *trans)
{
    // Gravity
    if (kin->mass != 0.f && !std::isinf(kin->mass)) {
        if (kin->isGrounded) {
            // If grounded, stop any downward velocity
            if (kin->velocity.y > 0) {
                kin->velocity.y = 0;
            }
        }
        else {
            // If airborne, apply gravity
            kin->acceleration += kin->gravity;
        }
    }

    // Accelerate
    if (hasFlag(kin->behavior, KinematicsBehavior::Accelerate)) {
        kin->velocity += kin->acceleration * dt;
        kin->angularVelocity += kin->angularAcceleration * dt;
    }
    // Drag
    if (kin->drag > 0.f) {
        float dampingFactor = 1.f / (1.f + kin->drag * dt);
        // only apply drag horizontally
        kin->velocity.x *= dampingFactor;
        if (VecLengthSquared(kin->velocity) < 1.f) {
            kin->velocity = {0.f, 0.f};
        }
    }
    // Homing
    if (hasFlag(kin->behavior, KinematicsBehavior::Homing)) {
        if (kin->aimPoint) {
            sf::Vector2f dir = *kin->aimPoint - trans->position;
            dir = VecNormalized(dir);
            kin->velocity = dir * VecLength(kin->velocity);
        }
    }
}

void KinematicsSystem::handlePosition(float dt, KinematicsComponent *kin, TransformComponent *trans,
                                      Entity *entity)
{
    // Priority 1: Attached (Overrides all other movement)
    if (hasFlag(kin->behavior, KinematicsBehavior::Attached)) {
        if (kin->targetPoint) {
            trans->position = *kin->targetPoint;
            kin->teleported = true;
        }
    }
    // Priority 2: Orbital (Overrides velocity movement)
    else if (hasFlag(kin->behavior, KinematicsBehavior::Orbital)) {
        if (kin->targetPoint) {
            kin->orbitAngle += kin->orbitAngularVelocity * dt;
            float radians = ToRadians(kin->orbitAngle);

            sf::Vector2f offset(std::cos(radians) * kin->orbitRadius,
                                std::sin(radians) * kin->orbitRadius);
            trans->position = *kin->targetPoint + offset;
            kin->teleported = true;
        }
    }
    // Priority 3: Standard velocity movement
    if (!kin->teleported && (hasFlag(kin->behavior, KinematicsBehavior::Linear) ||
                             hasFlag(kin->behavior, KinematicsBehavior::Accelerate) ||
                             hasFlag(kin->behavior, KinematicsBehavior::Homing))) {
        trans->position += kin->velocity * dt;
    }
}

void KinematicsSystem::handleFacingDirection(KinematicsComponent *kin, TransformComponent *trans,
                                             DirectionComponent *dir, Entity *entity)
{
    if (!dir) {
        return;
    }
    // Priortiy 1, FaceTarget override all
    if (hasFlag(kin->behavior, KinematicsBehavior::FaceTarget)) {
        dir->faceRight();
    }
    // Priority 2: Match Owner's Facing (if Attached)
    else if (hasFlag(kin->behavior, KinematicsBehavior::Attached)) {
        if (auto *owner = entity->getComponent<OwnerComponent>()) {
            if (auto *ownerDir = owner->owner->getComponent<DirectionComponent>()) {
                dir->setFacing(ownerDir->getFacing());
            }
        }
    }
    // Priority 3: Face Velocity
    else {
        if (kin->velocity.x > EPSILON)
            dir->faceRight();
        else if (kin->velocity.x < -EPSILON)
            dir->faceLeft();
    }

    float absScaleX = std::abs(kin->baseScale.x);
    if (dir->isFacingLeft()) {
        trans->scale.x = -absScaleX;
    }
    else {
        trans->scale.x = absScaleX;
    }
    trans->scale.y = kin->baseScale.y;
}

void KinematicsSystem::handleRotation(float dt, KinematicsComponent *kin, TransformComponent *trans,
                                      Entity *entity)
{
    // Priority 1: Face Target (Overrides other rotation)
    if (hasFlag(kin->behavior, KinematicsBehavior::FaceTarget) && kin->aimPoint) {
        // Find the target angle
        sf::Vector2f diff = *kin->aimPoint - trans->position;
        float targetAngle = ToDegrees(std::atan2(diff.y, diff.x));
        if (targetAngle > 90.f || targetAngle < -90.f) {
            // Aiming left, flip sprite vertically
            trans->scale.y = -kin->baseScale.y;
        }
        else {
            // Aiming right, use normal scale
            trans->scale.y = kin->baseScale.y;
        }

        // Find the shortest angle to turn
        float currentAngle = trans->rotation;
        float angleDiff = targetAngle - currentAngle;

        // Normalize angle diff to be between -180 and 180
        while (angleDiff > 180.f) angleDiff -= 360.f;
        while (angleDiff < -180.f) angleDiff += 360.f;

        // Apply turn speed (angularVelocity)
        float turnSpeed = kin->angularVelocity * dt;

        if (std::abs(angleDiff) < turnSpeed) {
            trans->rotation = targetAngle;
        }
        else if (angleDiff > 0.f) {
            trans->rotation += turnSpeed;
        }
        else {
            trans->rotation -= turnSpeed;
        }
    }
    // Priority 2: Match Owner's Rotation (if Attached)
    else if (hasFlag(kin->behavior, KinematicsBehavior::Attached)) {
        if (auto *owner = entity->getComponent<OwnerComponent>()) {
            if (auto *ownerTransform = owner->owner->getComponent<TransformComponent>()) {
                trans->rotation = ownerTransform->rotation;
            }
        }
    }
    // Priority 3: Simple Rotating
    else if (hasFlag(kin->behavior, KinematicsBehavior::Rotating)) {
        trans->rotation += kin->angularVelocity * dt;
    }
}

void KinematicsSystem::handleScaling(float dt, KinematicsComponent *kin, TransformComponent *trans)
{
    if (hasFlag(kin->behavior, KinematicsBehavior::Pulsing)) {
        float pulseMultiplier =
            1.f + kin->pulseAmplitude * std::sin(kin->currentTime * kin->pulseFrequency);
        trans->scale *= pulseMultiplier;
    }
}
