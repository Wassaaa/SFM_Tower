#pragma once
#include "Component.h"

class HealthComponent : public Component
{
public:
    float currentHealth;
    float maxHealth;

    HealthComponent(float health)
        : currentHealth(health)
        , maxHealth(health)
    {}

    virtual const char *getName() const override { return "HealthComponent"; }
};
