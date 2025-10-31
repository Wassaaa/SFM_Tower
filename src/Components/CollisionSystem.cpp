#include "CollisionSystem.h"
#include "../Entity.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "KinematicsComponent.h"
#include "../MathUtils.h"
#include <limits>
#include <cmath>
#include <iostream>

void CollisionSystem::update(std::vector<std::unique_ptr<Entity>> &entities, float deltaTime)
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
                collision1->isColliding = true;
                collision2->isColliding = true;

                float mass1 = kin1 ? kin1->mass : std::numeric_limits<float>::infinity();
                float mass2 = kin2 ? kin2->mass : std::numeric_limits<float>::infinity();

                if (std::isinf(mass1) && std::isinf(mass2)) {
                    handleStaticStaticCollision(entities[i].get(), entities[j].get(), result.normal,
                                                result.depth);
                }
                else if (std::isinf(mass1)) {
                    handleStaticDynamicCollision(entities[i].get(), entities[j].get(),
                                                 result.normal, result.depth);
                }
                else if (std::isinf(mass2)) {
                    handleStaticDynamicCollision(entities[j].get(), entities[i].get(),
                                                 -result.normal, result.depth);
                }
                else {
                    handleDynamicDynamicCollision(entities[i].get(), entities[j].get(),
                                                  result.normal, result.depth);
                }
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

float CollisionSystem::getWorldRadius(const CollisionComponent &col,
                                      const TransformComponent &trans) const
{
    // Averaging entity scale
    sf::Vector2f finalScale =
        sf::Vector2f(trans.scale.x * col.scale.x, trans.scale.y * col.scale.y);
    return col.radius * (finalScale.x + finalScale.y) * 0.5f;
}

