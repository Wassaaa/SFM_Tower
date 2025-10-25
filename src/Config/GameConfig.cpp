#include "GameConfig.h"
#include "ComponentDataBuilder.h"
#include "EntityConfigBuilder.h"

namespace Config {
    const EntityConfig PLAYER =
        EntityConfigBuilder()
            .setVisual(VisualDataBuilder()
                           .setFilename("soldier.png")
                           .setScale({3.f, 3.f})
                           .setOrigin({50.f, 50.f})
                           .build())
            .setCollision(CollisionDataBuilder()
                              .setBox({50.f, 55.f})
                              .setScale({1.f, 1.f})
                              .setOrigin({25.f, 30.f})
                              .setDebugColor({0, 255, 0, 128})
                              .build())
            .addAnimation(EntityState::IDLE, AnimationInfoBuilder()
                                                 .setFrameSize({100, 100})
                                                 .setStartPos({0, 0})
                                                 .setFrameCount(6)
                                                 .setFrameDuration(sf::milliseconds(100))
                                                 .setLoop(true)
                                                 .build())
            .addAnimation(EntityState::MOVE_RIGHT, AnimationInfoBuilder()
                                                       .setFrameSize({100, 100})
                                                       .setStartPos({0, 1})
                                                       .setFrameCount(8)
                                                       .setFrameDuration(sf::milliseconds(100))
                                                       .setLoop(true)
                                                       .build())
            .addAnimation(EntityState::WEAPON_1, AnimationInfoBuilder()
                                                     .setFrameSize({100, 100})
                                                     .setStartPos({0, 5})
                                                     .setLoop(false)
                                                     .setFrameDuration(sf::milliseconds(100))
                                                     .setFrameCount(4)
                                                     .build())
            .build();

    const EntityConfig LASER_WEAPON =
        EntityConfigBuilder()
            .setVisual(VisualDataBuilder()
                           .setFilename("waveform2.png")
                           .setScale({1.0f, 1.0f})
                           .setOrigin({0.f, 15.f})
                           .build())
            .setCollision(CollisionDataBuilder()
                              .setBox({95.f, 32.f})
                              .setScale({1.f, 1.f})
                              .setOrigin({0.f, 15.f})
                              .setDebugColor({255, 0, 0, 128})
                              .build())
            .addAnimation(EntityState::WEAPON_1, AnimationInfoBuilder()
                                                     .setFrameSize({95, 32})
                                                     .setStartPos({0, 0})
                                                     .setFrameCount(1)
                                                     .setFrameDuration(sf::milliseconds(100))
                                                     .setLoop(true)
                                                     .build())
            .setWeapon(WeaponDataBuilder()
                           .setBaseDamage(10.f)
                           .setBasePiercing(1.f)
                           .setMaxHits(1)
                           .setLifetime(5.f)
                           .setRadius(16.f)
                           .setBehavior(WeaponBehavior::None)
                           .build())
            .setKinematics(KinematicsDataBuilder()
                               .setAngularVelocity(720.f)
                               .setOrbitAngularVelocity(90.f)
                               .setOrbitRadius(100.f)
                               .setBehavior(KinematicsBehavior::Orbital |
                                            KinematicsBehavior::Rotating |
                                            KinematicsBehavior::Pulsing)
                               .build())
            .build();

    const EntityConfig TOWER =
        EntityConfigBuilder()
            .setVisual(VisualDataBuilder()
                           .setFilename("soldier.png")
                           .setScale({2.f, 2.f})
                           .setOrigin({50.f, 50.f})
                           .build())
            .setCollision(CollisionDataBuilder()
                              .setBox({50.f, 55.f})
                              .setScale({1.f, 1.f})
                              .setOrigin({25.f, 30.f})
                              .setDebugColor({0, 0, 255, 128})
                              .build())
            .addAnimation(EntityState::IDLE, AnimationInfoBuilder()
                                                 .setFrameSize({100, 100})
                                                 .setStartPos({0, 0})
                                                 .setFrameCount(6)
                                                 .setFrameDuration(sf::milliseconds(100))
                                                 .setLoop(true)
                                                 .build())
            .build();

    const EntityConfig VAMPIRE =
        EntityConfigBuilder()
            .setVisual(VisualDataBuilder()
                           .setFilename("vampire.png")
                           .setScale({2.f, 2.f})
                           .setOrigin({8.f, 8.f})
                           .build())
            .setCollision(CollisionDataBuilder()
                              .setBox({16.f, 16.f})
                              .setScale({2.f, 2.f})
                              .setOrigin({8.f, 8.f})
                              .setDebugColor({255, 0, 0, 128})
                              .build())
            .addAnimation(EntityState::IDLE, AnimationInfoBuilder()
                                                 .setFrameSize({16, 16})
                                                 .setStartPos({0, 0})
                                                 .setFrameCount(1)
                                                 .setFrameDuration(sf::milliseconds(100))
                                                 .setLoop(true)
                                                 .build())
            .build();

    const EntityConfig TEST_BOX = EntityConfigBuilder()
                                      .setCollision(CollisionDataBuilder()
                                                        // .setCircle(25.f)
                                                        .setBox({50.f, 50.f})
                                                        .setScale({1.f, 1.f})
                                                        .setOrigin({25.f, 25.f})
                                                        .setDebugColor({100, 100, 255, 125})
                                                        .build())
                                      .setKinematics(KinematicsDataBuilder()
                                                         .setVelocity({0.f, 0.f})
                                                         .setBehavior(KinematicsBehavior::Linear)
                                                         .build())
                                      .build();

    const std::unordered_map<EntityType, const EntityConfig &> ENTITY_CONFIGS = {
        {EntityType::PLAYER, PLAYER},
        {EntityType::TOWER, TOWER},
        {EntityType::LASER_WEAPON, LASER_WEAPON},
        {EntityType::VAMPIRE, VAMPIRE},
        {EntityType::TEST_BOX, TEST_BOX}};
} // namespace Config
