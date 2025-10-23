#include <iostream>
#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(const WeaponComponentData &data)
    : m_data(data)
    , currentDamage(data.baseDamage)
    , piercing(data.basePiercing)
    , currentHits(0)
    , maxHits(data.maxHits)
    , lifetime(data.lifetime)
    , currentLifetime(0.f)
    , radius(data.radius)
{}

void WeaponComponent::update(float dt)
{
    currentLifetime += dt;
}

void WeaponComponent::addDamage(float amount)
{
    currentDamage += amount;
    std::cout << "Weapon Damage increased by " << amount << "\n";
}

void WeaponComponent::addPiercing()
{
    piercing += 1.f;
    maxHits += 1;
    std::cout << "Weapon Piercing increased\n";
}

void WeaponComponent::addRadius(float amount)
{
    radius += amount;
    std::cout << "Weapon Radius increased by " << amount << "\n";
}

void WeaponComponent::registerHit()
{
    if (maxHits > 0) {
        currentHits++;
    }
}

bool WeaponComponent::canStillHit() const
{
    return maxHits < 0 || currentHits < maxHits;
}

bool WeaponComponent::isExpired() const
{
    return lifetime > 0.f && currentLifetime >= lifetime;
}
