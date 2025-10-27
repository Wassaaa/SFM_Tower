#include "CollisionComponent.h"
#include "../Constants.h"
#include "../MathUtils.h"
#include <cmath>
#include <limits>
#include <iostream>

CollisionComponent::CollisionComponent(const CollisionComponentData &data)
    : m_data(data)
    , m_type(data.type)
    , m_radius(data.radius)
    , m_localPoints(data.points)
{
    applyTransforms();
    this->setDebugDraw(Constants::DEBUG_DRAW);
}

void CollisionComponent::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (!m_debugDraw) {
        return;
    }

    states.transform *= getTransform();

    // Set outline color based on collision state
    sf::Color outlineColor = m_isColliding ? sf::Color::Red : sf::Color::White;

    if (m_type == CollisionShape::Circle) {
        sf::CircleShape circle(m_radius);
        circle.setOrigin(m_radius, m_radius);
        circle.setFillColor(m_data.debugColor);
        circle.setOutlineColor(outlineColor);
        circle.setOutlineThickness(1.0f);
        target.draw(circle, states);
    }
    else {
        sf::ConvexShape polygon;
        polygon.setPointCount(m_localPoints.size());
        for (size_t i = 0; i < m_localPoints.size(); i++) {
            polygon.setPoint(i, m_localPoints[i]);
        }
        polygon.setFillColor(m_data.debugColor);
        polygon.setOutlineColor(outlineColor);
        polygon.setOutlineThickness(1.0f);
        target.draw(polygon, states);
    }
}

void CollisionComponent::applyTransforms()
{
    setScale(m_data.scale);
    setOrigin(m_data.origin);
    setPosition(m_data.offset);
    setRotation(m_data.rotation);
}

sf::Vector2f CollisionComponent::getCenter() const
{
    if (m_type == CollisionShape::Circle) {
        return getTransform().transformPoint(sf::Vector2f(0.f, 0.f));
    }

    const std::vector<sf::Vector2f> worldPoints = getWorldPoints();
    if (worldPoints.empty()) {
        return getTransform().transformPoint(m_data.origin);
    }

    sf::Vector2f sum(0.f, 0.f);
    for (const auto &point : worldPoints) {
        sum.x += point.x;
        sum.y += point.y;
    }

    float invCount = 1.f / static_cast<float>(worldPoints.size());
    return sf::Vector2f(sum.x * invCount, sum.y * invCount);
}

std::vector<sf::Vector2f> CollisionComponent::getWorldPoints() const
{
    std::vector<sf::Vector2f> worldPoints;
    worldPoints.reserve(m_localPoints.size());
    sf::Transform transform = getTransform();

    for (const auto &point : m_localPoints) {
        worldPoints.push_back(transform.transformPoint(point));
    }
    return worldPoints;
}

float CollisionComponent::getWorldRadius() const
{
    // Account for scale (use average of x and y scale)
    sf::Vector2f scale = getScale();
    return m_radius * (scale.x + scale.y) * 0.5f;
}

bool CollisionComponent::intersects(const CollisionComponent &other) const
{
    return checkCollision(other).intersects;
}

CollisionResult CollisionComponent::checkCollision(const CollisionComponent &other) const
{
    // Broad AABB check
    if (!getBounds().intersects(other.getBounds())) {
        return {false, {0.f, 0.f}, 0.f};
    }

    // Narrow check based on shape types
    if (m_type == CollisionShape::Circle && other.m_type == CollisionShape::Circle) {
        return circleCircleCollision(other);
    }
    else if (m_type == CollisionShape::Circle || other.m_type == CollisionShape::Circle) {
        // One is circle, one is polygon
        if (m_type == CollisionShape::Circle) {
            CollisionResult result = circlePolygonCollision(other);
            // Flip normal since we're checking from polygon's perspective
            result.normal = -result.normal;
            return result;
        }
        else {
            return other.circlePolygonCollision(*this);
        }
    }
    else {
        // Both polygons
        return polygonPolygonCollision(other);
    }
}

CollisionResult CollisionComponent::circleCircleCollision(const CollisionComponent &other) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};
    sf::Vector2f center1 = getCenter();
    sf::Vector2f center2 = other.getCenter();
    float radius1 = getWorldRadius();
    float radius2 = other.getWorldRadius();

    sf::Vector2f diff = center2 - center1;
    float distSq = VecLengthSquared(diff);
    float radiusSum = radius1 + radius2;

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