sf::Vector2f CollisionSystem::getCenter(const CollisionComponent &col,
                                        const TransformComponent &trans) const
{
    if (col.type == CollisionShape::Circle) {
        // Center of a circle is its origin point, transformed
        return getComponentTransform(col, trans).transformPoint(0, 0);
    }

    // For polygons, calculate centroid
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
    if (col.type == CollisionShape::Circle) {
        sf::Vector2f center = getCenter(col, trans);
        float radius = getWorldRadius(col, trans);
        return sf::FloatRect(center.x - radius, center.y - radius, radius * 2, radius * 2);
    }
    else {
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
}

void CollisionSystem::handleStaticDynamicCollision(Entity *staticEntity, Entity *dynamicEntity,
                                                   const sf::Vector2f &normal, float depth)
{
    auto *kin = dynamicEntity->getComponent<KinematicsComponent>();
    if (!kin)
        return;

    sf::Vector2f velocity = kin->velocity;
    float velAlongNormal = DotProduct(velocity, normal);

    // Apply impulse if approaching
    if (velAlongNormal < EPSILON) {
        const float restitution = 1.0f;
        float impulseMagnitude = -(1.f + restitution) * velAlongNormal;
        sf::Vector2f impulse = normal * impulseMagnitude;
        dynamicEntity->applyCollisionImpulse(impulse);
    }

    sf::Vector2f pushVector = normal * depth * 1.2f;
    dynamicEntity->resolveCollision(pushVector);
}

void CollisionSystem::handleStaticStaticCollision(Entity *entityA, Entity *entityB,
                                                  const sf::Vector2f &normal, float depth)
{
    auto *kinA = entityA->getComponent<KinematicsComponent>();
    auto *kinB = entityB->getComponent<KinematicsComponent>();

    if (!kinA || !kinB)
        return;

    // both pure static, do nothing, shouldn't happen
    if (kinA->isStatic && kinB->isStatic)
        return;

    // both infinite mass, no pure static
    if (!kinA->isStatic && !kinB->isStatic)
        return handleDynamicDynamicCollision(entityA, entityB, normal, depth);

    // one pure static, one infinite mass
    if (kinA->isStatic)
        return handleStaticDynamicCollision(entityA, entityB, normal, depth);
    if (kinB->isStatic)
        return handleStaticDynamicCollision(entityB, entityA, normal, depth);
}

void CollisionSystem::handleDynamicDynamicCollision(Entity *entityA, Entity *entityB,
                                                    const sf::Vector2f &normal, float depth)
{
    auto *kinA = entityA->getComponent<KinematicsComponent>();
    auto *kinB = entityB->getComponent<KinematicsComponent>();

    if (!kinA || !kinB)
        return;

    float massA = kinA->mass;
    float massB = kinB->mass;
    if (massA == std::numeric_limits<float>::infinity() &&
        massB == std::numeric_limits<float>::infinity()) {
        massA = 1.f;
        massB = 1.f;
    }
    float totalMass = massA + massB;

    // Get relative velocity along collision normal
    sf::Vector2f velA = kinA->velocity;
    sf::Vector2f velB = kinB->velocity;
    sf::Vector2f relativeVel = velA - velB;

    // Calculate velocity along normal (dot product)
    // Normal points from entityA -> entityB, so positive velAlongNormal means approaching
    float velAlongNormal = DotProduct(relativeVel, normal);

    // Apply impulse if approaching OR resting
    if (velAlongNormal > -EPSILON) {
        // Calculate impulse with baseline push to handle resting contacts
        float impulseMagnitude = (velAlongNormal + EPSILON) / totalMass;
        sf::Vector2f impulse = normal * impulseMagnitude;

        // Apply impulse proportional to mass
        entityA->applyCollisionImpulse(-impulse * massB);
        entityB->applyCollisionImpulse(impulse * massA);
    }

    // Positional correction split by mass ratio
    float ratioA = massA / totalMass;
    float ratioB = massB / totalMass;
    sf::Vector2f separationA = -normal * depth * ratioA;
    sf::Vector2f separationB = normal * depth * ratioB;

    entityA->resolveCollision(separationA);
    entityB->resolveCollision(separationB);
}

CollisionResult CollisionSystem::checkCollision(const CollisionComponent &colA,
                                                const TransformComponent &transA,
                                                const CollisionComponent &colB,
                                                const TransformComponent &transB) const
{
    if (!getBounds(colA, transA).intersects(getBounds(colB, transB))) {
        return {false, {0.f, 0.f}, 0.f};
    }

    if (colA.type == CollisionShape::Circle && colB.type == CollisionShape::Circle) {
        return circleCircleCollision(colA, transA, colB, transB);
    }
    else if (colA.type == CollisionShape::Circle) {
        auto result = circlePolygonCollision(colA, transA, colB, transB);
        result.normal = -result.normal; // Flip normal
        return result;
    }
    else if (colB.type == CollisionShape::Circle) {
        return circlePolygonCollision(colB, transB, colA, transA);
    }
    else {
        return polygonPolygonCollision(colA, transA, colB, transB);
    }
}

CollisionResult CollisionSystem::circleCircleCollision(const CollisionComponent &colA,
                                                       const TransformComponent &transA,
                                                       const CollisionComponent &colB,
                                                       const TransformComponent &transB) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};
    sf::Vector2f centerA = getCenter(colA, transA);
    sf::Vector2f centerB = getCenter(colB, transB);
    float radiusA = getWorldRadius(colA, transA);
    float radiusB = getWorldRadius(colB, transB);

    sf::Vector2f diff = centerB - centerA;
    float distSq = VecLengthSquared(diff);
    float radiusSum = radiusA + radiusB;

    if (distSq >= radiusSum * radiusSum) {
        return result;
    }

    result.intersects = true;
    float dist = std::sqrt(distSq);
    result.depth = radiusSum - dist;

    if (dist > EPSILON)
        result.normal = diff / dist;
    else
        result.normal = sf::Vector2f(1.f, 0.f);

    return result;
}

