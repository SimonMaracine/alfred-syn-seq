#include "alfred/math.hpp"

#include <fftw3.h>

// https://fftw.org/

namespace alfred::math {
    void normalize(double* samples, std::size_t size) {
        double max {};

        for (std::size_t i {}; i < size; i++) {
            max = std::max(max, std::abs(samples[i]));
        }

        max = std::max(max, 1.0e-5);

        for (std::size_t i {}; i < size; i++) {
            samples[i] /= max;
        }
    }

    namespace ft {
        Frequencies::Frequencies(std::size_t size) {
            m_sine = std::make_unique<double[]>(size);
            m_cosine = std::make_unique<double[]>(size);
        }

        Transform::Transform(std::size_t size)
            : m_size(size) {
            m_temporary = std::make_unique<double[]>(m_size);
            m_plan = fftw_plan_r2r_1d(int(m_size), m_temporary.get(), m_temporary.get(), FFTW_R2HC, FFTW_ESTIMATE);
        }

        Transform::~Transform() {
            fftw_destroy_plan(m_plan);
        }

        void Transform::samples_to_frequencies(const double* samples, Frequencies& frequencies) {
            for (std::size_t i {}; i < m_size; i++) {
                m_temporary[i] = samples[i];
            }

            fftw_execute(m_plan);

            for (std::size_t i {}; i < m_size / 2; i++) {
                frequencies.cosine()[i] = m_temporary[i];

                if (i != 0) {
                    frequencies.sine()[i] = m_temporary[m_size - i];
                }
            }
        }

        InverseTransform::InverseTransform(std::size_t size)
            : m_size(size) {
            m_temporary = std::make_unique<double[]>(m_size);
            m_plan = fftw_plan_r2r_1d(int(m_size), m_temporary.get(), m_temporary.get(), FFTW_HC2R, FFTW_ESTIMATE);
        }

        InverseTransform::~InverseTransform() {
            fftw_destroy_plan(m_plan);
        }

        void InverseTransform::frequencies_to_samples(const Frequencies& frequencies, double* samples) {
            for (std::size_t i {}; i < m_size / 2; i++) {
                m_temporary[i] = frequencies.cosine()[i];

                if (i != 0) {
                    m_temporary[m_size - i] = frequencies.sine()[i];
                }
            }

            m_temporary[m_size / 2] = 0.0;

            fftw_execute(m_plan);

            for (std::size_t i {}; i < m_size; i++) {
                samples[i] = m_temporary[i];
            }
        }
    }
}
