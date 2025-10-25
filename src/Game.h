#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include <unordered_set>
#include "Constants.h"
#include "Types.h"

class Tower;
class Entity;
class Game;
class GameInput;

namespace sf {
    class Clock;
}

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

    Tower *getTower() const;
    Entity *getPlayerEntity() const { return m_pPlayerEntity; }

private:
    std::unique_ptr<Tower> m_pTower;
    std::vector<std::unique_ptr<Entity>> m_entities;
    Entity *m_pPlayerEntity;

    GameState m_state;
    std::unique_ptr<sf::Clock> m_pClock;

    sf::Font m_font;

    // Input state for WASD
    bool m_keyW{false}, m_keyA{false}, m_keyS{false}, m_keyD{false};
};
