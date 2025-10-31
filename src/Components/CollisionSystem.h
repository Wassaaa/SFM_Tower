#pragma once

#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>
#include "../Config/GameConfig.h"

class Entity;
class CollisionComponent;
class TransformComponent;

class CollisionSystem
{
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;

    // Main update loop
    void update(std::vector<std::unique_ptr<Entity>> &entities, float deltaTime);

private:
    // Collision resolution logic
    void handleStaticDynamicCollision(Entity *staticEntity, Entity *dynamicEntity,
                                      const sf::Vector2f &normal, float depth);
    void handleDynamicDynamicCollision(Entity *entity1, Entity *entity2, const sf::Vector2f &normal,
                                       float depth);
    void handleStaticStaticCollision(Entity *entityA, Entity *entityB, const sf::Vector2f &normal,
                                     float depth);

    // Main check function
    CollisionResult checkCollision(const CollisionComponent &colA, const TransformComponent &transA,
                                   const CollisionComponent &colB,
                                   const TransformComponent &transB) const;

    // Specific shape-vs-shape checks
    CollisionResult circleCircleCollision(const CollisionComponent &colA,
                                          const TransformComponent &transA,
                                          const CollisionComponent &colB,
                                          const TransformComponent &transB) const;
    CollisionResult circlePolygonCollision(const CollisionComponent &circleCol,
                                           const TransformComponent &circleTrans,
                                           const CollisionComponent &polyCol,
                                           const TransformComponent &polyTrans) const;
    CollisionResult polygonPolygonCollision(const CollisionComponent &colA,
                                            const TransformComponent &transA,
                                            const CollisionComponent &colB,
                                            const TransformComponent &transB) const;

    // Helper functions
    std::vector<sf::Vector2f> getWorldPoints(const CollisionComponent &col,
                                             const TransformComponent &trans) const;
    float getWorldRadius(const CollisionComponent &col, const TransformComponent &trans) const;
    sf::Vector2f getCenter(const CollisionComponent &col, const TransformComponent &trans) const;
    sf::FloatRect getBounds(const CollisionComponent &col, const TransformComponent &trans) const;
    sf::Transform getComponentTransform(const CollisionComponent &col,
                                        const TransformComponent &trans) const;

    // Low-level math helpers
    void projectOntoAxis(const std::vector<sf::Vector2f> &points, const sf::Vector2f &axis,
                         float &min, float &max) const;
    bool hasSeparatingAxis(const std::vector<sf::Vector2f> &points1,
                           const std::vector<sf::Vector2f> &points2, const sf::Vector2f &axis,
                           float &minOverlap, sf::Vector2f &minAxis) const;
};
