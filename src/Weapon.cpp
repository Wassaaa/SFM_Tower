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
    if (auto weapon = getComponent<WeaponComponent>()) {
        weapon->update(dt);
    }

    sf::Vector2f ownerPos = m_owner->getPosition();

    auto *kinematics = getComponent<KinematicsComponent>();
    if (kinematics) {
        // Update orbit center for orbital/homing behaviors
        m_orbitCenter = ownerPos;
        kinematics->setTargetPoint(&m_orbitCenter);

        auto *visual = getComponent<VisualComponent>();
        auto *collision = getComponent<CollisionComponent>();
        if (visual && collision) {
            kinematics->update(dt, *visual, *collision);
        }
    }
    else {
        // No kinematics, just follow owner position
        setPosition(ownerPos);
    }

    updateAnimationAndVisual(dt);
}

void Weapon::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    Entity::draw(target, states);
}

void Weapon::addSpeed() {}
void Weapon::addRange() {}
