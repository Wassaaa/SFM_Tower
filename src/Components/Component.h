#pragma once

class Component
{
public:
    virtual ~Component() = default;

    virtual const char *getName() const = 0;

    void enable() { m_enabled = true; }
    void disable() { m_enabled = false; }
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

protected:
    bool m_enabled{true};
};
