#include "CollisionSystem.h"
#include "../Entity.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "KinematicsComponent.h"
#include "WeaponComponent.h"
#include "HealthComponent.h"
#include "OwnerComponent.h"
#include "../MathUtils.h"
#include <limits>
#include <cmath>
#include <iostream>

void CollisionSystem::update(float deltaTime, std::vector<std::unique_ptr<Entity>> &entities)
{
    // Reset collision state for all entities
    for (auto &entity : entities) {
        if (auto *collision = entity->getComponent<CollisionComponent>()) {
            collision->isColliding = false;
        }
    }

    for (size_t i = 0; i < entities.size(); i++) {
        auto *collision1 = entities[i]->getComponent<CollisionComponent>();
        auto *transform1 = entities[i]->getComponent<TransformComponent>();
        if (!collision1 || !transform1 || !collision1->isEnabled())
            continue;

        for (size_t j = i + 1; j < entities.size(); j++) {
            auto *collision2 = entities[j]->getComponent<CollisionComponent>();
            auto *transform2 = entities[j]->getComponent<TransformComponent>();
            if (!collision2 || !transform2 || !collision2->isEnabled())
                continue;

            auto *kin1 = entities[i]->getComponent<KinematicsComponent>();
            auto *kin2 = entities[j]->getComponent<KinematicsComponent>();
            if (!kin1 && !kin2) {
                continue;
            }
            if (kin1 && kin1->isStatic && kin2 && kin2->isStatic) {
                continue;
            }

            CollisionResult result =
                checkCollision(*collision1, *transform1, *collision2, *transform2);

            if (result.intersects) {
                skipPhysics = false;
                processCombat(entities[i].get(), entities[j].get());
                if (skipPhysics) {
                    continue;
                }
                collision1->isColliding = true;
                collision2->isColliding = true;

                handleCollision(entities[i].get(), entities[j].get(), result.normal, result.depth);
            }
        }
    }
}

sf::Transform CollisionSystem::getComponentTransform(const CollisionComponent &col,
                                                     const TransformComponent &trans) const
{
    // Combine local offsets
    sf::Transform t = trans.getTransform();

    t.translate(col.offset);
    t.rotate(col.rotation);
    t.scale(col.scale);
    t.translate(-col.origin);
    return t;
}

void CollisionSystem::projectOntoAxis(const std::vector<sf::Vector2f> &points,
                                      const sf::Vector2f &axis, float &min, float &max) const
{
    min = max = DotProduct(points[0], axis);

    for (size_t i = 1; i < points.size(); i++) {
        float projection = DotProduct(points[i], axis);
        if (projection < min)
            min = projection;
        if (projection > max)
            max = projection;
    }
}

bool CollisionSystem::hasSeparatingAxis(const std::vector<sf::Vector2f> &pointsA,
                                        const std::vector<sf::Vector2f> &pointsB,
                                        const sf::Vector2f &axis, float &minOverlap,
                                        sf::Vector2f &minAxis) const
{
    float minA, maxA, minB, maxB;
    projectOntoAxis(pointsA, axis, minA, maxA);
    projectOntoAxis(pointsB, axis, minB, maxB);

    // Check for separation
    if (maxA < minB || maxB < minA) {
        return true; // no collision
    }

    float overlap1 = maxA - minB;
    float overlap2 = maxB - minA;
    float overlap = std::min(overlap1, overlap2);
    // Square overlap for avoiding sqrt
    overlap *= overlap;

    float axisLengthSq = VecLengthSquared(axis);
    if (axisLengthSq > EPSILON) {
        overlap = overlap / axisLengthSq;
    }

    // Track minimum overlap / collision depth
    if (overlap < minOverlap) {
        minOverlap = overlap;
        // Find the center of the 1D shadow for each object
        float centerA = (minA + maxA) * 0.5f;
        float centerB = (minB + maxB) * 0.5f;

        // Point the axis from 1 to 2
        if (centerA < centerB) {
            minAxis = axis;
        }
        else {
            minAxis = -axis;
        }
    }

    return false;
}

