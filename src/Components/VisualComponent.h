#pragma once
#include "Component.h"
#include "../ResourceManager.h"
#include "../Config/GameConfig.h"
#include <iostream>

class VisualComponent : public Component, public sf::Drawable
{
public:
    VisualComponent(const VisualComponentData &data)
    {
        if (!data.filename.empty()) {
            if (!m_texture.loadFromFile(ResourceManager::getFilePath(data.filename))) {
                std::cerr << "Unable to load texture, creating a bank texture" << std::endl;
                m_texture.create(1, 1);
                sf::Color color = sf::Color::White;
                sf::Uint8 whitePixel[4] = {color.r, color.g, color.b, color.a};
                m_texture.update(whitePixel);
                m_texture.setRepeated(true);
            }
        }
        m_sprite.setTexture(m_texture);
        m_sprite.scale(data.scale);
        m_sprite.setOrigin(data.origin);
        m_sprite.setPosition(data.offset);
        m_sprite.setRotation(data.rotation);
    }
    ~VisualComponent() = default;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        target.draw(m_sprite, states);
    }
    void setTextureRect(const sf::IntRect &rect) { m_sprite.setTextureRect(rect); }
    virtual const char *getName() const override { return "VisualComponent"; }

private:
    sf::Sprite m_sprite;
    sf::Texture m_texture;
};