CollisionResult CollisionComponent::circlePolygonCollision(const CollisionComponent &other) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};

    // Circle is 'this', polygon is 'other'
    sf::Vector2f circleCenter = getCenter();
    float radius = getWorldRadius();
    std::vector<sf::Vector2f> polyPoints = other.getWorldPoints();

    if (polyPoints.empty()) {
        return result;
    }

    // Find closest point on polygon to circle center
    float minDistSq = std::numeric_limits<float>::max();
    sf::Vector2f closestPoint;
    float radiusSq = radius * radius;

    for (size_t i = 0; i < polyPoints.size(); i++) {
        sf::Vector2f p1 = polyPoints[i];
        sf::Vector2f p2 = polyPoints[(i + 1) % polyPoints.size()];

        sf::Vector2f pointOnEdge = ClosestPointOnSegment(circleCenter, p1, p2);
        float distSq = DistanceSquared(circleCenter, pointOnEdge);

        if (distSq < minDistSq) {
            minDistSq = distSq;
            closestPoint = pointOnEdge;
        }
    }

    if (minDistSq >= radiusSq) {
        return result;
    }

    result.intersects = true;
    float dist = std::sqrt(minDistSq);
    result.depth = radius - dist;

    sf::Vector2f diff = circleCenter - closestPoint;
    if (dist > EPSILON)
        result.normal = diff / dist;
    else
        result.normal = sf::Vector2f(1.f, 0.f);

    return result;
}

bool CollisionComponent::hasSeparatingAxis(const std::vector<sf::Vector2f> &points1,
                                           const std::vector<sf::Vector2f> &points2,
                                           const sf::Vector2f &axis, float &minOverlap,
                                           sf::Vector2f &minAxis) const
{
    float min1, max1, min2, max2;
    projectOntoAxis(points1, axis, min1, max1);
    projectOntoAxis(points2, axis, min2, max2);

    // Check for separation
    if (max1 < min2 || max2 < min1) {
        return true; // no collision
    }

    float overlap1 = max1 - min2; // (moving left on axis)
    float overlap2 = max2 - min1; // (moving right on axis)
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
        float center1 = (min1 + max1) * 0.5f;
        float center2 = (min2 + max2) * 0.5f;

        // Point the axis from 1 to 2
        if (center1 < center2) {
            minAxis = axis;
        }
        else {
            minAxis = -axis;
        }
    }

    return false;
}

CollisionResult CollisionComponent::polygonPolygonCollision(const CollisionComponent &other) const
{
    CollisionResult result = {false, {0.f, 0.f}, 0.f};

    std::vector<sf::Vector2f> points1 = getWorldPoints();
    std::vector<sf::Vector2f> points2 = other.getWorldPoints();

    if (points1.empty() || points2.empty()) {
        return result;
    }

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f minAxis;

    // Test axes from first polygon's edges
    for (size_t i = 0; i < points1.size(); i++) {
        sf::Vector2f p1 = points1[i];
        sf::Vector2f p2 = points1[(i + 1) % points1.size()];
        sf::Vector2f edge = p2 - p1;
        sf::Vector2f axis = Perpendicular(edge);

        if (hasSeparatingAxis(points1, points2, axis, minOverlap, minAxis)) {
            return result;
        }
    }

    // Test axes from second polygon's edges
    for (size_t i = 0; i < points2.size(); i++) {
        sf::Vector2f p1 = points2[i];
        sf::Vector2f p2 = points2[(i + 1) % points2.size()];
        sf::Vector2f edge = p2 - p1;
        sf::Vector2f axis = Perpendicular(edge);

        if (hasSeparatingAxis(points1, points2, axis, minOverlap, minAxis)) {
            return result;
        }
    }

    // Collision, sqrt depth and normalize normal only in the end here
    result.intersects = true;
    result.depth = std::sqrt(minOverlap);
    result.normal = VecNormalized(minAxis);

    // Ensure normal points from this to other using center direction
    // sf::Vector2f center1 = getCenter();
    // sf::Vector2f center2 = other.getCenter();
    // if (DotProduct(result.normal, center2 - center1) < 0.f) {
    //     result.normal = -result.normal;
    // }

    return result;
}

void CollisionComponent::projectOntoAxis(const std::vector<sf::Vector2f> &points,
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

sf::FloatRect CollisionComponent::getBounds() const
{
    if (m_type == CollisionShape::Circle) {
        sf::Vector2f center = getCenter();
        float radius = getWorldRadius();
        return sf::FloatRect(center.x - radius, center.y - radius, radius * 2, radius * 2);
    }
    else {
        std::vector<sf::Vector2f> worldPoints = getWorldPoints();
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
