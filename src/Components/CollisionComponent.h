#pragma once
#include "Component.h"
#include "../Constants.h"
#include "../Config/GameConfig.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <memory>

class CollisionComponent : public Component, public sf::Drawable
{
public:
    // config based runtime state
    CollisionShape type;
    float radius;
    std::vector<sf::Vector2f> localPoints;

    // Local transform properties
    sf::Vector2f scale;
    sf::Vector2f origin;
    sf::Vector2f offset;
    float rotation;

    sf::Color debugColor;

    // runtime state
    bool isColliding{false};

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        if (isColliding)
            m_debugShape->setOutlineColor(sf::Color::Red);
        else
            m_debugShape->setOutlineColor(sf::Color::White);
        target.draw(*m_debugShape, states);
    }

    CollisionComponent(const CollisionComponentData &data)
        : type(data.type)
        , radius(data.radius)
        , localPoints(data.points)
        , scale(data.scale)
        , origin(data.origin)
        , offset(data.offset)
        , rotation(data.rotation)
        , debugColor(data.debugColor)
    {
        if (Constants::DEBUG_DRAW) {
            initDebug();
        }
    }

    virtual const char *getName() const override { return "CollisionComponent"; }

private:
    std::shared_ptr<sf::Shape> m_debugShape;

    void initDebug()
    {
        switch (type) {
        case CollisionShape::Circle: {
            auto circle = std::make_shared<sf::CircleShape>(radius);
            circle->setOrigin(radius, radius);
            m_debugShape = std::move(circle);
            break;
        }
        case CollisionShape::Polygon: {
            auto polygon = std::make_shared<sf::ConvexShape>(localPoints.size());
            for (size_t i = 0; i < localPoints.size(); i++) {
                polygon->setPoint(i, localPoints[i]);
            }
            m_debugShape = std::move(polygon);
            break;
        }
        default:
            break;
        }
        m_debugShape->setScale(scale);
        m_debugShape->setRotation(rotation);
        m_debugShape->setPosition(offset);
        m_debugShape->setFillColor(debugColor);
    }
};
