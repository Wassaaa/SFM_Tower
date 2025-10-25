#pragma once
#include "Components/VisualComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"

class RenderSystem
{
public:
    static void applyDirection(VisualComponent &visual, const DirectionComponent &direction);

    static void updateVisual(VisualComponent &visual, const AnimationComponent *anim,
                             const DirectionComponent *direction);
};
