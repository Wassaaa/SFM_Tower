#include "RenderSystem.h"
#include <cmath>

void RenderSystem::applyDirection(VisualComponent &visual, const DirectionComponent &direction)
{
    sf::Vector2f scale = visual.getScale();
    float absScaleX = std::abs(scale.x);

    if (direction.isFacingLeft()) {
        // Face left (negative scale)
        if (scale.x > 0)
            visual.setScale(-absScaleX, scale.y);
    }
    else {
        // Face right (positive scale)
        if (scale.x < 0)
            visual.setScale(absScaleX, scale.y);
    }
}

void RenderSystem::updateVisual(VisualComponent &visual, const AnimationComponent *anim,
                                const DirectionComponent *direction)
{
    if (anim) {
        visual.setTextureRect(anim->getCurrentFrame());
    }

    if (direction) {
        applyDirection(visual, *direction);
    }
}
