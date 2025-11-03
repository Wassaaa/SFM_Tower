#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <memory>
#include <vector>

class Entity;
class CollisionComponent;
class TransformComponent;

struct CollisionResult
{
    bool intersects;
    sf::Vector2f normal;
    float depth;
};

class CollisionSystem
{
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;

    // Main update loop
    void update(float deltaTime, std::vector<std::unique_ptr<Entity>> &entities);

private:
    // Combat handling
    void processCombat(Entity *entityA, Entity *entityB);
    bool skipPhysics{false};
    // Collision resolution logic
    void handleCollision(Entity *entityA, Entity *entityB, const sf::Vector2f &normal, float depth);

    // Main check function
    CollisionResult checkCollision(const CollisionComponent &colA, const TransformComponent &transA,
                                   const CollisionComponent &colB,
                                   const TransformComponent &transB) const;

    CollisionResult polygonPolygonCollision(const CollisionComponent &colA,
                                            const TransformComponent &transA,
                                            const CollisionComponent &colB,
                                            const TransformComponent &transB) const;

    // Helper functions
    std::vector<sf::Vector2f> getWorldPoints(const CollisionComponent &col,
                                             const TransformComponent &trans) const;
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
