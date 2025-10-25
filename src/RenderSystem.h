#pragma once
#include "Components/VisualComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/DirectionComponent.h"

// Forward declaration to avoid circular dependency
class ComponentContainer;

class RenderSystem
{
public:
    // Low-level: Apply direction flipping to visual
    static void applyDirection(VisualComponent &visual, const DirectionComponent &direction);

    // Mid-level: Update visual with animation frame and direction
    static void updateVisual(VisualComponent &visual, const AnimationComponent *anim,
                             const DirectionComponent *direction);

    // High-level: Update animation and apply to visual in one call
    // Handles the common pattern: update animation -> apply to visual
    static void updateAnimationAndVisual(ComponentContainer *entity, float dt);
};
