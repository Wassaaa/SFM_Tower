#include "AnimationSystem.h"
#include "../Entity.h"
#include "../Components/AnimationComponent.h"
#include "../Components/VisualComponent.h"
#include "../Components/KinematicsComponent.h"
#include "../MathUtils.h"
#include "../Constants.h"

void AnimationSystem::update(float dt, std::vector<std::unique_ptr<Entity>> &entities)
{
    for (auto &entity : entities) {
        auto *anim = entity->getComponent<AnimationComponent>();
        auto *visual = entity->getComponent<VisualComponent>();

        if (!anim || !visual || !anim->isEnabled()) {
            continue;
        }

        // Calculate velocity-based animation speed
        if (auto *kin = entity->getComponent<KinematicsComponent>()) {
            float speed = VecLength(kin->velocity);
            // Scale animation speed: 0.3 at rest, 1.0 at max speed (400)
            anim->velocityScale =
                std::max(0.3f, std::min(1.0f, speed / Constants::ANIM_VELOCITY_MAX));
        }

        handleStateTransition(anim);
        updateFrame(dt, anim);
        applyToVisual(anim, visual);
    }
}

void AnimationSystem::handleStateTransition(AnimationComponent *anim) const
{
    if (anim->requestedState == EntityState::NOTHING ||
        anim->requestedState == anim->currentState) {

        anim->requestedState = EntityState::NOTHING;
        return;
    }

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
    // Apply velocity scaling only if animation supports it
    float timeScale = (data.velocityScaled) ? anim->velocityScale : 1.0f;
    anim->currentTime += sf::seconds(dt * timeScale);
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
