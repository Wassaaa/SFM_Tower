#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

struct InputState
{
    // Movement
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;

    // Actions
    bool action1 = false;
    bool action1Released = true;
    bool spawnBox = false;

    // Mouse
    sf::Vector2f mouseWorldPosition{0.f, 0.f};

    bool hasMovementInput() const { return moveUp || moveDown || moveLeft || moveRight; }
};

class InputHandler
{
public:
    InputHandler() = default;
    ~InputHandler() = default;

    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);

    InputState &getState() { return m_state; }

private:
    InputState m_state;
};
