#pragma once

#include <SFML/Graphics.hpp>
#include "Components/ComponentContainer.h"
#include "Types.h"

class Game;
struct InputState;

class Entity : public ComponentContainer
{
public:
    Entity(Game *pGame, EntityType type, const sf::Vector2f &position);
    virtual ~Entity() = default;

    virtual void update(float dt);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    void updateAnimationAndVisual(float dt);

    sf::Vector2f getPosition() const { return m_position; }
    virtual void setPosition(const sf::Vector2f &pos);
    sf::Vector2f getCenter() const;

    EntityType getType() const { return m_type; }
    void setColliding(bool colliding);

    Game *getGame() const { return m_pGame; }

    void resolveCollision(const sf::Vector2f &pushVector);

    virtual void handleInput(float deltaTime, const InputState &input);

protected:
    virtual void initComponents();

    Game *m_pGame;
    EntityType m_type;
    sf::Vector2f m_position;
};
