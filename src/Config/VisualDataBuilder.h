#pragma once
#include <SFML/Graphics.hpp>

struct VisualComponentData
{
    std::string filename;
    sf::Vector2f scale{1.f, 1.f};
    sf::Vector2f origin{0.f, 0.f};
    sf::Vector2f offset{0.f, 0.f};
    float rotation{0.f};
};

class VisualDataBuilder
{
public:
    VisualDataBuilder &setFilename(const std::string &filename)
    {
        m_data.filename = filename;
        return *this;
    }
    VisualDataBuilder &setScale(const sf::Vector2f &scale)
    {
        m_data.scale = scale;
        return *this;
    }
    VisualDataBuilder &setOrigin(const sf::Vector2f &origin)
    {
        m_data.origin = origin;
        return *this;
    }
    VisualDataBuilder &setOffset(const sf::Vector2f &offset)
    {
        m_data.offset = offset;
        return *this;
    }
    VisualDataBuilder &setRotation(float rotation)
    {
        m_data.rotation = rotation;
        return *this;
    }
    VisualComponentData build() const { return m_data; }

private:
    VisualComponentData m_data;
};
