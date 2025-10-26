#include "Entity.h"
#include "Game.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/KinematicsComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"
#include "InputHandler.h"
#include <cmath>

Entity::Entity(Game *pGame, EntityType type, const sf::Vector2f &position)
    : m_pGame(pGame)
    , m_type(type)
    , m_position(position)
{
    initComponents();
}

void Entity::initComponents()
{
    const EntityData &entityData = EntityManager::getInstance().getEntityData(m_type);

    // Add components from entity data
    if (auto *visual = entityData.getComponent<VisualComponent>())
        addComponent<VisualComponent>(*visual);
    if (auto *collision = entityData.getComponent<CollisionComponent>())
        addComponent<CollisionComponent>(*collision);
    if (auto *kinematics = entityData.getComponent<KinematicsComponent>())
        addComponent<KinematicsComponent>(*kinematics);

    // Set initial position for components
    setPosition(m_position);
}

void Entity::setPosition(const sf::Vector2f &pos)
{
    m_position = pos;

    if (auto *visual = getComponent<VisualComponent>())
        visual->setPosition(m_position);
    if (auto *collision = getComponent<CollisionComponent>())
        collision->setPosition(m_position);
}

void Entity::update(float dt)
{
    // Update kinematics if it exists
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        kinematics->update(dt, *this);

        // Sync position from components (visual takes priority, then collision)
        if (auto *visual = getComponent<VisualComponent>()) {
            m_position = visual->getPosition();
        }
        else if (auto *collision = getComponent<CollisionComponent>()) {
            m_position = collision->getPosition();
        }

        // Keep within screen bounds
        if (auto *collision = getComponent<CollisionComponent>()) {
            const float halfSize = 25.f;
            if (m_position.x - halfSize < 0.f)
                m_position.x = halfSize;
            if (m_position.x + halfSize > Constants::SCREEN_WIDTH)
                m_position.x = Constants::SCREEN_WIDTH - halfSize;
            if (m_position.y - halfSize < 0.f)
                m_position.y = halfSize;
            if (m_position.y + halfSize > Constants::SCREEN_HEIGHT)
                m_position.y = Constants::SCREEN_HEIGHT - halfSize;

            setPosition(m_position);
        }
    }
}

void Entity::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (auto *visual = getComponent<VisualComponent>()) {
        target.draw(*visual, states);
    }

    if (auto *collision = getComponent<CollisionComponent>()) {
        target.draw(*collision, states);
    }
}

void Entity::handleInput(float deltaTime, const InputState &input)
{
    // Apply acceleration based on input
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        sf::Vector2f acceleration(0.f, 0.f);
        const float accelerationForce = 1500.f; // Acceleration force

        if (input.moveUp)
            acceleration.y -= accelerationForce;
        if (input.moveDown)
            acceleration.y += accelerationForce;
        if (input.moveLeft)
            acceleration.x -= accelerationForce;
        if (input.moveRight)
            acceleration.x += accelerationForce;

        // Normalize diagonal movement to keep same acceleration magnitude
        float length = std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
        if (length > 0.f) {
            acceleration /= length;
            acceleration *= accelerationForce;
        }

        kinematics->setAcceleration(acceleration);
    }
}

void Entity::setColliding(bool colliding)
{
    if (auto *collision = getComponent<CollisionComponent>()) {
        collision->setColliding(colliding);
    }
}

sf::Vector2f Entity::getCenter() const
{
    if (auto *collision = getComponent<CollisionComponent>())
        return collision->getCenter();
    return m_position;
}

void Entity::updateAnimationAndVisual(float dt)
{
    // Update animation state
    if (auto *anim = getComponent<AnimationComponent>()) {
        anim->update(dt);

        // Apply animation frame to visual
        if (auto *visual = getComponent<VisualComponent>()) {
            visual->setTextureRect(anim->getCurrentFrame());
        }
    }

    // Apply direction flipping to visual
    if (auto *visual = getComponent<VisualComponent>()) {
        if (auto *direction = getComponent<DirectionComponent>()) {
            sf::Vector2f scale = visual->getScale();
            float absScaleX = std::abs(scale.x);

            if (direction->isFacingLeft()) {
                if (scale.x > 0)
                    visual->setScale(-absScaleX, scale.y);
            }
            else {
                if (scale.x < 0)
                    visual->setScale(absScaleX, scale.y);
            }
        }
    }
}

void Entity::resolveCollision(const sf::Vector2f &pushVector)
{
    // Simple collision response - just push the entity away
    m_position += pushVector;
    setPosition(m_position);
}
