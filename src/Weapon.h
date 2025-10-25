#pragma once
#include "Entity.h"
#include "Types.h"

class Weapon : public Entity
{
public:
    Weapon(Entity *owner, EntityType type);
    ~Weapon() = default;

    void update(float dt) override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // Upgrade methods
    void addSpeed();
    void addRange();

protected:
    void initComponents() override;

private:
    Entity *m_owner;
    sf::Vector2f m_orbitCenter;
};
