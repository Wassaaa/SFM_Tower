#pragma once
#include "Component.h"
#include "../Config/GameConfig.h"

class WeaponComponent : public Component
{
public:
    float damage;
    int piercing; // How many enemies it can pierce
    int maxHits;  // -1 for unlimited (beams)
    float lifetime;

    WeaponComponent(const WeaponComponentData &data)
        : damage(data.damage)
        , piercing(data.piercing)
        , maxHits(data.maxHits)
        , lifetime(data.lifetime)
    {}

    virtual const char *getName() const override { return "WeaponComponent"; }
};
