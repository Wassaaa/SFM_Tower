#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <optional>
#include "../Types.h"

// Kinematics behavior types (can be combined with bitwise OR)
enum class KinematicsBehavior : uint32_t
{
    None = 0,
    Linear = 1 << 0,     // Move with velocity
    Accelerate = 1 << 1, // Velocity changes over time
    Homing = 1 << 2,     // Track target position
    Orbital = 1 << 3,    // Circle around point
    Rotating = 1 << 4,   // Rotate over time (was Sweeping)
    Extending = 1 << 5,  // Scale over time (lasers)
    Pulsing = 1 << 6,    // Scale up/down repeatedly
    FaceTarget = 1 << 7  // Always keep facing the target
};

// Weapon behavior types
enum class WeaponBehavior : uint32_t
{
    None = 0,
    FollowOwner = 1 << 0 // Stay at owner position (attached)
};

// Enable bitwise operations
inline KinematicsBehavior operator|(KinematicsBehavior a, KinematicsBehavior b)
{
    return static_cast<KinematicsBehavior>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline KinematicsBehavior operator&(KinematicsBehavior a, KinematicsBehavior b)
{
    return static_cast<KinematicsBehavior>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool hasFlag(KinematicsBehavior value, KinematicsBehavior flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

// Collision shape types
enum class CollisionShape
{
    Circle,
    Polygon
};

// Collision response data
struct CollisionResult
{
    bool intersects;
    sf::Vector2f normal;
    float depth;
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
    CollisionShape type;

    // For circles:
    float radius;

    // For polygons:
    std::vector<sf::Vector2f> points; // Local space points

    // Transform data
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
};

struct WeaponComponentData
{
    float baseDamage;
    float basePiercing;
    int maxHits;    // -1 for unlimited (beams)
    float lifetime; // 0 for infinite
    float radius;
    WeaponBehavior behavior;
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
    float orbitAngularVelocity;
    float pulseFrequency;
    float pulseAmplitude;
    float drag; // Linear damping (0 = no drag, 1 = instant stop)
    float mass; // For physics response (1.0 = default)
    bool isStatic;
};

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
