#pragma once
#include <SFML/Graphics.hpp>

#include "Component.h"

class TransformComponent : public Component
{
public:
    TransformComponent() = default;
    TransformComponent(const sf::Vector2f &position, float rotation = 0.f,
                       const sf::Vector2f &scale = {1.f, 1.f})
        : position(position)
        , rotation(rotation)
        , scale(scale)
    {}

    sf::Transform getTransform() const
    {
        sf::Transform t;

        t.translate(position);
        t.rotate(rotation);
        t.scale(scale);

        return t;
    }

    sf::Vector2f position{0.f, 0.f};
    float rotation{0.f};
    sf::Vector2f scale{1.f, 1.f};

    virtual const char *getName() const override { return "TransformComponent"; }
};
