#pragma once
#include <vector>
#include <memory>

class Entity;
class KinematicsComponent;
class TransformComponent;

// The KinematicsSystem is responsible for all movement logic.
// It reads from KinematicsComponent and writes to TransformComponent.
class KinematicsSystem
{
public:
    KinematicsSystem() = default;
    ~KinematicsSystem() = default;

    void update(float dt, std::vector<std::unique_ptr<Entity>> &entities);
};
