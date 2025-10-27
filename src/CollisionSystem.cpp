#include "CollisionSystem.h"

#include <limits>
#include <iostream>

#include "Components/CollisionComponent.h"
#include "Components/KinematicsComponent.h"
#include "Entity.h"
#include "MathUtils.h"

void CollisionSystem::update(std::vector<std::unique_ptr<Entity>> &entities, float deltaTime)
{
    // Track which entities are colliding this frame
    std::vector<bool> isColliding(entities.size(), false);

    for (size_t i = 0; i < entities.size(); i++) {
        auto *collision1 = entities[i]->getComponent<CollisionComponent>();
        if (!collision1)
            continue;

        for (size_t j = i + 1; j < entities.size(); j++) {
            auto *collision2 = entities[j]->getComponent<CollisionComponent>();
            if (!collision2)
                continue;

            auto *kin1 = entities[i]->getComponent<KinematicsComponent>();
            auto *kin2 = entities[j]->getComponent<KinematicsComponent>();

            // Skip collision check if both entities are static (no kinematics)
            if (!kin1 && !kin2) {
                continue;
            }

            // Check for collision and get collision data
            CollisionResult result = collision1->checkCollision(*collision2);

            if (result.intersects) {
                // Mark both entities as colliding
                isColliding[i] = true;
                isColliding[j] = true;

                // Get masses - use infinity for static objects (no kinematics)
                float mass1 = kin1 ? kin1->getMass() : std::numeric_limits<float>::infinity();
                float mass2 = kin2 ? kin2->getMass() : std::numeric_limits<float>::infinity();

                // Handle collision based on mass configuration
                if (std::isinf(mass1)) {
                    // Entity i is static, j is dynamic
                    handleStaticDynamicCollision(entities[i].get(), entities[j].get(),
                                                 result.normal, result.depth);
                }
                else if (std::isinf(mass2)) {
                    // Entity j is static, i is dynamic
                    handleStaticDynamicCollision(entities[j].get(), entities[i].get(),
                                                 -result.normal, result.depth);
                }
                else {
                    // Both dynamic
                    handleDynamicDynamicCollision(entities[i].get(), entities[j].get(),
                                                  result.normal, result.depth);
                }
            }
        }
    }

    // Apply collision states
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->setColliding(isColliding[i]);
    }
}

void CollisionSystem::handleStaticDynamicCollision(Entity *staticEntity, Entity *dynamicEntity,
                                                   const sf::Vector2f &normal, float depth)
{
    auto *kin = dynamicEntity->getComponent<KinematicsComponent>();
    if (!kin)
        return;

    sf::Vector2f velocity = kin->getVelocity();
    float velAlongNormal = DotProduct(velocity, normal);

    // Apply impulse if approaching
    if (velAlongNormal < EPSILON) {
        const float restitution = 1.0f;
        float impulseMagnitude = -(1.f + restitution) * velAlongNormal;
        sf::Vector2f impulse = normal * impulseMagnitude;
        dynamicEntity->applyCollisionImpulse(impulse);
    }

    // Push dynamic entity away from static entity with extra separation
    // Use 1.2x to ensure full separation even at high speeds
    sf::Vector2f pushVector = normal * depth * 1.2f;
    dynamicEntity->resolveCollision(pushVector);

    sf::Vector2f newVel = kin->getVelocity();
    sf::Vector2f newPos = dynamicEntity->getPosition();
    std::cout << "  -> Push: (" << pushVector.x << ", " << pushVector.y << ")"
              << " | NewPos: (" << newPos.x << ", " << newPos.y << ")"
              << " | NewVel: (" << newVel.x << ", " << newVel.y << ")" << std::endl;
}

void CollisionSystem::handleDynamicDynamicCollision(Entity *entity1, Entity *entity2,
                                                    const sf::Vector2f &normal, float depth)
{
    auto *kin1 = entity1->getComponent<KinematicsComponent>();
    auto *kin2 = entity2->getComponent<KinematicsComponent>();

    if (!kin1 || !kin2)
        return;

    float mass1 = kin1->getMass();
    float mass2 = kin2->getMass();
    float totalMass = mass1 + mass2;

    // Get relative velocity along collision normal
    sf::Vector2f vel1 = kin1->getVelocity();
    sf::Vector2f vel2 = kin2->getVelocity();
    sf::Vector2f relativeVel = vel1 - vel2;

    // Calculate velocity along normal (dot product)
    // Normal points from entity1→entity2, so positive velAlongNormal means approaching
    float velAlongNormal = DotProduct(relativeVel, normal);

    // Apply impulse if approaching OR resting
    if (velAlongNormal > -EPSILON) {
        // Calculate impulse with baseline push to handle resting contacts
        float impulseMagnitude = (velAlongNormal + EPSILON) / totalMass;
        sf::Vector2f impulse = normal * impulseMagnitude;

        // Apply impulse proportional to mass
        entity1->applyCollisionImpulse(-impulse * mass2);
        entity2->applyCollisionImpulse(impulse * mass1);
    }

    // Positional correction split by mass ratio
    float ratio1 = mass2 / totalMass;
    float ratio2 = mass1 / totalMass;
    sf::Vector2f separation1 = -normal * depth * ratio1;
    sf::Vector2f separation2 = normal * depth * ratio2;

    entity1->resolveCollision(separation1);
    entity2->resolveCollision(separation2);
}
