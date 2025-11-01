#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.h"

class VisualDataBuilder
{
public:
    VisualDataBuilder &setFilename(const std::string &filename)
    {
        m_filename = filename;
        return *this;
    }
    VisualDataBuilder &setScale(const sf::Vector2f &scale)
    {
        m_scale = scale;
        return *this;
    }
    VisualDataBuilder &setOrigin(const sf::Vector2f &origin)
    {
        m_origin = origin;
        return *this;
    }
    VisualDataBuilder &setOffset(const sf::Vector2f &offset)
    {
        m_offset = offset;
        return *this;
    }
    VisualDataBuilder &setRotation(float rotation)
    {
        m_rotation = rotation;
        return *this;
    }
    VisualComponentData build() const
    {
        return {m_filename, m_scale, m_origin, m_offset, m_rotation};
    }

private:
    std::string m_filename;
    sf::Vector2f m_scale{1.f, 1.f};
    sf::Vector2f m_origin{0.f, 0.f};
    sf::Vector2f m_offset{0.f, 0.f};
    float m_rotation{0.f};
};

class CollisionDataBuilder
{
public:
    CollisionDataBuilder &setCircle(float radius)
    {
        m_type = CollisionShape::Circle;
        m_radius = radius;
        m_origin = {radius, radius};
        return *this;
    }
    CollisionDataBuilder &setPolygon(const std::vector<sf::Vector2f> &points)
    {
        m_type = CollisionShape::Polygon;
        m_points = points;
        return *this;
    }
    CollisionDataBuilder &setBox(const sf::Vector2f &size)
    {
        m_type = CollisionShape::Polygon;
        m_points = {{0, 0}, {size.x, 0}, {size.x, size.y}, {0, size.y}};
        return *this;
    }
    CollisionDataBuilder &setScale(const sf::Vector2f &scale)
    {
        m_scale = scale;
        return *this;
    }
    CollisionDataBuilder &setOrigin(const sf::Vector2f &origin)
    {
        m_origin = origin;
        if (m_type == CollisionShape::Circle) {
            m_origin = {m_radius, m_radius};
        }
        return *this;
    }
    CollisionDataBuilder &setOffset(const sf::Vector2f &offset)
    {
        m_offset = offset;
        return *this;
    }
    CollisionDataBuilder &setRotation(float rotation)
    {
        m_rotation = rotation;
        return *this;
    }
    CollisionDataBuilder &setDebugColor(const sf::Color &color)
    {
        m_debugColor = color;
        return *this;
    }
    CollisionComponentData build() const
    {
        return {m_type, m_radius, m_points, m_scale, m_origin, m_offset, m_rotation, m_debugColor};
    }

private:
    CollisionShape m_type{CollisionShape::Polygon};
    float m_radius{0.f};
    std::vector<sf::Vector2f> m_points;
    sf::Vector2f m_scale{1.f, 1.f};
    sf::Vector2f m_origin{0.f, 0.f};
    sf::Vector2f m_offset{0.f, 0.f};
    float m_rotation{0.f};
    sf::Color m_debugColor{255, 0, 0, 128};
};

class AnimationInfoBuilder
{
public:
    AnimationInfoBuilder &setFrameSize(const sf::Vector2i &size)
    {
        m_frameSize = size;
        return *this;
    }
    AnimationInfoBuilder &setStartPos(const sf::Vector2i &pos)
    {
        m_startPos = pos;
        return *this;
    }
    AnimationInfoBuilder &setFrameCount(size_t count)
    {
        m_frameCount = count;
        return *this;
    }
    AnimationInfoBuilder &setFrameDuration(const sf::Time &duration)
    {
        m_frameDuration = duration;
        return *this;
    }
    AnimationInfoBuilder &setLoop(bool loop)
    {
        m_loop = loop;
        return *this;
    }
    AnimationInfo build() const
    {
        return {m_frameSize, m_startPos, m_frameCount, m_frameDuration, m_loop};
    }

private:
    sf::Vector2i m_frameSize;
    sf::Vector2i m_startPos;
    size_t m_frameCount{1};
    sf::Time m_frameDuration{sf::milliseconds(100)};
    bool m_loop{true};
};

