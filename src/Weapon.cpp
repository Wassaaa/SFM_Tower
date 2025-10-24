#include "Weapon.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/KinematicsComponent.h"

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
    
    // Add animations directly from config
    if (!config.animations.empty()) {
        auto &animComponent = addComponent<AnimationComponent>(*getComponent<VisualComponent>());
        for (const auto &[state, info] : config.animations)
            animComponent.addAnimation(state, info);
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
        
        // KinematicsComponent handles all transforms (position, rotation, scale)
        kinematics->update(dt, *visual, *collision);
    }
    else if (visual && collision) {
        // Fallback for weapons without kinematics - just stay attached to owner
        visual->setPosition(ownerPos);
        collision->setPosition(ownerPos);
    }
}

void Weapon::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (auto visual = getComponent<VisualComponent>())
        visual->draw(target, states);
    if (auto collision = getComponent<CollisionComponent>())
        collision->draw(target, states);
}
