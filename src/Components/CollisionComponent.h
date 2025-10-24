#pragma once
#include "Component.h"
#include "../Config/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <array>

class CollisionComponent : public Component, public sf::Drawable, public sf::Transformable
{
public:
    CollisionComponent(const CollisionComponentData &data);
    ~CollisionComponent() = default;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    // collision detection
    bool intersects(const CollisionComponent &other) const;
    sf::FloatRect getBounds() const;

    // getters and setters
    void setDebugDraw(bool enabled)
    {
        m_debugDraw = enabled;
    }
    bool isDebugDrawEnabled() const
    {
        return m_debugDraw;
    }
    virtual const char *getName() const override
    {
        return "CollisionComponent";
    }
    sf::Vector2f getCenter() const
    {
        sf::FloatRect bounds = m_shape.getGlobalBounds();
        return sf::Vector2f(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    }

private:
    // members
    sf::RectangleShape m_shape;
    const CollisionComponentData &m_data;
    bool m_debugDraw{false};

    // member functions
    void initShape();
    void applyTransforms();
    bool intersectsOBB(const CollisionComponent &other) const;
    void getWorldCorners(std::array<sf::Vector2f, 4> &corners) const;
    void projectOntoAxis(const std::array<sf::Vector2f, 4> &corners, const sf::Vector2f &axis,
                         float &min, float &max) const;
};
