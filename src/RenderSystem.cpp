#include "RenderSystem.h"
#include "Entity.h"
#include "Components/VisualComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/DirectionComponent.h"
#include "Constants.h"

void RenderSystem::draw(sf::RenderTarget &target, sf::RenderStates states,
                        const std::vector<std::unique_ptr<Entity>> &entities)
{
    for (const auto &entity : entities) {
        auto *transform = entity->getComponent<TransformComponent>();
        if (!transform) {
            continue;
        }
        auto *visual = entity->getComponent<VisualComponent>();
        auto *collision = entity->getComponent<CollisionComponent>();
        auto *dir = entity->getComponent<DirectionComponent>();

        prepareEntity(visual, collision, dir);

        sf::RenderStates entityStates = states;
        entityStates.transform *= transform->getTransform();

        drawEntity(target, entityStates, visual, collision);
    }
}

void RenderSystem::prepareEntity(VisualComponent *visual, CollisionComponent *collision,
                                 DirectionComponent *dir) const
{
    // Sprite Flipping
    if (visual && dir && visual->isEnabled()) {
        sf::Vector2f scale = visual->getScale();
        float absScaleX = std::abs(scale.x);

        if (dir->isFacingLeft() && scale.x > 0)
            visual->setScale({-absScaleX, scale.y});
        else if (dir->isFacingRight() && scale.x < 0)
            visual->setScale({absScaleX, scale.y});
    }

    // collision outline
    if (Constants::DEBUG_DRAW && collision && collision->isEnabled()) {
        sf::Shape *debugShape = collision->getDebugShape();
        if (debugShape) {
            if (collision->isColliding)
                debugShape->setOutlineColor(sf::Color::Red);
            else
                debugShape->setOutlineColor(sf::Color::White);
        }
    }
}

void RenderSystem::drawEntity(sf::RenderTarget &target, sf::RenderStates states,
                              VisualComponent *visual, CollisionComponent *collision) const
{
    // draw visual component
    if (visual && visual->isEnabled()) {
        target.draw(*visual, states);
    }

    // Draw collision Debug boundaries
    if (Constants::DEBUG_DRAW) {
        if (collision && collision->isEnabled()) {
            target.draw(*collision, states);
        }
    }
}
