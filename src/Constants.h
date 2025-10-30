#pragma once

namespace Constants {
    // Debug
    constexpr int DEBUG_DRAW = true;
    // Screen
    constexpr int SCREEN_WIDTH = 1600;
    constexpr int SCREEN_HEIGHT = 1200;

    // Player
    constexpr float PLAYER_SPEED_MAX = 400.f;
    constexpr float PLAYER_SPEED_MIN = 3.1f;
    constexpr float PLAYER_ACCELERATION = 100.f;
    constexpr float PLAYER_DRAG = 0.72f;

    // Vampire
    constexpr float VAMPIRE_HEIGHT = 32.f;
    constexpr float VAMPIRE_WIDTH = 32.f;
    constexpr float VAMPIRE_SPEED = 100.f;

    // Wall
    constexpr float WALL_THICKNESS = 500.f;
} // namespace Constants
