#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>

#include "ResourceManager.h"
#include "Entity.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/KinematicsComponent.h"
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
    m_renderSystem = std::make_unique<RenderSystem>();
    m_animationSystem = std::make_unique<AnimationSystem>();

    // Create a player controlled box
    auto playerEntity =
        std::make_unique<Entity>(this, EntityType::PLAYER, sf::Vector2f(200.f, 200.f));

    // playerEntity->setMass(10.f);
    // auto playerEntity =
    //     std::make_unique<Entity>(this, EntityType::TEST_BOX, sf::Vector2f(200.f, 200.f));
    // playerEntity->setMass(std::numeric_limits<float>::infinity());
    m_pPlayerEntity = playerEntity.get();

    if (auto *kin = m_pPlayerEntity->getComponent<KinematicsComponent>()) {
        kin->behavior = kin->behavior | KinematicsBehavior::FaceTarget;
        kin->angularVelocity = 720.f;
    }

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
    topWall->setStatic(true);
    m_entities.push_back(std::move(topWall));

    // Bottom wall
    auto bottomWall = std::make_unique<Entity>(
        this, EntityType::WALL_HORIZONTAL,
        sf::Vector2f(Constants::SCREEN_WIDTH / 2.f, Constants::SCREEN_HEIGHT +
                                                        (Constants::WALL_THICKNESS / 2) -
                                                        visibleThickness));
    bottomWall->setStatic(true);
    m_entities.push_back(std::move(bottomWall));

    // Left wall
    auto leftWall = std::make_unique<Entity>(
        this, EntityType::WALL_VERTICAL,
        sf::Vector2f(0.f - (Constants::WALL_THICKNESS / 2) + visibleThickness,
                     Constants::SCREEN_HEIGHT / 2.f));
    leftWall->setStatic(true);
    m_entities.push_back(std::move(leftWall));

    // right wall
    auto rightWall = std::make_unique<Entity>(
        this, EntityType::WALL_VERTICAL,
        sf::Vector2f(Constants::SCREEN_WIDTH + (Constants::WALL_THICKNESS / 2) - visibleThickness,
                     Constants::SCREEN_HEIGHT / 2.f));
    rightWall->setStatic(true);
    m_entities.push_back(std::move(rightWall));
}

void Game::update(float deltaTime, sf::RenderWindow &window)
{
    // Cap deltaTime
    deltaTime = std::min(deltaTime, 0.1f);

    switch (m_state) {
    case GameState::ACTIVE: {
        InputState &input = m_inputHandler.getState();

        if (input.spawnBox) {
            spawnBox();
            input.spawnBox = false;
        }

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        input.mouseWorldPosition = worldPos;

        if (m_pPlayerEntity) {
            m_pPlayerEntity->handleInput(deltaTime, input);
            if (auto *kin = m_pPlayerEntity->getComponent<KinematicsComponent>()) {
                kin->targetPoint = &input.mouseWorldPosition;
            }
        }

        // Run logic systems
        m_kinematicsSystem->update(deltaTime, m_entities);
        m_collisionSystem->update(deltaTime, m_entities);
        m_animationSystem->update(deltaTime, m_entities);
    } break;

    case GameState::WAITING:
        break;
    }
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

    m_renderSystem->draw(target, states, m_entities);
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
    sf::Vector2f rnd;

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
        std::uniform_real_distribution<> rdm(-100, 100);
        spawnPos = sf::Vector2f(width(gen), height(gen));
        rnd = sf::Vector2f(rdm(gen), rdm(gen));
    }

    auto box = std::make_unique<Entity>(this, EntityType::TEST_BOX, spawnPos);
    if (auto *kin = box->getComponent<KinematicsComponent>()) {
        // kin->mass = std::numeric_limits<float>::infinity();
        kin->velocity = rnd;
    }
    m_entities.push_back(std::move(box));
}
