#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>

#include "ResourceManager.h"
#include "Tower.h"
#include "Entity.h"
#include "Components/CollisionComponent.h"
#include "Components/VisualComponent.h"
#include "Constants.h"

Game::Game()
    : m_state(GameState::ACTIVE)
    , m_pClock(std::make_unique<sf::Clock>())
    , m_pPlayerEntity(nullptr)
{}

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

    // Create a tower in the center of the screen
    // sf::Vector2f towerPosition(Constants::SCREEN_WIDTH / 2.f, Constants::SCREEN_HEIGHT / 2.f);
    // m_pTower = std::make_unique<Tower>(this, towerPosition);
    // Add a weapon to the tower for testing
    // m_pTower->addWeapon(EntityType::LASER_WEAPON);

    // Create a player controlled box
    auto playerEntity =
        std::make_unique<Entity>(this, EntityType::TEST_BOX, sf::Vector2f(200.f, 200.f));
    m_pPlayerEntity = playerEntity.get();
    m_pPlayerEntity->getComponent<VisualComponent>()->setScale(3.f, 3.f);
    m_entities.push_back(std::move(playerEntity));
    sf::Vector2f specificPos(200.f, 200.f);
    // spawnBox(&specificPos);

    // Create boundary walls
    createBoundaryWalls();

    return true;
}

void Game::createBoundaryWalls()
{
    const float visibleThickness = 5.f;
    const float horizontalCollisionThickness = 2000.f;
    const float verticalCollisionThickness = 2000.f;

    const float halfHorizontal = horizontalCollisionThickness * 0.5f;
    const float halfVertical = verticalCollisionThickness * 0.5f;

    // Top wall
    auto topWall = std::make_unique<Entity>(
        this, EntityType::WALL,
        sf::Vector2f(Constants::SCREEN_WIDTH / 2.f, visibleThickness - halfHorizontal));
    if (auto *collision = topWall->getComponent<CollisionComponent>()) {
        collision->setScale(
            sf::Vector2f(Constants::SCREEN_WIDTH / 10.f, horizontalCollisionThickness / 10.f));
    }
    m_entities.push_back(std::move(topWall));

    // Bottom wall
    auto bottomWall = std::make_unique<Entity>(
        this, EntityType::WALL,
        sf::Vector2f(Constants::SCREEN_WIDTH / 2.f,
                     Constants::SCREEN_HEIGHT - visibleThickness + halfHorizontal));
    if (auto *collision = bottomWall->getComponent<CollisionComponent>()) {
        collision->setScale(
            sf::Vector2f(Constants::SCREEN_WIDTH / 10.f, horizontalCollisionThickness / 10.f));
    }
    m_entities.push_back(std::move(bottomWall));

    // Left wall
    auto leftWall = std::make_unique<Entity>(
        this, EntityType::WALL,
        sf::Vector2f(visibleThickness - halfVertical, Constants::SCREEN_HEIGHT / 2.f));
    if (auto *collision = leftWall->getComponent<CollisionComponent>()) {
        collision->setScale(
            sf::Vector2f(verticalCollisionThickness / 10.f, Constants::SCREEN_HEIGHT / 10.f));
    }
    m_entities.push_back(std::move(leftWall));

    // Right wall
    auto rightWall = std::make_unique<Entity>(
        this, EntityType::WALL,
        sf::Vector2f(Constants::SCREEN_WIDTH - visibleThickness + halfVertical,
                     Constants::SCREEN_HEIGHT / 2.f));
    if (auto *collision = rightWall->getComponent<CollisionComponent>()) {
        collision->setScale(
            sf::Vector2f(verticalCollisionThickness / 10.f, Constants::SCREEN_HEIGHT / 10.f));
    }
    m_entities.push_back(std::move(rightWall));
}

void Game::update(float deltaTime)
{
    // Cap deltaTime to prevent huge jumps
    deltaTime = std::min(deltaTime, 0.1f);

    switch (m_state) {
    case GameState::ACTIVE: {
        if (m_pTower) {
            m_pTower->update(deltaTime);
        }

        const InputState &input = m_inputHandler.getState();

        // Handle game-level input actions
        if (input.spawnBox) {
            spawnBox();
        }

        // Pass input to player entity
        if (m_pPlayerEntity) {
            m_pPlayerEntity->handleInput(deltaTime, input);
        }

        // Update all entities
        for (auto &entity : m_entities) {
            entity->update(deltaTime);
        }

        // Handle collisions
        m_collisionSystem.update(m_entities, deltaTime);
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
    if (m_pTower) {
        m_pTower->draw(target, states);
    }
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

void Game::spawnBox(const sf::Vector2f *position)
{
    sf::Vector2f spawnPos;

    if (position) {
        // Use specified position
        spawnPos = *position;
    }
    else {
        // Use random position
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> width(0, Constants::SCREEN_WIDTH);
        std::uniform_real_distribution<> height(0, Constants::SCREEN_HEIGHT);
        spawnPos = sf::Vector2f(width(gen), height(gen));
    }

    m_entities.push_back(std::make_unique<Entity>(this, EntityType::TEST_BOX, spawnPos));
}
