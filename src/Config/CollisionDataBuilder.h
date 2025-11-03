#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct CollisionComponentData
{
    std::vector<sf::Vector2f> points;
    sf::Vector2f scale{1.f, 1.f};
    sf::Vector2f origin{0.f, 0.f};
    sf::Vector2f offset{0.f, 0.f};
    float rotation{0.f};
    sf::Color debugColor{255, 0, 0, 128};
};

class CollisionDataBuilder
{
public:
    CollisionDataBuilder &setCircle(float radius, size_t pointCount = 15)
    {
        sf::CircleShape circle(radius, pointCount);
        m_data.points.reserve(pointCount);
        for (int i = 0; i < circle.getPointCount(); i++) {
            m_data.points.push_back(circle.getPoint(i));
        }
        m_data.origin = {radius, radius};
        return *this;
    }
    CollisionDataBuilder &setPolygon(const std::vector<sf::Vector2f> &points)
    {
        m_data.points = points;
        return *this;
    }
    CollisionDataBuilder &setBox(const sf::Vector2f &size)
    {
        m_data.points = {{0, 0}, {size.x, 0}, {size.x, size.y}, {0, size.y}};
        return *this;
    }
    CollisionDataBuilder &setScale(const sf::Vector2f &scale)
    {
        m_data.scale = scale;
        return *this;
    }
    CollisionDataBuilder &setOrigin(const sf::Vector2f &origin)
    {
        m_data.origin = origin;
        return *this;
    }
    CollisionDataBuilder &setOffset(const sf::Vector2f &offset)
    {
        m_data.offset = offset;
        return *this;
    }
    CollisionDataBuilder &setRotation(float rotation)
    {
        m_data.rotation = rotation;
        return *this;
    }
    CollisionDataBuilder &setDebugColor(const sf::Color &color)
    {
        m_data.debugColor = color;
        return *this;
    }
    CollisionComponentData build() const { return m_data; }

private:
    CollisionComponentData m_data;
};
