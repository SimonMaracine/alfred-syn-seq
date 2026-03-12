#pragma once

#include <numbers>
#include <algorithm>
#include <limits>
#include <memory>

struct fftw_plan_s;

namespace math {
    inline constexpr double PI = std::numbers::pi;
    inline constexpr double TWO_PI = 2.0 * std::numbers::pi;

    constexpr double w(double hertz) {
        return hertz * 2.0 * PI;
    }

    namespace literals {
        consteval double operator""_hz(long double hertz) {
            return w(double(hertz));
        }
    }

    constexpr double clamp(double x) {
        return std::clamp(x, 0.0, 1.0);
    }

    constexpr double clamp_sample(double sample) {
        return std::clamp(sample, -1.0, 1.0);
    }

    static constexpr double clamp_min(double x, double min = 1.0e-6) {
        return std::max(x, min);
    }

    template<typename Resolution>
    constexpr Resolution encode_sample(double sample) {
        static_assert(std::numeric_limits<Resolution>::is_integer && std::numeric_limits<Resolution>::is_signed);
        return Resolution(sample * double(std::numeric_limits<Resolution>::max()));
    }

    template<typename T>
    constexpr T map(T x, T in_min, T in_max, T out_min, T out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template<typename T>
    constexpr T mix(T x, T y, T a) {
        return x * (T(1) - a) + y * a;
    }

    constexpr unsigned int seconds_to_milliseconds(double seconds) {
        return static_cast<unsigned int>(seconds * 1000.0);
    }

    void normalize(double* samples, std::size_t size);

    namespace ft {
        class Frequencies {
        public:
            explicit Frequencies(std::size_t size);

            double* sine() { return m_sine.get(); }
            double* cosine() { return m_cosine.get(); }
            const double* sine() const { return m_sine.get(); }
            const double* cosine() const { return m_cosine.get(); }
        private:
            std::unique_ptr<double[]> m_sine;
            std::unique_ptr<double[]> m_cosine;
        };

        class Transform {
        public:
            explicit Transform(std::size_t size);
            ~Transform();

            Transform(const Transform&) = delete;
            Transform& operator=(const Transform&) = delete;
            Transform(Transform&&) = delete;
            Transform& operator=(Transform&&) = delete;

            void samples_to_frequencies(const double* samples, Frequencies& frequencies);
        private:
            std::size_t m_size {};
            std::unique_ptr<double[]> m_temporary;
            fftw_plan_s* m_plan {};
        };

        class InverseTransform {
        public:
            explicit InverseTransform(std::size_t size);
            ~InverseTransform();

            InverseTransform(const InverseTransform&) = delete;
            InverseTransform& operator=(const InverseTransform&) = delete;
            InverseTransform(InverseTransform&&) = delete;
            InverseTransform& operator=(InverseTransform&&) = delete;

            void frequencies_to_samples(const Frequencies& frequencies, double* samples);
        private:
            std::size_t m_size {};
            std::unique_ptr<double[]> m_temporary;
            fftw_plan_s* m_plan {};
        };
    }
}
