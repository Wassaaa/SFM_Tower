#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>

#include "ResourceManager.h"
#include "Entity.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Constants.h"

Game::Game()
    : m_state(GameState::ACTIVE)
    , m_pClock(std::make_unique<sf::Clock>())
    , m_pPlayerEntity(nullptr)
{}

Game::~Game() {}

bool Game::initialise()
{
    if (!m_font.loadFromFile(ResourceManager::getFilePath("Lavigne.ttf"))) {
        std::cerr << "Unable to load font" << std::endl;
        return false;
    }

    // init systems
    m_collisionSystem = std::make_unique<CollisionSystem>();
    m_kinematicsSystem = std::make_unique<KinematicsSystem>();
    m_debugRenderSystem = std::make_unique<DebugRenderSystem>();

    // Create a player controlled box
    auto playerEntity =
        std::make_unique<Entity>(this, EntityType::TEST_BOX, sf::Vector2f(200.f, 200.f));
    m_pPlayerEntity = playerEntity.get();

    m_entities.push_back(std::move(playerEntity));

    // Create boundary walls
    createBoundaryWalls();

    return true;
}

void Game::createBoundaryWalls()
{
    const float visibleThickness = 5.f;

    // Top wall
    auto topWall = std::make_unique<Entity>(
        this, EntityType::WALL_HORIZONTAL,
        sf::Vector2f(Constants::SCREEN_WIDTH / 2.f,
                     0.f - (Constants::WALL_THICKNESS / 2) + visibleThickness));
    m_entities.push_back(std::move(topWall));

    // Bottom wall
    auto bottomWall = std::make_unique<Entity>(
        this, EntityType::WALL_HORIZONTAL,
        sf::Vector2f(Constants::SCREEN_WIDTH / 2.f, Constants::SCREEN_HEIGHT +
                                                        (Constants::WALL_THICKNESS / 2) -
                                                        visibleThickness));
    m_entities.push_back(std::move(bottomWall));

    // Left wall
    auto leftWall = std::make_unique<Entity>(
        this, EntityType::WALL_VERTICAL,
        sf::Vector2f(0.f - (Constants::WALL_THICKNESS / 2) + visibleThickness,
                     Constants::SCREEN_HEIGHT / 2.f));
    m_entities.push_back(std::move(leftWall));

    // right wall
    auto rightWall = std::make_unique<Entity>(
        this, EntityType::WALL_VERTICAL,
        sf::Vector2f(Constants::SCREEN_WIDTH + (Constants::WALL_THICKNESS / 2) - visibleThickness,
                     Constants::SCREEN_HEIGHT / 2.f));
    m_entities.push_back(std::move(rightWall));
}

void Game::update(float deltaTime)
{
    // Cap deltaTime
    deltaTime = std::min(deltaTime, 0.1f);

    switch (m_state) {
    case GameState::ACTIVE: {
        const InputState &input = m_inputHandler.getState();

        if (input.spawnBox) {
            spawnBox();
        }

        if (m_pPlayerEntity) {
            m_pPlayerEntity->handleInput(deltaTime, input);
        }

        // Run logic systems
        m_kinematicsSystem->update(deltaTime, m_entities);
        m_collisionSystem->update(m_entities, deltaTime);

        // AnimationSystem, etc
    } break;

    case GameState::WAITING:
        break;
    }
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (const auto &entity : m_entities) {
        if (entity->getType() != EntityType::TEST_BOX) {
            // This uses the old VisualComponent draw for now
            entity->draw(target, states);
        }
    }

    m_debugRenderSystem->draw(target, states, m_entities, Constants::DEBUG_DRAW);
}

void Game::onKeyPressed(sf::Keyboard::Key key)
{
    m_inputHandler.onKeyPressed(key);
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    m_inputHandler.onKeyReleased(key);
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
