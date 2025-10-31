#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Entity;

class RenderSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem() = default;
    void draw(sf::RenderTarget &target, sf::RenderStates states,
              const std::vector<std::unique_ptr<Entity>> &entities) const;
};
