#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Entity;
class TransformComponent;
class VisualComponent;
class CollisionComponent;
class DirectionComponent;

class RenderSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void draw(sf::RenderTarget &target, sf::RenderStates states,
              const std::vector<std::unique_ptr<Entity>> &entities);

private:
    void prepareEntity(VisualComponent *visual, CollisionComponent *collision,
                       DirectionComponent *dir) const;
    void drawEntity(sf::RenderTarget &target, sf::RenderStates states, VisualComponent *visual,
                    CollisionComponent *collision) const;
};
