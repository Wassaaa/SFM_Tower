#include "TargetingSystem.h"
#include "../Entity.h"
#include "OwnerComponent.h"
#include "TransformComponent.h"
#include "KinematicsComponent.h"

void TargetingSystem::update(std::vector<std::unique_ptr<Entity>> &entities)
{
    for (const auto &entity : entities) {
        // All following is owner based, so check for owner and owner's transform
        auto *owner = entity->getComponent<OwnerComponent>();
        if (!owner || !owner->owner) {
            continue;
        }
        // Get the owner's transform
        auto *ownerTransform = owner->owner->getComponent<TransformComponent>();
        if (!ownerTransform) {
            continue;
        }

        // Only care about entities that need to move
        auto *kinematics = entity->getComponent<KinematicsComponent>();
        if (!kinematics) {
            continue;
        }

        // Calculate owner offset based location
        sf::Transform ownerSFTransform = ownerTransform->getTransform();
        sf::Vector2f finalTargetPos = ownerSFTransform.transformPoint(owner->offset);

        // Store this position in the kinematics cache
        kinematics->targetCache = finalTargetPos;

        // Link the targetPoint pointer if it's not set
        if (kinematics->targetPoint == nullptr) {
            kinematics->targetPoint = &kinematics->targetCache;
        }
    }
}
