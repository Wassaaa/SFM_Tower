#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include <unordered_set>
#include "Constants.h"
#include "Types.h"
#include "InputHandler.h"
#include "Components/CollisionSystem.h"
#include "Components/KinematicsSystem.h"
#include "DebugRenderSystem.h"

class Entity;
class Game;

class Game : public sf::Drawable
{
public:
    Game();
    ~Game();

    bool initialise();
    void update(float deltaTime);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    GameState getState() const { return m_state; }

    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);

    Entity *getPlayerEntity() const { return m_pPlayerEntity; }

    void spawnBox(const sf::Vector2f *position = nullptr);
    void createBoundaryWalls();

private:
    std::vector<std::unique_ptr<Entity>> m_entities;
    Entity *m_pPlayerEntity;

    GameState m_state;
    std::unique_ptr<sf::Clock> m_pClock;

    sf::Font m_font;

    InputHandler m_inputHandler;

    // Systems
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<KinematicsSystem> m_kinematicsSystem;
    std::unique_ptr<DebugRenderSystem> m_debugRenderSystem;
};
