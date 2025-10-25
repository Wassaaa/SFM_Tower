#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>

enum class FacingDirection
{
    RIGHT = 1,
    LEFT = -1
};

// Component to track which way an entity is facing
class DirectionComponent : public Component
{
public:
    DirectionComponent(FacingDirection initialDirection = FacingDirection::RIGHT);
    ~DirectionComponent() = default;

    virtual const char *getName() const override { return "DirectionComponent"; }

    // Getters
    FacingDirection getFacing() const;
    bool isFacingRight() const;
    bool isFacingLeft() const;

    // Setters
    void setFacing(FacingDirection direction);
    void faceRight();
    void faceLeft();

    void updateFromVelocity(const sf::Vector2f &velocity);
    void updateFromTarget(const sf::Vector2f &currentPos, const sf::Vector2f &targetPos);

private:
    FacingDirection m_facing;
};
