#pragma once

#include <numbers>
#include <memory>

struct fftw_plan_s;

namespace math {
    inline constexpr double PI {std::numbers::pi};

    constexpr double w(double hertz) {
        return hertz * 2.0 * std::numbers::pi;
    }

    consteval double operator""_hz(long double hertz) {
        return w(double(hertz));
    }

    constexpr bool less_than_eps(double x, double epsilon = 0.00001) {
        return x <= epsilon;
    }

    constexpr double zero_if_less_than_eps(double x, double epsilon = 0.00001) {
        if (less_than_eps(x, epsilon)) {
            return 0.0;
        } else {
            return x;
        }
    }

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

            void frequencies_to_samples(const Frequencies& frequencies, double* samples);
        private:
            std::size_t m_size {};
            std::unique_ptr<double[]> m_temporary;
            fftw_plan_s* m_plan {};
        };
    }
}
