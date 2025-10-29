#pragma once
#include "Component.h"
#include "../Config/GameConfig.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>

class CollisionComponent : public Component
{
public:
    // config based runtime state
    CollisionShape type;
    float radius;
    std::vector<sf::Vector2f> localPoints;

    // Local transform properties
    sf::Vector2f scale;
    sf::Vector2f origin;
    sf::Vector2f offset;
    float rotation;
    sf::Color debugColor;

    // runtime state
    bool isColliding{false};

    CollisionComponent(const CollisionComponentData &data)
        : type(data.type)
        , radius(data.radius)
        , localPoints(data.points)
        , scale(data.scale)
        , origin(data.origin)
        , offset(data.offset)
        , rotation(data.rotation)
        , debugColor(data.debugColor)
    {}

    virtual const char *getName() const override { return "CollisionComponent"; }
};
