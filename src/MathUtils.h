#pragma once

#include <math.h>
#include <SFML/System.hpp>

// Math constants
static constexpr float PI = 3.14159265358979323846f;
static constexpr float DEG_TO_RAD = PI / 180.0f;
static constexpr float RAD_TO_DEG = 180.0f / PI;

// Angle conversion helpers
inline float ToRadians(float degrees)
{
    return degrees * DEG_TO_RAD;
}

inline float ToDegrees(float radians)
{
    return radians * RAD_TO_DEG;
}

// Vectors
inline float DotProduct(sf::Vector2f a, sf::Vector2f b)
{
    return a.x * b.x + a.y * b.y;
}

// Vector magnitude calculations
inline float VecLengthSquared(sf::Vector2f a)
{
    return a.x * a.x + a.y * a.y;
}

inline float VecLength(sf::Vector2f a)
{
    return sqrtf(VecLengthSquared(a));
}

// Vector normalization
inline sf::Vector2f VecNormalized(sf::Vector2f a)
{
    float length = VecLength(a);

    if (length != 0.0f)
        return sf::Vector2f(a.x / length, a.y / length);

    return a;
}

// Vector operations
inline sf::Vector2f operator*(sf::Vector2f vec, float scalar)
{
    return sf::Vector2f(vec.x * scalar, vec.y * scalar);
}

inline sf::Vector2f operator*(float scalar, sf::Vector2f vec)
{
    return vec * scalar;
}

inline sf::Vector2f operator/(sf::Vector2f vec, float scalar)
{
    return sf::Vector2f(vec.x / scalar, vec.y / scalar);
}
