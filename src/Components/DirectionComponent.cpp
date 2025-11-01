#include "DirectionComponent.h"
#include "../MathUtils.h"
#include <cmath>

DirectionComponent::DirectionComponent(FacingDirection initialDirection)
    : m_facing(initialDirection)
{}

FacingDirection DirectionComponent::getFacing() const
{
    return m_facing;
}

bool DirectionComponent::isFacingRight() const
{
    return m_facing == FacingDirection::RIGHT;
}

bool DirectionComponent::isFacingLeft() const
{
    return m_facing == FacingDirection::LEFT;
}

void DirectionComponent::setFacing(FacingDirection direction)
{
    m_facing = direction;
}

void DirectionComponent::faceRight()
{
    m_facing = FacingDirection::RIGHT;
}

void DirectionComponent::faceLeft()
{
    m_facing = FacingDirection::LEFT;
}

void DirectionComponent::updateFromVelocity(const sf::Vector2f &velocity)
{
    if (velocity.x > EPSILON)
        m_facing = FacingDirection::RIGHT;
    else if (velocity.x < -EPSILON)
        m_facing = FacingDirection::LEFT;
    // If velocity.x is ~0, keep current facing
}

void DirectionComponent::updateFromTarget(const sf::Vector2f &currentPos,
                                          const sf::Vector2f &targetPos)
{
    float dx = targetPos.x - currentPos.x;
    if (dx > EPSILON)
        m_facing = FacingDirection::RIGHT;
    else if (dx < -EPSILON)
        m_facing = FacingDirection::LEFT;
}
