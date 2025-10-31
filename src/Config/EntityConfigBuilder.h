#pragma once
#include "GameConfig.h"

class EntityConfigBuilder
{
public:
    EntityConfigBuilder &setVisual(const VisualComponentData &visual)
    {
        m_visual = visual;
        return *this;
    }

    EntityConfigBuilder &setCollision(const CollisionComponentData &collision)
    {
        m_collision = collision;
        return *this;
    }

    EntityConfigBuilder &addAnimation(EntityState state, const AnimationInfo &info)
    {
        m_animations[state] = info;
        return *this;
    }

    EntityConfigBuilder &setWeapon(const WeaponComponentData &weapon)
    {
        m_weapon = weapon;
        return *this;
    }

    EntityConfigBuilder &setKinematics(const KinematicsComponentData &kinematics)
    {
        m_kinematics = kinematics;
        return *this;
    }

    EntityConfig build() const
    {
        return {m_visual, m_collision, m_animations, m_weapon, m_kinematics};
    }

private:
    std::optional<VisualComponentData> m_visual;
    std::optional<CollisionComponentData> m_collision;
    std::unordered_map<EntityState, AnimationInfo> m_animations;
    std::optional<WeaponComponentData> m_weapon;
    std::optional<KinematicsComponentData> m_kinematics;
};
