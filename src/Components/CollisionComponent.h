#pragma once
#include "Component.h"
#include "../Config/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class CollisionComponent : public Component, public sf::Drawable, public sf::Transformable
{
public:
    CollisionComponent(const CollisionComponentData &data);
    ~CollisionComponent() = default;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // collision detection
    bool intersects(const CollisionComponent &other) const;
    CollisionResult checkCollision(const CollisionComponent &other) const;
    sf::FloatRect getBounds() const;

    // getters and setters
    void setDebugDraw(bool enabled) { m_debugDraw = enabled; }
    bool isDebugDrawEnabled() const { return m_debugDraw; }
    virtual const char *getName() const override { return "CollisionComponent"; }
    sf::Vector2f getCenter() const;
    CollisionShape getType() const { return m_type; }
    const CollisionComponentData &getData() const { return m_data; }
    void setColliding(bool colliding) { m_isColliding = colliding; }

private:
    // members
    CollisionShape m_type;
    std::vector<sf::Vector2f> m_localPoints;
    float m_radius; // only for circles
    const CollisionComponentData &m_data;
    bool m_debugDraw{false};
    bool m_isColliding{false};

    // member functions
    void applyTransforms();

    // Collision detection methods
    CollisionResult circleCircleCollision(const CollisionComponent &other) const;
    CollisionResult circlePolygonCollision(const CollisionComponent &other) const;
    CollisionResult polygonPolygonCollision(const CollisionComponent &other) const;

    // Helper methods
    std::vector<sf::Vector2f> getWorldPoints() const;
    float getWorldRadius() const;
    void projectOntoAxis(const std::vector<sf::Vector2f> &points, const sf::Vector2f &axis,
                         float &min, float &max) const;
    bool hasSeparatingAxis(const std::vector<sf::Vector2f> &points1,
                           const std::vector<sf::Vector2f> &points2, const sf::Vector2f &axis,
                           float &minOverlap, sf::Vector2f &minAxis) const;
};
