option(ALFRED_DISTRIBUTION "Build for distribution" OFF)
option(ALFRED_ASAN "Build with sanitizers on Linux or not" OFF)

include(CMakeDependentOption)

cmake_dependent_option(ALFRED_ASAN_THREAD "Sanitize threading" OFF ALFRED_ASAN True)
