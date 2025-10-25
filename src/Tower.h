#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Weapon.h"
#include "Entity.h"
#include "Types.h"

class Tower : public Entity
{
public:
    Tower(Game *pGame, const sf::Vector2f &position);
    ~Tower() = default;

    void update(float dt) override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // weapons
    void addWeapon(EntityType weaponType);
    std::vector<std::unique_ptr<Weapon>> &getWeapons() { return m_weapons; }

protected:
    void initComponents() override;

private:
    void initVariables();

    std::vector<std::unique_ptr<Weapon>> m_weapons;
    EntityState m_currentState{EntityState::IDLE};
};
