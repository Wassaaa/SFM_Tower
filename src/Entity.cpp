#include "Entity.h"
#include "Game.h"
#include "Config/EntityManager.h"
// NEW INCLUDES
#include "Components/TransformComponent.h"
#include "Components/KinematicsComponent.h"
#include "Components/CollisionComponent.h"
// OLD INCLUDES (for refactoring)
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"
#include "Components/VisualComponent.h" // Will be refactored
#include "InputHandler.h"
#include "MathUtils.h"
#include <cmath>

Entity::Entity(Game *pGame, EntityType type, const sf::Vector2f &position)
    : m_pGame(pGame)
    , m_type(type)
    , m_initialPosition(position)
{
    initComponents();
}

void Entity::initComponents()
{
    const EntityData &entityData = EntityManager::getInstance().getEntityData(m_type);
    const EntityConfig &config = Config::ENTITY_CONFIGS.at(m_type);

    sf::Vector2f baseScale{1, 1};
    float rotation{0.f};
    if (config.visual.has_value()) {
        baseScale = config.visual.value().scale;
        rotation = config.visual.value().rotation;
    }

    auto &transform = addComponent<TransformComponent>(m_initialPosition, rotation, baseScale);

    if (auto *kinematicsData = entityData.getComponent<KinematicsComponent>()) {
        auto &kinematics = addComponent<KinematicsComponent>(*kinematicsData);
        kinematics.baseScale = transform.scale;
    }

    if (auto *collisionData = entityData.getComponent<CollisionComponent>()) {
        addComponent<CollisionComponent>(*collisionData);
    }

    if (auto *visualData = entityData.getComponent<VisualComponent>()) {
        addComponent<VisualComponent>(*visualData);
    }

    // add others later
}

void Entity::draw(sf::RenderTarget &target, sf::RenderStates states) const {}

sf::Vector2f Entity::getPosition() const
{
    if (auto *transform = getComponent<TransformComponent>()) {
        return transform->position;
    }
    return m_initialPosition;
}

void Entity::setPosition(const sf::Vector2f &pos)
{
    if (auto *transform = getComponent<TransformComponent>()) {
        transform->position = pos;
    }
}

void Entity::handleInput(float deltaTime, const InputState &input)
{
    // This now writes acceleration data to the component.
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

        // Normalize
        float length = std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
        if (length > 0.f) {
            acceleration /= length;
            acceleration *= accelerationForce;
        }

        kinematics->acceleration = acceleration;
    }
}

void Entity::setColliding(bool colliding)
{
    if (auto *collision = getComponent<CollisionComponent>()) {
        collision->isColliding = colliding;
    }
}

void Entity::enableCollision(bool state)
{
    if (auto *collision = getComponent<CollisionComponent>()) {
        collision->setEnabled(state);
    }
}

void Entity::setMass(float mass)
{
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        kinematics->mass = mass;
    }
}

void Entity::setStatic(bool isStatic)
{
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        if (isStatic) {
            kinematics->acceleration = sf::Vector2f(0, 0);
            kinematics->velocity = sf::Vector2f(0, 0);
        }
        kinematics->isStatic = isStatic;
    }
}

sf::Vector2f Entity::getCenter() const
{
    if (auto *transform = getComponent<TransformComponent>()) {
        return transform->position;
    }
    return m_initialPosition;
}

void Entity::resolveCollision(const sf::Vector2f &pushVector)
{
    if (auto *transform = getComponent<TransformComponent>()) {
        transform->position += pushVector;
    }
}

void Entity::applyCollisionImpulse(const sf::Vector2f &velocityChange)
{
    // This now applies to the KinematicsComponent
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        kinematics->velocity += velocityChange;
    }
}

// void Entity::updateAnimationAndVisual(float dt)
// {
//     // Update animation state
//     if (auto *anim = getComponent<AnimationComponent>()) {
//         anim->update(dt);

//         // Apply animation frame to visual
//         if (auto *visual = getComponent<VisualComponent>()) {
//             visual->setTextureRect(anim->getCurrentFrame());
//         }
//     }

//     // Apply direction flipping to visual
//     if (auto *visual = getComponent<VisualComponent>()) {
//         if (auto *direction = getComponent<DirectionComponent>()) {
//             sf::Vector2f scale = visual->getScale();
//             float absScaleX = std::abs(scale.x);

//             if (direction->isFacingLeft()) {
//                 if (scale.x > 0)
//                     visual->setScale(-absScaleX, scale.y);
//             }
//             else {
//                 if (scale.x < 0)
//                     visual->setScale(absScaleX, scale.y);
//             }
//         }
//     }
// }
