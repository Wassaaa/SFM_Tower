#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>

#include "ResourceManager.h"
#include "Tower.h"
#include "Entity.h"
#include "Components/CollisionComponent.h"
#include "Components/KinematicsComponent.h"
#include "Constants.h"

Game::Game()
    : m_state(GameState::ACTIVE)
    , m_pClock(std::make_unique<sf::Clock>())
    , m_pPlayerEntity(nullptr)
{
    // Create a tower in the center of the screen
    sf::Vector2f towerPosition(Constants::SCREEN_WIDTH / 2.f, Constants::SCREEN_HEIGHT / 2.f);
    m_pTower = std::make_unique<Tower>(this, towerPosition);
}

Game::~Game() {}

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

    // Create a player controlled box
    auto playerEntity =
        std::make_unique<Entity>(this, EntityType::TEST_BOX, sf::Vector2f(200.f, 200.f));
    m_pPlayerEntity = playerEntity.get();
    m_entities.push_back(std::move(playerEntity));

    return true;
}

void Game::update(float deltaTime)
{
    // Cap deltaTime to prevent huge jumps
    deltaTime = std::min(deltaTime, 0.1f);

    switch (m_state) {
    case GameState::ACTIVE: {
        m_pTower->update(deltaTime);

        const InputState &input = m_inputHandler.getState();

        // Handle game-level input actions
        if (input.spawnBox) {
            spawnRandomBox();
        }

        // Pass input to player entity
        if (m_pPlayerEntity) {
            m_pPlayerEntity->handleInput(deltaTime, input);
        }

        // Update all entities
        for (auto &entity : m_entities) {
            entity->update(deltaTime);
        }

        // Check collisions between all entities
        for (size_t i = 0; i < m_entities.size(); i++) {
            bool isColliding = false;
            auto *collision1 = m_entities[i]->getComponent<CollisionComponent>();
            if (!collision1)
                continue;

            for (size_t j = 0; j < m_entities.size(); j++) {
                if (i == j)
                    continue;

                auto *collision2 = m_entities[j]->getComponent<CollisionComponent>();
                if (!collision2)
                    continue;

                if (collision1->intersects(*collision2)) {
                    isColliding = true;
                    break;
                }
            }

            m_entities[i]->setColliding(isColliding);
        }
    } break;

    case GameState::WAITING:
        break;
    }
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw entities
    for (const auto &entity : m_entities) {
        entity->draw(target, states);
    }

    // Draw tower
    m_pTower->draw(target, states);
}

void Game::onKeyPressed(sf::Keyboard::Key key)
{
    m_inputHandler.onKeyPressed(key);
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    m_inputHandler.onKeyReleased(key);
}

Tower *Game::getTower() const
{
    return m_pTower.get();
}

void Game::spawnRandomBox()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> width(0, Constants::SCREEN_WIDTH);
    std::uniform_real_distribution<> height(0, Constants::SCREEN_HEIGHT);

    m_entities.push_back(std::make_unique<Entity>(this, EntityType::TEST_BOX,
                                                  sf::Vector2f(width(gen), height(gen))));
}
