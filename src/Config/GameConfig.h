#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <optional>
#include "../Types.h"

// Kinematics behavior types
enum class KinematicsBehavior
{
    Linear,     // Constant velocity
    Accelerate, // Changing velocity
    Homing,     // Track target
    Orbital,    // Circle around point
    Extending,  // Scale over time (laser)
    Sweeping,   // Rotate over time (laser sweep)
    Pulsing     // Scale up/down repeatedly
};

// Data structures for component configurations
struct VisualComponentData
{
    std::string filename;
    sf::Vector2f scale;
    sf::Vector2f origin;
    sf::Vector2f offset;
    float rotation;
};

struct CollisionComponentData
{
    sf::Vector2f size;
    sf::Vector2f scale;
    sf::Vector2f origin;
    sf::Vector2f offset;
    float rotation;
    sf::Color debugColor;
};

struct AnimationInfo
{
    sf::Vector2i frameSize;
    sf::Vector2i startPos;
    size_t frameCount;
    sf::Time frameDuration;
    bool loop;
    bool flipX;
};

struct WeaponComponentData
{
    float baseDamage;
    float basePiercing;
    int maxHits;    // -1 for unlimited (beams)
    float lifetime; // 0 for infinite
    float radius;
};

struct KinematicsComponentData
{
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float angularVelocity;
    float angularAcceleration;
    sf::Vector2f scaleVelocity;
    KinematicsBehavior behavior;
    float orbitRadius;
};

struct EntityConfig
{
    VisualComponentData visual;
    CollisionComponentData collision;
    std::unordered_map<EntityState, AnimationInfo> animations;
    std::optional<WeaponComponentData> weapon;
    std::optional<KinematicsComponentData> kinematics;
};

namespace Config {
    extern const EntityConfig PLAYER;
    extern const EntityConfig TOWER;
    extern const EntityConfig LASER_WEAPON;
    extern const EntityConfig VAMPIRE;
    extern const std::unordered_map<EntityType, const EntityConfig &> ENTITY_CONFIGS;
} // namespace Config
