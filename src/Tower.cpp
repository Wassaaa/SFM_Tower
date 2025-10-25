#include <array>

#include "Tower.h"
#include "Game.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"
#include "RenderSystem.h"

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
    const EntityConfig &config = Config::ENTITY_CONFIGS.at(EntityType::TOWER);

    // Add components from entity data
    if (auto *visual = entityData.getComponent<VisualComponent>())
        addComponent<VisualComponent>(*visual);
    if (auto *collision = entityData.getComponent<CollisionComponent>())
        addComponent<CollisionComponent>(*collision);

    addComponent<DirectionComponent>(FacingDirection::RIGHT);

    if (!config.animations.empty()) {
        auto &animComponent = addComponent<AnimationComponent>();
        for (const auto &[state, info] : config.animations) animComponent.addAnimation(state, info);
    }

    // Set position for visual and collision components
    if (auto *visual = getComponent<VisualComponent>())
        visual->setPosition(m_position);
    if (auto *collision = getComponent<CollisionComponent>())
        collision->setPosition(m_position);
}

void Tower::update(float &dt)
{
    RenderSystem::updateAnimationAndVisual(this, dt);

    // Update weapons with tower position
    for (auto &weapon : weapons) {
        weapon->update(dt, m_position);
    }
}

void Tower::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (auto *visual = getComponent<VisualComponent>()) {
        target.draw(*visual, states);
    }

    if (auto *collision = getComponent<CollisionComponent>()) {
        target.draw(*collision, states);
    }

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
