#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "../Types.h"
#include "../Config/GameConfig.h"
#include "Component.h"

struct AnimData
{
    std::vector<sf::IntRect> frames;
    sf::Time frameDuration = sf::Time::Zero;
    bool loop = true;
};

class AnimationComponent : public Component
{
public:
    AnimationComponent() = default;
    ~AnimationComponent() = default;

    // setup
    void addAnimation(EntityState state, const AnimationInfo &animInfo);

    std::unordered_map<EntityState, AnimData> animations;
    EntityState defaultState{EntityState::NOTHING};

    // State
    EntityState requestedState{EntityState::NOTHING};
    EntityState currentState{EntityState::NOTHING};
    size_t currentFrame{0};
    sf::Time currentTime{sf::Time::Zero};
    bool isPlaying{false};

    virtual const char *getName() const override { return "AnimationComponent"; }
};
