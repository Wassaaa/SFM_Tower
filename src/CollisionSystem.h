#pragma once

#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

class Entity;

class CollisionSystem
{
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;

    // Update collision detection and response for all entities
    void update(std::vector<std::unique_ptr<Entity>> &entities, float deltaTime);

private:
    // Handle collision between a static and dynamic entity
    void handleStaticDynamicCollision(Entity *staticEntity, Entity *dynamicEntity,
                                      const sf::Vector2f &normal, float depth);

    // Handle collision between two dynamic entities
    void handleDynamicDynamicCollision(Entity *entity1, Entity *entity2, const sf::Vector2f &normal,
                                       float depth);
};