std::vector<sf::Vector2f> CollisionSystem::getWorldPoints(const CollisionComponent &col,
                                                          const TransformComponent &trans) const
{
    std::vector<sf::Vector2f> worldPoints;
    worldPoints.reserve(col.localPoints.size());
    sf::Transform transform = getComponentTransform(col, trans);

    for (const auto &point : col.localPoints) {
        worldPoints.push_back(transform.transformPoint(point));
    }
    return worldPoints;
}

sf::Vector2f CollisionSystem::getCenter(const CollisionComponent &col,
                                        const TransformComponent &trans) const
{
    // Calculate centroid
    const std::vector<sf::Vector2f> worldPoints = getWorldPoints(col, trans);
    if (worldPoints.empty()) {
        return trans.position;
    }
    sf::Vector2f sum(0.f, 0.f);
    for (const auto &point : worldPoints) {
        sum += point;
    }
    return sum / static_cast<float>(worldPoints.size());
}

sf::FloatRect CollisionSystem::getBounds(const CollisionComponent &col,
                                         const TransformComponent &trans) const
{
    std::vector<sf::Vector2f> worldPoints = getWorldPoints(col, trans);
    if (worldPoints.empty()) {
        return sf::FloatRect();
    }
    float minX = worldPoints[0].x, maxX = worldPoints[0].x;
    float minY = worldPoints[0].y, maxY = worldPoints[0].y;
    for (const auto &point : worldPoints) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }
    return sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
}

void CollisionSystem::processCombat(Entity *entityA, Entity *entityB)
{
    // Get components for A attacking B
    auto *weaponA = entityA->getComponent<WeaponComponent>();
    auto *healthB = entityB->getComponent<HealthComponent>();
    auto *ownerA = entityA->getComponent<OwnerComponent>();
    auto *ownerB = entityB->getComponent<OwnerComponent>();

    // Check for friendly fire
    if (ownerA && ownerA->owner == entityB) {
        skipPhysics = true;
        return;
    }
    if (ownerA && ownerB && ownerA->owner == ownerB->owner) {
        skipPhysics = true;
        return;
    }

    // Check for A attacking B
    if (weaponA && healthB) {
        skipPhysics = true;
        healthB->currentHealth -= weaponA->damage;
        std::cout << "Entity B health: " << healthB->currentHealth << std::endl;
        // TODO: Handle entity death, piercing and other stats
    }

    // Get components for B attacking A
    auto *weaponB = entityB->getComponent<WeaponComponent>();
    auto *healthA = entityA->getComponent<HealthComponent>();

    // Check for friendly fire (weapon's owner is not A)
    if (ownerB && ownerB->owner == entityA) {
        skipPhysics = true;
        return;
    }
    // Check for B attacking A
    if (weaponB && healthA) {
        skipPhysics = true;
        healthA->currentHealth -= weaponB->damage;
        std::cout << "Entity A health: " << healthA->currentHealth << std::endl;
        // TODO: Handle entity death, piercing and other stats
    }
}

/**
 * @brief Resolves a collision between ANY two entities using impulse and positional correction.
 * @param entityA Entity A
 * @param entityB Entity B
 * @param normal The collision normal, MUST point from entityA TO entityB.
 * @param depth The penetration depth.
 */
