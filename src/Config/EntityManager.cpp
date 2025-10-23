#include "EntityManager.h"
#include "GameConfig.h"
#include "../Components/VisualComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/WeaponComponent.h"
#include "../Components/KinematicsComponent.h"

void EntityManager::loadEntityData()
{
    for (const auto &[type, config] : Config::ENTITY_CONFIGS) {
        EntityData &entity = entityData[type];
        entity.addComponent<VisualComponent>(config.visual);
        entity.addComponent<CollisionComponent>(config.collision);
        auto &animData = entity.addComponent<AnimationData>();
        for (const auto &[state, info] : config.animations) animData.addAnimation(state, info);

        if (config.weapon.has_value())
            entity.addComponent<WeaponComponent>(config.weapon.value());

        if (config.kinematics.has_value())
            entity.addComponent<KinematicsComponent>(config.kinematics.value());
    }
}

const EntityData &EntityManager::getEntityData(EntityType type) const
{
    return entityData.at(type);
}
