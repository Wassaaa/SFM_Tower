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
