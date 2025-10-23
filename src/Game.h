#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <unordered_set>
#include "Constants.h"
#include "Types.h"

class Tower;
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

    GameState getState() const
    {
        return m_state;
    }

    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);

    Tower *getTower() const;

private:
    std::unique_ptr<Tower> m_pTower;

    GameState m_state;
    std::unique_ptr<sf::Clock> m_pClock;

    sf::Font m_font;
};
