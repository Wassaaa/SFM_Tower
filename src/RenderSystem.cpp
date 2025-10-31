#include "RenderSystem.h"
#include "Entity.h"
#include "Components/VisualComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Constants.h"

void RenderSystem::draw(sf::RenderTarget &target, sf::RenderStates _states,
                        const std::vector<std::unique_ptr<Entity>> &entities) const
{
    for (const auto &entity : entities) {
        auto *transform = entity->getComponent<TransformComponent>();

        if (!transform) {
            continue;
        }

        sf::RenderStates states;
        states.transform = transform->getTransform();

        // draw visual component
        auto *visual = entity->getComponent<VisualComponent>();
        if (visual && visual->isEnabled()) {
            target.draw(*visual, states);
        }

        // Draw collision Debug boundaries
        if (Constants::DEBUG_DRAW) {
            auto *collisions = entity->getComponent<CollisionComponent>();
            if (collisions && collisions->isEnabled()) {
                target.draw(*collisions, states);
            }
        }
    }
}
