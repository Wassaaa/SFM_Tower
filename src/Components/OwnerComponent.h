#pragma once
#include "Component.h"

class Entity;

class OwnerComponent : public Component
{
public:
    Entity *owner;

    OwnerComponent(Entity *owner)
        : owner(owner)
    {}

    virtual const char *getName() const override { return "OwnerComponent"; }
};
