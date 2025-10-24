#include "CollisionComponent.h"
#include "../Constants.h"
#include "../MathUtils.h"

CollisionComponent::CollisionComponent(const CollisionComponentData &data)
    : m_data(data)
{
    initShape();
    applyTransforms();
    this->setDebugDraw(Constants::DEBUG_DRAW);
}

void CollisionComponent::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_debugDraw) {
        states.transform *= getTransform();
        target.draw(m_shape, states);
    }
}

void CollisionComponent::initShape()
{
    m_shape.setSize(m_data.size);
    m_shape.setFillColor(sf::Color(m_data.debugColor.r, m_data.debugColor.g, m_data.debugColor.b,
                                   m_data.debugColor.a / 2));
    m_shape.setOutlineColor(m_data.debugColor);
    m_shape.setOutlineThickness(1.0f);
}

void CollisionComponent::applyTransforms()
{
    setScale(m_data.scale);
    setOrigin(m_data.origin);
    setPosition(m_data.offset);
    setRotation(m_data.rotation);
}

bool CollisionComponent::intersectsOBB(const CollisionComponent &other) const
{
    std::array<sf::Vector2f, 4> corners1, corners2;
    std::array<sf::Vector2f, 4> axes;

    getWorldCorners(corners1);
    other.getWorldCorners(corners2);

    axes[0] = VecNormalized(corners1[1] - corners1[0]);
    axes[1] = VecNormalized(corners1[3] - corners1[0]);
    axes[2] = VecNormalized(corners2[1] - corners2[0]);
    axes[3] = VecNormalized(corners2[3] - corners2[0]);

    for (auto axis : axes) {
        float min1, max1, min2, max2;
        projectOntoAxis(corners1, axis, min1, max1);
        projectOntoAxis(corners2, axis, min2, max2);
        if (max1 < min2 || max2 < min1)
            return false;
    }
    return true;
}

void CollisionComponent::getWorldCorners(std::array<sf::Vector2f, 4> &corners) const
{
    sf::Transform transform = getTransform();
    sf::FloatRect local = m_shape.getLocalBounds();

    corners[0] = transform.transformPoint(local.left, local.top);
    corners[1] = transform.transformPoint(local.left + local.width, local.top);
    corners[2] = transform.transformPoint(local.left + local.width, local.top + local.height);
    corners[3] = transform.transformPoint(local.left, local.top + local.height);
}

void CollisionComponent::projectOntoAxis(const std::array<sf::Vector2f, 4> &corners,
                                         const sf::Vector2f &axis, float &min, float &max) const
{
    // Initialize with first corner's projection
    min = max = DotProduct(corners[0], axis);

    // Check remaining corners
    for (size_t i = 1; i < corners.size(); i++) {
        float projection = DotProduct(corners[i], axis);
        if (projection < min)
            min = projection;
        if (projection > max)
            max = projection;
    }
}

bool CollisionComponent::intersects(const CollisionComponent &other) const
{
    if (getBounds().intersects(other.getBounds())) {
        return intersectsOBB(other);
    }
    return false;
}

sf::FloatRect CollisionComponent::getBounds() const
{
    return getTransform().transformRect(m_shape.getGlobalBounds());
}
