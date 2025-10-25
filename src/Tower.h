#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Weapon.h"
#include "Game.h"
#include "Types.h"
#include "Components/ComponentContainer.h"
#include "Config/EntityManager.h"

class Tower : public ComponentContainer
{
public:
    Tower(Game *pGame, const sf::Vector2f &position);
    ~Tower() = default;

    void update(float &dt);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    // weapons
    void addWeapon(EntityType weaponType);
    std::vector<std::unique_ptr<Weapon>> &getWeapons() { return weapons; }

    // Position/Transform getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenter() const;

private:
    // init functions
    void initVariables();
    void initComponents();

    // update functions
    void updateAnimation(float &dt);

    Game *m_pGame;
    std::vector<std::unique_ptr<Weapon>> weapons;
    EntityState currentState{EntityState::IDLE};
    sf::Vector2f m_position;
};
