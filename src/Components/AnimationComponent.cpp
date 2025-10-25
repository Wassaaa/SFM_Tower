#include "AnimationComponent.h"

void AnimationComponent::addAnimation(EntityState state, const AnimationInfo &animInfo)
{
    animations.try_emplace(state, animInfo.frameDuration, animInfo.loop);
    for (size_t i = 0; i < animInfo.frameCount; i++) {
        sf::IntRect frame(animInfo.startPos.x + i * animInfo.frameSize.x,
                          animInfo.startPos.y * animInfo.frameSize.y, animInfo.frameSize.x,
                          animInfo.frameSize.y);
        animations[state].addFrame(frame);
    }
    if (currentAnimation == EntityState::NOTHING) {
        defaultAnimation = state;
        playDefaultAnimation();
    }
}

void AnimationComponent::playAnimation(EntityState newAnimation)
{
    if (animations.find(newAnimation) == animations.end())
        return;
    // Don't interrupt non-looping animations unless it's another non-looping (one-cycle) animation
    if (currentAnimation != EntityState::NOTHING && !animations[currentAnimation].isLooping() &&
        animations[currentAnimation].isPlaying() && animations[newAnimation].isLooping()) {
        return;
    }
    if (currentAnimation != newAnimation) {
        if (currentAnimation != EntityState::NOTHING)
            animations[currentAnimation].stop();
        // Store previous animation if current is non-looping (one-cycle)
        if (!animations[currentAnimation].isLooping())
            previousAnimation = currentAnimation;
    }

    currentAnimation = newAnimation;
    animations[newAnimation].play();
}

void AnimationComponent::update(float dt)
{
    if (currentAnimation == EntityState::NOTHING) {
        playDefaultAnimation();
        return;
    }
    // Just update animation state, visual update happens in draw()
    animations[currentAnimation].update(dt);

    if (!animations[currentAnimation].isPlaying() && !animations[currentAnimation].isLooping())
        onAnimationComplete();
}

void AnimationComponent::onAnimationComplete()
{
    if (previousAnimation != EntityState::NOTHING) {
        playAnimation(previousAnimation);
        previousAnimation = EntityState::NOTHING;
    }
    else {
        playAnimation(EntityState::IDLE);
    }
}

void AnimationComponent::playDefaultAnimation()
{
    if (defaultAnimation != EntityState::NOTHING)
        playAnimation(defaultAnimation);
}

sf::IntRect AnimationComponent::getCurrentFrame() const
{
    if (currentAnimation != EntityState::NOTHING &&
        animations.find(currentAnimation) != animations.end()) {
        return animations.at(currentAnimation).getCurrentFrame();
    }
    return sf::IntRect(0, 0, 0, 0);
}
