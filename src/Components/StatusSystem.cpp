#include "StatusSystem.h"
#include "../Entity.h"
#include "../Constants.h"
#include "WeaponComponent.h"
#include "HealthComponent.h"
#include "TransformComponent.h"
#include "KinematicsComponent.h"

void StatusSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        if (entity->markedForDeath) {
            continue;
        }

        // Weapon Lifetime
        if (auto *weapon = entity->getComponent<WeaponComponent>()) {
            if (weapon->lifetime > 0.f) {
                weapon->lifetime -= dt;
                if (weapon->lifetime <= 0.f) {
                    entity->markedForDeath = true;
                    continue;
                }
            }
        }

        // Heath check
        if (auto *health = entity->getComponent<HealthComponent>()) {
            if (health->currentHealth <= 0.f) {
                entity->markedForDeath = true;
                continue;
            }
        }

        // Out of bounds
        if (auto *kin = entity->getComponent<KinematicsComponent>()) {
            if (kin->isStatic) {
                continue;
            }
            if (auto *transform = entity->getComponent<TransformComponent>()) {
                const auto &pos = transform->position;
                if (pos.x < 0 - m_screenMargin ||
                    pos.x > Constants::SCREEN_WIDTH + m_screenMargin ||
                    pos.y < 0 - m_screenMargin ||
                    pos.y > Constants::SCREEN_HEIGHT + m_screenMargin) {
                    entity->markedForDeath = true;
                    continue;
                }
            }
        }
    }
}
