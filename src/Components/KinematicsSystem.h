#pragma once
#include <vector>
#include <memory>

class Entity;
class KinematicsComponent;
class TransformComponent;
class DirectionComponent; // <-- Add this

// The KinematicsSystem is responsible for all movement logic.
// It reads from KinematicsComponent and writes to TransformComponent.
class KinematicsSystem
{
public:
    KinematicsSystem() = default;
    ~KinematicsSystem() = default;

    void update(float dt, std::vector<std::unique_ptr<Entity>> &entities);

private:
    void handleVelocity(float dt, KinematicsComponent *kin);
    void handlePosition(float dt, KinematicsComponent *kin, TransformComponent *trans,
                        Entity *entity);
    void handleFacingDirection(KinematicsComponent *kinematics, TransformComponent *transform,
                               DirectionComponent *dir, Entity *entity);
    void handleRotation(float dt, KinematicsComponent *kin, TransformComponent *trans,
                        Entity *entity);
    void handleScaling(float dt, KinematicsComponent *kin, TransformComponent *trans);
};