CollisionResult CollisionSystem::circlePolygonCollision(const CollisionComponent &circleCol,
                                                        const TransformComponent &circleTrans,
                                                        const CollisionComponent &polyCol,
                                                        const TransformComponent &polyTrans) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};

    // Circle is 'this', polygon is 'other'
    sf::Vector2f circleCenter = getCenter(circleCol, circleTrans);
    float radius = getWorldRadius(circleCol, circleTrans);
    std::vector<sf::Vector2f> polyPoints = getWorldPoints(polyCol, polyTrans);

    if (polyPoints.empty()) {
        return result;
    }

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f minAxis;
    sf::Vector2f polyCenter = getCenter(polyCol, polyTrans);

    for (size_t i = 0; i < polyPoints.size(); i++) {
        sf::Vector2f p1 = polyPoints[i];
        sf::Vector2f p2 = polyPoints[(i + 1) % polyPoints.size()];
        sf::Vector2f edge = p2 - p1;
        sf::Vector2f axis = Perpendicular(edge);

        // Normalize the axis to correctly project the circle's radius
        axis = VecNormalized(axis);
        if (VecLengthSquared(axis) < EPSILON * EPSILON) {
            continue;
        }

        // Project the polygon
        float minPoly, maxPoly;
        projectOntoAxis(polyPoints, axis, minPoly, maxPoly);

        // Project the circle
        float circleProj = DotProduct(circleCenter, axis);
        float minCircle = circleProj - radius;
        float maxCircle = circleProj + radius;

        // Check for a separating axis
        if (maxCircle < minPoly || maxPoly < minCircle) {
            return result;
        }

        // No gap, calculate the overlap
        float overlap = std::min(maxCircle - minPoly, maxPoly - minCircle);

        // Track the minimum overlap
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axis;
        }
    }

    sf::Vector2f closestVertex;
    float minVertexDistSq = std::numeric_limits<float>::max();

    // Find the polygon vertex closest to the circle's center
    for (const auto &vertex : polyPoints) {
        float distSq = DistanceSquared(circleCenter, vertex);
        if (distSq < minVertexDistSq) {
            minVertexDistSq = distSq;
            closestVertex = vertex;
        }
    }

    // Create the axis from the center to the closest vertex
    sf::Vector2f axis = closestVertex - circleCenter;

    // Check if circle center is on a vertex
    if (VecLengthSquared(axis) < EPSILON * EPSILON) {
        // This is a rare case, but it's a definite collision.
    }
    else {
        axis = VecNormalized(axis);

        // Project the polygon
        float minPoly, maxPoly;
        projectOntoAxis(polyPoints, axis, minPoly, maxPoly);

        // Project the circle
        float circleProj = DotProduct(circleCenter, axis);
        float minCircle = circleProj - radius;
        float maxCircle = circleProj + radius;

        // Check for a separating axis
        if (maxCircle < minPoly || maxPoly < minCircle) {
            return result; // Found a gap, no collision
        }

        // Calculate overlap
        float overlap = std::min(maxCircle - minPoly, maxPoly - minCircle);

        // Track the minimum overlap
        if (overlap < minOverlap) {
            minOverlap = overlap;
            minAxis = axis;
        }
    }

    result.intersects = true;
    result.depth = minOverlap;

    sf::Vector2f toCircle = circleCenter - polyCenter;
    if (DotProduct(minAxis, toCircle) < 0.f) {
        minAxis = -minAxis;
    }

    result.normal = minAxis;

    return result;
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

// ... (Implementations for circleCircleCollision, circlePolygonCollision,
//      polygonPolygonCollision, projectOntoAxis, hasSeparatingAxis,
//      handleStaticDynamicCollision, handleDynamicDynamicCollision) ...
//
// (These are large blocks of math logic. You can copy-paste them
// directly from your old CollisionComponent.cpp and adapt them to
// take the components as parameters, just like we did with getBounds/getCenter)
