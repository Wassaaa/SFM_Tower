#pragma once

enum class GameState
{
    WAITING,
    ACTIVE,
};

enum EntityState
{
    NOTHING = -1,
    IDLE,
    MOVE_UP,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_LEFT,
    WEAPON_1
};

enum class EntityType
{
    PLAYER,
    TOWER,
    LASER_WEAPON,
    EXPLOSION_WEAPON,
    CONE_WEAPON,
    VAMPIRE,
    TEST_BOX,
    WALL_HORIZONTAL,
    WALL_VERTICAL,
    BULLET
};

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
    FaceTarget = 1 << 7, // Always keep facing the target
    Attached = 1 << 8    // stick to targetPoint
};

// Enable bitwise operations for KinematicsBehavior
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

inline const char *EntityTypeToString(EntityType type)
{
    switch (type) {
    case EntityType::PLAYER:
        return "PLAYER";
    case EntityType::TOWER:
        return "TOWER";
    case EntityType::LASER_WEAPON:
        return "LASER_WEAPON";
    case EntityType::EXPLOSION_WEAPON:
        return "EXPLOSION_WEAPON";
    case EntityType::CONE_WEAPON:
        return "CONE_WEAPON";
    case EntityType::VAMPIRE:
        return "VAMPIRE";
    case EntityType::TEST_BOX:
        return "TEST_BOX";
    case EntityType::WALL_HORIZONTAL:
        return "WALL_HORIZONTAL";
    case EntityType::WALL_VERTICAL:
        return "WALL_VERTICAL";
    case EntityType::BULLET:
        return "BULLET";
    default:
        return "UNKNOWN_TYPE";
    }
}
