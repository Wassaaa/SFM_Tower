#include "DebugRenderSystem.h"
#include "Entity.h"
#include "Components/CollisionComponent.h"
#include "Components/TransformComponent.h"

void DebugRenderSystem::draw(sf::RenderTarget &target, sf::RenderStates states,
                             const std::vector<std::unique_ptr<Entity>> &entities,
                             bool debugDrawEnabled) const
{
    if (!debugDrawEnabled) {
        return;
    }

    for (const auto &entity : entities) {
        auto *col = entity->getComponent<CollisionComponent>();
        auto *trans = entity->getComponent<TransformComponent>();

        if (!col || !trans || !col->isEnabled()) {
            continue;
        }

        // Calculate the final transform for this component
        sf::Transform componentTransform;
        componentTransform.translate(trans->position + col->offset);
        componentTransform.rotate(trans->rotation + col->rotation);
        componentTransform.scale(trans->scale.x * col->scale.x, trans->scale.y * col->scale.y);

        // Apply origin
        componentTransform.translate(-col->origin);

        sf::RenderStates componentState = states;
        componentState.transform *= componentTransform;

        sf::Color outlineColor = col->isColliding ? sf::Color::Red : sf::Color::White;

        if (col->type == CollisionShape::Circle) {
            sf::CircleShape circle(col->radius);
            circle.setOrigin(col->radius, col->radius);
            circle.setFillColor(col->debugColor);
            circle.setOutlineColor(outlineColor);
            circle.setOutlineThickness(1.0f);
            target.draw(circle, componentState);
        }
        else {
            sf::ConvexShape polygon;
            polygon.setPointCount(col->localPoints.size());
            for (size_t i = 0; i < col->localPoints.size(); i++) {
                polygon.setPoint(i, col->localPoints[i]);
            }
            polygon.setFillColor(col->debugColor);
            polygon.setOutlineColor(outlineColor);
            polygon.setOutlineThickness(1.0f);
            target.draw(polygon, componentState);
        }
    }
}
