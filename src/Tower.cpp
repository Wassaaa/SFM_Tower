#include <array>

#include "Tower.h"
#include "Game.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"

Tower::Tower(Game *pGame, const sf::Vector2f &position)
    : m_pGame(pGame)
    , m_position(position)
{
    initVariables();
    initComponents();
}

void Tower::initVariables()
{
    this->currentState = EntityState::IDLE;
}

void Tower::initComponents()
{
    const EntityData &entityData = EntityManager::getInstance().getEntityData(EntityType::TOWER);

    // Add components from entity data
    if (auto *visual = entityData.getComponent<VisualComponent>())
        addComponent<VisualComponent>(*visual);
    if (auto *collision = entityData.getComponent<CollisionComponent>())
        addComponent<CollisionComponent>(*collision);
    if (auto *animData = entityData.getComponent<AnimationData>()) {
        auto &animComponent = addComponent<AnimationComponent>(*getComponent<VisualComponent>());
        // Load animations from animation data
        for (const auto &[state, info] : animData->animations)
            animComponent.addAnimation(state, info);
    }

    // Set position for visual and collision components
    if (auto *visual = getComponent<VisualComponent>())
        visual->setPosition(m_position);
    if (auto *collision = getComponent<CollisionComponent>())
        collision->setPosition(m_position);
}

void Tower::update(float &dt)
{
    updateAnimation(dt);

    // Update weapons with tower position
    for (auto &weapon : weapons) {
        weapon->update(dt, m_position);
    }
}

void Tower::updateAnimation(float &dt)
{
    if (auto *anim = getComponent<AnimationComponent>()) {
        // Towers are stationary, so we just keep them in IDLE state
        if (currentState != EntityState::IDLE) {
            currentState = EntityState::IDLE;
            anim->playAnimation(currentState);
        }
        anim->update(dt);
    }
}

void Tower::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw visual component
    if (auto *visual = getComponent<VisualComponent>())
        target.draw(*visual, states);

    // Draw collision debug if enabled
    if (auto *collision = getComponent<CollisionComponent>()) {
        target.draw(*collision, states);
    }

    // Draw weapons
    for (const auto &weapon : weapons) {
        weapon->draw(target, states);
    }
}

sf::Vector2f Tower::getPosition() const
{
    if (auto *visual = getComponent<VisualComponent>())
        return visual->getPosition();
    return m_position;
}

sf::Vector2f Tower::getCenter() const
{
    if (auto *collision = getComponent<CollisionComponent>())
        return collision->getCenter();
    return m_position;
}

void Tower::addWeapon(EntityType weaponType)
{
    weapons.emplace_back(std::make_unique<Weapon>(weaponType));
}
