if(NOT UNIX AND NOT WIN32)
    message(FATAL_ERROR "Operating system is not Linux or Windows")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Compiler is not GCC or MSVC")
endif()

if(ALFRED_DISTRIBUTION AND ALFRED_ASAN)
    message(FATAL_ERROR "Asan is incompatible with distribution mode")
endif()
