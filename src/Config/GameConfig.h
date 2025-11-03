#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <optional>
#include "VisualDataBuilder.h"
#include "CollisionDataBuilder.h"
#include "AnimationInfoBuilder.h"
#include "WeaponDataBuilder.h"
#include "KinematicsDataBuilder.h"
#include "../Types.h"

struct EntityConfig
{
    std::optional<VisualComponentData> visual;
    std::optional<CollisionComponentData> collision;
    std::unordered_map<EntityState, AnimationInfo> animations;
    std::optional<WeaponComponentData> weapon;
    std::optional<KinematicsComponentData> kinematics;
};

namespace Config {
    extern const EntityConfig PLAYER;
    extern const EntityConfig TOWER;
    extern const EntityConfig LASER_WEAPON;
    extern const EntityConfig VAMPIRE;
    extern const EntityConfig TEST_BOX;
    extern const std::unordered_map<EntityType, const EntityConfig &> ENTITY_CONFIGS;
} // namespace Config
