cmake_minimum_required(VERSION 3.20)

add_subdirectory(synthesis)
add_subdirectory(audio)
add_subdirectory(instruments)

# Forward the variable to the parent
set(ALFRED_FFTW_DLL "${ALFRED_FFTW_DLL}" PARENT_SCOPE)
