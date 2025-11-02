#pragma once
#include "Component.h"

class Entity;

class OwnerComponent : public Component
{
public:
    Entity *owner;
    sf::Vector2f offset{0.f, 0.f};

    OwnerComponent(Entity *owner, const sf::Vector2f &offset = {0.f, 0.f})
        : owner(owner)
    {}

    virtual const char *getName() const override { return "OwnerComponent"; }
};
