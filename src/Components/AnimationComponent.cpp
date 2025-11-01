#include "AnimationComponent.h"

void AnimationComponent::addAnimation(EntityState state, const AnimationInfo &animInfo)
{
    auto [it, inserted] = animations.try_emplace(state);
    if (inserted) {
        it->second.frameDuration = animInfo.frameDuration;
        it->second.loop = animInfo.loop;

        for (size_t i = 0; i < animInfo.frameCount; i++) {
            sf::IntRect frame(animInfo.startPos.x + i * animInfo.frameSize.x, animInfo.startPos.y,
                              animInfo.frameSize.x, animInfo.frameSize.y);
            it->second.frames.emplace_back(frame);
        }
    }

    if (defaultState == EntityState::NOTHING) {
        defaultState = state;
        requestedState = state;
    }
}
