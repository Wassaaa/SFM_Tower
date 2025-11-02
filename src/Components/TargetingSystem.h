#pragma once
#include <vector>
#include <memory>

class Entity;

class TargetingSystem
{
public:
    TargetingSystem() = default;
    ~TargetingSystem() = default;

    void update(std::vector<std::unique_ptr<Entity>> &entities);
};
