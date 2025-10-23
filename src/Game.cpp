#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include "ResourceManager.h"
#include "Tower.h"

Game::Game()
    : m_state(GameState::ACTIVE)
    , m_pClock(std::make_unique<sf::Clock>())
{
    // Create a tower in the center of the screen
    sf::Vector2f towerPosition(Constants::SCREEN_WIDTH / 2.f, Constants::SCREEN_HEIGHT / 2.f);
    m_pTower = std::make_unique<Tower>(this, towerPosition);
}

Game::~Game()
{}

bool Game::initialise()
{
    /*
        PAY ATTENTION HIVER!
            If you want to load any assets (fonts, textures) please use the pattern shown below
    */

    if (!m_font.loadFromFile(ResourceManager::getFilePath("Lavigne.ttf"))) {
        std::cerr << "Unable to load font" << std::endl;
        return false;
    }

    // Add a weapon to the tower for testing
    m_pTower->addWeapon(EntityType::LASER_WEAPON);

    return true;
}

void Game::update(float deltaTime)
{
    // Cap deltaTime to prevent huge jumps
    deltaTime = std::min(deltaTime, 0.1f);

    switch (m_state) {
    case GameState::ACTIVE: {
        m_pTower->update(deltaTime);
    } break;

    case GameState::WAITING:
        break;
    }
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw tower
    m_pTower->draw(target, states);
}

void Game::onKeyPressed(sf::Keyboard::Key key)
{
    // Handle key presses if needed
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    // Handle key releases if needed
}

Tower *Game::getTower() const
{
    return m_pTower.get();
}
