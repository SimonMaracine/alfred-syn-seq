#include "alfred/math.hpp"

#include <fftw3.h>

namespace math {
    namespace fft {
        Transform::Transform(std::size_t size)
            : m_size(size) {
            m_temporary = std::make_unique<double[]>(m_size);
            m_plan = fftw_plan_r2r_1d(int(m_size), m_temporary.get(), m_temporary.get(), FFTW_R2HC, FFTW_ESTIMATE);
        }

        Transform::~Transform() {
            fftw_destroy_plan(m_plan);
        }

        void Transform::samples_to_frequencies(const double* samples, Frequencies& frequencies) {

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

        }
    }
}
