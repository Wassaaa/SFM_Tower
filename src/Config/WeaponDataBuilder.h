#pragma once

struct WeaponComponentData
{
    float damage{0.f};
    int piercing{0};
    int maxHits{1};
    float lifetime{0.f};
};

class WeaponDataBuilder
{
public:
    WeaponDataBuilder &setDamage(float damage)
    {
        m_data.damage = damage;
        return *this;
    }
    WeaponDataBuilder &setPiercing(int piercing)
    {
        m_data.piercing = piercing;
        return *this;
    }
    WeaponDataBuilder &setMaxHits(int hits)
    {
        m_data.maxHits = hits;
        return *this;
    }
    WeaponDataBuilder &setLifetime(float lifetime)
    {
        m_data.lifetime = lifetime;
        return *this;
    }
    WeaponComponentData build() const { return m_data; }

private:
    WeaponComponentData m_data;
};