class WeaponDataBuilder
{
public:
    WeaponDataBuilder &setDamage(float damage)
    {
        m_damage = damage;
        return *this;
    }
    WeaponDataBuilder &setPiercing(int piercing)
    {
        m_piercing = piercing;
        return *this;
    }
    WeaponDataBuilder &setMaxHits(int hits)
    {
        m_maxHits = hits;
        return *this;
    }
    WeaponDataBuilder &setLifetime(float lifetime)
    {
        m_lifetime = lifetime;
        return *this;
    }
    WeaponDataBuilder &setBehavior(WeaponBehavior behavior)
    {
        m_behavior = behavior;
        return *this;
    }
    WeaponComponentData build() const
    {
        return {m_damage, m_piercing, m_maxHits, m_lifetime, m_behavior};
    }

private:
    float m_damage{0.f};
    int m_piercing{0};
    int m_maxHits{1};
    float m_lifetime{0.f};
    WeaponBehavior m_behavior{WeaponBehavior::None};
};

class KinematicsDataBuilder
{
public:
    KinematicsDataBuilder &setVelocity(const sf::Vector2f &velocity)
    {
        m_velocity = velocity;
        return *this;
    }
    KinematicsDataBuilder &setAcceleration(const sf::Vector2f &acceleration)
    {
        m_acceleration = acceleration;
        return *this;
    }
    KinematicsDataBuilder &setAngularVelocity(float angularVelocity)
    {
        m_angularVelocity = angularVelocity;
        return *this;
    }
    KinematicsDataBuilder &setAngularAcceleration(float angularAcceleration)
    {
        m_angularAcceleration = angularAcceleration;
        return *this;
    }
    KinematicsDataBuilder &setScaleVelocity(const sf::Vector2f &scaleVelocity)
    {
        m_scaleVelocity = scaleVelocity;
        return *this;
    }
    KinematicsDataBuilder &setBehavior(KinematicsBehavior behavior)
    {
        m_behavior = behavior;
        return *this;
    }
    KinematicsDataBuilder &setOrbitRadius(float orbitRadius)
    {
        m_orbitRadius = orbitRadius;
        return *this;
    }
    KinematicsDataBuilder &setOrbitAngularVelocity(float orbitAngularVelocity)
    {
        m_orbitAngularVelocity = orbitAngularVelocity;
        return *this;
    }
    KinematicsDataBuilder &setPulseFrequency(float pulseFrequency)
    {
        m_pulseFrequency = pulseFrequency;
        return *this;
    }
    KinematicsDataBuilder &setPulseAmplitude(float pulseAmplitude)
    {
        m_pulseAmplitude = pulseAmplitude;
        return *this;
    }
    KinematicsDataBuilder &setDrag(float drag)
    {
        m_drag = drag;
        return *this;
    }
    KinematicsDataBuilder &setMass(float mass)
    {
        m_mass = mass;
        return *this;
    }
    KinematicsDataBuilder &setStatic(bool isStatic)
    {
        m_isStatic = isStatic;
        return *this;
    }
    KinematicsComponentData build() const
    {
        return {m_velocity,
                m_acceleration,
                m_angularVelocity,
                m_angularAcceleration,
                m_scaleVelocity,
                m_behavior,
                m_orbitRadius,
                m_orbitAngularVelocity,
                m_pulseFrequency,
                m_pulseAmplitude,
                m_drag,
                m_mass,
                m_isStatic};
    }

private:
    sf::Vector2f m_velocity{0.f, 0.f};
    sf::Vector2f m_acceleration{0.f, 0.f};
    float m_angularVelocity{0.f};
    float m_angularAcceleration{0.f};
    sf::Vector2f m_scaleVelocity{0.f, 0.f};
    KinematicsBehavior m_behavior{KinematicsBehavior::None};
    float m_orbitRadius{0.f};
    float m_orbitAngularVelocity{0.f};
    float m_pulseFrequency{5.f};
    float m_pulseAmplitude{0.2f};
    float m_drag{0.f};
    float m_mass{1.f};
    bool m_isStatic{false};
};
