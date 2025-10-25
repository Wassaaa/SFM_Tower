#pragma once
#include "Component.h"
#include "../Config/GameConfig.h"

class WeaponComponent : public Component
{
public:
    WeaponComponent(const WeaponComponentData &data);
    ~WeaponComponent() = default;

    void update(float dt);
    virtual const char *getName() const override { return "WeaponComponent"; }

    // Getters
    float getDamage() const { return currentDamage; }
    float getLifetime() const { return currentLifetime; }
    float getPiercing() const { return piercing; }
    float getRadius() const { return radius; }
    int getCurrentHits() const { return currentHits; }
    int getMaxHits() const { return maxHits; }

    // Upgrade methods
    void addDamage(float amount);
    void addPiercing();
    void addRadius(float amount);

    // Hit tracking
    void registerHit();
    bool canStillHit() const;
    bool isExpired() const;

private:
    const WeaponComponentData &m_data;

    // Combat stats
    float currentDamage;
    float piercing; // How many enemies it can pierce
    int currentHits;
    int maxHits; // -1 for unlimited (beams)

    // Lifetime
    float lifetime;
    float currentLifetime;

    // Effects
    float radius; // Collision/damage radius
};
