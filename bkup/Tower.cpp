#include "Tower.h"
#include "Game.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"

Tower::Tower(Game *pGame, const sf::Vector2f &position)
    : Entity(pGame, EntityType::TOWER, position)
{
    initVariables();
    initComponents();
}

void Tower::initVariables()
{
    m_currentState = EntityState::IDLE;
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
    setPosition(m_position);
}

void Tower::update(float dt)
{
    updateAnimationAndVisual(dt);

    // Update weapons
    for (auto &weapon : m_weapons) {
        weapon->update(dt);
    }
}

void Tower::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    Entity::draw(target, states);

    // Draw weapons
    for (const auto &weapon : m_weapons) {
        weapon->draw(target, states);
    }
}

void Tower::addWeapon(EntityType weaponType)
{
    m_weapons.emplace_back(std::make_unique<Weapon>(this, weaponType));
}
