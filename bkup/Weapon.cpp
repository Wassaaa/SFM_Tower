#include "Weapon.h"
#include "Config/EntityManager.h"
#include "Components/VisualComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/KinematicsComponent.h"
#include "Components/DirectionComponent.h"

Weapon::Weapon(Entity *owner, EntityType type)
    : Entity(owner->getGame(), type, owner->getPosition())
    , m_owner(owner)
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

    // Initialize position to owner's position
    setPosition(m_owner->getPosition());
}

void Weapon::update(float dt)
{
    auto *weaponComp = getComponent<WeaponComponent>();
    if (weaponComp) {
        weaponComp->update(dt);
    }

    // If weapon has kinematics, let it control all movement
    if (auto *kinematics = getComponent<KinematicsComponent>()) {
        // Set orbit center to owner's center for orbital/homing behaviors
        m_orbitCenter = m_owner->getCenter();
        kinematics->setTargetPoint(&m_orbitCenter);

        kinematics->update(dt, *this);
    }
    // No kinematics: check if weapon should follow owner
    else if (weaponComp && weaponComp->getBehavior() == WeaponBehavior::FollowOwner) {
        setPosition(m_owner->getPosition());
    }

    updateAnimationAndVisual(dt);
}

void Weapon::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    Entity::draw(target, states);
}

void Weapon::addSpeed() {}
void Weapon::addRange() {}
