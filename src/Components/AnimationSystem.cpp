#include "AnimationSystem.h"
#include "../Entity.h"
#include "AnimationComponent.h"
#include "VisualComponent.h"

void AnimationSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        auto *anim = entity->getComponent<AnimationComponent>();
        auto *visual = entity->getComponent<VisualComponent>();

        if (!anim || !visual || !anim->isEnabled()) {
            continue;
        }
        handleStateTransition(anim);
        updateFrame(dt, anim);
        applyToVisual(anim, visual);
    }
}

void AnimationSystem::handleStateTransition(AnimationComponent *anim) const
{
    bool canChange = true;

    if (anim->currentState != EntityState::NOTHING && anim->animations.count(anim->currentState)) {
        const auto &currentAnimData = anim->animations.at(anim->currentState);
        // not interrupting non-looping animations
        if (!currentAnimData.loop && anim->isPlaying) {
            canChange = false;
        }
    }

    // Check if a new state has been requested
    if (anim->requestedState == EntityState::NOTHING || !canChange) {
        return;
    }

    // Apply the new state
    if (anim->animations.count(anim->requestedState)) {
        anim->currentState = anim->requestedState;
        anim->currentTime = sf::Time::Zero;
        anim->currentFrame = 0;
        anim->isPlaying = true;
    }

    // 'consume' the requested state
    anim->requestedState = EntityState::NOTHING;
}

void AnimationSystem::updateFrame(float dt, AnimationComponent *anim) const
{
    // Check if there is animation to play
    if (!anim->isPlaying || !anim->animations.count(anim->currentState)) {
        return;
    }

    // Check if there is any frames
    const auto &data = anim->animations.at(anim->currentState);
    if (data.frames.empty()) {
        return;
    }

    // Play the animation / update frames
    anim->currentTime += sf::seconds(dt);
    if (anim->currentTime >= data.frameDuration) {
        anim->currentTime -= data.frameDuration;
        anim->currentFrame++;

        // Check if animation finished
        if (anim->currentFrame >= data.frames.size()) {
            if (data.loop) {
                // loop back
                anim->currentFrame = 0;
            }
            else {
                // stay on last frame but request a change
                anim->currentFrame = data.frames.size() - 1;
                anim->isPlaying = false;
                anim->requestedState = anim->defaultState;
            }
        }
    }
}

void AnimationSystem::applyToVisual(AnimationComponent *anim, VisualComponent *visual) const
{
    if (!anim->animations.count(anim->currentState)) {
        return;
    }

    const auto &data = anim->animations.at(anim->currentState);

    if (!data.frames.empty() && anim->currentFrame < data.frames.size()) {
        visual->setTextureRect(data.frames[anim->currentFrame]);
    }
}
