#pragma once
#include <SFML/Graphics.hpp>

struct AnimationInfo
{
    sf::Vector2i frameSize;
    sf::Vector2i startPos;
    size_t frameCount{1};
    sf::Time frameDuration{sf::milliseconds(100)};
    bool loop{true};
    bool velocityScaled{false};
};

class AnimationInfoBuilder
{
public:
    AnimationInfoBuilder &setFrameSize(const sf::Vector2i &size)
    {
        m_data.frameSize = size;
        return *this;
    }
    AnimationInfoBuilder &setStartPos(const sf::Vector2i &pos)
    {
        m_data.startPos = pos;
        return *this;
    }
    AnimationInfoBuilder &setFrameCount(size_t count)
    {
        m_data.frameCount = count;
        return *this;
    }
    AnimationInfoBuilder &setFrameDuration(const sf::Time &duration)
    {
        m_data.frameDuration = duration;
        return *this;
    }
    AnimationInfoBuilder &setLoop(bool loop)
    {
        m_data.loop = loop;
        return *this;
    }
    AnimationInfoBuilder &withVelocityScaled(bool scaled)
    {
        m_data.velocityScaled = scaled;
        return *this;
    }
    AnimationInfo build() const { return m_data; }

private:
    AnimationInfo m_data;
};
