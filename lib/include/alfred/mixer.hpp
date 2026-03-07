#pragma once

#include <algorithm>
#include <numeric>

namespace mixer {
    inline constexpr double MIN {50.0};
    inline constexpr double MAX {1.0};

    class Volume {
    public:
        constexpr Volume(double value = 1.0)
            : m_value(value) {}

        constexpr operator double() const { return m_value; }
        constexpr Volume operator+(const Volume& other) const { return m_value + other.m_value; }
        constexpr Volume& operator+=(const Volume& other) { m_value += other.m_value; return *this; }
        constexpr Volume operator-(const Volume& other) const { return m_value - other.m_value; }
        constexpr Volume& operator-=(const Volume& other) { m_value -= other.m_value; return *this; }
    private:
        double m_value {};
    };

    constexpr Volume clamp(Volume volume) {
        return std::clamp(volume, Volume(MAX), Volume(MIN));
    }

    constexpr void amplitudes(std::size_t size, const double* const* divisors, double** result) {
        for (std::size_t i {}; i < size; i++) {
            *result[i] = 1.0 / *divisors[i];
        }

        const double sum {std::accumulate(result, result, 0.0, [](auto lhs, auto rhs) { return lhs + *rhs; })};
        const double normalization {sum / double(size)};

        for (std::size_t i {}; i < size; i++) {
            *result[i] /= normalization;
        }
    }
}
