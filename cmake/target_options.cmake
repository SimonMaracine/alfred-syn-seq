function(set_target_cpp_mode target)
    target_compile_features(${target} PRIVATE cxx_std_23)
    set_target_properties(
        ${target} PROPERTIES
        CXX_EXTENSIONS OFF
        CMAKE_CXX_STANDARD_REQUIRED ON
    )
endfunction()

function(enable_target_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
    elseif(WIN32)
        target_compile_options(${target} PRIVATE "/W4")
    endif()
endfunction()

function(enable_target_diagnostics target)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
        target_compile_options(${target} PRIVATE "-g")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND ALFRED_ASAN)
        target_compile_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
        target_link_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined")
    endif()
endfunction()
