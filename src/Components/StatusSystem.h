#pragma once
#include <vector>
#include <memory>

class Entity;

class StatusSystem
{
public:
    StatusSystem() = default;
    ~StatusSystem() = default;

    void update(float dt, std::vector<std::unique_ptr<Entity>> &entities);

private:
    float m_screenMargin = 500.f;
};
