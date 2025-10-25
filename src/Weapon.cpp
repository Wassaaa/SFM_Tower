#include "Weapon.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/KinematicsComponent.h"
#include "Components/DirectionComponent.h"
#include "RenderSystem.h"

Weapon::Weapon(EntityType type)
    : m_type(type)
{
    initComponents();
}

void Weapon::initComponents()
{
    const EntityData &entityData = EntityManager::getInstance().getEntityData(m_type);
    const EntityConfig &config = Config::ENTITY_CONFIGS.at(m_type);

    if (auto *visual = entityData.getComponent<VisualComponent>())
        addComponent<VisualComponent>(*visual);
    if (auto *collision = entityData.getComponent<CollisionComponent>())
        addComponent<CollisionComponent>(*collision);
    if (auto *weapon = entityData.getComponent<WeaponComponent>())
        addComponent<WeaponComponent>(*weapon);
    if (auto *kinematics = entityData.getComponent<KinematicsComponent>())
        addComponent<KinematicsComponent>(*kinematics);

    addComponent<DirectionComponent>(FacingDirection::RIGHT);

    if (!config.animations.empty()) {
        auto &animComponent = addComponent<AnimationComponent>();
        for (const auto &[state, info] : config.animations) animComponent.addAnimation(state, info);
    }
}

void Weapon::update(float dt, sf::Vector2f ownerPos)
{
    if (auto weapon = getComponent<WeaponComponent>()) {
        weapon->update(dt);
    }

    auto *visual = getComponent<VisualComponent>();
    auto *collision = getComponent<CollisionComponent>();
    auto *kinematics = getComponent<KinematicsComponent>();

    if (kinematics && visual && collision) {
        // Initialize position to owner on first update
        if (!m_positionInitialized) {
            visual->setPosition(ownerPos);
            collision->setPosition(ownerPos);
            m_positionInitialized = true;
        }
        // Update orbit center for orbital/homing behaviors
        m_orbitCenter = ownerPos;

        kinematics->setTargetPoint(&m_orbitCenter);
        kinematics->update(dt, *visual, *collision);
    }
    else if (visual && collision) {
        visual->setPosition(ownerPos);
        collision->setPosition(ownerPos);
    }

    if (auto *anim = getComponent<AnimationComponent>()) {
        anim->update(dt);
    }

    // Apply animation frame and direction to visual (non-const context)
    if (visual) {
        RenderSystem::updateVisual(*visual, getComponent<AnimationComponent>(),
                                   getComponent<DirectionComponent>());
    }
}
void Weapon::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Just draw - visual already updated in update()
    if (auto *visual = getComponent<VisualComponent>()) {
        visual->draw(target, states);
    }
    if (auto *collision = getComponent<CollisionComponent>())
        collision->draw(target, states);
}
