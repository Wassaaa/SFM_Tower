#pragma once

#include <SFML/Graphics.hpp>
#include "Components/ComponentContainer.h"
#include "Types.h"

class Game;
class TransformComponent; // Forward declaration
struct InputState;

class Entity : public ComponentContainer
{
public:
    Entity(Game *pGame, EntityType type, const sf::Vector2f &position);
    virtual ~Entity() = default;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    sf::Vector2f getPosition() const;
    virtual void setPosition(const sf::Vector2f &pos);

    sf::Vector2f getCenter() const;

    EntityType getType() const { return m_type; }
    void setColliding(bool colliding);

    Game *getGame() const { return m_pGame; }
    void resolveCollision(const sf::Vector2f &pushVector);
    void applyCollisionImpulse(const sf::Vector2f &velocityChange);
    virtual void handleInput(float deltaTime, const InputState &input);

protected:
    virtual void initComponents();

    Game *m_pGame;
    EntityType m_type;
    sf::Vector2f m_initialPosition;
};
