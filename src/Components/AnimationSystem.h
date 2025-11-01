#pragma once
#include <vector>
#include <memory>

class Entity;
class AnimationComponent;
class VisualComponent;

class AnimationSystem
{
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;

    void update(float dt, std::vector<std::unique_ptr<Entity>> &entities);

private:
    void handleStateTransition(AnimationComponent *anim) const;
    void updateFrame(float dt, AnimationComponent *anim) const;
    void applyToVisual(AnimationComponent *anim, VisualComponent *visual) const;
};
