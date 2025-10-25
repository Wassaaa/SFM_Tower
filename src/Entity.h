#pragma once

#include <SFML/Graphics.hpp>
#include "Components/ComponentContainer.h"
#include "Types.h"

class Game;

class Entity : public ComponentContainer
{
public:
    Entity(Game *pGame, EntityType type, const sf::Vector2f &position);
    ~Entity() = default;

    void update(float dt);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    sf::Vector2f getPosition() const { return m_position; }
    void setPosition(const sf::Vector2f &pos);

    EntityType getType() const { return m_type; }
    void setColliding(bool colliding);

    void handleInput(float deltaTime, bool keyW, bool keyA, bool keyS, bool keyD);

private:
    void initComponents();

    Game *m_pGame;
    EntityType m_type;
    sf::Vector2f m_position;
};