void CollisionSystem::handleCollision(Entity *entityA, Entity *entityB, const sf::Vector2f &normal,
                                      float depth)
{
    auto *kinA = entityA->getComponent<KinematicsComponent>();
    auto *kinB = entityB->getComponent<KinematicsComponent>();

    if (!kinA || !kinB)
        return;

    // Calculate inverse masses
    if (kinA->isStatic && kinB->isStatic) {
        return;
    }

    float invMassA = 0.f;
    float invMassB = 0.f;
    if (kinA->mass != 0.f && !std::isinf(kinA->mass)) {
        invMassA = 1.f / kinA->mass;
    }
    if (kinB->mass != 0.f && !std::isinf(kinB->mass)) {
        invMassB = 1.f / kinB->mass;
    }
    float totalInvMass = invMassA + invMassB;

    if (totalInvMass < EPSILON) {
        // if total mass is near 0, its probably 2 infinite objects, that can bounce
        // but still make sure that we are not going to move a static entity
        // 1.f if not static, 0.f if static
        invMassA = !kinA->isStatic;
        invMassB = !kinB->isStatic;

        totalInvMass = invMassA + invMassB;
    }

    // calc impulse
    sf::Vector2f velA = kinA->velocity;
    sf::Vector2f velB = kinB->velocity;
    sf::Vector2f relativeVel = velB - velA;

    // Calculate velocity along the normal
    float velAlongNormal = DotProduct(relativeVel, normal);

    bool approaching = velAlongNormal < 0.f;
    if (approaching) {
        float combinedRestitution = std::min(kinA->restitution, kinB->restitution);

        // Calculate impulse scalar (j)
        float j = -(1.f + combinedRestitution) * velAlongNormal;
        j /= totalInvMass;

        sf::Vector2f impulse = j * normal;
        entityA->applyCollisionImpulse(-impulse * invMassA);
        entityB->applyCollisionImpulse(impulse * invMassB);
    }

    // if correction pushes entity up, we know its grounded
    if (normal.y > 0.f) {
        kinA->isGrounded = true;
    }
    if (normal.y < 0.f) {
        kinB->isGrounded = true;
    }

    sf::Vector2f correction = std::max(depth - Constants::SLOP, 0.f) / totalInvMass *
                              Constants::CORRECTION_PER_FRAME * normal;

    entityA->resolveCollision(-correction * invMassA);
    entityB->resolveCollision(correction * invMassB);
}

CollisionResult CollisionSystem::checkCollision(const CollisionComponent &colA,
                                                const TransformComponent &transA,
                                                const CollisionComponent &colB,
                                                const TransformComponent &transB) const
{
    if (!getBounds(colA, transA).intersects(getBounds(colB, transB))) {
        return {false, {0.f, 0.f}, 0.f};
    }
    return polygonPolygonCollision(colA, transA, colB, transB);
}

CollisionResult CollisionSystem::polygonPolygonCollision(const CollisionComponent &colA,
                                                         const TransformComponent &transA,
                                                         const CollisionComponent &colB,
                                                         const TransformComponent &transB) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};

    std::vector<sf::Vector2f> pointsA = getWorldPoints(colA, transA);
    std::vector<sf::Vector2f> pointsB = getWorldPoints(colB, transB);

    if (pointsA.empty() || pointsB.empty()) {
        return result;
    }

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f minAxis;

    // Test axes from first polygon's edges
    for (size_t i = 0; i < pointsA.size(); i++) {
        sf::Vector2f p1 = pointsA[i];
        sf::Vector2f p2 = pointsA[(i + 1) % pointsA.size()];
        sf::Vector2f edge = p2 - p1;
        sf::Vector2f axis = Perpendicular(edge);

        if (hasSeparatingAxis(pointsA, pointsB, axis, minOverlap, minAxis)) {
            return result;
        }
    }

    // Test axes from second polygon's edges
    for (size_t i = 0; i < pointsB.size(); i++) {
        sf::Vector2f p1 = pointsB[i];
        sf::Vector2f p2 = pointsB[(i + 1) % pointsB.size()];
        sf::Vector2f edge = p2 - p1;
        sf::Vector2f axis = Perpendicular(edge);

        if (hasSeparatingAxis(pointsA, pointsB, axis, minOverlap, minAxis)) {
            return result;
        }
    }

    // Collision, sqrt depth and normalize normal only in the end here
    result.intersects = true;
    result.depth = std::sqrt(minOverlap);
    result.normal = VecNormalized(minAxis);

    return result;
}
