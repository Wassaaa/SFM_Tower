#include "InputHandler.h"

void InputHandler::onKeyPressed(sf::Keyboard::Key key)
{
    // Movement (WASD)
    if (key == sf::Keyboard::W)
        m_state.moveUp = true;
    else if (key == sf::Keyboard::S)
        m_state.moveDown = true;
    else if (key == sf::Keyboard::A)
        m_state.moveLeft = true;
    else if (key == sf::Keyboard::D)
        m_state.moveRight = true;

    // Actions
    else if (key == sf::Keyboard::Space) {
        if (m_state.action1Released)
            m_state.action1 = true;
        m_state.action1Released = false;
    }
    else if (key == sf::Keyboard::B) {
        if (m_state.spawnBoxReleased)
            m_state.spawnBox = true;
        m_state.spawnBoxReleased = false;
    }
}

void InputHandler::onKeyReleased(sf::Keyboard::Key key)
{
    // Movement (WASD)
    if (key == sf::Keyboard::W)
        m_state.moveUp = false;
    else if (key == sf::Keyboard::S)
        m_state.moveDown = false;
    else if (key == sf::Keyboard::A)
        m_state.moveLeft = false;
    else if (key == sf::Keyboard::D)
        m_state.moveRight = false;

    // Actions
    else if (key == sf::Keyboard::Space) {
        m_state.action1 = false;
        m_state.action1Released = true;
    }
    else if (key == sf::Keyboard::B) {
        m_state.spawnBox = false;
        m_state.spawnBoxReleased = true;
    }
}
