#pragma once

namespace tomato
{
    class Timestep final
    {
    public:
        Timestep(float time)
            : m_Time(time) { }

        operator float() const { return m_Time; }

        [[nodiscard]] float GetSeconds() const { return m_Time; }
        [[nodiscard]] float GetMilliseconds() const { return m_Time * 1000.0f; }

    private:
        float m_Time = 0.f;
    };
}
