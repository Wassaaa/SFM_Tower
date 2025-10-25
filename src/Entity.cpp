#include "Entity.h"
#include "Game.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/KinematicsComponent.h"
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
        m_position += kinematics->getVelocity() * dt;

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
        }

        setPosition(m_position);
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
    // Set velocity based on input
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        sf::Vector2f velocity(0.f, 0.f);
        const float speed = 200.f;

        if (input.moveUp)
            velocity.y -= speed;
        if (input.moveDown)
            velocity.y += speed;
        if (input.moveLeft)
            velocity.x -= speed;
        if (input.moveRight)
            velocity.x += speed;

        // Normalize diagonal movement
        float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (length > 0.f) {
            velocity /= length;
            velocity *= speed;
        }

        kinematics->setVelocity(velocity);
    }
}

void Entity::setColliding(bool colliding)
{
    if (auto *collision = getComponent<CollisionComponent>()) {
        collision->setColliding(colliding);
    }
}
