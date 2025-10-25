#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <iostream>

#include "../Anim.h"
#include "../Types.h"
#include "../Config/GameConfig.h"
#include "Component.h"

class AnimationComponent : public Component
{
public:
    AnimationComponent() = default;
    ~AnimationComponent() = default;

    void addAnimation(EntityState state, const AnimationInfo &animInfo);
    void playAnimation(EntityState anim);
    void playDefaultAnimation();
    void update(float dt);

    sf::IntRect getCurrentFrame() const;

    // inline getters
    EntityState getCurrentAnimation() const { return currentAnimation; }
    const std::unordered_map<EntityState, Anim> &getAnimations() const { return animations; }
    virtual const char *getName() const override { return "AnimationComponent"; }

private:
    void onAnimationComplete();

    std::unordered_map<EntityState, Anim> animations;
    EntityState currentAnimation{EntityState::NOTHING};
    EntityState previousAnimation{EntityState::NOTHING};
    EntityState defaultAnimation{EntityState::NOTHING};
};
